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


#include <piprofiler/ProfilerVersion.h>
#include <piprofiler/ProfilerTraces.h>
#include <kern_priv.h>

#include "GppSamplerImpl.h"

extern TUint*		IntStackPtr();
#define	TAG(obj)	(*(TUint32*)&(obj.iAsyncDeleteNext))

// properties for ISA task parsing
const TUid KIsaPropertyCat={0x2001E5AD};
enum TIsaPropertyKeys
	{
	EIsaPropertyIsaTaskParserStatus = 1,
	EIsaPropertyIsaTaskAddressStart,
	EIsaPropertyIsaTaskAddressEnd,
	EIsaPropertyIsaTaskAddress,
	EIsaPropertyIsaOsTaskRunningAddress,
	EIsaPropertyIsaTaskParsedName
	};


GppSamplerImpl::GppSamplerImpl()
	{
	LOGTEXT("GppSamplerImpl::GppSamplerImpl");
	iInterruptStack = (TUint*)IntStackPtr();

	LOGTEXT("GppSamplerImpl::GppSamplerImpl - attaching to properties");

	TInt err = iIsaStartAddr.Attach(KIsaPropertyCat, EIsaPropertyIsaTaskAddressStart);
	if(err != KErrNone)
		LOGTEXT("GppSamplerImpl::GppSamplerImpl() - Property EIsaPropertyIsaTaskAddressStart not available"); 
	err = iIsaEndAddr.Attach(KIsaPropertyCat, EIsaPropertyIsaTaskAddressEnd);
	if(err != KErrNone)
		LOGTEXT("GppSamplerImpl::GppSamplerImpl() - Property EIsaPropertyIsaTaskAddressEnd not available"); 
	err = iIsaPluginStatus.Attach(KIsaPropertyCat, EIsaPropertyIsaTaskParserStatus);
	if(err != KErrNone)
		LOGTEXT("GppSamplerImpl::GppSamplerImpl() - Property EIsaPropertyIsaTaskParserStatus not available"); 
	err = iIsaOsTaskRunning.Attach(KIsaPropertyCat, EIsaPropertyIsaOsTaskRunningAddress);
	if(err != KErrNone)
		LOGTEXT("GppSamplerImpl::GppSamplerImpl() - Property EIsaPropertyIsaOsTaskRunningAddress not available"); 
	
	PROFILER_ISA_TASK_NAMES
	
	Reset();
	}

GppSamplerImpl::~GppSamplerImpl() 
	{
	iIsaStartAddr.Close();
	iIsaEndAddr.Close();
	iIsaPluginStatus.Close();
	iIsaOsTaskRunning.Close();
	}

void GppSamplerImpl::Reset()
	{
	LOGTEXT("GppSamplerImpl::Reset");
	iLastPc = 0;
	iLastThread = 0;
	iRepeat = 0;
	iIsaStatus = 0;
	iIsaStart = 0;
	iIsaEnd = 0;
//	isaOsTaskRunningAddr = 0;
	iStartTime=( NKern::TickCount() & 0xfffffffc );
	
	TPropertyStatus status;
	TInt osAddr = 0;
	
	LOGTEXT("GppSamplerImpl::Reset - getting status");
	
	// get status of ISA plug-in
	if(iIsaPluginStatus.GetStatus(status))
		{
		iIsaPluginStatus.Get(iIsaStatus);
		LOGSTRING2("GppSamplerImpl::Reset - ISA plug-in status %d", iIsaStatus);
		}
	
	if(iIsaStatus > 0)
		{
		LOGTEXT("GppSamplerImpl::Reset - get isa start address");
		iIsaStartAddr.Get(iIsaStart);
		LOGTEXT("GppSamplerImpl::Reset - get isa end address");
		iIsaEndAddr.Get(iIsaEnd);
		LOGTEXT("GppSamplerImpl::Reset - get isa os_task_running address");
		iIsaOsTaskRunning.Get(osAddr);
		isaOsTaskRunningAddr = reinterpret_cast<TInt*>(osAddr);
		LOGSTRING2("GppSamplerImpl::Reset - got isa os_task_running address 0x%X", osAddr);
		}
	
	LOGTEXT("GppSamplerImpl::Reset - initializing isa task list");

	iIsaSample = false;
	
	for(TInt i=0;i<256;i++)
		knownIsaTasks[i] = -1;
	
	knownIsaTaskCount = 0;
	
	}

TUint8* GppSamplerImpl::EncodeTag(TUint8* aPtr)
//
// Encode a tag and version to the trace data. This allows the offline analyser to 
// identify the sample data.
//
{	
	_LIT(KGppSamplerVersion,"Bappea_GPP_V");
	_LIT(KProfilerVersion,"#Prof#");
	_LIT(KSamplerVersion,"#Samp#");
	
	TBuf<64> buf;
	buf.Zero();
	buf.Append(KGppSamplerVersion);
	buf.Append(PROFILER_GPP_SAMPLER_VERSION);
	buf.Append(KProfilerVersion);
	buf.Append(PROFILER_VERSION_SHORT);	
	buf.Append(KSamplerVersion);
	buf.Append(PROFILER_SAMPLER_VERSION);
	aPtr = EncodeText(aPtr, buf);
	return aPtr;
}

TUint8* GppSamplerImpl::EncodeInt(TUint8* aPtr,TInt aValue)
{
	LOGSTRING2("Encoding int 0x%x",aPtr);

	LOGSTRING2("TIint = 0x%x",aValue);

	TUint byte;
	for (;;)
		{
		byte = aValue & 0x7f;
		if ((aValue >> 6) == (aValue >> 7))
			break;
		aValue >>= 7;
		*aPtr++ = byte;
		}
	*aPtr++ = byte | 0x80;

	LOGSTRING2("Encoded int 0x%x",aPtr);

	return aPtr;
}

TUint8* GppSamplerImpl::EncodeUint(TUint8* aPtr,TUint aValue)
{
	LOGSTRING2("Encoding Uint 0x%x",aPtr);

	LOGSTRING2("TUint = 0x%x",aValue);


	TUint byte;
	for (;;)
		{
		byte = aValue & 0x7f;
		aValue >>= 7;
		if (aValue == 0)
			break;
		*aPtr++ = byte;
		}
	*aPtr++ = byte | 0x80;

	LOGSTRING2("Encoded Uint 0x%x",aPtr);

	return aPtr;
}

TUint8* GppSamplerImpl::EncodeText(TUint8* aPtr, const TDesC& aDes)
//
// Encode a descriptor into the data stream
// This is currently limited to a descriptor that is up to 255 characters in length,
// and Unicode characters are truncated to 8 bits
//
{
	LOGSTRING2("Encoding text 0x%x",aPtr);
	TInt len=aDes.Length();
	*aPtr++ = TUint8(len);
	const TText* p = aDes.Ptr();
	while (--len >= 0)
		{
		*aPtr++ = TUint8(*p++);
		}

	LOGSTRING2("Encoded text 0x%x",aPtr);
	return aPtr;
}


TUint8* GppSamplerImpl::EncodeName(TUint8* aPtr, DObject& aObject,TUint32 id)
//
// Encode the name of a kernel object
//
{
	LOGSTRING2("Encoding name 0x%x",aPtr);
	TBuf8<0x5f> name;
	aObject.TraceAppendName(name,false);

	if(id != 0xffffffff)
	{
		name.Append('[');
		name.AppendNum(id,EHex);
		name.Append(']');
	}
	else
	{
		name.Append('[');
		name.AppendNum((TUint32)((void*)&(((DThread*)&aObject)->iNThread)),EHex);
		name.Append(']');
	}

	aPtr = EncodeText(aPtr,name);
	LOGSTRING2("Encoded name 0x%x",aPtr);
	return aPtr;
}

TUint8* GppSamplerImpl::EncodeThread(TUint8* aPtr, DThread& aThread)
//
// Encode a thread name in the data stream.
// The thread is identified by its name, and the identity of its owning process.
// If the process has not been identified in the data stream already, it's name is
// also encoded.
//
{
	LOGSTRING2("Encoding thread 0x%x",aPtr);	

	DProcess& p = *aThread.iOwningProcess;
	
	aPtr = EncodeUint(aPtr, p.iId);
	if (TAG(p) != iStartTime)
	{
		TAG(p) = iStartTime;
		// Provide the name matching this process ID
		aPtr = EncodeName(aPtr, p, p.iId);
	}
	aPtr = EncodeName(aPtr, aThread,0xffffffff);
	
	LOGSTRING2("Encoded thread 0x%x",aPtr);	

	return aPtr;
}

TUint8* GppSamplerImpl::EncodeRepeat(TUint8* aPtr)
//
// Encode a repeated sequence of samples
//
{
	LOGSTRING2("Encoding repeat, 0x%x",iRepeat);	

	aPtr = EncodeInt(aPtr, 0);
	aPtr = EncodeUint(aPtr, iRepeat);
	iRepeat = 0;

	LOGSTRING2("Encoded repeat, 0x%x",iRepeat);	

	return aPtr;
}

TInt GppSamplerImpl::CreateFirstSample()
{
	LOGTEXT("GppSamplerImpl::CreateFirstSample");
	Reset();

	TUint8* w = this->tempBuf;
	w = EncodeTag(w);

	TInt length = w-tempBuf;

	LOGSTRING2("TAG encoded, length %d",length);
	return length;
}

TBool GppSamplerImpl::IsaTaskKnown(TUint8 task)
{
	for(TInt i=0;i<256;i++)
	{
		if(knownIsaTasks[i] == -1)
		{
			knownIsaTasks[i] = task;
			knownIsaTaskCount++;
			return false;
		}
		else if(knownIsaTasks[i] == task)
		{
			return true;
		}
	}

	return false;
}

TUint8* GppSamplerImpl::EncodeIsaTask(TUint8* aPtr, TUint task)

{
	LOGSTRING2("Encoding ISA task 0x%x",aPtr);	

	aPtr = EncodeUint(aPtr,task);
	// use the task name as the process name
	aPtr = EncodeIsaName(aPtr,task,true);
	// then encode the task name
	aPtr = EncodeIsaName(aPtr,task,false);
	
	LOGSTRING2("Encoded ISA task 0x%x",aPtr);	

	return aPtr;
}

TUint8* GppSamplerImpl::EncodeIsaName(TUint8* aPtr, TUint task,TBool process)
//
// Encode a descriptor into the data stream
// This is currently limited to a descriptor that is up to 255 characters in length,
// and Unicode characters are truncated to 8 bits
//
{
	TBuf8<256> aDes;
	
//	#ifdef NCP_COMMON_PROFILER_ISA_TASKS 
	if(iIsaStatus > 0)
		{
		// resolve the isa task name from the task name array
		if((task-100000) < PROFILER_ISA_OS_TASK_AMOUNT && process == false)
			{
			aDes.Append(isaTaskNames[(task-100000)]);
			}
		else
			{
			aDes.Append(_L8("NativeOS_Task"));
			}
		}
	else
		{
		aDes.Append(_L8("NativeOS_Task"));
		}
	
	aDes.Append('[');
	aDes.AppendNum((task-100000),EHex);
	aDes.Append(']');

	LOGSTRING2("Encoding ISA name 0x%x",aPtr);
	TInt len=aDes.Length();
	*aPtr++ = TUint8(len);
	const TText* p = aDes.Ptr();
	while (--len >= 0)
		{
		*aPtr++ = TUint8(*p++);
		}

	LOGSTRING2("Encoded ISA name 0x%x",aPtr);
	return aPtr;
}


TInt GppSamplerImpl::SampleImpl()
//
// ISR for the profile timer
// This extracts the thread and PC that was current when the interrupt went off and
// encodes it into the sample data buffer. If enough data has been generated, the
// DFC is triggered to complete a read request
//
{
	TUint8* w = this->tempBuf;
	
	TUint32 pc = iInterruptStack[-1];
	LOGSTRING3("pc value 0x%x sp 0x%x",pc,iInterruptStack);

	// ignore the low bit being set for THUMB mode - we use for something else
	pc &= ~1;			
	TInt diff = pc - iLastPc;
	iLastPc = pc;

	if(iIsaStatus > 0)
		{
		if((TUint32)pc > (TUint32)iIsaStart && (TUint32)pc < (TUint32)iIsaEnd)
			{
			LOGSTRING2("Identified ISA execution at 0x%x",pc);
			iIsaSample = true;
			}
		else
			{
			LOGSTRING2("Normal sample at 0x%x",pc);
			iIsaSample = false;
			}
		}

	DThread& t = Kern::CurrentThread();

	TUint tid;
	TUint8 isaTask = 0;
	if(iIsaSample)
	{
		LOGSTRING2("Reading ISA task number from 0x%x",isaOsTaskRunningAddr);

		// if we don't get reasonable ISA address to read, skip ISA task handling
		if(isaOsTaskRunningAddr == 0)
			{
			tid = 100000; // to tell the difference from SOS threads
			iIsaSample = false;
			}
		else	// normal ISA task parsing process
			{
			isaTask = *isaOsTaskRunningAddr;
			LOGSTRING2("ISA task = %d",isaTask);
			tid = isaTask;
			// this will make sure we don't mix ISA tasks and normal tasks
			tid += 100000;
			}

	}
	else
	{
		tid = t.iId;
	}

	if (tid != iLastThread)
	{
		// Change of thread is marked in the low bit of the PC difference
		diff |= 1;
	}
	TUint rp = iRepeat;
	if (diff == 0)
	{
		// Identical sample, bump up the repeat count
		iRepeat = rp + 1;
	}
	else
	{
		if (rp)
		{
			// Encode the repeat data
			w = EncodeRepeat(w);
		}
		// Encode the PC difference
		w = EncodeInt(w, diff);
		if (diff & 1)
		{
			// Encode the new thread ID
			if(iIsaSample)
			{
				iLastThread = tid;
				w = EncodeUint(w,tid);

				if(!this->IsaTaskKnown(isaTask))
				{
					w = EncodeIsaTask(w,iLastThread);
				}
				//LOGSTRING2("Sample total length: %d",w-tempBuf);
				TInt length = w-tempBuf;
				// encoded isa task, return here
				return length;
			}
		
			iLastThread = tid;
			w = EncodeUint(w, tid);

			if ((TAG(t) & 0xfffffffc) != iStartTime)
			{
			
				TAG(t) = ((TAG(t) & 0x3) | iStartTime);
				// The thread is 'unknown' to this sample, so encode the thread name
				w = EncodeThread(w, t);		
			}
		}
	}
	LOGSTRING2("Sample total length: %d",w-tempBuf);
	TInt length = w-tempBuf;

	return length;
}

