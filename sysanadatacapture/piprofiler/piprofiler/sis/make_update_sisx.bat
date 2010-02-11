rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off

echo Making partial update (PU) SISX installation packages for ProfilerEngine

call makesis -v PIProfiler_update.pkg
call signsis -v -s PIProfiler_update.sis PIProfiler.sisx ..\..\..\cert\RDTest_02.der ..\..\..\cert\RDTest_02.key

dir .\
