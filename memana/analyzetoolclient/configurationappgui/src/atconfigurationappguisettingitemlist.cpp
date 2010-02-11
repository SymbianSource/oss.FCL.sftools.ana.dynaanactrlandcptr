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

#include <avkon.hrh>
#include <avkon.rsg>
#include <eikmenup.h>
#include <aknappui.h>
#include <eikcmobs.h>
#include <barsread.h>
#include <stringloader.h>
#include <aknpopupfieldtext.h>
#include <eikenv.h>
#include <eikappui.h>
#include <aknviewappui.h>
#include <aknslidersettingpage.h> 
#include <atconfigurationappgui.rsg>

#include "atconfigurationappguisettingitemlist.h"
#include "atconfigurationappguisettingitemlistsettings.h"
#include "atconfigurationappgui.hrh"
#include "atconfigurationappguisettingitemlist.hrh"
#include "atconfigurationappguisettingitemlistview.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::CATConfigurationAppGuiSettingItemList()
// Construct the CATConfigurationAppGuiSettingItemList instance.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemList::CATConfigurationAppGuiSettingItemList( 
        CATConfigurationAppGuiSettingItemListSettings& aSettings, 
        MEikCommandObserver* aCommandObserver )
    : iSettings( aSettings ), iCommandObserver( aCommandObserver )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::CATConfigurationAppGuiSettingItemList()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::~CATConfigurationAppGuiSettingItemList()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemList::~CATConfigurationAppGuiSettingItemList()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::~CATConfigurationAppGuiSettingItemList()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::SizeChanged()
// Handle system notification that the container's size has changed.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::SizeChanged()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::SizeChanged()" );
    
    if ( ListBox() ) 
        {
        ListBox()->SetRect( Rect() );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::CreateSettingItemL()
// Create one setting item at a time, identified by id.
// -----------------------------------------------------------------------------
//
CAknSettingItem* CATConfigurationAppGuiSettingItemList::CreateSettingItemL( 
	TInt aId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::CreateSettingItemL()" );
    
    switch ( aId )
        {
        case EATConfigurationAppGuiSettingItemListViewBinarySetting1:
            {
            CAknBinaryPopupSettingItem* item = new ( ELeave ) 
                CAknBinaryPopupSettingItem( 
                    aId,
                    iSettings.BinarySetting1() );
            return item;
            }
        case EATConfigurationAppGuiSettingItemListViewEnumeratedTextPopup1:
            {
            //CAknEnumeratedTextPopupSettingItem* item = new ( ELeave ) 
            //CAknEnumeratedTextPopupSettingItem( aId, iSettings.EnumeratedTextPopup1() );
            CAknTextSettingItem* item = new (ELeave) CAknTextSettingItem( aId, iSettings.EnumeratedTextPopup1() );
            return item;
            }
        }
        
    return NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::EditItemL()
// Edit the setting item identified by the given id.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::EditItemL ( TInt aIndex, 
	TBool aCalledFromMenu )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::EditItemL()" );
    
    CAknSettingItem* item = ( *SettingItemArray() )[aIndex];

    if ( ListBox()->CurrentItemIndex() == 1 )// Update interval
        {
        TInt setting( iSettings.UpdateInterval() );
        CAknSliderSettingPage* settPage = new (ELeave) CAknSliderSettingPage
                  ( R_ATCONFIGURATION_APP_GUI_SLIDER_SETTING_PAGE, setting );
        CleanupStack::PushL( settPage );
        
        TBool accepted = settPage ->ExecuteLD
            ( CAknSettingPage::EUpdateWhenChanged );
        
        if ( accepted )
            {
            iSettings.SetUpdateIntervalL( setting );    
            }
        CleanupStack::Pop( settPage );
        item->LoadL();
        item->UpdateListBoxTextL();
        }
    else
        {
        switch ( item->Identifier() )
            {
            }
        
        CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );

        switch ( item->Identifier() )
            {
            }

        item->StoreL();
        SaveSettingValuesL();  
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::ChangeSelectedItemL()
// Handle the "Change" option on the Options menu.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::ChangeSelectedItemL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::ChangeSelectedItemL()" );
    
    if ( ListBox()->CurrentItemIndex() >= 0 )
        {
        EditItemL( ListBox()->CurrentItemIndex(), ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::LoadSettingValuesL()
// Load the initial contents of the setting items.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::LoadSettingValuesL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::LoadSettingValuesL()" );
    // Load values into iSettings
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::SaveSettingValuesL()
// Save the contents of the setting items.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::SaveSettingValuesL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::SaveSettingValuesL()" );
    // Store values from iSettings
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::HandleResourceChange()
// Handle global resource changes.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemList::HandleResourceChange( TInt aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::HandleResourceChange()" );
    CAknSettingItemList::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( TUid::Uid( EATConfigurationAppGuiSettingItemListViewId ) )->ClientRect() );
    }
                
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemList::OfferKeyEventL()
// Handle key event.
// -----------------------------------------------------------------------------
//
TKeyResponse CATConfigurationAppGuiSettingItemList::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemList::OfferKeyEventL()" );
    
    if ( aKeyEvent.iCode == EKeyLeftArrow 
        || aKeyEvent.iCode == EKeyRightArrow )
        {
        // Allow the tab control to get the arrow keys
        return EKeyWasNotConsumed;
        }

    return CAknSettingItemList::OfferKeyEventL( aKeyEvent, aType );
    }

// End of File
