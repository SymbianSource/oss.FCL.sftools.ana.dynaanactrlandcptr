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

#include <eikmenub.h>
#include <akncontext.h>
#include <akntitle.h>
#include <atconfigurationappgui.rsg>
#include <stringloader.h>
#include <aknnotewrappers.h>
#include <s32stor.h> // CDictionaryStore

#include "atconfigurationappguiappui.h"
#include "atconfigurationappgui.hrh"
#include "atconfigurationappguilistboxview.h"
#include "atconfigurationappguiprocessview.h"
#include "atconfigurationappguisettingitemlistview.h"
#include "atconfigurationappguiapplication.h"
#include "atlog.h"

#include <aknmessagequerydialog.h>

// CONSTANTS
const TInt KATDefaultUpdatingInterval = 2000000;
_LIT8( KSubTestId, "ATConfigUiSubTest" );

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::CATConfigurationAppGuiAppUi()
// Construct the CATConfigurationAppGuiAppUi instance
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiAppUi::CATConfigurationAppGuiAppUi()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::CATConfigurationAppGuiAppUi()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::ConstructL()
// Completes the second phase of Symbian object construction.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::ConstructL()" );
    
    BaseConstructL( EAknEnableSkin  | 
                     EAknEnableMSK ); 
    InitializeContainersL();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::~CATConfigurationAppGuiAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiAppUi::~CATConfigurationAppGuiAppUi()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::~CATConfigurationAppGuiAppUi()" );
    
    if ( iPeriodicTimer )
        {
        if ( iPeriodicTimer->IsActive() )
            {
            iPeriodicTimer->Cancel();
            }  
        delete iPeriodicTimer;  
        }
 
    iSubTestProcesses.Close();
    iStorageServer.Close();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::InitializeContainersL()
// Initializes all the controls.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::InitializeContainersL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::InitializeContainersL()" );
    
    iPeriodicTimer = CPeriodic::New( CActive::EPriorityLow );
        
    iATConfigurationAppGuiListBoxView = CATConfigurationAppGuiListBoxView::NewL();
    AddViewL( iATConfigurationAppGuiListBoxView );
    iATConfigurationAppGuiProcessView = CATConfigurationAppGuiProcessView::NewL();
    AddViewL( iATConfigurationAppGuiProcessView );
    //iATConfigurationAppGuiSettingItemListView = CATConfigurationAppGuiSettingItemListView::NewL();
    //AddViewL( iATConfigurationAppGuiSettingItemListView );
    
    SetDefaultViewL( *iATConfigurationAppGuiListBoxView );

    TInt error = iStorageServer.Connect();
        if ( error != KErrNone )
            {
            HBufC* noteText;
            noteText = StringLoader::LoadLC( R_CANT_CONNECT_SERVER );
            CAknErrorNote* note = new (ELeave) CAknErrorNote( ETrue );
            note->ExecuteLD( *noteText );
            CleanupStack::PopAndDestroy( noteText );
            }
    // Load application settings
    //LoadSettingsL();
    // Start updating the view
    if ( iPeriodicTimer )
        {
        StartTimerL();    
        }    
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::HandleCommandL()
// Handle a command for this appui.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::HandleCommandL( TInt aCommand )
    {
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::HandleCommandL(%i)", 
			aCommand );
    
    TBool commandHandled = EFalse;
    switch ( aCommand )
        { // Code to dispatch to the AppUi's menu and CBA commands is generated here
        default:
            break;
        }

    if ( !commandHandled ) 
        {
        if ( aCommand == EAknSoftkeyExit || aCommand == EEikCmdExit )
            {
            // Stop all subtests before closing handles
            StopAllSubtestsL();
            Exit();
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::HandleResourceChangeL()
// Override of the HandleResourceChangeL virtual function.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::HandleResourceChangeL( TInt aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::HandleResourceChangeL()" );
    
    CAknViewAppUi::HandleResourceChangeL( aType );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::HandleKeyEventL()
// Override of the HandleKeyEventL virtual function.
// -----------------------------------------------------------------------------
//
TKeyResponse CATConfigurationAppGuiAppUi::HandleKeyEventL(
        const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::HandleKeyEventL()" );
    
    // The inherited HandleKeyEventL is private and cannot be called
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::HandleViewDeactivation()
// Override of the HandleViewDeactivation virtual function.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::HandleViewDeactivation( 
        const TVwsViewId& aViewIdToBeDeactivated, 
        const TVwsViewId& aNewlyActivatedViewId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::HandleViewDeactivation()" );
    
    CAknViewAppUi::HandleViewDeactivation( 
            aViewIdToBeDeactivated, 
            aNewlyActivatedViewId );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::SetProcessSubTestStartL()
// Starts subtest for a process.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::SetProcessSubTestStartL( TUint aProcessId )
    {
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::SetProcessSubTestStartL(%i)", 
    		aProcessId );
    
    // Load the kernel side device driver
    TInt loadErr = User::LoadLogicalDevice( KAnalyzeToolLddName );
    TBool driverLoaded( EFalse );
            
    if ( loadErr == KErrNone || loadErr == KErrAlreadyExists )
        {
        driverLoaded = ETrue;
        }
        
    RAnalyzeTool analyzeTool;    
    TInt driverOpen = analyzeTool.Open();
    TInt handleCount( KErrNone );
    
    if ( driverOpen == KErrNone )
        {       
        TATProcessHandlesBuf params;
        params().iProcessId = aProcessId;
        
        analyzeTool.GetCurrentHandleCount( params );
        handleCount = params().iCurrentHandleCount;
        LOGSTR2( "CATConfigurationAppGuiAppUi::SetProcessSubTestStartL - %i current handle(s)",
                params().iCurrentHandleCount );       
        }
    
    TInt startErr = iStorageServer.StartSubTest( aProcessId, KSubTestId, handleCount );
    if ( KErrNone == startErr )
        {
        iSubTestProcesses.Append( aProcessId );    
        }
    else if ( KErrNotSupported == startErr )
        {
        ShowErrorNoteL( R_CANT_START_SUBTEST_NOTSUPPORTED );
        }
    else // Could not start for a process
        {
        ShowErrorNoteL( R_CANT_START_SUBTEST_PROCESSID );  
        }
    
    // The count of device driver users
    TClientCountBuf count;
    
    // Check the flag
    if ( driverOpen == KErrNone )
        {
        analyzeTool.ClientCount( count );
        analyzeTool.Close();
        }  
    // Check the flag
    if ( driverLoaded )
        {
        // Check if there is another user for device driver
        if ( count().iClientCount <= 1 )
            {
            // There was no other users -> unload the device driver
            User::FreeLogicalDevice( KAnalyzeToolLddName );
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::SetProcessSubTestStopL()
// Stops subtest for a process.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::SetProcessSubTestStopL( TUint aProcessId )
    {
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::SetProcessSubTestStopL(%i)", 
    		aProcessId );
    
    // Load the kernel side device driver
    TInt loadErr = User::LoadLogicalDevice( KAnalyzeToolLddName );
    TBool driverLoaded( EFalse );
            
    if ( loadErr == KErrNone || loadErr == KErrAlreadyExists )
        {
        driverLoaded = ETrue;
        }
        
    RAnalyzeTool analyzeTool;
    
    TInt driverOpen = analyzeTool.Open();
    TInt handleCount( KErrNone );
    
    if ( driverOpen == KErrNone )
        {  
        TATProcessHandlesBuf params;
        params().iProcessId = aProcessId;
        
        analyzeTool.GetCurrentHandleCount( params );
        handleCount = params().iCurrentHandleCount;
        LOGSTR2( "CATConfigurationAppGuiAppUi::SetProcessSubTestStopL - %i current handle(s)",
                params().iCurrentHandleCount );      
        }
        
    TInt stopErr = iStorageServer.StopSubTest( aProcessId, KSubTestId, handleCount );
    if ( KErrNone == stopErr )
        {
        TInt index = iSubTestProcesses.Find( aProcessId );
        if ( index > KErrNotFound )
            {
            iSubTestProcesses.Remove( index );
            if ( iSubTestProcesses.Count() == KErrNone )
                iSubTestProcesses.Reset();
            }             
        }
    else
        {
        ShowErrorNoteL( R_CANT_STOP_SUBTEST_PROCESSID );     
        }
    
    // The count of device driver users
    TClientCountBuf count;
    
    // Check the flag
    if ( driverOpen == KErrNone )
        {
        analyzeTool.ClientCount( count );
        analyzeTool.Close();
        }  
    // Check the flag
    if ( driverLoaded )
        {
        // Check if there is another user for device driver
        if ( count().iClientCount <= 1 )
            {
            // There was no other users -> unload the device driver
            User::FreeLogicalDevice( KAnalyzeToolLddName );
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::IsSubTestRunning()
// Returns ETrue if subtest is running for a process.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiAppUi::IsSubTestRunning( TUint aProcessId )
    {
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::IsSubTestRunning(%i)", 
    		aProcessId );
    
    return iSubTestProcesses.Find( aProcessId );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::LoadSettingsL()
// Loads application settings.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiAppUi::LoadSettingsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::LoadSettingsL()" );
    
    /*CDictionaryStore* iniFile = Application()->OpenIniFileLC( CCoeEnv::Static()->FsSession() );
 
    if ( iniFile->IsPresentL( KUidATConfigurationAppGuiApplication ) )
        {
        RDictionaryReadStream readStream;
        readStream.OpenLC( *iniFile, KUidATConfigurationAppGuiApplication );
        // Load warning tones value
        iWarningTones = static_cast<TInt>( readStream.ReadInt32L() );
        // Load update interval value
        iUpdateInterval = static_cast<TInt>( readStream.ReadInt32L() );
        // Initialize other settings
 
        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        // ini doesn't exist, so use default values
        iWarningTones = 0;
        iUpdateInterval = 0;
        }
 
    CleanupStack::PopAndDestroy(iniFile);*/      
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::ShowErrorNoteL()
// Shows error note.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::ShowErrorNoteL( TInt aResourceId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::ShowErrorNoteL()" );
    
    CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote();
    HBufC* errorText = StringLoader::LoadLC( aResourceId );
    errorNote->SetTextL( errorText->Des() );
    errorNote->ExecuteLD();
    CleanupStack::PopAndDestroy( errorText );      
    }

//-----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::HandleForegroundEventL
// Handles fore ground events
//-----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::HandleForegroundEventL( TBool aForeground )
    {  
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::HandleForegroundEventL(%i)", 
    		aForeground );
    
    if ( aForeground )
        {
        // Start timer when processes view gains foreground
        if ( iDeactivatedId.iViewUid == TUid::Uid( EATConfigurationAppGuiListBoxViewId ) )
            {
            StartTimerL();    
            }
        }
    else
        {
        // Cancel timer when not in foreground
        TVwsViewId viewId;
        GetActiveViewId( viewId );
        // Save deactivated view's Id
        iDeactivatedId = viewId;
        TimerNotifyL( EFalse );        
        }
    }

//-----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::StartTimerL
// Starts timer with a specific time interval
//-----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::StartTimerL()
    {   
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::StartTimerL()" );
    
    if ( iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }
    iPeriodicTimer->Start( KATDefaultUpdatingInterval, 
            KATDefaultUpdatingInterval, TCallBack( CallBackFunctionL, this ) );  
    }
    
//-----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::TimerNotifyL
// Periodic timer's callback function
//-----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::TimerNotifyL( TBool aPeriod )
    {
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::TimerNotifyL(%i)", 
    		aPeriod );
    
    if ( iPeriodicTimer->IsActive() )
        {
        iPeriodicTimer->Cancel();
        }   
    if ( aPeriod )
        {
        if ( iATConfigurationAppGuiListBoxView )
            {
            iATConfigurationAppGuiListBoxView->UpdateViewL();
            }
        StartTimerL();
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiAppUi::StopAllSubtestsL()
// Stops all subtests which are running
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiAppUi::StopAllSubtestsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiAppUi::StopAllSubtestsL()" );
    
    TInt index( KErrNone );
    RArray<TATProcessInfo> processes;
    TInt err( iStorageServer.GetProcessesL( processes ) );
    LOGSTR2( "iStorageServer.GetProcessesL(%i)", err ); 
    
    // Load the kernel side device driver
    TInt loadErr( User::LoadLogicalDevice( KAnalyzeToolLddName ) );
    TBool driverLoaded( EFalse );
    LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::StopAllSubtestsL() loadErr(%i)", 
    		loadErr );
            
    if ( loadErr == KErrNone || loadErr == KErrAlreadyExists )
        {
        driverLoaded = ETrue;
        }
    
    RAnalyzeTool analyzeTool;    
    TInt driverOpen( analyzeTool.Open() );
    
    while ( index < processes.Count() )
        {
        TUint processId = processes[ index ].iProcessId;
        TInt handleCount( KErrNone );
        
        if ( driverOpen == KErrNone )
            {  
            TATProcessHandlesBuf params;
            params().iProcessId = processId;
            // Get process handle count
            analyzeTool.GetCurrentHandleCount( params );
            handleCount = params().iCurrentHandleCount;
            LOGSTR2( "ATCU CATConfigurationAppGuiAppUi::StopAllSubtestsL() handleCount( %i )",
            		handleCount );      
            }        
        
        err = iStorageServer.StopSubTest( processId, KSubTestId, handleCount );
        LOGSTR2( "ATCU > iStorageServer.StopSubTest( %i )", err ); 
        
        TInt find( iSubTestProcesses.Find( processId ) );
        if ( find > KErrNotFound )
            {
            iSubTestProcesses.Remove( find );
            if ( iSubTestProcesses.Count() == KErrNone )
                {
                iSubTestProcesses.Reset();
                }  
            } 
        index++;
        }
    
    // Close processes array
    processes.Close();

    // The count of device driver users
    TClientCountBuf count;
    
    // Check the flag
    if ( driverOpen == KErrNone )
        {
        analyzeTool.ClientCount( count );
        analyzeTool.Close();
        }  
    // Check the flag
    if ( driverLoaded )
        {
        // Check if there is another user for device driver
        if ( count().iClientCount <= 1 )
            {
            // There was no other users -> unload the device driver
            err = User::FreeLogicalDevice( KAnalyzeToolLddName );
            LOGSTR2( "ATCU > FreeLogicalDevice( %i )", err ); 
            }
        }
    }

// End of File
