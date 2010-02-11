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


#ifndef PROFILER_ITT_SAMPLER_H
#define PROFILER_ITT_SAMPLER_H

#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"

/*
 *	
 *	ITT sampler definition
 *	
 */

class IttSamplerImpl
{
public:
	IttSamplerImpl();
	~IttSamplerImpl();

	TInt	SampleImpl(TUint32 pc,TUint32 sampleNum);
	TBool	SampleNeeded(TUint32 sampleNum);
	TInt 	CreateFirstSample();
	void	Reset();

	TUint8* itt_sample;
	TBool							iSampleProcesses;
private:

	TUint8					internalData[256];
	TPtr8*					ptrToData;
	TInt					currentLibCount;
	TInt					currentProcCount;
	
	TBuf8<64>				iVersionData;
	SDblQue* 				codeSegList;
};

template <int BufferSize>
class DProfilerIttSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerIttSampler(struct TProfilerGppSamplerData* gppSamplerDataIn);
	~DProfilerIttSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt	PostSample();
	TBool	PostSampleNeeded();

private:
    struct TProfilerGppSamplerData*	    gppSamplerData;
	IttSamplerImpl					ittSampler;
	TBool							sampleInProgress;
	TBool							sampleNeeded;
};

/*  
 *	
 *	ITT sampler implementation
 *	
 */ 

template <int BufferSize>
DProfilerIttSampler<BufferSize>::DProfilerIttSampler(struct TProfilerGppSamplerData* gppSamplerDataIn) :
	DProfilerGenericSampler<BufferSize>(PROFILER_ITT_SAMPLER_ID)
{
	this->gppSamplerData = (struct TProfilerGppSamplerData*)gppSamplerDataIn;
	this->sampleInProgress = false;
	LOGSTRING2("CProfilerIttSampler<%d>::CProfilerIttSampler",BufferSize);	
}

template <int BufferSize>
TInt DProfilerIttSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
{
	LOGSTRING2("CProfilerIttSampler<%d>::Reset - calling superclass reset",BufferSize);
	DProfilerGenericSampler<BufferSize>::Reset(aStream);
	LOGSTRING2("CProfilerIttSampler<%d>::Reset - called superclass reset",BufferSize);	
	TInt length = this->ittSampler.CreateFirstSample();
	this->iSampleBuffer->AddSample((TUint8*)&length,1);
	this->iSampleBuffer->AddSample(ittSampler.itt_sample,length);
	this->sampleInProgress = false;
	this->sampleNeeded = false;

	this->ittSampler.Reset();

	return KErrNone;
}

template <int BufferSize> 
TInt DProfilerIttSampler<BufferSize>::PostSample()
{
	if(sampleNeeded)
	{
		this->sampleNeeded = false;

		LOGSTRING3("CProfilerIttSampler<%d>::PostSample - state %d",BufferSize,this->sampleBuffer->GetBufferStatus());

		
		TInt length = this->ittSampler.SampleImpl(this->gppSamplerData->lastPcValue,
									        		  this->gppSamplerData->sampleNumber);
		if(length != 0)
		{		
			LOGSTRING("ITT sampler PostSample - starting to sample");

			while(length > 0)
			{
                this->iSampleBuffer->AddSample(ittSampler.itt_sample,length);
				length = this->ittSampler.SampleImpl( this->gppSamplerData->lastPcValue,
									        		  this->gppSamplerData->sampleNumber );	
				if(length == 0) 
				{
					LOGSTRING("MEM sampler PostSample - all samples generated!");
				}
			}
			LOGSTRING("ITT sampler PostSample - finished sampling");
		}
		this->sampleInProgress = false;
	}
	
	// finally perform superclass postsample
	TInt i = this->DProfilerGenericSampler<BufferSize>::PostSample();
	return i;
}

template <int BufferSize> 
TBool DProfilerIttSampler<BufferSize>::PostSampleNeeded()
{
	LOGSTRING3("CProfilerMemSampler<%d>::PostSampleNeeded - state %d",BufferSize,this->sampleBuffer->GetBufferStatus());

	TUint32 status = this->iSampleBuffer->iBufferStatus;

	if(status == DProfilerSampleBuffer::BufferCopyAsap || status == DProfilerSampleBuffer::BufferFull || this->sampleNeeded == true)
	{
		return true;
	}
	
	return false;
}

template <int BufferSize>
void DProfilerIttSampler<BufferSize>::Sample()
{
	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);	
	
	//#ifdef ITT_TEST
	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);	
	
	if(ittSampler.SampleNeeded(this->gppSamplerData->sampleNumber) && this->sampleInProgress == false) 
	{
		this->sampleInProgress = true;
		this->sampleNeeded = true;

		LOGSTRING2("CProfilerIttSampler<%d>::Sample - sample needed",BufferSize);	
	}	

	LOGSTRING2("CProfilerIttSampler<%d>::Sample",BufferSize);

	return;
}

template <int BufferSize>
DProfilerIttSampler<BufferSize>::~DProfilerIttSampler()
{
	LOGSTRING2("CProfilerIttSampler<%d>::~CProfilerIttSampler",BufferSize);		
}
#endif
