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

#ifndef ATCONFIGURATIONAPPGUILISTBOXVIEW_H
#define ATCONFIGURATIONAPPGUILISTBOXVIEW_H

// INCLUDES
#include <aknview.h>

// FORWARD DECLARATIONS
class CATConfigurationAppGuiListBox;

/**
 * Avkon view class for ATConfigurationAppGuiListBoxView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class CATConfigurationAppGuiListBoxView ATConfigurationAppGuiListBoxView.h
 */
class CATConfigurationAppGuiListBoxView : public CAknView
    {
public:
    /**
    * C++ default constructor.
    */
    CATConfigurationAppGuiListBoxView();
    
    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
    static CATConfigurationAppGuiListBoxView* NewL();
    
    /**
    * Two-phased constructor that can leave and leaves a pointer
    * on the cleanup stack.
    * @return A new instance of this class
    */
    static CATConfigurationAppGuiListBoxView* NewLC();
    
    /**
    *  Symbian 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Destructor.
    */
    virtual ~CATConfigurationAppGuiListBoxView();
                        
    /**
    * From CAknView.
    */
    TUid Id() const;
    
    /**
    * From CAknView.
    */
    void HandleCommandL( TInt aCommand );
    
    /**
    * Creates control for this view.
    * @return CATConfigurationAppGuiListBox* pointer to the control
    */
    CATConfigurationAppGuiListBox* CreateContainerL();
    
    /**
    * From CAknView.
    */
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
    
    /**
    * Launches view's menu.
    */
    //void LaunchMenuL();
 
    /**
    * Updates view.
    */
    void UpdateViewL();

    /**
    * Ends a process with a certain method.
    * @param aCommand command id
    */
    void EndProcessL( TInt aCommand );
    
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
    * Handles About -menu item execution.
    * @param aCommand command id
    * @return TBool not used
    */
    TBool HandleVersionMenuItemSelectedL( TInt aCommand );
    
private:
    
    /**
    * Setups status pane.
    */
    void SetupStatusPaneL();
    
    /**
    * Clears status pane and frees resources from it.
    */
    void CleanupStatusPane();
    
    // Pointer to the control class
    CATConfigurationAppGuiListBox* iATConfigurationAppGuiListBox;
    
    };

#endif // ATCONFIGURATIONAPPGUILISTBOXVIEW_H
