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

#ifndef ATCONFIGURATIONAPPGUIPROCESSVIEW_H
#define ATCONFIGURATIONAPPGUIPROCESSVIEW_H

// INCLUDES
#include <aknview.h>
#include <analyzetool/atstorageserverclnt.h>
#include <analyzetool/atcommon.h>

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CATConfigurationAppGuiProcessContainer;

/**
 * Avkon view class for CATConfigurationAppGuiProcessView. It is register with the view server
 * by the AppUi. It owns the container control.
 * @class	CATConfigurationAppGuiProcessView
 */
class CATConfigurationAppGuiProcessView : public CAknView
	{
public:
    /**
    * C++ default constructor.
    */
	CATConfigurationAppGuiProcessView();
	
    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiProcessView* NewL();
	
    /**
    * Two-phased constructor that can leave and leaves a pointer
    * on the cleanup stack.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiProcessView* NewLC();
	
    /**
    *  Symbian 2nd phase constructor.
    */
	void ConstructL();
	
    /**
    * Destructor.
    */
	virtual ~CATConfigurationAppGuiProcessView();
						
    /**
    * From CAknView.
    */
	TUid Id() const;
	
    /**
    * From CAknView.
    */
	void HandleCommandL( TInt aCommand );
	
    /**
    * From CAknView.
    */
	void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
    * Creates control for this view.
    * @return CATConfigurationAppGuiListBox* pointer to the control
    */
	CATConfigurationAppGuiProcessContainer* CreateContainerL();
	
    /**
    * Creates an item for list query dialog.
    * @param aBuffer buffer holding the text
    * @param aMainText listbox text
    */
	void CreateListQuery1ItemL( 
			TDes& aBuffer,
			const TDesC& aMainText );
	
    /**
    * Creates list item from resource.
    * @param aBuffer buffer holding the text
    * @param aResourceId resource id
    */
	void CreateListQuery1ResourceArrayItemL( 
			TDes& aBuffer, 
			TInt aResourceId );

    /**
    * Initializes components in the list query.
    * @return CDesCArray* array of list items
    */
	CDesCArray* InitializeListQuery1LC();

    /**
    * Initializes icons in the list query.
    * @return CArrayPtr< CGulIcon >* icon array
    */
	CArrayPtr< CGulIcon >* SetupListQuery1IconsLC();

    /**
    * Executes list query including DLLs in a particular process.
    * @param aOverrideText heading for the list query
    * @param aOverrideItemArray array holding DLLs
    * @param aOverrideIconArray array holding icons
    */
	TInt RunListQueryL(
			const TDesC* aOverrideText = NULL, 
			CDesCArray* aOverrideItemArray = NULL,
			CArrayPtr< CGulIcon >* aOverrideIconArray = NULL );
	
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
private:
    /**
    * Setups status pane.
    */
	void SetupStatusPaneL();
	
    /**
    * Clears status pane and frees resources from it.
    */
	void CleanupStatusPaneL();

	// Current navi decorator
	CAknNavigationDecorator* iNaviDecorator_;
	// Control for this view
	CATConfigurationAppGuiProcessContainer* iProcessContainer;
	// Logging option for this process
	TInt   iLogOption;
	// Process Id of current process
	TUint  iProcessId;
	// Starting time of current process
	TInt64 iProcessStartTime;
	// Loaded libraries for this process
	RArray< TBuf8<KMaxLibraryName> > iLibraries;
	
    /**
    * @enum TListQuery1Images
    * Defines icons for list query.
    */
	enum TListQuery1Images
		{
		EListQuery1FirstUserImageIndex    /**< First icon in listbox. */
		};
	};

#endif // ATCONFIGURATIONAPPGUIPROCESSVIEW_H
