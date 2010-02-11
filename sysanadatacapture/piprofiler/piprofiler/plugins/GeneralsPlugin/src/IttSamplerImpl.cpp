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

#include <kern_priv.h>
#include <plat_priv.h>

#include "IttSamplerImpl.h"

/*
 *	
 *	ITT sampler definition
 *	
 */

IttSamplerImpl::IttSamplerImpl()
{
	this->ptrToData = new TPtr8(((TUint8*)&(this->internalData[1])),255);
	this->currentLibCount = 0;
	this->currentProcCount = 0;
	this->iSampleProcesses = false;

	this->Reset();
}

IttSamplerImpl::~IttSamplerImpl()
{
	delete(this->ptrToData);
}

TInt IttSamplerImpl::CreateFirstSample() 
{				
	this->iVersionData.Zero();
	this->iVersionData.Append(_L8("Bappea_ITT_V"));
	this->iVersionData.Append(PROFILER_ITT_SAMPLER_VERSION);
	this->itt_sample = (TUint8*)iVersionData.Ptr();
	return iVersionData.Length();
}

TBool IttSamplerImpl::SampleNeeded(TUint32 sampleNum)
{
	return true;
}

TInt IttSamplerImpl::SampleImpl(TUint32 pc,TUint32 sampleNum)
{	
	// in order to avoid overloading the interrupt
	// only one dynamic file in each 50ms is added to the stream
	// with the application of the tool in mind, this is
	// a reasonable measure
	
//	#ifdef ITT_TEST
	//if(iSampleProcesses)
	{
		// encode a process binary
		ptrToData->Zero();
		
		//static RPointerArray<DCodeSeg> array = DCodeSeg::CodeSegsByName;
		//TInt arrayCount = array.Count();
		
		/* mietint‰‰...
		
		DCodeSeg* pSeg = Kern::CodeSegFromAddress( (TLinAddr)aPtr, TheCurrentThread->iOwningProcess );
		if (!pSeg)
			{
	 		return KErrNotFound;
			}
		Kern::KUDesPut(aModuleName, *pSeg->iFileName);

		// lis‰‰ pohdintaa
		
		TInt n=0;
		TInt i;
		TInt c=iDynamicCode.Count();	// DProcess.iDynamicCode, type = RArray<SCodeSegEntry>, SCodeSegEntry->iSeg/iLib
		for (i=-1; i<c; ++i)
		{
			DCodeSeg* pS=(i<0)?iCodeSeg:iDynamicCode[i].iSeg;
			if (pS && pS!=aExclude)
			{
				if ((aFlags & ETraverseFlagRestrict) && i>=0)
				{
					DLibrary* pL=iDynamicCode[i].iLib;
					if (pL && pL->iState!=DLibrary::EAttached)
						continue;
				}
				if (aFlags & ETraverseFlagAdd)
					n+=pS->ListDeps(aQ,aMark);
				else
					n+=pS->UnListDeps(aMark);
			}
		}
		

		*/

/*		
		// testi...
		// encode a process binary
		//Kern::Printf("Get Processes from container...");
		DObjectCon* proc = Kern::Containers()[EProcess];
		TInt procCount = proc->Count();

		//Kern::Printf("Got count of processes: %d", procCount);
		
		// go 20 executables through at a time
		for(TInt i=0;i<procCount;i++)
		{
			DProcess* pro = (DProcess*)(*proc)[i];
	
			//Kern::Printf("Inspecting DProcess array...");

			TInt c=pro->iDynamicCode.Count();	// DProcess.iDynamicCode, type = RArray<SCodeSegEntry>, SCodeSegEntry->iSeg/iLib

			//Kern::Printf("DProcess: count of segments %d", c);
			for (TInt i=-1; i<c; ++i)
			{
				DCodeSeg* seg=(i<0)?pro->iCodeSeg:pro->iDynamicCode[i].iSeg;
				if(seg != 0)
				{
					if( (seg->iMark & 0x80) == 0)
					{
						this->internalData[0] = seg->iFileName->Length();
						ptrToData->Append(*(seg->iFileName));
						ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
						ptrToData->Append((TUint8*)&(seg->iSize),4);
						//Kern::Printf("EXE1: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
						seg->iMark = (seg->iMark | 0x80);
						
						this->internalData[0] = ptrToData->Size();
						return ptrToData->Size()+1;
					}
				}
			}
		}
*/		
/*
		
		DObjectCon* libs = Kern::Containers()[ELibrary];
		TInt libCount = libs->Count();

		//Kern::Printf("Got count of processes: %d", procCount);
		
		// go 20 executables through at a time
		for(TInt i=0;i<libCount;i++)
		{
			//Kern::Printf("Inspecting DLibrary array...");

			DLibrary* lib = (DLibrary*)(*libs)[i];
			
			DCodeSeg* seg = lib->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					Kern::Printf("DLL: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}		
*/
	// original 
	if((sampleNum % 20) != 0) return 0;
	if((sampleNum % 40) == 0)
	{
/*
	if((sampleNum % 60) == 0)
	{
		// encode a process binary
		ptrToData->Zero();
		DObjectCon* proc = Kern::Containers()[EProcess];
		TInt procCount = proc->Count();
		
		// go 20 executables through at a time
		for(TInt i=0;i<20;i++)
		{
			if(currentProcCount >= procCount) 
			{
				currentProcCount = 0;
			}
			
			DProcess* pro = (DProcess*)(*proc)[currentProcCount];
			currentProcCount++;
			
			DCodeSeg* seg = pro->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("EXE: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}
	}	else  
	{ */
		// encode a library binary
		ptrToData->Zero();
		DObjectCon* libs = Kern::Containers()[ELibrary];
		TInt libCount = libs->Count();
		
		// go 20 binaries through at a time
		for(TInt i=0;i<20;i++)
		{
			if(currentLibCount >= libCount)
			{
				currentLibCount = 0;
			}
			
			DLibrary* lib = (DLibrary*)(*libs)[currentLibCount];
			currentLibCount++;
			
			DCodeSeg* seg = lib->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("DLL: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}
	} else
	{
		SDblQue* codeSegList = Kern::CodeSegList();
		//Kern::Printf("PI");
		//TUint c = 0;
		// the global list
		for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
		{				
			DCodeSeg* seg = _LOFF(codeseg, DCodeSeg, iLink);
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("EXE2: NM %S : RA:0x%x SZ:0x%x, time: %d",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}	
	}

/*
		// the garbage list 
		for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
		{				
			DCodeSeg* seg = _LOFF(codeseg,DCodeSeg, iGbgLink);
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("EXE2: NM %S : RA:0x%x SZ:0x%x, time: %d",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}	
*/
		
		// Another try
//	Kern::AccessCode();
	
	//Collect all non-XIP segments that are not already marked.
/*
	SDblQue* p = Kern::CodeSegList();
	SDblQueLink* anchor=&p->iA;
	SDblQueLink* a=anchor->iNext;
	for (; a!=anchor; a=a->iNext) 
	{
		DEpocCodeSeg* pSeg = (DEpocCodeSeg*) _LOFF(a, DCodeSeg, iLink);
		if (pSeg != 0)
		{
			if (pSeg->iXIP || pSeg->iMark&DCodeSeg::EMarkProfilerTAG)
			{
					//continue;
				//if (current > (max-KMaxCreateCodeSegRecordSize))
				//	break;//No more space. Finish now and wait for another GetSegments request.
					
				pSeg->iMark |= DCodeSeg::EMarkProfilerTAG;	//Mark this segment
				this->internalData[0] = pSeg->iFileName->Length();
				ptrToData->Append(*(pSeg->iFileName));
				ptrToData->Append((TUint8*)&(pSeg->iRunAddress),4);
				ptrToData->Append((TUint8*)&(pSeg->iSize),4);
				//Kern::Printf("EXE2: NM %S : RA:0x%x SZ:0x%x, time: %d",seg->iFileName,seg->iRunAddress,seg->iSize, sampleNum);
				
				this->internalData[0] = ptrToData->Size();
				return ptrToData->Size()+1;
			}
		}
	}
*/
	
//	Kern::EndAccessCode();		
		
		//Kern::Printf("PO");

/*		
		for(TInt i=0;i<arrayCount;i++)
		{
			currentProcCount++;
			DCodeSeg* seg = (DCodeSeg*)array[i];
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("EXE: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}
*/
	}
/*
	#else
	if((sampleNum % 50) != 0) return 0;
	if((sampleNum % 100) == 0)
	{
		// encode a process binary
		ptrToData->Zero();
		DObjectCon* proc = Kern::Containers()[EProcess];
		TInt procCount = proc->Count();
		
		// go 20 executables through at a time
		for(TInt i=0;i<20;i++)
		{
			if(currentProcCount >= procCount) 
			{
				currentProcCount = 0;
			}
			
			DProcess* pro = (DProcess*)(*proc)[currentProcCount];
			currentProcCount++;
			
			DCodeSeg* seg = pro->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("EXE: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}
	}
	else
	{
		// encode a library binary
		ptrToData->Zero();
		DObjectCon* libs = Kern::Containers()[ELibrary];
		TInt libCount = libs->Count();
		
		// go 20 binaries through at a time
		for(TInt i=0;i<20;i++)
		{
			if(currentLibCount >= libCount)
			{
				currentLibCount = 0;
			}
			
			DLibrary* lib = (DLibrary*)(*libs)[currentLibCount];
			currentLibCount++;
			
			DCodeSeg* seg = lib->iCodeSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) == 0)
				{
					this->internalData[0] = seg->iFileName->Length();
					ptrToData->Append(*(seg->iFileName));
					ptrToData->Append((TUint8*)&(seg->iRunAddress),4);
					ptrToData->Append((TUint8*)&(seg->iSize),4);
					//Kern::Printf("DLL: NM %S : RA:0x%x SZ:0x%x",seg->iFileName,seg->iRunAddress,seg->iSize);
					seg->iMark = (seg->iMark | 0x80);
					
					this->internalData[0] = ptrToData->Size();
					return ptrToData->Size()+1;
				}
			}
		}
	}
	#endif
*/	
	return 0;

}

void IttSamplerImpl::Reset()
{
	this->currentLibCount = 0;
	this->currentProcCount = 0;
	this->itt_sample = (TUint8*)&(this->internalData[0]);
	ptrToData->Zero();

//	#ifdef ITT_TEST	

	SDblQue* codeSegList = Kern::CodeSegList();
	// the global list
	for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
	{				
		DCodeSeg* seg = _LOFF(codeseg,DCodeSeg, iLink);
		//if(seg != 0)
		{
			if( (seg->iMark & 0x80) > 0)
			{
				seg->iMark = (seg->iMark & ~0x80);
			}
		}
	}	

	// the garbage list
/*
	for (SDblQueLink* codeseg= codeSegList->First(); codeseg!=(SDblQueLink*) codeSegList; codeseg=codeseg->iNext)
	{				
		DCodeSeg* seg = _LOFF(codeseg,DCodeSeg, iGbgLink);
		if(seg != 0)
		{
			if( (seg->iMark & 0x80) > 0)
			{
				seg->iMark = (seg->iMark & ~0x80);
			}
		}
	}
*/		
// another try
/*
//	Kern::AccessCode();
	SDblQue* p = Kern::CodeSegList();
	SDblQueLink* anchor=&p->iA;
	SDblQueLink* a=anchor->iNext;
	for (; a!=anchor; a=a->iNext) 
		{
		DEpocCodeSeg* pSeg = (DEpocCodeSeg*) _LOFF(a, DCodeSeg, iLink);
		if (!pSeg->iXIP)
			pSeg->iMark &= ~DCodeSeg::EMarkProfilerTAG;
		}

//	Kern::EndAccessCode();
*/
/*
	DObjectCon* proc = Kern::Containers()[EProcess];
	TInt procCount = proc->Count();
	
	// go 20 executables through at a time
	for(TInt i=0;i<procCount;i++)
	{
		DProcess* pro = (DProcess*)(*proc)[i];

		//Kern::Printf("Reset: Inspecting DProcess...");

		TInt c=pro->iDynamicCode.Count();	// DProcess.iDynamicCode, type = RArray<SCodeSegEntry>, SCodeSegEntry->iSeg/iLib

		//Kern::Printf("Reset: DProcess: count of segments %d", c);
		for (TInt i=-1; i<c; ++i)
		{
			DCodeSeg* seg=(i<0)?pro->iCodeSeg:pro->iDynamicCode[i].iSeg;
			if(seg != 0)
			{
				if( (seg->iMark & 0x80) > 0)
				{
					seg->iMark = (seg->iMark & ~0x80);
				}
			}
		}
	}
	
	//Kern::Printf("Reset: Exiting...");

	#else
*/
	DObjectCon* libs = Kern::Containers()[ELibrary];
	TInt libCount = libs->Count();
	for(TInt i=0;i<libCount;i++)
	{
		DLibrary* lib = (DLibrary*)(*libs)[i];
		DCodeSeg* seg = lib->iCodeSeg;
		if( (seg->iMark & 0x80) > 0)
		{
			seg->iMark = (seg->iMark & ~0x80);
		}
	}
	
	DObjectCon* procs = Kern::Containers()[EProcess];
	TInt procCount = procs->Count();
	for(TInt i=0;i<procCount;i++)
	{
		DProcess* pro = (DProcess*)(*procs)[i];
		DCodeSeg* seg = pro->iCodeSeg;
		if(seg != 0)
		{
			if( (seg->iMark & 0x80) > 0)
			{
				seg->iMark = (seg->iMark & ~0x80);
			}
		}
	}		
	//#endif
}
