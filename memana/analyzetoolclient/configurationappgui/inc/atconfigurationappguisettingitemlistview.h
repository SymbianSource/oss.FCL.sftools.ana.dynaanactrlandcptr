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

#ifndef ATCONFIGURATIONAPPGUISETTINGITEMLISTVIEW_H
#define ATCONFIGURATIONAPPGUISETTINGITEMLISTVIEW_H

// INCLUDES
#include <aknview.h>
#include "atconfigurationappguisettingitemlistsettings.h"

// FORWARD DECLARATIONS
class CATConfigurationAppGuiSettingItemList;

/**
 * Avkon view class for CATConfigurationAppGuiSettingItemListView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CATConfigurationAppGuiSettingItemListView
 */						
			
class CATConfigurationAppGuiSettingItemListView : public CAknView
	{
public:
    /**
    * C++ default constructor.
    */
	CATConfigurationAppGuiSettingItemListView();
	
    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiSettingItemListView* NewL();
	
    /**
    * Two-phased constructor that can leave and leaves a pointer
    * on the cleanup stack.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiSettingItemListView* NewLC();  
	
    /**
    *  Symbian 2nd phase constructor.
    */
	void ConstructL();
	
    /**
    * Destructor.
    */
	virtual ~CATConfigurationAppGuiSettingItemListView();
						
    /**
    * From CAknView.
    */
	TUid Id() const;
	
    /**
    * From CAknView.
    */
	void HandleCommandL( TInt aCommand );
	
protected:
    
    /**
    * From CAknView.
    */
	void DoActivateL(
		const TVwsViewId& aPrevViewId,
		TUid aCustomMessageId,
		const TDesC8& aCustomMessage );
	
    /**
    * From CAknView.
    */
	void DoDeactivate();
	
    /**
    * Setups status pane according to the screen size.
    */
	void HandleStatusPaneSizeChange();
	
    /**
    * Handles changing of a selected setting item.
    * @param aCommand command id
    * @return TBool ETrue if setting change successful
    */
	TBool HandleChangeSelectedSettingItemL( TInt aCommand );
	
private:
    
    /**
    * Setups status pane.
    */
	void SetupStatusPaneL();
	
    /**
    * Clears status pane and frees resources from it.
    */
	void CleanupStatusPane();
	
	// Setting item list
	CATConfigurationAppGuiSettingItemList* iATConfigUISettingItemList;
	// Values of the settings
	CATConfigurationAppGuiSettingItemListSettings* iSettings;
	// Id of the view which was previously active
	TUid   iLastViewId;
	
	};

#endif // ATCONFIGURATIONAPPGUISETTINGITEMLISTVIEW_H
