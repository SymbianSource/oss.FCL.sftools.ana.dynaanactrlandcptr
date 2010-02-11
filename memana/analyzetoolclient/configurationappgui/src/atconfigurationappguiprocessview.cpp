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
#include <aknnavide.h>
#include <aknnavi.h>
#include <eikbtgpc.h>
#include <aknlistquerydialog.h>
#include <atconfigurationappgui.rsg>

#include "atconfigurationappgui.hrh"
#include "atconfigurationappguiprocessview.h"
#include "atconfigurationappguiprocesscontainer.hrh"
#include "atconfigurationappguilistbox.hrh"
#include "atconfigurationappguiprocesscontainer.h"
#include "atconfigurationappguiappui.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::CATConfigurationAppGuiProcessView()
// First phase of Symbian two-phase construction.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessView::CATConfigurationAppGuiProcessView() :
    iLogOption( EATUseDefault )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::CATConfigurationAppGuiProcessView()" );
    
    iNaviDecorator_ = NULL;
    iProcessContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::~CATConfigurationAppGuiProcessView()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessView::~CATConfigurationAppGuiProcessView()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::~CATConfigurationAppGuiProcessView()" );
    
    if ( iNaviDecorator_ != NULL )
        {
        delete iNaviDecorator_;
        iNaviDecorator_ = NULL;
        }
    delete iProcessContainer;
    iProcessContainer = NULL;
    
    // Closes the array and frees all memory allocated to the array.
    iLibraries.Close();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::NewL()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessView* CATConfigurationAppGuiProcessView::NewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::NewL()" );
    
    CATConfigurationAppGuiProcessView* self = CATConfigurationAppGuiProcessView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::NewLC()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessView* CATConfigurationAppGuiProcessView::NewLC()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::NewLC()" );
    
    CATConfigurationAppGuiProcessView* self = new ( ELeave ) CATConfigurationAppGuiProcessView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::ConstructL()
// Second-phase constructor for view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::ConstructL()" );
    
    BaseConstructL( R_ATCONFIGURATION_APP_GUI_PROCESSCONTAINER_VIEW );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::Id()
// Returns the UID for this view.
// -----------------------------------------------------------------------------
//
TUid CATConfigurationAppGuiProcessView::Id() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::Id()" );
    
    return TUid::Uid( EATConfigurationAppGuiProcessViewId );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::HandleCommandL()
// Handle a command for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::HandleCommandL( TInt aCommand )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::HandleCommandL()" );
    
    // [[[ begin generated region: do not modify [Generated Code]
    TBool commandHandled = EFalse;
    switch ( aCommand )
        { // Code to dispatch to the AknView's menu and CBA commands is generated here
        case EATConfigurationAppGuiProcessView_librariesMenuItemCommand:
        	RunListQueryL();
            commandHandled = ETrue;
            break;
        case EAknSoftkeyBack:
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( EATConfigurationAppGuiListBoxViewId ) );
            commandHandled = ETrue;
            break;
            }
        case EATConfigurationAppGuiProcessViewSettingsMenuItemCommand:
            {
            AppUi()->ActivateLocalViewL( TUid::Uid( EATConfigurationAppGuiSettingItemListViewId ),
                    TUid::Uid( EATConfigurationAppGuiProcessViewId ), KNullDesC8 );
            commandHandled = ETrue;
            break;
            }
        case EATConfigurationAppGuiProcessViewEXTILogOption:
            {
            iLogOption = EATLogToXti;
            commandHandled = ETrue;
            break;
            }
        case EATConfigurationAppGuiProcessViewES60LogOption:
            {
            iLogOption = EATLogToFile;
            commandHandled = ETrue;
            break;
            }
        case EATConfigurationAppGuiProcessViewENoneLogOption:
            {
            iLogOption = EATLoggingOff;
            commandHandled = ETrue;
            break;
            }
        case EATConfigurationAppGuiProcessViewStart_subtestMenuItemCommand:
            {
            CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
            appUi->SetProcessSubTestStartL( iProcessId );
            SetupStatusPaneL();
            if ( iProcessContainer )
                {
                iProcessContainer->UpdateViewL();
                }
            break;
            }
        case EATConfigurationAppGuiProcessViewStop_subtestMenuItemCommand:
            {
            CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
            appUi->SetProcessSubTestStopL( iProcessId );
            SetupStatusPaneL();
            if ( iProcessContainer )
                {
                iProcessContainer->UpdateViewL();
                }
            break;
            }
        default:
            break;
        }
        
    if ( !commandHandled ) 
        {

        if ( aCommand == EAknSoftkeyBack || 
             aCommand == EATConfigurationAppGuiProcessViewExitMenuItemCommand )
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::DynInitMenuPaneL()
// Handles dynamically dimming the menu items.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::DynInitMenuPaneL( TInt aResourceId, 
	CEikMenuPane* aMenuPane )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::DynInitMenuPaneL()" );
    
    if ( aResourceId == R_ATCONFIGURATION_APP_GUI_PROCESS_MENU_PANE )
        {
        if( iProcessContainer->GetCurrentItemIndex() < 0 ) 
            {
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStart_subtestMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStop_subtestMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessView_librariesMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewSet_logging_modeMenuItemCommand, ETrue );
            } 
        else {
            CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
            TInt subTest = appUi->IsSubTestRunning( iProcessId );
            if ( subTest > KErrNotFound )
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStart_subtestMenuItemCommand, ETrue );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStop_subtestMenuItemCommand, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStart_subtestMenuItemCommand, EFalse );
                aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewStop_subtestMenuItemCommand, ETrue );
                }

            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessView_librariesMenuItemCommand, EFalse );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewSet_logging_modeMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewHelpMenuItemCommand, ETrue );
            aMenuPane->SetItemDimmed( EATConfigurationAppGuiProcessViewSettingsMenuItemCommand, ETrue );
            }
        }
        
    if ( aResourceId == R_ATCONFIGURATION_APP_GUI_RADIOBUTTON_MENUPANE )
        {
        if ( iLogOption == EATUseDefault )
            aMenuPane->SetItemButtonState( EATConfigurationAppGuiProcessViewEXTILogOption, 
                                           EEikMenuItemSymbolOn );
        else if ( iLogOption == EATLogToFile )
            aMenuPane->SetItemButtonState( EATConfigurationAppGuiProcessViewES60LogOption, 
                                           EEikMenuItemSymbolOn );
        else if ( iLogOption == EATLogToXti )
            aMenuPane->SetItemButtonState( EATConfigurationAppGuiProcessViewEXTILogOption, 
                                           EEikMenuItemSymbolOn );
        else if ( iLogOption == EATLoggingOff )
            aMenuPane->SetItemButtonState( EATConfigurationAppGuiProcessViewENoneLogOption, 
                                           EEikMenuItemSymbolOn );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::DoActivateL()
// Handles user actions during activation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::DoActivateL( 
    const TVwsViewId& /*aPrevViewId*/, TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::DoActivateL()" );
    
    CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )AppUi();
    iProcessId = appUi->GetCurrentProcessId();
    iProcessStartTime = appUi->GetCurrentProcessStartTime();

    SetupStatusPaneL();

    if ( iProcessContainer == NULL )
        {
        iProcessContainer = CreateContainerL();
        iProcessContainer->SetMopParent( this );
        AppUi()->AddToStackL( *this, iProcessContainer );
        // For supporting MSK context menu
        MenuBar()->SetContextMenuTitleResourceId( R_ATCONFIGURATION_PROCESS_OPTIONS_MENU );
        } 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::DoDeactivate()
// Handles user actions during deactivation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::DoDeactivate()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::DoDeactivate()" );
    
    TRAP_IGNORE( CleanupStatusPaneL() );

    if ( iProcessContainer != NULL )
        {
        AppUi()->RemoveFromViewStack( *this, iProcessContainer );
        delete iProcessContainer;
        iProcessContainer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::HandleStatusPaneSizeChange()
// Handle status pane size change for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::HandleStatusPaneSizeChange()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::HandleStatusPaneSizeChange()" );
    
    CAknView::HandleStatusPaneSizeChange();

    // This may fail, but we're not able to propagate exceptions here
    TInt result;
    TRAP( result, SetupStatusPaneL() ); 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::SetupStatusPaneL()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::SetupStatusPaneL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::SetupStatusPaneL()" );
    
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
        //TResourceReader reader;
        //iEikonEnv->CreateResourceReaderLC( reader, R_ATCONFIGURATION_APP_GUI_TITLE_RESOURCE );
        //title->SetFromResourceL( reader );
        //CleanupStack::PopAndDestroy(); // reader internal state
        
        // Get process's name
        RArray<TATProcessInfo> processes;
        CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
        appUi->StorageServer().GetProcessesL( processes );

        for ( TInt find = 0 ; find < processes.Count() ; find++ )
            {
            TATProcessInfo processInfo = processes[ find ];
            if ( processInfo.iProcessId == iProcessId )
                {
                HBufC* name = HBufC::NewLC( processInfo.iProcessName.Length() );
                name->Des().Copy( processInfo.iProcessName );
                title->SetTextL( name->Des(), ETrue );
                CleanupStack::PopAndDestroy( name );
                break;
                }
            }
		// Closes the array and frees all memory allocated to the array.
        processes.Close();
        }
        
    // Set the navi pane content
    TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
    CEikStatusPaneBase::TPaneCapabilities subPaneNavi = 
        StatusPane()->PaneCapabilities( naviPaneUid );
    if ( subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned() )
        {
        CAknNavigationControlContainer* naviPane = 
            static_cast< CAknNavigationControlContainer* >( 
                StatusPane()->ControlL( naviPaneUid ) );
        if ( iNaviDecorator_ != NULL )
            {
            delete iNaviDecorator_;
            iNaviDecorator_ = NULL;
            }
        // Check if subtest is running
        CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
        TInt subTest = appUi->IsSubTestRunning( iProcessId );
        if ( subTest > KErrNotFound )
            {
            HBufC* labelText = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_NAVI_TEXT1 );
            iNaviDecorator_ = naviPane->CreateNavigationLabelL( labelText->Des() );  
            CleanupStack::PopAndDestroy( labelText );
            }
        else
            {
            iNaviDecorator_ = naviPane->CreateNavigationLabelL( KNullDesC ); 
            }
        naviPane->PushL( *iNaviDecorator_ );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::CleanupStatusPaneL()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::CleanupStatusPaneL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::CleanupStatusPaneL()" );
    
    // Reset the navi pane 
    TUid naviPaneUid = TUid::Uid( EEikStatusPaneUidNavi );
    CEikStatusPaneBase::TPaneCapabilities subPaneNavi = 
        StatusPane()->PaneCapabilities( naviPaneUid );
    if ( subPaneNavi.IsPresent() && subPaneNavi.IsAppOwned() )
        {
        CAknNavigationControlContainer* naviPane = 
            static_cast< CAknNavigationControlContainer* >( 
                StatusPane()->ControlL( naviPaneUid ) );
        if ( iNaviDecorator_ != NULL )
            {
            naviPane->Pop( iNaviDecorator_ );
            delete iNaviDecorator_;
            iNaviDecorator_ = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::CreateContainerL()
// Creates the top-level container for the view.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessContainer* CATConfigurationAppGuiProcessView::CreateContainerL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::CreateContainerL()" );
    
    return CATConfigurationAppGuiProcessContainer::NewL( 
    		ClientRect(), this, this, iProcessId, iProcessStartTime );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::CreateListQuery1ItemL()
// Create a list box item with the given column values.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::CreateListQuery1ItemL( 
    TDes& aBuffer, const TDesC& aMainText )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::CreateListQuery1ItemL()" );
    
    _LIT ( KStringHeader, "%S" );

    aBuffer.Format( KStringHeader(), &aMainText );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::CreateListQuery1ResourceArrayItemL()
// Add an item to the list by reading the text items from resource.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessView::CreateListQuery1ResourceArrayItemL( 
        TDes& aBuffer, 
        TInt aResourceId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::CreateListQuery1ResourceArrayItemL()" );
    
    CDesCArray* array = CCoeEnv::Static()->ReadDesCArrayResourceL( aResourceId );
    CleanupStack::PushL( array );
    CreateListQuery1ItemL( aBuffer, ( *array ) [ 0 ] );
    CleanupStack::PopAndDestroy( array );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::InitializeListQuery1LC()
// Initialize contents of the popup item list.
// -----------------------------------------------------------------------------
//
CDesCArray* CATConfigurationAppGuiProcessView::InitializeListQuery1LC()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::InitializeListQuery1LC()" );
    
    ( ( CATConfigurationAppGuiAppUi* )AppUi() )->StorageServer().GetLoadedDllsL( iProcessId, iLibraries );
    const int KNumItems = iLibraries.Count();
    CDesCArray* itemArray = new ( ELeave ) CDesCArrayFlat( KNumItems ? KNumItems : 1 );
    CleanupStack::PushL( itemArray );
    // This is intended to be large enough, but if you get 
    // a USER 11 panic, consider reducing string sizes.

    for ( TInt count = 0 ; count < KNumItems ; count++ )
        {
        TBuf<512> des;
        TBuf<KMaxLibraryName> library;
        library.Copy( iLibraries[ count ] );
        CreateListQuery1ItemL( des, library );
        itemArray->AppendL( des );
        }
    
    // Empties the array, so that it is ready to be reused.
    iLibraries.Reset();
    return itemArray;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::SetupListQuery1IconsLC()
// Set up the list query's icon array.
// -----------------------------------------------------------------------------
//
CArrayPtr< CGulIcon >* CATConfigurationAppGuiProcessView::SetupListQuery1IconsLC()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::SetupListQuery1IconsLC()" );
    
    CArrayPtr< CGulIcon >* icons = NULL;
    return icons;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessView::RunListQueryL()
// Show the popup list query dialog for listQuery.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiProcessView::RunListQueryL( 
    const TDesC* aOverrideText, CDesCArray* aOverrideItemArray,
    CArrayPtr< CGulIcon >* aOverrideIconArray )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessView::RunListQueryL()" );
    
    TInt index = 0;
    CAknListQueryDialog* queryDialog = NULL;
    queryDialog = new ( ELeave ) CAknListQueryDialog( &index );
    CleanupStack::PushL( queryDialog );

    queryDialog->PrepareLC( R_ATCONFIGURATION_APP_GUI_LIST_QUERY1 );
    if ( aOverrideText != NULL )
        {
        queryDialog->SetHeaderTextL( *aOverrideText );
        }
        
    // Initialize list items
    CDesCArray* itemArray = NULL;

    if ( aOverrideItemArray != NULL ) 
        {
        CleanupStack::PushL( aOverrideItemArray );
        itemArray = aOverrideItemArray;
        }
    else
        {
        itemArray = InitializeListQuery1LC();
        }
        
    queryDialog->SetItemTextArray( itemArray );
    queryDialog->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( itemArray );

    // Initialize list icons
    CArrayPtr< CGulIcon >* iconArray = NULL;

    if ( aOverrideIconArray != NULL )
        {
        CleanupStack::PushL( aOverrideIconArray );
        iconArray = aOverrideIconArray;
        }
    else
        {
        iconArray = SetupListQuery1IconsLC();
        }
        
    if ( iconArray != NULL ) 
        {
        queryDialog->SetIconArrayL( iconArray ); // Passes ownership
        CleanupStack::Pop( iconArray );
        }
    // Enable text scrolling
    queryDialog->ListControl()->Listbox()->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    // Run dialog
    TInt result = queryDialog->RunLD();

    // Clean up
    CleanupStack::Pop( queryDialog );

    return result == 0 ? -1 : index;
    }

// End of File
