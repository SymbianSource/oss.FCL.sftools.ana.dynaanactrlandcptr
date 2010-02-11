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
* Description:  Definitions for class CATParseTrace.
*
*/


#ifndef __CATPARSETRACEFILE_H__
#define __CATPARSETRACEFILE_H__

#include "../inc/ATCommonDefines.h"
#include "../inc/CATDataSaver.h"
#include "../inc/CATBase.h"
#include "../inc/catallocs.h"

/**
* Class that parses trace data to data file.
*/
class CATParseTraceFile : public CATBase
{
public:
	CATParseTraceFile();

	/**
	* Main function to start parse.
	*
	* @param pFileName A trace file name.
	* @param pOutputFileName Pointer to output file name.
	*/
	bool StartParse( const char* pFileName, const char* pOutputFileName );

	CATDataSaver* GetDataSaver(void);	

#ifndef MODULE_TEST
private:
#endif
	CATDataSaver m_DataSaver;
};

class CSubTestData : public CATAllocs
{
public:
	bool bRunning;
	vector<string> vData;
	string sSubTestName;
	string sStartTime;
	string sEndTime;
	string sSubTestStartHandleCount;
	string sSubTestEndHandleCount;
};

class CProcessData : public CATAllocs
{
public:
	bool bProcessOnGoing;
	int iProcessID;
	vector<string> vData;
	vector<string> vHandleLeaks;
	vector<CSubTestData> vSubTests;
};

#endif
