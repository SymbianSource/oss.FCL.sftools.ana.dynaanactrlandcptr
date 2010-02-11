/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


//
// LDD for thread time profiling
//

#include <kern_priv.h>

#include "GeneralsDriver.h"
#include <piprofiler/PluginDriver.h>
#include <piprofiler/PluginSampler.h>
#include <piprofiler/ProfilerTraces.h>

#include "GppSamplerImpl.h"
#include "GfcSamplerImpl.h"
#include "IttSamplerImpl.h"
#include "MemSamplerImpl.h"
#include "PriSamplerImpl.h"


// just for testing
extern TUint* IntStackPtr();
extern void UsrModLr(TUint32*);

// the synch property, for other sampler implementations
//const TUid KGppPropertyCat={0x20201F70};
//enum TGppPropertyKeys
//	{
//	EGppPropertySyncSampleNumber
//	};

static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_FAIL( KDenyAllPolicy );


/*
 *
 *
 *	Class DGfcProfilerFactory definition
 *
 *
 */

class DGeneralsProfilerFactory : public DLogicalDevice
{
	public:
		DGeneralsProfilerFactory();


	public:
		virtual TInt Install();
		virtual void GetCaps(TDes8& aDes) const;
		virtual TInt Create(DLogicalChannelBase*& aChannel);
};

/*
 *
 *
 *	Class DGfcDriver definition
 *
 *
 */
class DPluginDriver;

class DGeneralsDriver : public DPluginDriver
{

public:
	DGeneralsDriver();
	~DGeneralsDriver();

private:
	TInt					NewStart(TInt aRate);
	static void				NewDoProfilerProfile(TAny*);
	static void				NewDoDfc(TAny*);
	TInt					Sample();

	TInt					GetSampleTime(TUint32* time);
	//TInt					Test(TUint32 testCase); 

	TInt					StartSampling();

	void					InitialiseSamplerList(); 

	DProfilerSamplerBase*		GetSamplerForId(TInt samplerId);
	TInt					GetSamplerVersion(TDes* aDes);
	
	TInt					ProcessStreamReadRequest(TBapBuf* aBuf,TRequestStatus* aStatus);

	TInt					MarkTraceActive(TInt samplerIdToActivate);
	TInt					MarkTraceInactive(TInt samplerIdToDisable);
	TInt					OutputSettingsForTrace(TInt samplerId,TInt settings);
	TInt					AdditionalTraceSettings(TInt samplerId,TInt settings);
	TInt					AdditionalTraceSettings2(TInt samplerId,TInt settings);
	TInt					SetSamplingPeriod(TInt /*samplerId*/,TInt settings);
private:
	// create the driver in EKA-2 version
	TInt					DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);

	// receive commands and control in EKA-2 version
	void					HandleMsg(TMessageBase* aMsg);
private:
	// timer mechanism in EKA-2 version
	NTimer						iTimer;
	TDfc						iNewDfc;
	TInt						iCount;
	TInt						iLastPcVal;					
	TInt						iPeriod;
	
	// sync sample number property for synchronizing other samplers
	RPropertyRef 				iSampleStartTimeProp;
	TInt						iSampleStartTime;
	
	DProfilerGppSampler<10000> 	gppSampler;
	DProfilerGfcSampler<10000> 	gfcSampler;
	DProfilerIttSampler<10000> 	ittSampler;
	DProfilerMemSampler<20000> 	memSampler;
	DProfilerPriSampler<10000> 	priSampler;

	static const TInt			KSamplerAmount = 5;
	DProfilerSamplerBase*		iSamplers[KSamplerAmount];
};

/*
 *
 *
 *	Class DGeneralsProfilerFactory implementation
 *
 *
 */

DECLARE_STANDARD_LDD()
    {
	return new DGeneralsProfilerFactory();
    }

TInt DGeneralsProfilerFactory::Create(DLogicalChannelBase*& aChannel)
    {
	aChannel = new DGeneralsDriver;
	return aChannel?KErrNone:KErrNoMemory;
    }


DGeneralsProfilerFactory::DGeneralsProfilerFactory()
    {
	// major, minor, and build version number
    iVersion=TVersion(1,0,1);
    }


TInt DGeneralsProfilerFactory::Install()
    {
    return(SetName(&KPluginSamplerName));
    }

void DGeneralsProfilerFactory::GetCaps(TDes8& aDes) const
    {
    TCapsSamplerV01 b;
    
    b.iVersion=TVersion(1,0,1);
    
    aDes.FillZ(aDes.MaxLength());
    aDes.Copy((TUint8*)&b,Min(aDes.MaxLength(),sizeof(b)));
    }

/*
 *
 *
 *	Class DGeneralsDriver implementation
 *
 *
 */
 
DGeneralsDriver::DGeneralsDriver() :
	iTimer(NewDoProfilerProfile,this),
	iNewDfc(NewDoDfc,this,NULL,7),
	gfcSampler(gppSampler.GetExportData()),
	ittSampler(gppSampler.GetExportData()),
	memSampler(gppSampler.GetExportData(), PROFILER_MEM_SAMPLER_ID),
	priSampler(gppSampler.GetExportData(), PROFILER_PRI_SAMPLER_ID)
    {
	LOGSTRING("DGeneralsDriver::DGeneralsDriver()");

	iState = EStopped;
	iEndRequestStatus = 0;
	doingDfc = 0;
	sampleRunning = 0;
	iSyncOffset = 0;
	InitialiseSamplerList();
    }

/*
 *
 *	This method has to be changed for each new sampler
 *
 */ 
void DGeneralsDriver::InitialiseSamplerList()
	{
	// initialize all samplers to zero
	for(TInt i=0;i<KSamplerAmount;i++)
		{
		iSamplers[i] = 0;
		}

	TInt i(0);
	iSamplers[i] = &gppSampler;i++;
	iSamplers[i] = &gfcSampler;i++;
	iSamplers[i] = &ittSampler;i++;
	iSamplers[i] = &memSampler;i++;
	iSamplers[i] = &priSampler;i++;
	
	// initialize synchronizing property
	LOGSTRING("DGeneralsDriver::InitialiseSamplerList() - initializing property");
	TInt r = iSampleStartTimeProp.Attach(KGppPropertyCat, EGppPropertySyncSampleNumber);
    if (r!=KErrNone)
        {
        LOGSTRING2("DGeneralsDriver::InitialiseSamplerList() - error in attaching counter property, error %d", r);
        }
    LOGSTRING("DGeneralsDriver::InitialiseSamplerList() - defining properties");
    r = iSampleStartTimeProp.Define(RProperty::EInt, KAllowAllPolicy, KDenyAllPolicy, 0, NULL);
    if (r!=KErrNone)
        {
        LOGSTRING2("DGeneralsDriver::InitialiseSamplerList() - error in defining counter property, error %d", r);
        }	
	}


DProfilerSamplerBase* DGeneralsDriver::GetSamplerForId(TInt samplerIdToGet)
    {
	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerIdToGet)
		    {
			return iSamplers[i];
		    }
	    }
	return (DProfilerSamplerBase*)0;
    }


TInt DGeneralsDriver::DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer)
    {
    TUint8 err(KErrNone);
    if (!Kern::QueryVersionSupported(TVersion(1,0,1),aVer))
	   	return KErrNotSupported;

	// just for testing 
	LOGTEXT("Initializing the stack pointer");
	stackTop=(TUint32*)IntStackPtr();
	LOGSTRING2("Got stack pointer 0x%x",(TUint32)stackTop);

	iClient = &Kern::CurrentThread();
	err = iClient->Open();
	
	iSampleStream.InsertCurrentClient(iClient);
	
	iTimer.Cancel();
	iNewDfc.Cancel();

	Kern::SetThreadPriority(24);
	SetDfcQ(Kern::DfcQue0());
	iNewDfc.SetDfcQ(iDfcQ);
	iMsgQ.Receive();
	return err;
    }

DGeneralsDriver::~DGeneralsDriver()
    {
	if (iState!=EStopped)
	    iTimer.Cancel();
	iNewDfc.Cancel();
	iSampleStartTimeProp.Close();
	Kern::SafeClose((DObject*&)iClient,NULL);
    }


TInt DGeneralsDriver::GetSampleTime(TUint32* time)
    {
	LOGSTRING("DGeneralsDriver::GetSampleTime - entry");

	Kern::ThreadRawWrite(	iClient,(TAny*)time, 
							(TAny*)&gppSampler.GetExportData()->sampleNumber, 
							4, iClient);

	LOGSTRING("DGeneralsDriver::GetSampleTime - exit");

	return KErrNone;
    }


TInt DGeneralsDriver::GetSamplerVersion(TDes* aDes)
    {
	LOGSTRING2("DGeneralsDriver::GetSamplerVersion - 0x%x",aDes);
	
	TBuf8<16> aBuf;
	aBuf.Append(PROFILER_SAMPLER_VERSION);
	Kern::ThreadDesWrite(iClient,aDes,aBuf,0,KChunkShiftBy0,iClient);
	LOGSTRING("DGeneralsDriver::GetSamplerVersion - written client descriptor");
	return KErrNone;
    }

TInt DGeneralsDriver::NewStart(TInt aDelay)
    {	
	LOGSTRING("DGeneralsDriver::NewStart");
	iEndRequestStatus = 0;
	
	aDelay = Min(KMaxDelay, Max(KMinDelay, aDelay));

	// always use this rate
	iPeriod = aDelay;
	
	iTimer.OneShot(aDelay);
	
	iState = ERunning;

	return KErrNone;
    }

/*
 *	This function is run in each interrupt
 */
// EKA-2 implementation of the sampler method

void DGeneralsDriver::NewDoProfilerProfile(TAny* pointer)
    {
    DGeneralsDriver& d = *((DGeneralsDriver*)pointer);

	if (d.iState == ERunning && d.sampleRunning == 0)
	    {
		d.iTimer.Again(d.iPeriod);
		d.sampleRunning++;

		TInt8 postSampleNeeded = 0;

		for(TInt i=0;i<KSamplerAmount;i++)
		    {
			if(d.iSamplers[i]->iEnabled)
			    {
				d.iSamplers[i]->Sample();
				postSampleNeeded += d.iSamplers[i]->PostSampleNeeded();
                }
            }
			
		if(postSampleNeeded > 0 && d.doingDfc == 0)
		    {
			d.doingDfc++;
			d.iNewDfc.Add();

			d.sampleRunning--;
			return;
            }
		d.sampleRunning--;
        }
	else if (d.iState == EStopping && d.sampleRunning == 0)
	    {
		// add a dfc for this final time
		d.iNewDfc.Add();
        }
	else
	    {
		// the previous sample has not finished,
		Kern::Printf("DGeneralsDriver::NewDoProfilerProfile - Profiler Sampler Error - interrupted before finished sampling!!");
        }
    }


TInt DGeneralsDriver::Sample()
    {
	return 0;
    }
/*
 *	This function is run when any of the samplers
 *	requires post sampling
 */
void DGeneralsDriver::NewDoDfc(TAny* pointer)
    {
	DGeneralsDriver& d = *((DGeneralsDriver*)pointer);
	
	if(d.iState == ERunning)
	    {
		// for all enabled samplers, perform
		// post sample if needed
		for(TInt i=0;i<KSamplerAmount;i++)
		    {
			if(d.iSamplers[i]->iEnabled)
			    {
				if(d.iSamplers[i]->PostSampleNeeded())
				    {
					d.iSamplers[i]->PostSample();
                    }
                }
            }
		d.doingDfc--;
        }

	else if(d.iState == EStopping)
	    {
		// for all enabled samplers,
		// perform end sampling
		TBool releaseBuffer = false;
		for(TInt i=0;i<KSamplerAmount;i++)
		    {
			if(d.iSamplers[i]->iEnabled)
			    {
				LOGSTRING("DGeneralsDriver::NewDoDfc() - ending");
				// perform end sampling for all samplers
				// stream mode samplers may be pending, if they
				// are still waiting for another client buffer
				if(d.iSamplers[i]->EndSampling() == KErrNotReady) 
				    {
					LOGSTRING("DGeneralsDriver::NewDoDfc() - stream data pending");
					releaseBuffer = true;
                    }
				else 
				    {
					LOGSTRING("DGeneralsDriver::NewDoDfc() - no data pending");
					releaseBuffer = true;
                    }		
                }
            }

		// At the end, once all the samplers are gone through, the buffer should be released
		if (true == releaseBuffer) 
		    {
			LOGSTRING("DGeneralsDriver::NewDoDfc() - release the buffer");
			d.iSampleStream.ReleaseIfPending();	
            }
		
		d.iState = EStopped;
		if(d.iEndRequestStatus != 0 && d.iClient != 0)
		    {
			// sampling has ended
			Kern::RequestComplete(d.iClient,d.iEndRequestStatus,KErrNone);
            }
        }
    }


/*
 *	All controls are handled here
 */
 
void DGeneralsDriver::HandleMsg(TMessageBase* aMsg)
    {
	TInt r=KErrNone;
	TThreadMessage& m=*(TThreadMessage*)aMsg;

	LOGSTRING5("DGeneralsDriver::HandleMsg 0x%x 0x%x 0x%x 0x%x",m.Int0(),m.Int1(),m.Int2(),m.Int3());
	
	if(m.iValue == (TInt)ECloseMsg)
	    {
		LOGSTRING("DGeneralsDriver::HandleMsg - received close message");
		iTimer.Cancel();
		iNewDfc.Cancel();
		m.Complete(KErrNone,EFalse);
		iMsgQ.CompleteAll(KErrServerTerminated);
		LOGSTRING("DGeneralsDriver::HandleMsg - cleaned up the driver!");
		return;
        }

	if (m.Client()!=iClient)
	    {
		LOGSTRING("DGeneralsDriver::HandleMsg - ERROR, wrong client");
		m.PanicClient(_L("GENERALSSAMPLER"),EAccessDenied);
		return;
        }

	TInt id=m.iValue;
	switch(id)
	    {
		 //Controls are handled here
		case RPluginSampler::EMarkTraceActive:
			LOGSTRING("DGeneralsDriver::HandleMsg - EMarkTraceActive");
			r = MarkTraceActive((TInt)m.Int0());
			break;

		case RPluginSampler::EOutputSettingsForTrace:
			LOGSTRING("DGeneralsDriver::HandleMsg - EOutputSettingsForTrace");
			r = OutputSettingsForTrace((TInt)m.Int0(),(TInt)m.Int1());
			break;

		case RPluginSampler::EAdditionalTraceSettings:
			LOGSTRING("DGeneralsDriver::HandleMsg - EAdditionalTraceSettings");
			r = AdditionalTraceSettings((TInt)m.Int0(),(TInt)m.Int1());
			break;

		case RPluginSampler::EAdditionalTraceSettings2:
			LOGSTRING("DGeneralsDriver::HandleMsg - EAdditionalTraceSettings2");
			r = AdditionalTraceSettings2((TInt)m.Int0(),(TInt)m.Int1());
			break;
			
		case RPluginSampler::ESetSamplingPeriod:
		    LOGSTRING2("DGeneralsDriver::HandleMsg - ESetSamplingPeriod %d", (TInt)m.Int1());
			r = SetSamplingPeriod((TInt)m.Int0(),(TInt)m.Int1());
			break;
			
		case RPluginSampler::EMarkTraceInactive:
			LOGSTRING("DGeneralsDriver::HandleMsg - EMarkTraceInactive");
			r = MarkTraceInactive((TInt)m.Int0());
			break;

		case RPluginSampler::ESample:
			LOGSTRING("DGeneralsDriver::HandleMsg - ESample");
			r = Sample();
			break;

		case RPluginSampler::EStartSampling:
			LOGSTRING("DGeneralsDriver::HandleMsg - EStartSampling");
			r = StartSampling();
			break;

		case RPluginSampler::EGetSampleTime:
			LOGSTRING("DGeneralsDriver::HandleMsg - EGetSampleTime");
			r = GetSampleTime(reinterpret_cast<TUint32*>(m.Ptr0()));
			break;

		case RPluginSampler::EGetSamplerVersion:
			LOGSTRING("DGeneralsDriver::HandleMsg - EGetSamplerVersion");
			r = GetSamplerVersion(reinterpret_cast<TDes*>(m.Ptr0()));
			break;
		
		case RPluginSampler::ECancelStreamRead:
			LOGSTRING("DGeneralsDriver::HandleMsg - ECancelStreamRead");
			iStreamReadCancelStatus = reinterpret_cast<TRequestStatus*>(m.Ptr0());
			r = ProcessStreamReadCancel();
			break;


		 //	Requests are handled here

		case ~RPluginSampler::EStopAndWaitForEnd:
			LOGSTRING("DGeneralsDriver::HandleMsg - EStopAndWaitForEnd");
			iEndRequestStatus = reinterpret_cast<TRequestStatus*>(m.Ptr0());
			r = StopSampling();
			break;

		case ~RPluginSampler::ERequestFillThisStreamBuffer:
			LOGSTRING("DGeneralsDriver::HandleMsg - ERequestFillThisStreamBuffer");			
			r = ProcessStreamReadRequest(	reinterpret_cast<TBapBuf*>(m.Ptr1()),
											reinterpret_cast<TRequestStatus*>(m.Ptr0()));
			break;

		default:
			LOGSTRING2("DGeneralsDriver::HandleMsg - ERROR, unknown command %d",id);
			r = KErrNotSupported;
			break;
        }

	LOGSTRING("DGeneralsDriver::HandleMsg - Completed");
	m.Complete(r,ETrue);
    }


inline TInt DGeneralsDriver::ProcessStreamReadRequest(TBapBuf* aBuf,TRequestStatus* aStatus)
	{
	LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest - entry");
	
	// a new sample buffer has been received from the client
	iSampleStream.AddSampleBuffer(aBuf,aStatus);
	
	// check if we are waiting for the last data to be written to the client
	if(iState == EStopped)
	    {
		LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest state = EStopped");
	
		// sampling has stopped and stream read cancel is pending
		// try to perform the end sampling procedure again
		TBool releaseBuffer = false;
		for(TInt i=0;i<KSamplerAmount;i++)
		    {
			// only for all enabled samplers that have stream output mode
			if(iSamplers[i]->iEnabled /*&& samplers[i]->outputMode == 2*/)
			    {
				//TInt pending = 0;
				// stream mode samplers may be pending, if they
				// are still waiting for another client buffer,
				// in that case, the request should be completed already
				if(iSamplers[i]->EndSampling() == KErrNotReady) 
				    {
					LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest - still data pending");
					releaseBuffer = true;
                    }
				else 
				    {
					LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest - no data pending");
					releaseBuffer = true;
                    }
                }
            }
		// At the end, once all the samplers are gone through, the buffer should be released
		if (true == releaseBuffer) 
		    {
			LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest - all data copied, release the buffer");
			iSampleStream.ReleaseIfPending();
		    }
        }
	LOGSTRING("DGeneralsDriver::ProcessStreamReadRequest - exit");
	
	return KErrNone;
	}


/*
 *	Mark traces active or inactive, this can be done
 *	only if sampling is not running
 */

inline TInt DGeneralsDriver::MarkTraceActive(TInt samplerIdToActivate)
	{
	LOGSTRING2("DGeneralsDriver::MarkTraceActive %d",samplerIdToActivate);

	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerIdToActivate)
		    {
			iSamplers[i]->SetEnabledFlag(true);
			return KErrNone;
            }
        }

	LOGSTRING2("DGeneralsDriver::MarkTraceActive - %d not supported",samplerIdToActivate);
	return KErrNotSupported;
	}

inline TInt DGeneralsDriver::MarkTraceInactive(TInt samplerIdToDisable)
	{
	LOGSTRING2("DGeneralsDriver::MarkTraceInactive %d",samplerIdToDisable);

	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerIdToDisable)
		    {
			iSamplers[i]->SetEnabledFlag(false);
			return KErrNone;
            }
        }

	LOGSTRING2("DGeneralsDriver::MarkTraceInactive - %d not supported",samplerIdToDisable);
	return KErrNotSupported;
	}

/*
 *	Set output settings for a trace
 */
 
inline TInt DGeneralsDriver::OutputSettingsForTrace(TInt samplerId,TInt settings)
	{
	LOGSTRING3("DGeneralsDriver::OutputSettingsForTrace id:%d set:%d",samplerId,settings);

	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerId)
		    {
			iSamplers[i]->SetOutputCombination(settings);
			return KErrNone;
		    }
	    }

	return KErrNotSupported;	
	}

/*
 *	Set additional settings for a trace
 */

inline TInt DGeneralsDriver::AdditionalTraceSettings(TInt samplerId,TInt settings)
	{
	LOGSTRING3("DGeneralsDriver::SetAdditionalTraceSettings id:%d set:%d",samplerId,settings);

	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerId)
		    {
			iSamplers[i]->SetAdditionalSettings(settings);
			return KErrNone;
            }
        }

	return KErrNotSupported;	
	}

inline TInt DGeneralsDriver::AdditionalTraceSettings2(TInt samplerId,TInt settings)
	{
	LOGSTRING3("DGeneralsDriver::SetAdditionalTraceSettings id:%d set:%d",samplerId,settings);

	for(TInt i=0;i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerId)
		    {
			iSamplers[i]->SetAdditionalSettings2(settings);
			return KErrNone;
		    }
        }

	return KErrNotSupported;	
	}

inline TInt DGeneralsDriver::SetSamplingPeriod(TInt samplerId,TInt settings)
	{
	LOGSTRING2("DGeneralsDriver::SetSamplingPeriod - set:%d",settings);

//	iPeriod = settings;

	for(TInt i(0);i<KSamplerAmount;i++)
	    {
		if(iSamplers[i]->iSamplerId == samplerId)
		    {
			iSamplers[i]->SetSamplingPeriod(settings);
			return KErrNone;
		    }
	    }

	return KErrNotSupported;	
	}

/*
 *	Mark traces active or inactive, this can be done
 *	only if sampling is not running
 */
 
TInt DGeneralsDriver::StartSampling()
	{
	LOGSTRING("DGeneralsDriver::StartSampling");

	if(iState == EStopped)
		{
		// reset iSampleStartTimeProp property value
		iSampleStartTime = NKern::TickCount();	// get the system tick value for sync purposes 
		TInt r = iSampleStartTimeProp.Set(iSampleStartTime);
		
		// reset all enabled samplers
		for(TInt i=0;i<KSamplerAmount;i++)
			{
			if(iSamplers[i]->iEnabled)
				{

				// reset with stream option
				LOGSTRING2(("DGeneralsDriver::StartSampling - stream reset for generals driver, sync offset %d"), 0);
				iSamplers[i]->Reset(&iSampleStream, 0);
				}
			}

		NewStart(gppSampler.GetPeriod());
		return KErrNone;
		}
	else
		{
		return KErrGeneral;
		}
	}



