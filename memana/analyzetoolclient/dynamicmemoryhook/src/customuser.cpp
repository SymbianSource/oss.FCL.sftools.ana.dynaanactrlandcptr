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
* Description:  Definitions for the class CustomUser.
*
*/

#include <f32file.h>
#include <utf.h>
#include "customuser.h"
#include "analyzetoolmainallocator.h"
#include "analyzetoolallocator.h"
#include "atlog.h"
#include "analyzetoolmemoryallocator.h"
#include "analyzetoolpanics.pan"
#include "atstorageservercommon.h"
#include "atdriveinfo.h"

// CONSTANTS
// When needed, update the version number directly inside _LIT macro.
// Constant for the atool API(staticlib) version.
_LIT( KAtoolApiVersion, "1.7.4" );

// Version number buffer length
const TInt KAtoolVersionNumberLength = 10;

// Wrong version error code
const TInt KAtoolVersionError = -1999;

// Version number separator
_LIT( KVersionSeparator, ";" );

// Incorrect version error strings 
_LIT8( KDataFileVersion, "DATA_FILE_VERSION 10\r\n" );
_LIT( KIncorrectText, "ERROR_OCCURED INCORRECT_ATOOL_VERSION [API v.%S][ATOOL v.%S]" );
_LIT( KIncorrectTextXti, "PCSS " );

// -----------------------------------------------------------------------------
// CustomUser::Panic()
// Overloaded User::Panic() function
// -----------------------------------------------------------------------------
//
EXPORT_C void CustomUser::Panic( const TDesC& aCategory, TInt aReason )
    {
    LOGSTR3( "ATMH CustomUser::Panic() %S %i", &aCategory, aReason );
    
    // Uninstall thread's RAllocator
    ( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
       
    // Call the "real" User::Panic()
    User::Panic( aCategory, aReason );
    }

// -----------------------------------------------------------------------------
// CustomUser::Exit()
// Overloaded User::Exit() function
// -----------------------------------------------------------------------------
//
EXPORT_C void CustomUser::Exit( TInt aReason )
    {
    LOGSTR3( "ATMH CustomUser::Exit() %i %i", aReason, RThread().Id().Id() );
    
    if ( aReason != KAtoolVersionError )
    	{
    	// Uninstall thread's RAllocator
    	( (RAnalyzeToolMemoryAllocator&) User::Allocator() ).Uninstall();
    	LOGSTR1( "ATMH CustomUser::Exit() - about to User::Exit" );
    	}
    
    // Call the "real" User::Exit()
    User::Exit( aReason );
    }

// -----------------------------------------------------------------------------
// CustomUser::SetCritical()
// Overloaded User::SetCritical() function which returns
// KErrNone, if successful; KErrArgument, if EAllThreadsCritical is 
// passed - this is a state associated with a process, and you use 
// User::SetProcessCritical() to set it.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetCritical( User::TCritical aCritical )
    {
    LOGSTR1( "ATMH CustomUser::SetCritical()" );
    // Check the given User::TCritical type
    if ( aCritical == User::EAllThreadsCritical )
        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }
    }
  
// -----------------------------------------------------------------------------
// CustomUser::SetProcessCritical()
// Overloaded User::SetProcessCritical() function
// KErrNone, if successful; KErrArgument, if either EProcessCritical or 
// EProcessPermanent is passed - these are states associated with a 
// thread, and you use User::SetCritical() to set them.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetProcessCritical( User::TCritical aCritical )
    {
    LOGSTR1( "ATMH CustomUser::SetProcessCritical()" );
     // Check the given User::TCritical type 
    if ( aCritical == User::EProcessCritical || 
         User::EProcessPermanent == aCritical )
        {
        return KErrArgument;
        }
    else
        {
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// CustomUser::SetupThreadHeap()
// Overloaded UserHeap::SetupThreadHeap function
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CustomUser::SetupThreadHeap( TBool aNotFirst, 
    SStdEpocThreadCreateInfo& aInfo, const TFileName aFileName,
    TUint32 aLogOption, TUint32 aIsDebug, const TFileName aVersion,
    TUint32 aAllocCallStackSize, TUint32 aFreeCallStackSize )
    {
    LOGSTR1( "ATMH CustomUser::SetupThreadHeap()" );
    LOGSTR2( "ATMH > Thread id(%d)", RThread().Id().operator TUint() );
    
    TInt ret( KErrNone );    
    // Check version number
    TBuf<KAtoolVersionNumberLength> atoolVer;
    if ( CheckVersion( aVersion, atoolVer ) != KErrNone )
    	{
    	LOGSTR1( "ATMH > Wrong API version > Inform user and Exit." );
    	ReportIncorrectVersion( aLogOption, aFileName, atoolVer );
    	return KAtoolVersionError;
    	}
    
    // Check is this shared heap
    if ( aInfo.iAllocator == NULL )
        {
        LOGSTR1( "ATMH creating a new heap" );
        // RAllocator is NULL so heap is not shared, creating a new heap
        ret = UserHeap::SetupThreadHeap( aNotFirst, aInfo );
        __ASSERT_ALWAYS( KErrNone == ret, AssertPanic( EFailedToCreateHeap ) );
        
#if ( SYMBIAN_VERSION_SUPPORT >= SYMBIAN_3 )
    #ifndef __WINS__
        // Set dummy Tls value
        TAny* dummyPtr( NULL );
        TInt setErr( UserSvr::DllSetTls( KDummyHandle, dummyPtr ) );
        LOGSTR2( "ATMH > Set Tls err(%i)", setErr );
    #endif
#endif
        // Install the RAllocator
        aInfo.iAllocator = &InstallAllocator( aNotFirst, aFileName, aLogOption, aIsDebug,
                aAllocCallStackSize, aFreeCallStackSize );
        }
    else
        {
        LOGSTR1( "ATMH sharing the heap" );
        // The heap is shared. Acquire pointer to the original heap
        RAnalyzeToolMemoryAllocator* allocator = 
			(RAnalyzeToolMemoryAllocator*) aInfo.iAllocator;
        // Share the heap
        allocator->ShareHeap();
        // Switch thread heap 
        User::SwitchAllocator( allocator );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CustomUser::InstallAllocator
// Installs the RAllocator
// -----------------------------------------------------------------------------
//
//lint -e{429} suppress "Custodial pointer 'allocator' has not been freed or returned"
EXPORT_C RAllocator& CustomUser::InstallAllocator( TBool aNotFirst, 
	const TFileName aFileName, TUint32 aLogOption, TUint32 aIsDebug,
	TUint32 aAllocCallStackSize, TUint32 aFreeCallStackSize )
    {
    LOGSTR1( "ATMH CustomUser::InstallAllocator()" );
    
    // Open handle to the device driver
    RAnalyzeTool analyzetool;
    TInt error = analyzetool.Open();
    
    // Check if the device driver has already loaded
    if ( KErrNone == error )
        {
        LOGSTR1( "ATMH CustomUser::InstallAllocator() - analyzetool.Open() returned KErrNone" );
        // The device driver has already loaded
        // Get pointer to the main thread allocator
        TMainThreadParamsBuf params;
        params().iProcessId = RProcess().Id().operator TUint();
        error = analyzetool.MainThreadAlloctor( params );

        __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantOpenHandle ) );
        
        // Close handle to the device driver
        analyzetool.Close();
        
        // Is this the first thread of the program
        if ( params().iAlone )
            {
            LOGSTR1( "ATMH CustomUser::InstallAllocator() - first thread of the program" );
            // Only one thread in the program. Must be main thread
            RAnalyzeToolMainAllocator* allocator = 
				new RAnalyzeToolMainAllocator( aNotFirst, aFileName, aLogOption,
				                               aIsDebug, aAllocCallStackSize, aFreeCallStackSize );
            
            __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
            
            // Change threads allocator
            User::SwitchAllocator( allocator );
            
            // Return reference to the RAllocator
            return *allocator;
            }
        // This is not the first thread. A new thread with a new heap created
        else
            {
            LOGSTR1( "ATMH CustomUser::InstallAllocator() - create a new allocator for the new thread" );
            // Create new RAllocator with handles from the main thread
            RAnalyzeToolAllocator* allocator = new RAnalyzeToolAllocator( 
                    aNotFirst,
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->StorageServer(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->Codeblocks(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->Mutex(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->ProcessId(), 
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->AnalyzeTool(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->StorageServerOpen(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->LogOption(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->AllocMaxCallStack(),
                    ((RAnalyzeToolMainAllocator*)params().iAllocator)->FreeMaxCallStack() );

            __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
            
            // Change threads allocator
            User::SwitchAllocator( allocator );
            
            // Return reference to the RAllocator
            return *allocator;
            }
        }
    // The device driver does not exists so this must be the first thread
    else
        {
        LOGSTR1( "ATMH CustomUser::InstallAllocator() - analyzetool.Open() returned error, creating DD" );
        RAnalyzeToolMainAllocator* allocator = 
			new RAnalyzeToolMainAllocator( aNotFirst, aFileName, aLogOption, aIsDebug,
			        aAllocCallStackSize, aFreeCallStackSize );
        
        __ASSERT_ALWAYS( allocator != NULL, AssertPanic( ENoMemory ) );
        
        // Change threads allocator
        User::SwitchAllocator( allocator );

        // Return reference to the RAllocator
        return *allocator;
        }
    } 
    
// -----------------------------------------------------------------------------
// AnalyzeToolInterface::StartSubTest
// Function for starting a subtest with a given name
// -----------------------------------------------------------------------------
//
EXPORT_C void AnalyzeToolInterface::StartSubTest( const TDesC8& aSubtestId )
    {
    LOGSTR1( "ATMH AnalyzeToolInterface::StartSubTest()" );
    // Open handle to the device driver
    RAnalyzeTool analyzetool;
    TInt error = analyzetool.Open();
    if ( KErrNone == error )
        {
        LOGSTR1( "ATMH AnalyzeToolInterface::StartSubTest() - DD opened" );
        // The device driver has already loaded
        // Get pointer to the main thread allocator
        TMainThreadParamsBuf params;
        params().iProcessId = RProcess().Id().operator TUint();
        error = analyzetool.MainThreadAlloctor( params );

        __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantOpenHandle ) );
        
        // Close handle to the device driver
        analyzetool.Close();

        if ( ( ( RAnalyzeToolMainAllocator* )params().iAllocator )->StorageServerOpen() )
            {
                LOGSTR1( "ATMH AnalyzeToolInterface::StartSubTest() - SS open -> try to start subtest" );
            // Start subtest
            ( ( RAnalyzeToolMainAllocator* )params().iAllocator )->
                        StorageServer().StartSubTest( aSubtestId );
            }
        }
    }

// -----------------------------------------------------------------------------
// AnalyzeToolInterface::StopSubTest
// Function for stopping a subtest with a given name
// -----------------------------------------------------------------------------
//
EXPORT_C void AnalyzeToolInterface::StopSubTest( const TDesC8& aSubtestId )
    {
    LOGSTR1( "ATMH AnalyzeToolInterface::StopSubTest()" );
    // Open handle to the device driver
    RAnalyzeTool analyzetool;
    TInt error = analyzetool.Open();
    if ( KErrNone == error )
        {
        LOGSTR1( "ATMH AnalyzeToolInterface::StopSubTest() - DD opened" );
        // The device driver has already loaded
        // Get pointer to the main thread allocator
        TMainThreadParamsBuf params;
        params().iProcessId = RProcess().Id().operator TUint();
        error = analyzetool.MainThreadAlloctor( params );

        __ASSERT_ALWAYS( KErrNone == error, AssertPanic( ECantOpenHandle ) );
        
        // Close handle to the device driver
        analyzetool.Close();

        if ( ( ( RAnalyzeToolMainAllocator* )params().iAllocator )->StorageServerOpen() )
            {
                LOGSTR1( "ATMH AnalyzeToolInterface::StopSubTest() - SS open -> try to stop subtest" );
            // Start subtest
            ( ( RAnalyzeToolMainAllocator* )params().iAllocator )->
                        StorageServer().StopSubTest( aSubtestId );
            }
        }
    }

// -----------------------------------------------------------------------------
// CustomUser::__DbgMarkEnd
// Marks the end of heap cell checking at the current nested level 
// for the current thread's default heap, or the kernel heap.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CustomUser::__DbgMarkEnd( TBool /*aKernel*/, TInt /*aCount*/ )
    {
    LOGSTR1( "ATMH CustomUser::__DbgMarkEnd()");
    // Return just zero to override system's memory leak check
    return 0;
    }

// -----------------------------------------------------------------------------
// CustomUser::CheckVersion
// Check atool version
// -----------------------------------------------------------------------------
//
TInt CustomUser::CheckVersion( const TFileName aVersion, TDes& aToolVersion )
    { 
    LOGSTR2( "ATMH CustomUser::CheckVersion(), aVersion( %S )", &aVersion );
    
    TFileName version;
    version.Copy( aVersion );
    TBuf<KAtoolVersionNumberLength> apiVer;
    	
    // Find separator place
    TInt findplace( version.Find( KVersionSeparator() ) );
    // Parse API version first [x.x.x;x.x.x]
    if ( findplace >= 0 && findplace <= apiVer.MaxLength() )
		{
		apiVer.Copy( version.Mid( 0, findplace ) ); 
		version.Delete( 0, findplace + KVersionSeparator().Length() );
		}
 
    if ( version.Length() <= aToolVersion.MaxLength() )
    	{
    	aToolVersion.Copy( version );
    	if ( aToolVersion.Compare( KAtoolApiVersion ) == KErrNone &&
    		 apiVer.Length() == 0 )
    		{
    		// Support 1.5.0 version (Version info: [1.5.0])
    		apiVer.Copy( version );
    		}
    	}
    
    LOGSTR3( "ATMH > API version( %S ), ATOOL version( %S )", 
    		&apiVer, &aToolVersion );
        
    // Check version numbers 
    if ( apiVer.Compare( KAtoolApiVersion ) == KErrNone )
    	{
    	return KErrNone;
    	}
    return KErrCancel;    
    }

// -----------------------------------------------------------------------------
// CustomUser::ReportIncorrectVersion
// Function for showing incorrect version information
// -----------------------------------------------------------------------------
//
void CustomUser::ReportIncorrectVersion( const TUint32 aLogOption,
	const TFileName aFileName, const TDes& aToolVersion )
	{
	LOGSTR2( "ATMH CustomUser::ReportIncorrectVersion(), aFileName( %S )", 
			&aFileName );
	
	switch ( aLogOption )
		{
		case EATLogToFile:
			{
			LOGSTR1( "ATMH ReportIncorrectVersion > EATLogToFile" );			
			
			// A handle to a file server session.
			RFs fs;
			// Creates and opens a file, 
			// and performs all operations on a single open file.
			RFile file;	
			// Create full path buffer
			TBuf<KMaxFileName> logFileBuf;
			// Connects a client to the file server.
			TInt err( fs.Connect() );
			
			if ( !err )
				{				
                err = TATDriveInfo::CreatePath( logFileBuf, aFileName, fs );
			    
				// Replace file if exists
				if ( err && err != KErrAlreadyExists )
					{
					LOGSTR2( "ATMH > TATDriveInfo::CreatePath() err( %i )", err );
					return;
					}
				
				// Replace file if exists (drive C)
				err = file.Replace( fs, logFileBuf, EFileWrite );
										
				// Write to file
				if ( !err )
					{
					err = file.Write( KDataFileVersion );
					// Error msg buffer
					TBuf8<KMaxFileName> msg;				     
					// Write the error code to the buffer  
					logFileBuf.Format( KIncorrectText, &KAtoolApiVersion, &aToolVersion );	
					CnvUtfConverter::ConvertFromUnicodeToUtf8( msg, logFileBuf );
					err = file.Write( msg );
					}
				// Closes the file.
				file.Close();
				}
			
			LOGSTR2( "ATMH > File err( %i )", err );			
			// Closes the handle.
			fs.Close();
			}
			break;
			
		case EATUseDefault:
		case EATLogToXti:
			{
			LOGSTR1( "ATMH > ReportIncorrectVersion > EATLogToXti" );
			// Error msg buffer
			TBuf<KMaxFileName> msg;	
			msg.Copy( KIncorrectTextXti );
			msg.Append( KIncorrectText );
			TBuf<KMaxFileName> xtiMsg;	
			// Write the error code to the buffer  
			xtiMsg.Format( msg, &KAtoolApiVersion, &aToolVersion );
			RDebug::Print( xtiMsg );
			}
			break;
		
		default:
			{
			LOGSTR1( "ATMH > ReportIncorrectVersion > default" );
			}
			break;
		}	
	}

// End of File
