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
* Description: Class representing memory allocation in
*               external data gathering.
*
*/


#ifndef __CATALLOC_H__
#define __CATALLOC_H__

#include "../inc/ATCommonDefines.h"

class CATAlloc
{
public:
	/**
	* Constructor.
	*/
	CATAlloc() : m_iCSCount(0) {}

	/**
	* Destructor.
	*/
	~CATAlloc(){}

	// Allocation time.
	string m_sTime;

	// Allocation size.
	string m_sSize;

	// Call stack address count.
	unsigned long m_iCSCount;

	// Call stack where packet number is key.
	map<unsigned long,string> m_vCallStack;

	// Get alloc string after address ( time, size, call stack).
	string GetAllocString( void );
};
#endif
//EOF
