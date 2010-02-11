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


#ifndef PROFILER_MEM_SAMPLER_H
#define PROFILER_MEM_SAMPLER_H

#include "GeneralsConfig.h"

#include <kern_priv.h>

#include <piprofiler/ProfilerGenericClassesKrn.h>
#include <piprofiler/ProfilerTraces.h>
#include "GppSamplerImpl.h"

// defines the maximum thread amount that is assumed to
// be possible with MEM trace
#define PROFILER_MAX_THREAD_AMOUNT	512

/*
 *	
 *	MEM sampler definition
 *	
 */

class MemSamplerImpl
{
public:
	enum EProcessingState
	{
		EStartingToProcess,
		EProcessingNames,
		EProcessingData,
		ENothingToProcess
	};

	MemSamplerImpl();
	~MemSamplerImpl();

	TInt	CreateFirstSample();
	TInt	SampleImpl();
	TBool	SampleNeeded();
	void	Reset();
	TInt	ProcessChunks();
	TInt    ProcessThreads();
    TInt    GatherChunks();
    TInt    GatherThreads();
	
	TInt	EncodeChunkData(DChunk& chunk);
	TInt	EncodeChunkName(DChunk& chunk);
	TInt	EncodeChunkData(DThread& thread);
	TInt	EncodeChunkName(DThread& thread);	

	TInt 	EncodeTotalMemoryName();
	TInt 	EncodeTotalMemory();
	
	TInt	EncodeNameCode();
	TInt	EncodeDataCode();

	DChunk*		heapChunksToSample[PROFILER_MAX_THREAD_AMOUNT];
	DChunk*		heapChunkNamesToReport[PROFILER_MAX_THREAD_AMOUNT];
	TInt		iCount;
	TInt		iChunkCount;
	TInt		iNewChunkCount;
	TBuf8<0x50> name;
	DThread*	threadsToSample[PROFILER_MAX_THREAD_AMOUNT];
	DThread*	threadNamesToReport[PROFILER_MAX_THREAD_AMOUNT];
	TInt		iThreadCount;
	TInt		iNewThreadCount;

	TInt		iChunksProcessing;
    TInt        iThreadsProcessing;
	TInt		iMemSamplingPeriod;
	TInt		iMemSamplingPeriodDiv2;
	TBool		iSampleThreads;
	TBool       iTimeToSample;
	
	TBool 		iTotalMemoryOk;
	TBool		iTotalMemoryNameOk;

	TUint8		sample[257];
	TPtr8		sampleDescriptor;
};

template <int BufferSize>
class DProfilerMemSampler : public DProfilerGenericSampler<BufferSize>
{
public:
	DProfilerMemSampler(struct TProfilerGppSamplerData*, TInt id);
	~DProfilerMemSampler();

	void	Sample();
	TInt	Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset);
	TInt	PostSample();
	TBool	PostSampleNeeded();
	
private:
	MemSamplerImpl			           memSamplerImpl;
	struct TProfilerGppSamplerData*    gppSamplerData;
	TBool                              sampleNeeded;
};

/*
 *	
 *	MEM sampler implementation
 *	
 */

template <int BufferSize>
DProfilerMemSampler<BufferSize>::DProfilerMemSampler(struct TProfilerGppSamplerData* gppSamplerDataIn, TInt id) :
	DProfilerGenericSampler<BufferSize>(PROFILER_MEM_SAMPLER_ID)
    {
	LOGSTRING2("CProfilerMemSampler<%d>::CProfilerMemSampler",BufferSize);
	this->gppSamplerData = gppSamplerDataIn;
	this->iSamplingPeriod = 3000;	// set default setting
    }

template <int BufferSize>
TInt DProfilerMemSampler<BufferSize>::Reset(DProfilerSampleStream* aStream, TUint32 aSyncOffset)
    {
	LOGSTRING2("CProfilerMemSampler<%d>::Reset - calling superclass reset",BufferSize);
	DProfilerGenericSampler<BufferSize>::Reset(aStream);
	LOGSTRING2("CProfilerMemSampler<%d>::Reset - called superclass reset",BufferSize);
	memSamplerImpl.Reset();

	this->memSamplerImpl.iMemSamplingPeriod = this->iSamplingPeriod;
	this->memSamplerImpl.iMemSamplingPeriodDiv2 = (TInt)(this->memSamplerImpl.iMemSamplingPeriod / 2);
	
	LOGSTRING3("CProfilerMemSampler<%d>::Reset - set mem sampling period to",
							BufferSize,this->memSamplerImpl.iMemSamplingPeriod);

	TInt length = memSamplerImpl.CreateFirstSample();
	this->iSampleBuffer->AddSample(memSamplerImpl.sample,length);
	
	this->sampleNeeded = false;

	return KErrNone;
    }

template <int BufferSize> 
TInt DProfilerMemSampler<BufferSize>::PostSample()
    {
    this->sampleNeeded = false;

    LOGSTRING3("CProfilerMemSampler<%d>::PostSample - state %d",BufferSize,this->sampleBuffer->GetBufferStatus());
    
    TInt interruptLevel = NKern::DisableInterrupts(0);
    
    // first collect chunk data
    TInt length = this->memSamplerImpl.SampleImpl();
    if(length != 0)
        {
        LOGSTRING("MEM sampler PostSample - starting to sample");

        // then, encode the sample to the buffer
        while(length > 0)
            {
            this->iSampleBuffer->AddSample(memSamplerImpl.sample,length);
            length = this->memSamplerImpl.SampleImpl();
            // indicate that the whole MEM sample ends by having a 0x00 in the end
            if(length == 0)
                {
                TUint8 number = 0;
                LOGSTRING("MEM sampler PostSample - all samples generated!");
                
                this->iSampleBuffer->AddSample(&number,1);
                LOGSTRING("MEM sampler PostSample - end mark added");
                }
            }
        }
    
    LOGSTRING("MEM sampler PostSample - finished sampling");
    
    NKern::RestoreInterrupts(interruptLevel);
   
    // finally perform superclass postsample
	TInt i = this->DProfilerGenericSampler<BufferSize>::PostSample();
	return i;

    }

template <int BufferSize> 
TBool DProfilerMemSampler<BufferSize>::PostSampleNeeded()
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
void DProfilerMemSampler<BufferSize>::Sample()
    {

	LOGSTRING2("CProfilerMemSampler<%d>::Sample",BufferSize);	
	
	if(memSamplerImpl.SampleNeeded()) 
	    {
		this->sampleNeeded = true;

		LOGSTRING2("CProfilerMemSampler<%d>::Sample - sample needed",BufferSize);	
		
		// start the MEM sample with the sample time
		TUint8 number = 4;    // mem sampler id
		this->iSampleBuffer->AddSample(&number,1);
		this->iSampleBuffer->AddSample((TUint8*)&(gppSamplerData->sampleNumber),4);

		// leave the rest of the processing for PostSample()
	    }	

	LOGSTRING2("CProfilerMemSampler<%d>::Sample",BufferSize);
	return;
    }

template <int BufferSize>
DProfilerMemSampler<BufferSize>::~DProfilerMemSampler()
    {
	LOGSTRING2("CProfilerMemSampler<%d>::~CProfilerMemSampler",BufferSize);		
    }
#endif
