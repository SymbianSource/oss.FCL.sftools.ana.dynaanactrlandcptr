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


#include <e32base.h>
#include <stringloader.h>
#include <barsread.h>
#include <atconfigurationappgui.rsg>
#include "atconfigurationappguisettingitemlistsettings.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::CATConfigurationAppGuiSettingItemListSettings()
// C/C++ constructor for settings data.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListSettings::CATConfigurationAppGuiSettingItemListSettings() :
    iUpdateInterval( 0 )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::CATConfigurationAppGuiSettingItemListSettings()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::NewL()
// Two-phase constructor for settings data.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiSettingItemListSettings* CATConfigurationAppGuiSettingItemListSettings::NewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::NewL()" );
    
    CATConfigurationAppGuiSettingItemListSettings* data = new( ELeave ) CATConfigurationAppGuiSettingItemListSettings;
    CleanupStack::PushL( data );
    data->ConstructL();
    CleanupStack::Pop( data );
    return data;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::ConstructL()
// Second phase for initializing settings data.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListSettings::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::ConstructL()" );
    
    SetBinarySetting1( 1 );
    SetEnumeratedTextPopup1L( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::BinarySetting1()
// -----------------------------------------------------------------------------
//
TBool& CATConfigurationAppGuiSettingItemListSettings::BinarySetting1()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::BinarySetting1()" );
    
    return iBinarySetting1;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::SetBinarySetting1()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListSettings::SetBinarySetting1( 
	const TBool& aValue )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::SetBinarySetting1()" );
    
    iBinarySetting1 = aValue;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::EnumeratedTextPopup1()
// -----------------------------------------------------------------------------
//
TDes& CATConfigurationAppGuiSettingItemListSettings::EnumeratedTextPopup1()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::EnumeratedTextPopup1()" );
    
    return iTemp;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::UpdateInterval()
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiSettingItemListSettings::UpdateInterval()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::UpdateInterval()" );
    
    return iUpdateInterval;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::SetUpdateIntervalL()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListSettings::SetUpdateIntervalL( 
	TInt aUpdateInterval )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::SetUpdateIntervalL()" );
    
    iUpdateInterval = aUpdateInterval;
    // Update also the text setting shown to the 
    // user
    SetEnumeratedTextPopup1L( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiSettingItemListSettings::SetEnumeratedTextPopup1L()
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiSettingItemListSettings::SetEnumeratedTextPopup1L( 
	const TDesC& /*aValue*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiSettingItemListSettings::SetEnumeratedTextPopup1L()" );
    
    _LIT( KATSpace, " " );
    iTemp.Zero();
    if ( iUpdateInterval > 1 )
        {
        iTemp.AppendNum( iUpdateInterval );
        HBufC* minutes = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_SETTING_ITEM_SECONDS );
        iTemp.Append( KATSpace );
        iTemp.Append( minutes->Des() );
        CleanupStack::PopAndDestroy( minutes );
        }
    else if ( iUpdateInterval == KErrNone )
        {
        HBufC* offText = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_SETTING_ITEM_OFF );
        iTemp.Append( offText->Des() );
        CleanupStack::PopAndDestroy( offText );
        }
    /*else
        {
            iTemp.AppendNum( iUpdateInterval );
        HBufC* minute = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_SETTING_ITEM_MINUTE );
        iTemp.Append( KATSpace );
        iTemp.Append( minute->Des() );
        CleanupStack::PopAndDestroy( minute );
        }*/
    }

// End of File
