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
#include <plat_priv.h>

#include "MemSamplerImpl.h"

// for testing precise stack utilization tracing...
// crashes at the moment

#include <nk_cpu.h>

#if !defined(__NKERN_H__)
#include <nkern.h>
#endif 

#define TAG(obj) (*(TUint32*)&(obj->iAsyncDeleteNext))
#define PROFILER_CHUNK_MARK		((TUint32)0x00001000)
#define PROFILER_MEM_THREAD_MARK	((TUint32)0x00000001)

_LIT8(KMemVersion, "1.56");

MemSamplerImpl::MemSamplerImpl() :
	sampleDescriptor(&(this->sample[1]),0,256)
    {
	LOGTEXT("MemSamplerImpl::MemSamplerImpl() - konstruktori");

	iCount = 0;
	iSampleThreads = true;
	iTimeToSample = false;
	
	iTotalMemoryOk = false;
	iTotalMemoryNameOk = false;
	
	iNewChunkCount = 0;
	iChunkCount = 0;
	iChunksProcessing = ENothingToProcess;
    iThreadsProcessing = ENothingToProcess;
	
	for(TInt i(0);i<PROFILER_MAX_THREAD_AMOUNT;i++)
	    {
		this->heapChunksToSample[i] = 0;
		this->heapChunkNamesToReport[i] = 0;
	    }
  

	iNewThreadCount = 0;
	iThreadCount = 0;
	
	for(TInt i=0;i<PROFILER_MAX_THREAD_AMOUNT;i++)
	    {
		this->threadsToSample[i] = 0;
		this->threadNamesToReport[i] = 0;
	    }
    }

MemSamplerImpl::~MemSamplerImpl()
    {
	
    }

TInt MemSamplerImpl::CreateFirstSample()
    {
	LOGTEXT("MemSamplerImpl::CreateFirstSample - entry");
	
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L8("Bappea_V"));
	this->sampleDescriptor.Append(KMemVersion);
	this->sampleDescriptor.Append(_L8("_MEM"));
	
	sample[0] = this->sampleDescriptor.Size();

	LOGTEXT("MemSamplerImpl::CreateFirstSample - exit");

	return (TInt)(sample[0]+1);
    }

TBool MemSamplerImpl::SampleNeeded()
    {

	iCount++;
	if ((iCount % iMemSamplingPeriod) == 0 || (iCount % iMemSamplingPeriodDiv2) == 0)
	    {
		LOGTEXT("MemSamplerImpl::SampleNeeded - true1");
		iTimeToSample = true;
		return true;
        }
	else 
	    {
		return false;
        }

    }


TInt MemSamplerImpl::SampleImpl()
    {

	/*
	 *
	 *	EKA-2 implementation of MEM trace
	 *
	 */
    
    // check if either chunk or thread lists have unprocessed items
    if(this->iChunksProcessing == ENothingToProcess && !iSampleThreads)
        {
        if(!iTimeToSample)
            {
            return 0;
            }
        else
            {
            iTimeToSample = false;
            // gather first all chunks
            return GatherChunks();
            }
        }
    else if(!iSampleThreads)
        {
        // still something to go through in lists
        TInt length = this->ProcessChunks();
        
        if(length == 0) 
            {
            this->iChunksProcessing = ENothingToProcess;
            // switch to collect thread data
            iSampleThreads = true;
            }
        return length;
        }
    
    if(this->iThreadsProcessing == ENothingToProcess && iSampleThreads)
        {
        if(!iTimeToSample)
            {
            return 0;
            }
        else
            {
            iTimeToSample = false;
            // gather first all thread stacks
            return GatherThreads();
            }
        }
    
    else if(iSampleThreads)
        {
        // process now thread stack list
        TInt length = this->ProcessThreads();

        if(length == 0)
            {
            this->iThreadsProcessing = ENothingToProcess;
            // switch to collect chunk data
            iSampleThreads = false;
            }
        return length;
        }

    // should not reach this point...
    return 0;
    }

inline TInt MemSamplerImpl::GatherChunks()
    {
    // encode a process binary
    name.Zero();
    DObjectCon& chunks = *Kern::Containers()[EChunk];
    
    this->iChunkCount = 0; 
    this->iNewChunkCount = 0;
    this->iTotalMemoryOk = false;
    TInt totalChunkCount = chunks.Count();
    
    for(TInt i=0;i<totalChunkCount;i++)
        {
        DChunk* c = (DChunk*)(chunks)[i];

        LOGSTRING3("Processing chunk %d, tag: 0x%x",i,TAG(c));
        if( (TAG(c) & 0x0000ffff) != PROFILER_CHUNK_MARK)
            {
            LOGSTRING2("Marking chunk %d",i);
            // this chunk has not been tagged yet
            name.Zero();
            c->TraceAppendName(name,false);
            const TUint8* ptr = name.Ptr();
            
            TAG(c) = (PROFILER_CHUNK_MARK);
            this->heapChunkNamesToReport[iNewChunkCount] = c;
            iNewChunkCount++;
            }
        else
            {
            LOGSTRING3("Chunk %d marked already - 0x%x",i,TAG(c));
            }

        // the chunk has been tagged, add heap chunks to the list
        this->heapChunksToSample[this->iChunkCount] = c;
        this->iChunkCount++;
        LOGSTRING2("Added chunk %d to Chunks",i);
        }

    if(this->iChunkCount > 0 || this->iNewChunkCount > 0)
        {
        this->iChunksProcessing = EStartingToProcess;
        
        // process the first sample
        TInt length = this->ProcessChunks();
        
        if(length == 0)
            {
            this->iChunksProcessing = ENothingToProcess;
            }
    
        return length;
        }
    else
        {
        // there were no threads, should not take place
        LOGTEXT("MemSamplerImpl::SampleImpl - Error, no threads"); 
        return 0;
        }
    }

inline TInt MemSamplerImpl::GatherThreads()
    {
    // The thread memory consumption
    DObjectCon& threads = *Kern::Containers()[EThread];

    this->iThreadCount = 0; 
    this->iNewThreadCount = 0;
    this->iTotalMemoryOk = false;           

    TInt totalThreadCount = threads.Count();

    for(TInt i=0;i<totalThreadCount;i++)
        {
        DThread* t = (DThread*)(threads)[i];

        LOGSTRING3("Processing thread %d, tag: 0x%x",i,TAG(t));

        if( (TAG(t) & PROFILER_MEM_THREAD_MARK) == 0)
            {
            LOGSTRING2("Marking thread %d",i);
            // this thread's chunk has not been reported yet
            this->threadNamesToReport[iNewThreadCount] = t;
            iNewThreadCount++;
            // tag the thread
            TAG(t) |= PROFILER_MEM_THREAD_MARK;
            LOGSTRING2("New Thread %d",i);
            }
        else
            {
            LOGSTRING3("Thread %d marked already - 0x%x",i,TAG(t));
            }

        // the chunk has been tagged, add heap chunks to the list
        this->threadsToSample[this->iThreadCount] = t;
        this->iThreadCount++;
        LOGSTRING2("Added thread %d to threads to sample",i);
        }
    
    if(this->iThreadCount > 0 || this->iNewThreadCount > 0)
        {
        this->iThreadsProcessing = EStartingToProcess;
        
        // process the first sample
        TInt length = this->ProcessThreads();
        
        if(length == 0)
            {
            this->iThreadsProcessing = ENothingToProcess;
            }
    
        return length;
        }
    else
        {
        // there were no threads, should not take place
        LOGTEXT("MemSamplerImpl::SampleImpl - Error, no threads"); 
        return 0;
        }
    }

inline TInt MemSamplerImpl::ProcessChunks()
    {
    if(iNewChunkCount > 0)
        {
            
        if(this->iChunksProcessing == EStartingToProcess)
            {
            // this is the first sample, encode a code for names
            this->iChunksProcessing = EProcessingNames;
            return EncodeNameCode();
            }

        if(iTotalMemoryNameOk == false)
            {
            return EncodeTotalMemoryName();
            }
        
        // there are new chunk names to report
        iNewChunkCount--;
        DChunk* c = this->heapChunkNamesToReport[iNewChunkCount];
        return EncodeChunkName(*c);
        
        }
    else if(iChunkCount > 0)
        {
        if(this->iChunksProcessing == EProcessingNames || this->iChunksProcessing == EStartingToProcess)
            {
            // this is the first data sample, encode a code for data
            this->iChunksProcessing = EProcessingData;
            return EncodeDataCode();
            }
        
        if(this->iTotalMemoryOk == false)
            {
            return EncodeTotalMemory();	
            }

        // there are no new chunks to report
        // thus generate the real report
        iChunkCount--;
        DChunk* c = this->heapChunksToSample[iChunkCount];
        return EncodeChunkData(*c);
        }
    else
        {
        // everything is processed
        return 0;
        }
    }

inline TInt MemSamplerImpl::ProcessThreads()
    {

    if(iNewThreadCount > 0)
        {
        if(this->iThreadsProcessing == EStartingToProcess)
            {
            // this is the first sample, encode a code for names
            this->iThreadsProcessing = EProcessingNames;
            return EncodeNameCode();
            }
        
        if(iTotalMemoryNameOk == false)
            {
            return EncodeTotalMemoryName();
            }

        iNewThreadCount--;
        DThread* t = this->threadNamesToReport[iNewThreadCount];
        return EncodeChunkName(*t);

        
        }
    else if(iThreadCount > 0)
        {
        if(this->iThreadsProcessing == EProcessingNames || this->iThreadsProcessing == EStartingToProcess)
            {
            // this is the first data sample, encode a code for data
            this->iThreadsProcessing = EProcessingData;
            return EncodeDataCode();
            }

        if(this->iTotalMemoryOk == false)
            {
            return EncodeTotalMemory(); 
            }

        // there are no new threads to report
        // thus generate the real report
        iThreadCount--;
        DThread* t = this->threadsToSample[iThreadCount];
        return EncodeChunkData(*t);
        }
    else
        {   
        // everything is processed
        return 0;
        }
    }

inline TInt MemSamplerImpl::EncodeNameCode()
    {
	sample[0] = 1;
	sample[1] = 0xaa;
	return 2;
    }

inline TInt MemSamplerImpl::EncodeDataCode()
    {
	sample[0] = 1;
	sample[1] = 0xdd;
	return 2;
    }

inline TInt MemSamplerImpl::EncodeTotalMemoryName()
    {
	this->iTotalMemoryNameOk = true;
	
	TUint8* size = &sample[0];
	*size = 0;
		
	// encode name
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L("TOTAL_MEMORY"));
	*size += this->sampleDescriptor.Size();
		
	// add id here
	TUint32 id = 0xbabbeaaa;
	this->sampleDescriptor.Append((TUint8*)&(id),sizeof(TUint32));
	*size += sizeof(TUint32);
	
	// the size is the descriptor length + the size field
	return ((TInt)(*size))+1;	
    }

inline TInt MemSamplerImpl::EncodeTotalMemory()
    {	
	
	TUint8* size = &sample[0];
	*size = 0;

	NKern::LockSystem();
	TUint32 freeRam = Kern::FreeRamInBytes();
	TUint32 totalRam = Kern::SuperPage().iTotalRamSize;
	NKern::UnlockSystem();

	this->sampleDescriptor.Zero();
	
	TUint32 id = 0xbabbeaaa;
	TUint32 zero = 0;
		
	this->sampleDescriptor.Append((TUint8*)&(id),sizeof(TUint32));
	*size += sizeof(TUint);
	
	this->sampleDescriptor.Append((TUint8*)&(totalRam),sizeof(TInt));
	*size += sizeof(TInt);
		
	// append the cell amount allocated
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size
	this->sampleDescriptor.Append((TUint8*)&(freeRam),sizeof(TUint));
	*size += sizeof(TUint);
		
	// append the thread user stack size
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);

	this->iTotalMemoryOk = true;

	return ((TInt)(*size))+1;
    }

inline TInt MemSamplerImpl::EncodeChunkName(DChunk& c)
    {	
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
		
	// encode chunk name
	this->sampleDescriptor.Zero();
	this->sampleDescriptor.Append(_L("C_"));
	c.TraceAppendFullName(this->sampleDescriptor,false);
	*size += this->sampleDescriptor.Size();
		
	// add chunk object address here
	TUint32 chunkAddr = (TUint32)&c;
	this->sampleDescriptor.Append((TUint8*)&(chunkAddr),sizeof(TUint32));
	*size += sizeof(TUint32);

	// the size is the descriptor length + the size field
	LOGSTRING2("Non-Heap Chunk Name - %d",*size);
	return ((TInt)(*size))+1;			
    }

inline TInt MemSamplerImpl::EncodeChunkName(DThread& t)
    {		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();
	
	this->sampleDescriptor.Append(_L("T_"));
	t.TraceAppendFullName(this->sampleDescriptor,false);
	*size += this->sampleDescriptor.Size();
	
	// copy the 4 bytes from the thread id field
	this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
	*size += sizeof(TUint);

	// the size is the descriptor length + the size field
	LOGSTRING2("Name - %d",*size);
	return ((TInt)(*size))+1;
    }

inline TInt MemSamplerImpl::EncodeChunkData(DChunk& c)
    {
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();
	TInt zero(0);

	TUint32 address = (TUint32)&c;
		
	this->sampleDescriptor.Append((TUint8*)&address,sizeof(TUint32));
	*size += sizeof(TUint);
	
	// copy the total amount of memory allocated
	this->sampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TInt));
	*size += sizeof(TInt);
		
	// append the cell amount allocated
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size
	this->sampleDescriptor.Append((TUint8*)&(c.iSize),sizeof(TUint));
	*size += sizeof(TUint);
		
	// append the thread user stack size
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TInt));
	*size += sizeof(TInt);

	LOGSTRING2("Data - %d",*size);
	return ((TInt)(*size))+1;

    }

inline TInt MemSamplerImpl::EncodeChunkData(DThread& t)
    {
	LOGTEXT("MemSamplerImpl::EncodeChunkData - entry");
	//LOGSTRING2("MemSamplerImpl::EncodeChunkData - processing thread 0x%x ",&t);
		
	// the size of the following name is in the first byte
	TUint8* size = &sample[0];
	*size = 0;
	this->sampleDescriptor.Zero();

	LOGTEXT("MemSamplerImpl::EncodeChunkData - cleared");

	this->sampleDescriptor.Append((TUint8*)&(t.iId),sizeof(TUint));
	*size += sizeof(TUint);
		
	// copy the total amount of memory allocated for user side stack
	this->sampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
	*size += sizeof(TInt);

	TInt zero = 0;		
	// append the cell amount allocated (zero, not in use here)
	this->sampleDescriptor.Append((TUint8*)&zero,sizeof(TInt));
	*size += sizeof(TInt);
	
	// append the chunk size (this is not a chunk)
	this->sampleDescriptor.Append((TUint8*)&(zero),sizeof(TUint));
	*size += sizeof(TUint);
		
	/*
		// for testing precise stack utlilsation tracing...
		// crashes at the moment

		// append the current status of thread's stack utilisation
		// this equals (stack pointer - stack base address)
		TUint32* sp=(TUint32*)t.iNThread.iSavedSP;
		TUint aUsrSP=sp[SP_R13U];
		//TInt value = aUsrSP-t.iUserStackRunAddress;
		this->sampleDescriptor.Append((TUint8*)&(aUsrSP),sizeof(TInt));
		*size += sizeof(TInt);
	*/
	
	// append user stack (max) size
	this->sampleDescriptor.Append((TUint8*)&(t.iUserStackSize),sizeof(TInt));
	*size += sizeof(TInt);

	LOGSTRING2("Data -> %d",*size);
	return ((TInt)(*size))+1;
    }

void MemSamplerImpl::Reset()
    {
	LOGTEXT("MemSamplerImpl::Reset");
	iCount = 0; // sample threads 1 cycle after actual MEM sample time...
	this->iSampleThreads = true;
    this->iTimeToSample = false;
    this->iChunkCount = 0;
	this->iNewChunkCount = 0;

	this->iTotalMemoryOk = false;
	this->iTotalMemoryNameOk = false;

	this->iChunksProcessing = ENothingToProcess;
    this->iThreadsProcessing = ENothingToProcess;
	this->sampleDescriptor.Zero();

	// clear all chunk tags
	DObjectCon* chunks = Kern::Containers()[EChunk];
	TInt totalChunkCount = chunks->Count();
	for(TInt i=0;i<totalChunkCount;i++)
	    {
		DChunk* c = (DChunk*)(*chunks)[i];
		TAG(c) = 0;
	    }

	LOGTEXT("MemSamplerImpl::Reset");
	this->iThreadCount = 0;
	this->iNewThreadCount = 0;
	this->sampleDescriptor.Zero();

	// clear all chunk tags
	DObjectCon* threads = Kern::Containers()[EThread];
	TInt totalThreadCount = threads->Count();
	for(TInt i=0;i<totalThreadCount;i++)
	    {
		DThread* t = (DThread*)(*threads)[i];
		TAG(t) = (TAG(t) & 0xfffffffe);
	    }
    }

