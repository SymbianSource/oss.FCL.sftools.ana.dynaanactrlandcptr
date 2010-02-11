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

#include <aknviewappui.h>
#include <eikmenub.h>
#include <avkon.hrh>
#include <barsread.h>
#include <stringloader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <akncontext.h>
#include <akntitle.h>
#include <eikbtgpc.h>
#include <atconfigurationappgui.rsg>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <apgtask.h>

#include "atconfigurationappgui.hrh"
#include "atconfigurationappguilistboxview.h"
#include "atconfigurationappguilistbox.hrh"
#include "atconfigurationappguilistbox.h"
#include "atconfigurationappguiappui.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::CATConfigurationAppGuiListBoxView()
// First phase of Symbian two-phase construction.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBoxView::CATConfigurationAppGuiListBoxView()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::CATConfigurationAppGuiListBoxView()" );
    
    iATConfigurationAppGuiListBox = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::~CATConfigurationAppGuiListBoxView()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBoxView::~CATConfigurationAppGuiListBoxView()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::~CATConfigurationAppGuiListBoxView()" );
    
    delete iATConfigurationAppGuiListBox;
    iATConfigurationAppGuiListBox = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::NewL()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBoxView* CATConfigurationAppGuiListBoxView::NewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::NewL()" );
    
    CATConfigurationAppGuiListBoxView* self = CATConfigurationAppGuiListBoxView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::NewLC()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBoxView* CATConfigurationAppGuiListBoxView::NewLC()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::NewLC()" );
    
    CATConfigurationAppGuiListBoxView* self = new ( ELeave ) CATConfigurationAppGuiListBoxView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::ConstructL()
// Second-phase constructor for view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::ConstructL()" );
    
    BaseConstructL( R_ATCONFIGURATION_APP_GUI_LIST_BOX_ATCONFIGURATION_APP_GUI_LIST_BOX_VIEW );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::Id()
// Returns The UID for this view.
// -----------------------------------------------------------------------------
//
TUid CATConfigurationAppGuiListBoxView::Id() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::Id()" );
    
    return TUid::Uid( EATConfigurationAppGuiListBoxViewId );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::HandleCommandL()
// Handle a command for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::HandleCommandL( TInt aCommand )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::HandleCommandL()" );
    
    TBool commandHandled = EFalse;
    switch ( aCommand )
        { // Code to dispatch to the AknView's menu and CBA commands is generated here
        case EATConfigurationAppGuiListBoxViewVersionMenuItemCommand:
            commandHandled = HandleVersionMenuItemSelectedL( aCommand );
            break;
        case EATConfigurationAppGuiListBoxViewDetailsMenuItemCommand:
            {
            // Listbox items are in the same order as in storage server
            // thus using current index
            if ( iATConfigurationAppGuiListBox )
                {
                TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
                RArray<TATProcessInfo> processes;
                CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
                appUi->StorageServer().GetProcessesL( processes );
                if ( current >= KErrNone && current < processes.Count() )
                    {
                    TUint processId = processes[ current ].iProcessId;
                    TInt64 processStartTime = processes[ current ].iStartTime;
                    appUi->SetCurrentProcessId( processId );
                    appUi->SetCurrentProcessStartTime( processStartTime );
                    AppUi()->ActivateLocalViewL( TUid::Uid( EATConfigurationAppGuiProcessViewId ) );
                    }
                // Closes the array and frees all memory allocated to the array.
                processes.Close();
                }
            break;
            }
        /*case EATConfigurationAppGuiListBoxViewEnd_processMenuItemCommand:
            {
            EndProcessL( EATConfigurationAppGuiListBoxViewEndTaskMenuItemCommand );
            break;
            }*/
        case EATConfigurationAppGuiListBoxViewEndTaskMenuItemCommand:
            {
            EndProcessL( EATConfigurationAppGuiListBoxViewEndTaskMenuItemCommand );
            break;
            }
        case EATConfigurationAppGuiListBoxViewKillProcessMenuItemCommand:
            {
            EndProcessL( EATConfigurationAppGuiListBoxViewKillProcessMenuItemCommand );
            break;
            }
        case EATConfigurationAppGuiListBoxViewTerminateProcessMenuItemCommand:
            {
            EndProcessL( EATConfigurationAppGuiListBoxViewTerminateProcessMenuItemCommand );
            break;
            }
        case EATConfigurationAppGuiListBoxViewPanicProcessMenuItemCommand:
            {
            EndProcessL( EATConfigurationAppGuiListBoxViewPanicProcessMenuItemCommand );
            break;
            }
        case EATConfigurationAppGuiListBoxViewCancel_loggingMenuItemCommand:
            {
            // Listbox items are in the same order as in storage server
            // thus using current index
            if ( iATConfigurationAppGuiListBox )
                {
                TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
                RArray<TATProcessInfo> processes;
                CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
                appUi->StorageServer().GetProcessesL( processes );
                if ( current >= KErrNone && current < processes.Count() )
                    {
                    TUint processId = processes[ current ].iProcessId;
                    // Stop subtesting for this process
                    //if ( appUi->IsSubTestRunning( processId ) )
                        //appUi->SetProcessSubTestStopL( processId );
                    
                    TInt cancelErr = appUi->StorageServer().CancelLogging( processId );
                    if ( cancelErr != KErrNone )
                        {
                        // Logging could not be cancelled
                        appUi->ShowErrorNoteL( R_ATCONFIGURATION_APP_GUI_LISTBOX_CANCEL_FAILURE_NOTE );
                        }
                    else
                        {
                        iATConfigurationAppGuiListBox->UpdateViewL();
                        }
                    }
				// Closes the array and frees all memory allocated to the array.
                processes.Close();
                }
            break;
            }
        case EATConfigurationAppGuiListBoxViewLog_settingMenuItemCommand:
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( EATConfigurationAppGuiSettingItemListViewId ),
                    TUid::Uid( EATConfigurationAppGuiListBoxViewId ), KNullDesC8 );
            break;
            }
        case EATConfigurationAppGuiListBoxViewStart_subtestMenuItemCommand:
            {
            if ( iATConfigurationAppGuiListBox )
                {
                TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
                RArray<TATProcessInfo> processes;
                CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
                appUi->StorageServer().GetProcessesL( processes );
                if ( current >= KErrNone && current < processes.Count() )
                    {
                    TUint processId = processes[ current ].iProcessId;
                    appUi->SetProcessSubTestStartL( processId );
                    iATConfigurationAppGuiListBox->UpdateViewL();
                    }
				// Closes the array and frees all memory allocated to the array.
                processes.Close();
                }
            break;
            }
        case EATConfigurationAppGuiListBoxViewStop_subtestMenuItemCommand:
            {
            if ( iATConfigurationAppGuiListBox )
                {
                TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
                RArray<TATProcessInfo> processes;
                CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
                appUi->StorageServer().GetProcessesL( processes );
                if ( current >= KErrNone && current < processes.Count() )
                    {
                    TUint processId = processes[ current ].iProcessId;
                    appUi->SetProcessSubTestStopL( processId );
                    iATConfigurationAppGuiListBox->UpdateViewL();
                    }
				// Closes the array and frees all memory allocated to the array.
                processes.Close();
                }
            break;
            }
        default:
            break;
        }
     
    if ( !commandHandled ) 
        {
    
        if ( aCommand == EAknSoftkeyExit || aCommand == EAknSoftkeyExit )
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            }
    
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::DoActivateL()
// Handles user actions during activation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::DoActivateL( 
        const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/,
        const TDesC8& /*aCustomMessage*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::DoActivateL()" );
    
    SetupStatusPaneL();
    
    if ( iATConfigurationAppGuiListBox == NULL )
        {
        iATConfigurationAppGuiListBox = CreateContainerL();
        iATConfigurationAppGuiListBox->SetMopParent( this );
        AppUi()->AddToStackL( *this, iATConfigurationAppGuiListBox );
        // For supporting MSK context menu
        MenuBar()->SetContextMenuTitleResourceId( R_ATCONFIGURATION_APP_GUI_LIST_BOX_OPTIONS_MENU );
        // Start update timer
        ( (CATConfigurationAppGuiAppUi*)AppUi() )->StartTimerL();
        } 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::DoDeactivate()
// Handles user actions during deactivation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::DoDeactivate()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::DoDeactivate()" );
    
    CleanupStatusPane();
    
    if ( iATConfigurationAppGuiListBox != NULL )
        {
        // Cancel timer events
        TRAP_IGNORE( ( (CATConfigurationAppGuiAppUi*)AppUi() )->TimerNotifyL( EFalse ) );
        AppUi()->RemoveFromViewStack( *this, iATConfigurationAppGuiListBox );
        delete iATConfigurationAppGuiListBox;
        iATConfigurationAppGuiListBox = NULL;
        }
    CleanupStatusPane();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::DynInitMenuPaneL()
// Handles dynamical menu item dimming.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::DynInitMenuPaneL( TInt aResourceId, 
	CEikMenuPane* aMenuPane )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::DynInitMenuPaneL()" );
    
    if ( R_ATCONFIGURATION_APP_GUI_LIST_BOX_MENU_PANE1_MENU_PANE == aResourceId )
        {
        if( iATConfigurationAppGuiListBox->GetCurrentItemIndex() < 0 ) 
            {
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewDetailsMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStart_subtestMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStop_subtestMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewEnd_processMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewLog_settingMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewHelpMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewCancel_loggingMenuItemCommand, ETrue );
            } 
        else 
            {
            TInt subTest( KErrNotFound );
            TATLogOption loggingMode = EATUseDefault;//lint !e644
            if ( iATConfigurationAppGuiListBox )
                {
                TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
                RArray<TATProcessInfo> processes;
                CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
                appUi->StorageServer().GetProcessesL( processes );
                if ( current >= KErrNone && current < processes.Count() )
                    {
                    TUint processId = processes[ current ].iProcessId;
                    subTest = appUi->IsSubTestRunning( processId );
                    // Get logging mode
                    appUi->StorageServer().GetLoggingModeL( processId, loggingMode );
                    }
				// Closes the array and frees all memory allocated to the array.
                processes.Close();
                }
            if ( subTest > KErrNotFound )
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStart_subtestMenuItemCommand, ETrue );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStop_subtestMenuItemCommand, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStart_subtestMenuItemCommand, EFalse );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStop_subtestMenuItemCommand, ETrue );
                }
            
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewDetailsMenuItemCommand, EFalse );
            
            if ( loggingMode == EATLoggingOff )
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewCancel_loggingMenuItemCommand, ETrue );
                // Logging cancelled -> hide necessary items
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewDetailsMenuItemCommand, ETrue );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStart_subtestMenuItemCommand, ETrue );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewStop_subtestMenuItemCommand, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewCancel_loggingMenuItemCommand, EFalse );
                }
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewLog_settingMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiListBoxViewHelpMenuItemCommand, ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::HandleStatusPaneSizeChange()
// Handle status pane size change for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::HandleStatusPaneSizeChange()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::HandleStatusPaneSizeChange()" );
    
    CAknView::HandleStatusPaneSizeChange();
    
    // This may fail, but we're not able to propagate exceptions here
    TInt result;
    TRAP( result, SetupStatusPaneL() ); 
    }  

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::SetupStatusPaneL()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::SetupStatusPaneL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::SetupStatusPaneL()" );
    
    // Reset the context pane
    TUid contextPaneUid = TUid::Uid( EEikStatusPaneUidContext );
    CEikStatusPaneBase::TPaneCapabilities subPaneContext = 
        StatusPane()->PaneCapabilities( contextPaneUid );
    if ( subPaneContext.IsPresent() && subPaneContext.IsAppOwned() )
        {
        CAknContextPane* context = static_cast< CAknContextPane* > ( 
            StatusPane()->ControlL( contextPaneUid ) );
        context->SetPictureToDefaultL();
        }
    
    // Setup the title pane
    TUid titlePaneUid = TUid::Uid( EEikStatusPaneUidTitle );
    CEikStatusPaneBase::TPaneCapabilities subPaneTitle = 
        StatusPane()->PaneCapabilities( titlePaneUid );
    if ( subPaneTitle.IsPresent() && subPaneTitle.IsAppOwned() )
        {
        CAknTitlePane* title = static_cast< CAknTitlePane* >( 
            StatusPane()->ControlL( titlePaneUid ) );
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader, R_ATCONFIGURATION_APP_GUI_LIST_BOX_TITLE_RESOURCE );
        title->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // reader internal state
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::CleanupStatusPane()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::CleanupStatusPane()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::CleanupStatusPane()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::CreateContainerL()
// Creates the top-level container for the view.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBox* CATConfigurationAppGuiListBoxView::CreateContainerL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::CreateContainerL()" );
    
    return CATConfigurationAppGuiListBox::NewL( ClientRect(), this, this );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::HandleVersionMenuItemSelectedL()
// Shows About dialog of the application.
// -----------------------------------------------------------------------------
//
TBool CATConfigurationAppGuiListBoxView::HandleVersionMenuItemSelectedL( 
	TInt /*aCommand*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::HandleVersionMenuItemSelectedL()" );
    CAknMessageQueryDialog* dlg = new (ELeave) CAknMessageQueryDialog;
    dlg->ExecuteLD( R_ABOUT_HEADING_PANE );
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::UpdateViewL()
// Updates view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::UpdateViewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBoxView::UpdateViewL()" );
    
    if ( iATConfigurationAppGuiListBox )
        {
        iATConfigurationAppGuiListBox->UpdateViewL();
        } 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBoxView::EndProcessL()
// Ends process with a specific method.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBoxView::EndProcessL( TInt aCommand )
    {
    LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL" );  
                    
    TInt current = iATConfigurationAppGuiListBox->GetCurrentItemIndex();
    RArray<TATProcessInfo> processes;
    CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
    appUi->StorageServer().GetProcessesL( processes );
     if ( current >= KErrNone && current < processes.Count() )
         {
         RProcess endProcess;
         TUint processId = processes[ current ].iProcessId;
         TBuf<KMaxProcessName> processName;
         processName.Copy( processes[ current ].iProcessName );
         TInt find = processName.Find( _L( "." ) );
         if ( find > KErrNotFound )
             processName.Delete( find, processName.Length() - find );
         
         TInt openErr = endProcess.Open( TProcessId( processId ) );
         if ( openErr ==KErrNone )
             {
             if ( aCommand == EATConfigurationAppGuiListBoxViewEndTaskMenuItemCommand )
                 {
                 LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL - end task" );
                 endProcess.Close();

                 TApaTaskList apList = TApaTaskList( iEikonEnv->WsSession() );
                 TApaTask apTask = apList.FindApp( processName );
                 if ( apTask.Exists() )
                     {
                     LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL - task exists -> end" );
                     apTask.EndTask();
                     }
                 else
                     {
                     _LIT( KAppNotExist, "Could not end this process!" );
                     CAknErrorNote* note = new ( ELeave ) CAknErrorNote();
                     note->SetTextL( KAppNotExist() );
                     note->ExecuteLD();
                     }
                 }
             else if ( aCommand == EATConfigurationAppGuiListBoxViewKillProcessMenuItemCommand )
                 {
                 LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL - kill process" );
                 endProcess.Kill( KErrNone );
                 endProcess.Close();
                 }
             else if ( aCommand == EATConfigurationAppGuiListBoxViewTerminateProcessMenuItemCommand )
                 {
                 LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL - terminate process" );
                 endProcess.Terminate( KErrNone );
                 endProcess.Close();
                 }
             else if ( aCommand == EATConfigurationAppGuiListBoxViewPanicProcessMenuItemCommand )
                 {
                 LOGSTR1( "CATConfigurationAppGuiListBoxView::EndProcessL - panic process" );
                 _LIT( KPanicTxt, "Panic");
                 endProcess.Panic( KPanicTxt, KErrNone );
                 endProcess.Close();
                 }
             }
         else
             {
             CAknErrorNote* errorNote = new ( ELeave ) CAknErrorNote();
             _LIT( KErrTxt, "Could not open the process!");
             errorNote->SetTextL( KErrTxt );
             errorNote->ExecuteLD();
             }
         }   
	// Closes the array and frees all memory allocated to the array.
    processes.Close();
    }

// End of File
