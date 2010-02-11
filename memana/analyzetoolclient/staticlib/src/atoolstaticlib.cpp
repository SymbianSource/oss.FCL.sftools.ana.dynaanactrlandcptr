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
* Description:  The file contains declarations of extern methods and definitions
*                of overloaded User methods.
*
*/



#include <analyzetool/customuser.h>
#include "atlog.h"

// -----------------------------------------------------------------------------
// LogFileName()
// Extern function for acquiring the log filename
// -----------------------------------------------------------------------------
// 
GLREF_C const TFileName LogFileName();

// -----------------------------------------------------------------------------
// LogOption()
// Extern function for acquiring logging option
// -----------------------------------------------------------------------------
// 
GLREF_C TUint32 LogOption();

// -----------------------------------------------------------------------------
// IsDebug()
// Extern function for acquiring UDEB/UREL information
// -----------------------------------------------------------------------------
// 
GLREF_C TUint32 IsDebug();

// -----------------------------------------------------------------------------
// AtoolVersion()
// Extern function for acquiring the version number
// -----------------------------------------------------------------------------
// 
GLREF_C const TFileName AtoolVersion();

// -----------------------------------------------------------------------------
// AllocCallStackSize()
// Extern function for acquiring max allocation call stack size
// -----------------------------------------------------------------------------
// 
GLREF_C TUint32 AllocCallStackSize();

// -----------------------------------------------------------------------------
// FreeCallStackSize()
// Extern function for acquiring max free call stack size
// -----------------------------------------------------------------------------
// 
GLREF_C TUint32 FreeCallStackSize();

// -----------------------------------------------------------------------------
// User::Exit()
// Overloaded User::Exit() function
// -----------------------------------------------------------------------------
// 
void User::Exit( TInt aReason )
    {   
    LOGSTR2( "ATMH User::Exit %i", aReason ); 
    CustomUser::Exit( aReason );
    }

// -----------------------------------------------------------------------------
// User::Panic()
// Overloaded User::Panic() function
// -----------------------------------------------------------------------------
//
void User::Panic( const TDesC& aCategory, TInt aReason )
    {
    LOGSTR3( "ATMH User::Panic() %S %i", &aCategory, aReason );
    CustomUser::Panic( aCategory, aReason );
    }
 
// -----------------------------------------------------------------------------
// User::SetCritical()
// Overloaded User::SetCritical() function which returns
// KErrNone, if successful; KErrArgument, if EAllThreadsCritical is 
// passed - this is a state associated with a process, and you use 
// User::SetProcessCritical() to set it.
// -----------------------------------------------------------------------------
// 
TInt User::SetCritical( TCritical aCritical )
    {
    LOGSTR2( "ATMH User::SetCritical() %i", aCritical );
    return CustomUser::SetCritical( aCritical );
    }

// -----------------------------------------------------------------------------
// User::SetProcessCritical()
// Overloaded User::SetProcessCritical() function
// KErrNone, if successful; KErrArgument, if either EProcessCritical or 
// EProcessPermanent is passed - these are states associated with a 
// thread, and you use User::SetCritical() to set them.
// -----------------------------------------------------------------------------
// 
TInt User::SetProcessCritical( TCritical aCritical )
    {
    LOGSTR2( "ATMH User::SetProcessCritical() %i", aCritical );
    return CustomUser::SetProcessCritical( aCritical );
    }
    
// -----------------------------------------------------------------------------
// UserHeap::SetupThreadHeap()
// Overloaded UserHeap::SetupThreadHeap function
// -----------------------------------------------------------------------------
//  
TInt UserHeap::SetupThreadHeap( TBool aNotFirst, 
                                SStdEpocThreadCreateInfo& aInfo )
    {
    LOGSTR1( "ATMH UserHeap::SetupThreadHeap()" );
    
    return CustomUser::SetupThreadHeap( aNotFirst, aInfo, LogFileName(), 
    		LogOption(), IsDebug(), AtoolVersion(), AllocCallStackSize(), FreeCallStackSize() );
    }
  
// End of File
