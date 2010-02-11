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
* Description:  Class representing a memory address and its details.
*
*/


#ifndef __CATMEMORYADDRESS_H__
#define __CATMEMORYADDRESS_H__

#include "../inc/ATCommonDefines.h"

class CATMemoryAddress
{
public:
	// Enumeration representing the state of locating code lines.
	enum ADDRESS_TO_LINE_STATE
	{
		OUT_OF_PROCESS = 0, /* Not located code line.*/
		OUT_OF_RANGE, /* Outside functions range. */
		SYMBOL, /* Symbol/ Function located (no codeline) */
		FUNCTION, /* Function and line number.*/
		EXACT /* Exact code line located with all information.*/
	};

	/**
	* Constructor
	* @param sAddress
	* @param iOffSet value used if need to use offset value
	*/
	CATMemoryAddress( string& sAddress, unsigned long iOffSet );

	/**
	* Destructor
	*/
	virtual ~CATMemoryAddress();

	/**
	* Find which binary this address belongs to.
	* Sets also the offsetfrommodulestart.
	* @param vDlls container of binarys to find from.
	* @return true if found
	*/
	bool FindSetModuleName(vector<DLL_LOAD_INFO>* vDlls);

	/**
	* Get vector index to which module we found this address to belong to.
	* @return -1 if not set.
	*/
	int GetDllLoadInfoIndex();

	/**
	* Note return value includes the set offset.
	* So this value is not binary start - address.
	* Instead it is.
	* memory address - binary start address + offset
	* @return adress
	*/
	unsigned long GetOffSetFromModuleStart();

	/**
	* Get the binary start address
	* @return binary start address
	*/
	unsigned long GetModuleStartAddress() const;
	

	/**
	* Simple get and sets for member variables.
	*/
	void SetTime( unsigned long long& ullTime );
	unsigned long long GetTime();
	void SetAddress( string& sAddress );
	string GetAddressString();
	void SetAddress( unsigned long iAddress );
	unsigned long GetAddress();
	void SetModuleName( string& sModuleName );
	string GetModuleName();
	void SetAddressToLineState( ADDRESS_TO_LINE_STATE eState );
	int GetAddressToLineState();
	void SetFileName(string& sFileName);
	string GetFileName();
	void SetFunctionName(string& sFunctionName);
	string GetFunctionName();
	void SetFunctionLineNumber(int iFunctionLineNumber);
	int GetFunctionLineNumber();
	void SetExactLineNumber(int iExactLineNumber);
	int GetExactLineNumber();
	void SetModuleStartAddress(unsigned long iAddress);

#ifndef MODULE_TEST
private:
#endif
	// Used offset to add to addresses
	unsigned long m_iOffSet;
	// Address related members
	string m_sAddress;
	unsigned long m_iAddress;

	// Time (microseconds from 1970)
	unsigned long long m_iTime;

	// Module related members(if FindSetModule is successful)
	string m_sModuleName;
	unsigned long m_iOffSetFromModuleStart;
	unsigned long m_iModuleStartAddress;

	// Module to which address belong.
	int m_iDllLoadinfoIndex;

	// Pin pointing related members
	int m_ePinPointState;
	string m_sFileName;
	string m_sFunctionName;

	int m_iFunctionLineNumber;
	int m_iExactLineNumber;
};
#endif