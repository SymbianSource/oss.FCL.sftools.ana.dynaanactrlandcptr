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
#include <akncontext.h>
#include <akntitle.h>
#include <stringloader.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <atconfigurationappgui.rsg>

#include "atconfigurationappgui.hrh"
#include "atconfigurationappguisettingitemlistview.h"
#include "atconfigurationappguisettingitemlist.hrh"
#include "atconfigurationappguisettingitemlist.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::CATConfigurationAppGuiSettingItemListView()
// First phase of Symbian two-phase construction.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListView::CATConfigurationAppGuiSettingItemListView() :
iLastViewId( TUid::Uid( EATConfigurationAppGuiListBoxViewId ) )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::CATConfigurationAppGuiSettingItemListView()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::~CATConfigurationAppGuiSettingItemListView()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListView::~CATConfigurationAppGuiSettingItemListView()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::~CATConfigurationAppGuiSettingItemListView()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::NewL()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListView* CATConfigurationAppGuiSettingItemListView::NewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::NewL()" );
    
    CATConfigurationAppGuiSettingItemListView* self = CATConfigurationAppGuiSettingItemListView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::NewLC()
// Symbian two-phase constructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListView* CATConfigurationAppGuiSettingItemListView::NewLC()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::NewLC()" );
    
    CATConfigurationAppGuiSettingItemListView* self = new ( ELeave ) CATConfigurationAppGuiSettingItemListView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::ConstructL()
// Second-phase constructor for view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::ConstructL()" );
    
    BaseConstructL( R_ATCONFIGURATION_APP_GUI_SETTING_ITEM_LIST_VIEW );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::Id()
// Returns the UID for this view.
// -----------------------------------------------------------------------------
//
TUid CATConfigurationAppGuiSettingItemListView::Id() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::Id()" );
    
    return TUid::Uid( EATConfigurationAppGuiSettingItemListViewId );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::HandleCommandL()
// Handle a command for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::HandleCommandL( TInt aCommand )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::HandleCommandL()" );
    
    TBool commandHandled = EFalse;
    switch ( aCommand )
        { // Code to dispatch to the AknView's menu and CBA commands is generated here
        case EATConfigurationAppGuiSettingItemListViewMenuItem1Command:
            commandHandled = HandleChangeSelectedSettingItemL( aCommand );
            break;
        case EAknSoftkeyBack:
            {
            AppUi()->ActivateLocalViewL( iLastViewId );
            commandHandled = ETrue;
            break;    
            }
        default:
            break;
        }

        
    if ( !commandHandled ) 
        {

        if ( aCommand == EAknSoftkeyBack )
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            }

        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::DoActivateL()
// Handles user actions during activation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::DoActivateL( 
        const TVwsViewId& /*aPrevViewId*/,
        TUid aCustomMessageId,
        const TDesC8& /*aCustomMessage*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::DoActivateL()" );
    
    SetupStatusPaneL();
    iLastViewId = aCustomMessageId;

    if ( iATConfigUISettingItemList == NULL )
        {
        iSettings = CATConfigurationAppGuiSettingItemListSettings::NewL();
        iATConfigUISettingItemList = new ( ELeave ) CATConfigurationAppGuiSettingItemList( *iSettings, this );
        iATConfigUISettingItemList->SetMopParent( this );
        iATConfigUISettingItemList->ConstructFromResourceL( R_ATCONFIGURATION_APP_GUI_SETTING_ITEM_LIST );
        iATConfigUISettingItemList->ActivateL();
        iATConfigUISettingItemList->LoadSettingValuesL();
        iATConfigUISettingItemList->LoadSettingsL();
        AppUi()->AddToStackL( *this, iATConfigUISettingItemList );
        } 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::DoDeactivate()
// Handles user actions during deactivation of the view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::DoDeactivate()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::DoDeactivate()" );
    
    CleanupStatusPane();

    if ( iATConfigUISettingItemList != NULL )
        {
        AppUi()->RemoveFromStack( iATConfigUISettingItemList );
        delete iATConfigUISettingItemList;
        iATConfigUISettingItemList = NULL;
        delete iSettings;
        iSettings = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::HandleStatusPaneSizeChange()
// Handle status pane size change for this view.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::HandleStatusPaneSizeChange()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::HandleStatusPaneSizeChange()" );
    
    CAknView::HandleStatusPaneSizeChange();

    // This may fail, but we're not able to propagate exceptions here
    TInt result;
    TRAP( result, SetupStatusPaneL() ); 
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::SetupStatusPaneL()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::SetupStatusPaneL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::SetupStatusPaneL()" );
    
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
        iEikonEnv->CreateResourceReaderLC( reader, R_ATCONFIGURATION_APP_GUI_SETTINGS_TITLE_RESOURCE );
        title->SetFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // Reader internal state
        }
                
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::CleanupStatusPane()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListView::CleanupStatusPane()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::CleanupStatusPane()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListView::HandleChangeSelectedSettingItemL()
// Handle the selected event.
// -----------------------------------------------------------------------------
//
TBool CATConfigurationAppGuiSettingItemListView::HandleChangeSelectedSettingItemL( 
	TInt /*aCommand*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListView::HandleChangeSelectedSettingItemL()" );
    
    iATConfigUISettingItemList->ChangeSelectedItemL();
    return ETrue;
    }

// End of File
