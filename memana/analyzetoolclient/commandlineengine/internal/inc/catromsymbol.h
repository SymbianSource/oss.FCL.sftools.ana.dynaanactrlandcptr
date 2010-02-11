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
* Description:  Locates symbols/functions for memory addresses
*               which are located in given rom/rofs symbol file.
*
*/


#ifndef __CATROMSYMBOL_H__
#define __CATROMSYMBOL_H__

#include "ATCommonDefines.h"
#include "iaddresstoline.h"

const string ROM_SYMBOL_IDENTIFY_STRING = "80";
const string ROFS_SYMBOL_IDENTIFY_STRING = "00";
const int IDENTIFY_MAX_LINES_READ = 20;

// Symbol structure.
struct Symbol
{
	// Start address.
	unsigned long iStartAddress;
	// End address.
	unsigned long iEndAddress;
	// Function/Symbol name.
	string sFunction;
	// Default constructor for structure.
	Symbol() { iStartAddress=0; iEndAddress=0; sFunction = ""; }
	~Symbol() {}
};

// Rofs binary item class
class RofsBinary {
public:
	string m_sBinary;
	vector<Symbol*> vSymbols;
	RofsBinary();
	RofsBinary( const string& sbinary );
	~RofsBinary();
};


class CATRomSymbol : public IAddressToLine
{
public:
	/**
	* Constructor.
	*/
	CATRomSymbol();
	/**
	* Destructor.
	*/
	virtual ~CATRomSymbol();
	/**
	* "Flag" will we show progress when reading files.
	*/
	bool m_bShowProgressMessages;
	/**
	* Empty functions does nothing returns false always.
	* @return true if successful.
	*/
	bool Open( const string& sString, const unsigned long iLong);
	/**
	* Set symbol files.
	* This also checks that files exists and identifies them as rom/rofs.
	* @vSymbols.
	*/
	bool SetSymbols( const vector<string>& vSymbols);
	/**
	* Get error string. In case of any method failed use this to acquire details on error.
	* @return error string.
	*/
	string GetError( void );
	/**
	* Close rom symbol file.
	* @return true if succesful.
	*/
	bool Close( void );
	/**
	* Locates symbol and binary name for given address if found in rom.
	* @result memory address object.
	* @return true if successful.
	*/
	bool AddressToLine( CATMemoryAddress* result );
#ifndef MODULE_TEST
private:
#endif
	enum SYMBOL_FILE_TYPE {
		SYMBOL_FILE_INVALID = 0,
		SYMBOL_FILE_ROM,
		SYMBOL_FILE_ROFS
	};
	// Identify symbol file
	int IdentifySymbolFile( const string& sFile );
	// Locate symbol and binary name for given address if found in rom.
	bool AddressToLineRom( CATMemoryAddress* result );
	// Locate symbol and binary name for given address if found in rofs.
	bool AddressToLineRofs( CATMemoryAddress* result );
	// Reads rom file.
	bool ReadRomFiles();
	bool ReadRomFile( const string& sFile );
	// Read rofs file.
	bool ReadRofsFiles();
	bool ReadRofsFile( const string& sFile );
	// Parse symbol from a line in rom/rofs file.
	void ParseSymbolFromLine( const string& sLine, Symbol* pSymbol );
#ifndef MODULE_TEST
private:
#endif
	// Have we identified symbol file(s).
	bool m_bFilesIdentified;
	// Have we read symbol file(s).
	bool m_bSymbolsRead;

	// Rom symbol file(s).
	vector<string> m_vRomFiles;
	// Cached rom symbols.
	vector<Symbol*> m_vRomCache;
	// All rom symbols.
	vector<Symbol*> m_vRomSymbols;
	// Rom start address.
	unsigned long m_iRomStartAddress;
	// Rom end address.
	unsigned long m_iRomEndAddress;

	// Rofs symbol file(s)
	vector<string> m_vRofsFiles;
	// Rofs binaries
	vector<RofsBinary*> m_vRofsBinaries;

	// Error message.
	string m_sErrorMessage;
};
#endif