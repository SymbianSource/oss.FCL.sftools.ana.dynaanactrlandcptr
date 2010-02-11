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



// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <f32file.h>

#include "atconsoleui.h"
#include "atconsoleviews.h"


// CONSTANTS
_LIT( KNameTxt, "AT CONSOLE UI" );

// FORWARD DECLARATIONS
LOCAL_C void MainL();

// -----------------------------------------------------------------------------
// CConsoleMain::NewL()
// Construct the console main class.
// -----------------------------------------------------------------------------
//
CConsoleMain* CConsoleMain::NewL( )
    {
    CConsoleMain* self = new ( ELeave ) CConsoleMain();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CConsoleMain::ConstructL()
// Second level constructor.
// -----------------------------------------------------------------------------
//
void CConsoleMain::ConstructL( )
    {
    // Construct the console
    iConsole = Console::NewL( KNameTxt,
                             TSize( KConsFullScreen, KConsFullScreen ) );
    
    iStorageServerOpen = EFalse;

    iConsole->Printf(_L("\nAnalyzeTool console starting\n"));
    }

// -----------------------------------------------------------------------------
// CConsoleMain::CConsoleMain()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CConsoleMain::CConsoleMain()
    {
    }

// -----------------------------------------------------------------------------
// CConsoleMain::~CConsoleMain()
// Destructor.
// -----------------------------------------------------------------------------
//
CConsoleMain::~CConsoleMain()
    {
    // Close storage server
    iStorageServer.Close();
    
    delete iReader;
    iReader = NULL;
    
    delete iScroller;
    iScroller = NULL;
    
    delete iConsole;
    iConsole = NULL;

    delete iMainView;
    iMainView = NULL;
    
    iSubTestProcesses.Close();
    
    iProcesses.Close();
    }

// -----------------------------------------------------------------------------
// CConsoleMain::StartL()
// Construct menu objects and start the menu handling.
// -----------------------------------------------------------------------------
//
TInt CConsoleMain::StartL()
    {
    // Construct keystroke reader
    iReader = CConsoleReader::NewL( this, iConsole );
    // Construct the main menu
    iMainView = CMainView::NewL( this, NULL, _L( "Processes view" ) );
    // Connect to the storage server
    TInt error = iStorageServer.Connect();
    if( error != KErrNone )
        {
        // Notify if error occurs
        }
    else
        {
        iStorageServerOpen = ETrue;
        iStorageServer.GetProcessesL( iProcesses );
        }
    
    // TEST
    /*TATProcessInfo pr1;
    pr1.iProcessId = 1;
    TBuf8<KMaxProcessName> pr1Name;
    pr1Name.Copy( _L("MemoryLeaker.exe") );
    pr1.iProcessName.Copy( pr1Name );
    pr1.iStartTime = 1234556789;
    iProcesses.Append( pr1 );

    TATProcessInfo pr2;
    pr2.iProcessId = 2;
    TBuf8<KMaxProcessName> pr2Name;
    pr2Name.Copy( _L("ConsoleApplication.exe") );
    pr2.iProcessName.Copy( pr2Name );
    pr2.iStartTime = 12345567559;
    iProcesses.Append( pr2 );
    
    TATProcessInfo pr3;
    pr3.iProcessId = 3;
    TBuf8<KMaxProcessName> pr3Name;
    pr3Name.Copy( _L("Player.exe") );
    pr3.iProcessName.Copy( pr3Name );
    pr3.iStartTime = 1234577789;
    iProcesses.Append( pr3 );*/
    // TEST END
    
    if ( iProcesses.Count() > KErrNone )
        {
        CView* processMenu  = NULL;
        for ( TInt count = 0 ; count < iProcesses.Count() ; count++ )
            {
            TBuf<KMaxProcessName> processName;
            processName.Copy( iProcesses[ count ].iProcessName );
            TInt64 processId = iProcesses[ count ].iProcessId;
            // Create and add menu
            processMenu   = CProcessInfoView::NewL( this, iMainView, processName, processId );
            iMainView->AddItemL( processMenu );  
            }           
        }
           
    iScroller = CScrollerTimer::NewL ( this );
    iScroller->StartL();
    // Print the main menu
    iCurrentView = iMainView;
    iCurrentView->PrintViewL( CView::EViewPrint );
    // Start to process keyboard events
    iReader->StartL();
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CConsoleMain::Processes()
// Returns processes.
// -----------------------------------------------------------------------------
//
RArray<TATProcessInfo> CConsoleMain::Processes()
    {
    return iProcesses;
    }

// -----------------------------------------------------------------------------
// CConsoleMain::KeyPressedL()
// Process keyboard events. Print new menu.
// -----------------------------------------------------------------------------
//
void CConsoleMain::KeyPressedL()
    {
    TBool cont = ETrue;
    // Read the key
    TKeyCode key = iConsole->KeyCode();
    CView* tmp = iCurrentView;
    // Update the screen
    TimerUpdate();
    
    // Let the menu handle the key press
    TRAPD( err, 
        iCurrentView = iCurrentView->SelectL( key, cont );
        );
    
    if( err != KErrNone )
        {
        User::InfoPrint( 
            _L( "Processing keystroke failed" ) );  
        }
            
    if ( iCurrentView == NULL )
        {
            iCurrentView = tmp;
        }

   // If "not-exit" key pressed, continue
    if ( cont )
        {
        // Either only update old menu or new menu.
        if ( tmp == iCurrentView )
            {
            TRAP( err, iCurrentView->PrintViewL( CView::EViewRefresh ); );
            }
        else
            {
            TRAP( err, iCurrentView->PrintViewL( CView::EViewPrint ); )
            }
        if( err != KErrNone )
            {
            User::InfoPrint( 
                _L( "Printing view failed" ) );  
            }
        // Enable keystrokes
        iReader->StartL();
        }
    else
        {
        // Stop all subtests before closing handles
        StopAllSubtestsL();
        // "Exit", stop scheduler and exit
        CActiveScheduler::Stop();
        }
    }

// -----------------------------------------------------------------------------
// CConsoleMain::StopAllSubtestsL()
// Stops all subtests which are running
// -----------------------------------------------------------------------------
//
void CConsoleMain::StopAllSubtestsL()
    {
    TInt index( KErrNone );
    RArray<TATProcessInfo> processes;
    iStorageServer.GetProcessesL( processes );
    while ( index < processes.Count() )
        {
        TUint processId = processes[ index ].iProcessId;
        _LIT8( KSubTestId, "ATConsoleUiSubTest" );
        TInt stopErr = iStorageServer.StopSubTest( processId, KSubTestId );
        TInt find = iSubTestProcesses.Find( processId );
        
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
    processes.Close();
    }
    
// -----------------------------------------------------------------------------
// CConsoleMain::Close()
// Close instance.
// -----------------------------------------------------------------------------
//
void CConsoleMain::Close( TInt aHandle )
    {
    if( aHandle < 0 )
        {
        return;
        } 
    }
           
// -----------------------------------------------------------------------------
// CConsoleMain::GetConsole()
// Returns the console.
// -----------------------------------------------------------------------------
//
CConsoleBase* CConsoleMain::GetConsole()
    {
    return iConsole;
    }

// -----------------------------------------------------------------------------
// CConsoleMain::TimerUpdate()
// Updates current menu from timer.
// -----------------------------------------------------------------------------
//
void CConsoleMain::TimerUpdate()
    {
    // Update processes
    if ( iCurrentView->Name().Compare( _L( "Processes view" ) ) == KErrNone )
        {
        UpdateProcessesL();
        iCurrentView->ResetItems();
        if ( iProcesses.Count() > KErrNone )
            {
            CView* processMenu  = NULL;
            for ( TInt count = 0 ; count < iProcesses.Count() ; count++ )
                {
                TBuf<KMaxProcessName> processName;
                processName.Copy( iProcesses[ count ].iProcessName );
                TInt64 processId = iProcesses[ count ].iProcessId;
                // Create and add menu
                processMenu   = CProcessInfoView::NewL( this, iMainView, processName, processId );
                iMainView->AddItemL( processMenu );  
                }           
             }                      
        }
    iCurrentView->TimerUpdate();
    }

// -----------------------------------------------------------------------------
// CConsoleMain::UpdateProcessesL()
// Updates processes.
// -----------------------------------------------------------------------------
//
void CConsoleMain::UpdateProcessesL()
    {
    if ( iStorageServerOpen )
        {
        iStorageServer.GetProcessesL( iProcesses );
        }
    }

// -----------------------------------------------------------------------------
// CConsoleMain::SetProcessSubTestStart()
// Starts subtest for a process.
// -----------------------------------------------------------------------------
//
void CConsoleMain::SetProcessSubTestStart( TUint aProcessId )
    {
    // load the kernel side device driver
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
        }
        
    _LIT8( KSubTestId, "ATConsoleUiSubTest" );
    TInt startErr = iStorageServer.StartSubTest( aProcessId, KSubTestId, handleCount );
    if ( KErrNone == startErr )
        {
        iSubTestProcesses.Append( aProcessId );
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
// CConsoleMain::SetProcessSubTestStop()
// Stops subtest for a process.
// -----------------------------------------------------------------------------
//
void CConsoleMain::SetProcessSubTestStop( TUint aProcessId )
    {
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
        }
        
    _LIT8( KSubTestId, "ATConsoleUiSubTest" );
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
// CConsoleMain::IsSubTestRunning()
// Returns ETrue if subtest is running for a process.
// -----------------------------------------------------------------------------
//
TInt CConsoleMain::IsSubTestRunning( TUint aProcessId )
    {
    return iSubTestProcesses.Find( aProcessId );
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::NewL()
// Construct a new CScrollerTimer object.
// -----------------------------------------------------------------------------
//
CScrollerTimer* CScrollerTimer::NewL( CConsoleMain* aMain )
    {
    CScrollerTimer* self = new ( ELeave ) CScrollerTimer();
    CleanupStack::PushL( self );
    self->ConstructL( aMain );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::ConstructL()
// Second level constructor.
// -----------------------------------------------------------------------------
//
void CScrollerTimer::ConstructL( CConsoleMain* aMain )
    {
    // Store module information
    iMain = aMain;
    iTimer.CreateLocal();

    CActiveScheduler::Add ( this );
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::CScrollerTimer()
// Constructor.
// -----------------------------------------------------------------------------
//
CScrollerTimer::CScrollerTimer() : CActive (CActive::EPriorityStandard)
    {
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::~CScrollerTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CScrollerTimer::~CScrollerTimer( )
    {
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::StartL()
// Start timer.
// -----------------------------------------------------------------------------
//
void CScrollerTimer::StartL( )
    {
    SetActive();
    iTimer.After ( iStatus, KScrollPeriod );
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::RunL()
// RunL.
// -----------------------------------------------------------------------------
//
void CScrollerTimer::RunL( )
    {
    iMain->TimerUpdate();

    // Restart request
    SetActive();
    iTimer.After ( iStatus, KScrollPeriod );
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::DoCancel()
// Cancels timer.
// -----------------------------------------------------------------------------
//
void CScrollerTimer::DoCancel( )
    {
    iTimer.Cancel();
    }

// -----------------------------------------------------------------------------
// CScrollerTimer::RunError()
// Returns error.
// -----------------------------------------------------------------------------
//
TInt CScrollerTimer::RunError( TInt aError)
    {
    return aError;
    }    

// -----------------------------------------------------------------------------
// CConsoleReader::NewL()
// First phase constructor.
// -----------------------------------------------------------------------------
//
CConsoleReader* CConsoleReader::NewL( CConsoleMain* aMain, 
                                      CConsoleBase* aConsole )
    {
    CConsoleReader* self = 
        new ( ELeave ) CConsoleReader( aMain, aConsole );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CConsoleReader::ConstructL()
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CConsoleReader::ConstructL( )
    {
    }

// -----------------------------------------------------------------------------
// CConsoleReader::CConsoleReader()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CConsoleReader::CConsoleReader( CConsoleMain* aMain, 
                                CConsoleBase* aConsole ): 
    CActive( EPriorityStandard )
    {
    iMain = aMain;
    iConsole = aConsole;
    
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CConsoleReader::~CConsoleReader()
// Destructor.
// -----------------------------------------------------------------------------
//
CConsoleReader::~CConsoleReader( )
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CConsoleReader::StartL()
// Starts console reader.
// -----------------------------------------------------------------------------
//
void CConsoleReader::StartL( )
    {
    // Start to process keyboard events
    SetActive();
    iConsole->Read(iStatus);
    }
  
// -----------------------------------------------------------------------------
// CConsoleReader::RunError()
// Returns error.
// -----------------------------------------------------------------------------
//
TInt CConsoleReader::RunError( TInt aError )
    {
    return aError;
    }

// -----------------------------------------------------------------------------
// CConsoleReader::RunL()
// Handles key pressings.
// -----------------------------------------------------------------------------
//
void CConsoleReader::RunL()
    {
    iMain->KeyPressedL(); 
    }

// -----------------------------------------------------------------------------
// CConsoleReader::DoCancel()
// Cancel request.
// -----------------------------------------------------------------------------
//
void CConsoleReader::DoCancel()
    {
    iConsole->ReadCancel();
    }

// -----------------------------------------------------------------------------
// MainL()
// The main function that can leave.
// Create the CMainConsole object and create, initialise and 
// start active scheduler.
// -----------------------------------------------------------------------------
//
LOCAL_C void MainL()
    {
    // Construct and install active scheduler
    CActiveScheduler* scheduler=new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    // Construct the console
    CConsoleMain* mainConsole = CConsoleMain::NewL();
    CleanupStack::PushL( mainConsole );

    // Start the console
    mainConsole->StartL();

    // Start handling requests
    CActiveScheduler::Start();
    // Execution continues from here after CActiveScheduler::Stop()

    // Clean-up
    CleanupStack::PopAndDestroy( mainConsole );
    CleanupStack::PopAndDestroy( scheduler );
    }

// -----------------------------------------------------------------------------
// E32Main()
// The main function. Execution starts from here.
// Create clean-up stack and trap the MainL function which does the
// real work.
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;

    // Get clean-up stack
    CTrapCleanup* cleanup=CTrapCleanup::New();

    // Call the main function
    TRAPD( error, MainL() );

    // Clean-up
    delete cleanup; 
    cleanup = NULL;

    __UHEAP_MARKEND;

    return error;
    }

#if defined(__WINS__)
// -----------------------------------------------------------------------------
// WinsMain()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt WinsMain()
    {
    E32Main();
    return KErrNone;
    }

#endif // __WINS__

// End of File
