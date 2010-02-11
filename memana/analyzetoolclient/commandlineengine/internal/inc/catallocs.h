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
* Description: Class representing collection of memory allocations
*              in external data gathering.
*/

#ifndef __CATALLOCS_H__
#define __CATALLOCS_H__

#include "../inc/ATCommonDefines.h"
#include "../inc/CATBase.h"
#include "../inc/catalloc.h"

// Memory allocates and frees handling class.
class CATAllocs : public CATBase
{
public:
	
	/**
	* "old" alloc message (prior to 1.6).
	* @param sAllocString allocation string.
	*/
	void Alloc( const string& sAllocString );

	/*
	* Alloc header message (multimessage call stack).
	* @param sAllocHString allocation string.
	*/
	void AllocH( const string& sAllocHString );

	/*
	* Alloc fragment message (fragment of call stack in multimessage alloc).
	* @param sAllocFString allocation string.
	*/
	void AllocF( const string& sAllocFString );

	/**
	* Free message.
	* @param sFreeString string.
	*/
	void Free( const string& sFreeString );
	
	/**
	* Free header message.
	* (not implemented yeat.)
	* @param sFreeHString string.
	*/
	void FreeH( const string& sFreeHString );

	/**
	* Free fragment message.
	* (not implemented yeat.)
	* @param sFreeFString string.
	*/
	void FreeF( const string& sFreeFString );

	/**
	* Get "leak" list ordered by allocation time.
	* @param vLeaklist container where leaks stored.
	*/
	void GetLeakList( vector<string>& vLeakList);
	
	/**
	* Clear leaks.
	*/
	void ClearAllocs( void );
#ifndef MODULE_TEST
private:
#endif
	// Allocation storage.
	map<string, CATAlloc> m_vAllocs;
};
#endif
//EOF
