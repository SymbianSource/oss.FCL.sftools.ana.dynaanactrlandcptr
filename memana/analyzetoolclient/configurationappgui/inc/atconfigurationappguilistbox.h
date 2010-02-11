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

#ifndef ATCONFIGURATIONAPPGUILISTBOX_H
#define ATCONFIGURATIONAPPGUILISTBOX_H

// INCLUDES
#include <coecntrl.h>        
#include <analyzetool/atstorageserverclnt.h>

#include "atconfigurationappguilistboxview.h"

// FORWARD DECLARATIONS
class MEikCommandObserver;        
class CAknDoubleStyleListBox;
class CEikTextListBox;

/**
 * Container class for ATConfigurationAppGuiListBox
 * 
 * @class CATConfigurationAppGuiListBox ATConfigurationAppGuiListBox.h
 */
class CATConfigurationAppGuiListBox : public CCoeControl, public MEikListBoxObserver
    {
public:
    /**
    * C++ default constructor.
    */
    CATConfigurationAppGuiListBox();

    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
    static CATConfigurationAppGuiListBox* NewL( 
        const TRect& aRect, 
        CATConfigurationAppGuiListBoxView* aParent, 
        MEikCommandObserver* aCommandObserver );

    /**
    * Two-phased constructor that can leave and leaves a pointer
    * on the cleanup stack.
    * @return A new instance of this class
    */
    static CATConfigurationAppGuiListBox* NewLC( 
        const TRect& aRect, 
        CATConfigurationAppGuiListBoxView* aParent, 
        MEikCommandObserver* aCommandObserver );
 
    /**
    *  Symbian 2nd phase constructor.
    */
    void ConstructL( 
        const TRect& aRect, 
        CATConfigurationAppGuiListBoxView* aParent, 
        MEikCommandObserver* aCommandObserver );

    /**
    * Destructor.
    */
    virtual ~CATConfigurationAppGuiListBox();

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
    
    /**
    * Updates view's components.
    */
    void UpdateViewL();

    /**
    * From CCoeControl.
    */
    void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
    * From MEikListBoxObserver.
    */    
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
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
 
    /**
    * Updates processes in the view.
    */
    void UpdateProcessesL();
    
private: 
    // Listbox
    CAknDoubleStyleListBox* iListBox;
    
    // Array of processes under testing
    RArray<TATProcessInfo> iProcesses;
    CATConfigurationAppGuiListBoxView* iParent;
public:
    /**
    * @enum TControls
    * Defines controls.
    */
    enum TControls
        {
        EListBox,       /**< Listbox control. */
        ELastControl    /**< Last control in the view. */
        };
    /**
    * @enum TListBoxImages
    * Defines listbox icons.
    */
    enum TListBoxImages
        {
        EListBoxFirstUserImageIndex /**< First icon in listbox. */
        };
    };
                
#endif // ATCONFIGURATIONAPPGUILISTBOX_H
