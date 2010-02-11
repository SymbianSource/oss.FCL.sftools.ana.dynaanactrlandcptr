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

#ifndef ATCONFIGURATIONAPPGUIPROCESSCONTAINER_H
#define ATCONFIGURATIONAPPGUIPROCESSCONTAINER_H

//INCLUDES
#include <coecntrl.h>

#include "atconfigurationappguiprocessview.h"

// FORWARD DECLARATIONS
class MEikCommandObserver;		
class CAknDoubleStyleListBox;
class CEikTextListBox;

/**
 * Container class for CATConfigurationAppGuiProcessContainer
 * 
 * @class	CATConfigurationAppGuiProcessContainer
 */
class CATConfigurationAppGuiProcessContainer : public CCoeControl
	{
public:
    /**
    * C++ default constructor.
    */
	CATConfigurationAppGuiProcessContainer();
	
    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiProcessContainer* NewL( 
		const TRect& aRect, 
		CATConfigurationAppGuiProcessView* aParent, 
		MEikCommandObserver* aCommandObserver, TUint aProcessId, TInt64 aProcessStartTime );
	
    /**
    * Two-phased constructor that can leave and leaves a pointer
    * on the cleanup stack.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiProcessContainer* NewLC( 
		const TRect& aRect, 
		CATConfigurationAppGuiProcessView* aParent, 
		MEikCommandObserver* aCommandObserver, TUint aProcessId, TInt64 aProcessStartTime );
	
    /**
    *  Symbian 2nd phase constructor.
    */
	void ConstructL( 
		const TRect& aRect, 
		CATConfigurationAppGuiProcessView* aParent, 
		MEikCommandObserver* aCommandObserver, TUint aProcessId, TInt64 aProcessStartTime );
	
    /**
    * Destructor.
    */
	virtual ~CATConfigurationAppGuiProcessContainer();
	
    /**
    * Updates view components.
    */
	void UpdateViewL();

public:

    /**
    * From CCoeControl.
    */
	TInt CountComponentControls() const;

    /**
    * From CCoeControl.
    */
	CCoeControl* ComponentControl( TInt aIndex ) const;
	
    /**
    * From CCoeControl.
    */
	TKeyResponse OfferKeyEventL( 
			const TKeyEvent& aKeyEvent, 
			TEventCode aType );
	
    /**
    * From CCoeControl.
    */
	void HandleResourceChange( TInt aType );
	
protected:

    /**
    * From CCoeControl.
    */
	void SizeChanged();

private:

    /**
    * From CCoeControl.
    */
	void Draw( const TRect& aRect ) const;

private:
    
    /**
    * Initializes all the controls in the class.
    */
	void InitializeControlsL();
    /**
    * Layouts controls according to the screen size.
    */
	void LayoutControls();
	
    // Active control
	CCoeControl* iFocusControl;
    // Command observer
	MEikCommandObserver* iCommandObserver;
	CATConfigurationAppGuiProcessView* iParent;

public: 
    
    /**
    * Adds an item to the listbox.
    * @param aListBox pointer to the listbox
    * @param aString a text about to be shown in the listbox.
    */
	static void AddListBoxItemL( 
			CEikTextListBox* aListBox,
			const TDesC& aString );
	
    /**
    * Returns selected listbox items.
    * @param aListBox pointer to the listbox
    * @return RArray< TInt >* array of selected items
    */
	static RArray< TInt >* GetSelectedListBoxItemsLC( CEikTextListBox* aListBox );
	
    /**
    * Deletes selected listbox items.
    * @param aListBox pointer to the listbox
    */
	static void DeleteSelectedListBoxItemsL( CEikTextListBox* aListBox );
	
    /**
    * Returns this controls listbox.
    */
	CAknDoubleStyleListBox* ListBox();
	
    /**
    * Creates listbox item from the given text.
    * @param aBuffer text to be appended to the listbox
    * @param aMainText a text for the first line of the item
    * @param aSecondaryText a text for the second line of the item
    */
	static void CreateListBoxItemL( TDes& aBuffer, 
			const TDesC& aMainText,
			const TDesC& aSecondaryText );
	
    /**
    * Starts adding listbox items from the resource.
    * @param aResourceId resource id of the text
    */
	void AddListBoxResourceArrayItemL( TInt aResourceId );
	
    /**
    * Setups listbox icons.
    */
	void SetupListBoxIconsL();
	
    /**
    * Handles listbox command.
    * @param aCommand command id
    */
	TBool HandleMarkableListCommandL( TInt aCommand );
	
    /**
    * Gets current highlighted listbox index.
    * @return TInt listbox index
    */
	TInt GetCurrentItemIndex();
	
private: 
    
    // Listbox
	CAknDoubleStyleListBox* iListBox;
	// Id of the opened process
	TUint iProcessId;
	// Starting time of the opened process
	TInt64 iProcessStartTime;

protected: 
	
public: 
    /**
    * @enum TControls
    * Defines controls.
    */
	enum TControls
		{
		EListBox,     /**< Listbox control. */
		ELastControl  /**< Last control in the view. */
		};
    /**
    * @enum TListBoxImages
    * Defines listbox icons.
    */
	enum TListBoxImages
		{
		EListBoxFirstUserImageIndex   /**< First icon in listbox. */
		};
	};
				
#endif // ATCONFIGURATIONAPPGUIPROCESSCONTAINER_H
