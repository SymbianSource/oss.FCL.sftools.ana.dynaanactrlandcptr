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

#ifndef ATCONFIGURATIONAPPGUISETTINGITEMLIST_H
#define ATCONFIGURATIONAPPGUISETTINGITEMLIST_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class MEikCommandObserver;
class CATConfigurationAppGuiSettingItemListSettings;

/**
 * @class	CATConfigurationAppGuiSettingItemList 
 */
class CATConfigurationAppGuiSettingItemList : public CAknSettingItemList
	{
public: // constructors and destructor
    /**
    * C++ default constructor.
    * @param aSettings setting list
    * @param aCommandObserver command observer
    */
	CATConfigurationAppGuiSettingItemList( 
	        CATConfigurationAppGuiSettingItemListSettings& aSettings, 
			MEikCommandObserver* aCommandObserver );
	
    /**
    * Destructor.
    */
	virtual ~CATConfigurationAppGuiSettingItemList();

public:

    /**
    * From CCoeControl.
    */
	void HandleResourceChange( TInt aType );

    /**
    * From CAknSettingItemList.
    */
	CAknSettingItem* CreateSettingItemL( TInt id );
	
    /**
    * From CAknSettingItemList.
    */
	void EditItemL( TInt aIndex, TBool aCalledFromMenu );
	
    /**
    * From CCoeControl.
    */
	TKeyResponse OfferKeyEventL( 
			const TKeyEvent& aKeyEvent, 
			TEventCode aType );

public:
    /**
    * Handles changing a setting value from the menu.
    */
	void ChangeSelectedItemL();

    /**
    * Loads stored setting values.
    */
	void LoadSettingValuesL();
	
    /**
    * Stores settings.
    */
	void SaveSettingValuesL();
		
private:
    /**
    * From CAknSettingItemList.
    */
	void SizeChanged();

private:
	// Current settings values
    CATConfigurationAppGuiSettingItemListSettings& iSettings;
    // Command observer
	MEikCommandObserver* iCommandObserver;
	
	};
#endif // ATCONFIGURATIONAPPGUISETTINGITEMLIST_H
