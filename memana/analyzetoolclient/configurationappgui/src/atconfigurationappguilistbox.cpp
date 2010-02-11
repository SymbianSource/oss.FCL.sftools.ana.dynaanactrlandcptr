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

#include <barsread.h>
#include <stringloader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <aknviewappui.h>
#include <eikappui.h>
#include <atconfigurationappgui.rsg>
#include <aknnotewrappers.h>

#include "atconfigurationappguilistbox.h"
#include "atconfigurationappguilistboxview.h"
#include "atconfigurationappgui.hrh"
#include "atconfigurationappguilistbox.hrh"
#include "atconfigurationappguiappui.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::CATConfigurationAppGuiListBox()
// First phase of Symbian two-phase construction.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBox::CATConfigurationAppGuiListBox()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::CATConfigurationAppGuiListBox()" );
    
    iListBox = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::~CATConfigurationAppGuiListBox()
// Destroy child controls.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBox::~CATConfigurationAppGuiListBox()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::~CATConfigurationAppGuiListBox()" );
    
    if ( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }
    iProcesses.Close();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::NewL()
// Construct the control (first phase).
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBox* CATConfigurationAppGuiListBox::NewL( 
    const TRect& aRect, CATConfigurationAppGuiListBoxView* aParent, 
    MEikCommandObserver* aCommandObserver )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::NewL()" );
    
    CATConfigurationAppGuiListBox* self = CATConfigurationAppGuiListBox::NewLC( 
            aRect, 
            aParent, 
            aCommandObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::NewLC()
// Construct the control (first phase).
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiListBox* CATConfigurationAppGuiListBox::NewLC( 
    const TRect& aRect, CATConfigurationAppGuiListBoxView* aParent, 
    MEikCommandObserver* aCommandObserver )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::NewLC()" );
    
    CATConfigurationAppGuiListBox* self = new ( ELeave ) CATConfigurationAppGuiListBox();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent, aCommandObserver );
    return self;
    }
             
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::ConstructL()
// Construct the control (second phase).
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::ConstructL( 
    const TRect& aRect, CATConfigurationAppGuiListBoxView* aParent, 
    MEikCommandObserver* aCommandObserver )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::ConstructL()" );
    
    //if ( aParent == NULL )
        //{
        CreateWindowL();
        //}
    /*else
        {
        SetContainerWindowL( *aParent );
        }*/
    iFocusControl = NULL;
    iCommandObserver = aCommandObserver;
    iParent = aParent;
    InitializeControlsL();
    SetRect( aRect );
    ActivateL();
    }
    
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::CountComponentControls()
// Returns the number of controls in the container.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiListBox::CountComponentControls() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::CountComponentControls()" );
    
    return ( int ) ELastControl;
    }
                
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::ComponentControl()
// Get the control with the given index.
// -----------------------------------------------------------------------------
//
CCoeControl* CATConfigurationAppGuiListBox::ComponentControl( 
	TInt aIndex ) const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::ComponentControl()" );
    
    switch ( aIndex )
        {
    case EListBox:
        return iListBox;
        }
    return NULL;
    }
                
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::SizeChanged()
// Handle resizing of the container.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::SizeChanged()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::SizeChanged()" );
    
    CCoeControl::SizeChanged();
    LayoutControls();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::LayoutControls()
// Layout components.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::LayoutControls()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::LayoutControls()" );
    
    iListBox->SetExtent( TPoint( 0, 0 ), iListBox->MinimumSize() );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::HandleListBoxEventL()
// Handle listbox events.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::HandleListBoxEventL( 
	CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::HandleListBoxEventL()" );
    
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
            iParent->HandleCommandL( 
            		EATConfigurationAppGuiListBoxViewDetailsMenuItemCommand );
            }
            break;
        default:
            break;
        }
    }
    
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::OfferKeyEventL()
// Handle key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CATConfigurationAppGuiListBox::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::OfferKeyEventL()" );
    
    if ( aKeyEvent.iCode == EKeyLeftArrow 
        || aKeyEvent.iCode == EKeyRightArrow )
        {
        // Listbox takes all events even if it doesn't use them
        return EKeyWasNotConsumed;
        }
    
    if ( iFocusControl != NULL
        && iFocusControl->OfferKeyEventL( aKeyEvent, aType ) == EKeyWasConsumed )
        {       
        return EKeyWasConsumed;
        }
        
    return CCoeControl::OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::HandlePointerEventL(
                        const TPointerEvent& aPointerEvent)
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::HandlePointerEventL()" );
    
    if ( !AknLayoutUtils::PenEnabled() )
        {
        return;
        }
        
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }
  
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::InitializeControlsL()
// Initialize each control upon creation.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::InitializeControlsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::InitializeControlsL()" );
    
    iListBox = new ( ELeave ) CAknDoubleStyleListBox;
    iListBox->SetContainerWindowL( *this );
        {
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader, R_ATCONFIGURATION_APP_GUI_LIST_BOX_LIST_BOX );
        iListBox->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // Reader internal state
        }
    // The listbox owns the items in the list and will free them
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->
         SetScrollBarVisibilityL( CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn );
    iListBox->SetListBoxObserver(this);
    
    HBufC* noProcesses = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LBX_NO_PROCESSES );
    iListBox->View()->SetListEmptyTextL( noProcesses->Des() );
    CleanupStack::PopAndDestroy( noProcesses );
    
    // Setup the icon array so graphics-style boxes work
    SetupListBoxIconsL();    
    
    // Update processes array
    UpdateProcessesL();
    CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
    
    for ( TInt count = 0 ; count < iProcesses.Count() ; count++ )
        {
        // This is intended to be large enough, but if you get 
        // a USER 11 panic, consider reducing string sizes.
        TATProcessInfo processInfo = iProcesses[ count ];
        TUint processId = processInfo.iProcessId;
        TBuf<KMaxProcessName> processName;
        processName.Copy( processInfo.iProcessName );
        TATLogOption loggingMode;//lint !e644
        appUi->StorageServer().GetLoggingModeL( processId, loggingMode );
        TBuf<512> listString;
        if ( EATLoggingOff != loggingMode )
            {
            // Check is if this process's subtest is running
            TInt subTest = appUi->IsSubTestRunning( processId );
            if ( subTest > KErrNotFound )
                {
                HBufC* running = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LISTBOX_SUBTEST );
                CreateListBoxItemL( listString, processName, running->Des() );
                CleanupStack::PopAndDestroy( running );
                }
            else
                {
                CreateListBoxItemL( listString, processName, KNullDesC );
                // TEST START
                /*if ( count == KErrNone )
                    CreateListBoxItemL( listString, processName, KNullDesC );
                else if ( count == 1 )
                    CreateListBoxItemL( listString, processName, _L("logging cancelled") );
                else if ( count == 2 )
                    CreateListBoxItemL( listString, processName, _L("subtest running") );
                */
                //TEST END
                }                
            }
        else
            {
            HBufC* cancelled = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LISTBOX_LOGGING_CANCELLED );
            CreateListBoxItemL( listString, processName, cancelled->Des() );
            CleanupStack::PopAndDestroy( cancelled );
            }

        AddListBoxItemL( iListBox, listString );
        }
    iListBox->SetFocus( ETrue );
    iFocusControl = iListBox;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::HandleResourceChange()
// Handle global resource changes.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::HandleResourceChange( TInt aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::HandleResourceChange()" );
    
    CCoeControl::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( TUid::Uid( 
    		EATConfigurationAppGuiListBoxViewId ) )->ClientRect() );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::Draw()
// Draw container contents.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::Draw( const TRect& aRect ) const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::Draw()" );
    
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::AddListBoxItemL()
// Add a list box item to a list.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::AddListBoxItemL( 
    CEikTextListBox* aListBox, const TDesC& aString )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::AddListBoxItemL()" );
    
    CTextListBoxModel* model = aListBox->Model();
    CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
    itemArray->AppendL( aString );
    aListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::GetSelectedListBoxItemsLC()
// Get the array of selected item indices.
// -----------------------------------------------------------------------------
//
RArray< TInt >* CATConfigurationAppGuiListBox::GetSelectedListBoxItemsLC( 
	CEikTextListBox* aListBox )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::GetSelectedListBoxItemsLC()" );
    
    CAknFilteredTextListBoxModel* model = 
        static_cast< CAknFilteredTextListBoxModel *> ( aListBox->Model() );
    if ( model->NumberOfItems() == 0 )
        return NULL;
        
    // Get currently selected indices
    const CListBoxView::CSelectionIndexArray* selectionIndexes =
        aListBox->SelectionIndexes();
    TInt selectedIndexesCount = selectionIndexes->Count();
    if ( selectedIndexesCount == 0 )
        return NULL;
        
    // Copy the indices and sort numerically
    RArray<TInt>* orderedSelectedIndices = 
        new (ELeave) RArray< TInt >( selectedIndexesCount );
    
    // Push the allocated array
    CleanupStack::PushL( orderedSelectedIndices );
    
    // Dispose the array resource
    CleanupClosePushL( *orderedSelectedIndices );
    
    // See if the search field is enabled
    CAknListBoxFilterItems* filter = model->Filter();
    if ( filter != NULL )
        {
        // When filtering enabled, translate indices back to underlying model
        for ( TInt idx = 0; idx < selectedIndexesCount; idx++ )
            {
            TInt filteredItem = ( *selectionIndexes ) [ idx ];
            TInt actualItem = filter->FilteredItemIndex ( filteredItem );
            orderedSelectedIndices->InsertInOrder( actualItem );
            }
        }
    else
        {
        // The selection indices refer directly to the model
        for ( TInt idx = 0; idx < selectedIndexesCount; idx++ )
            orderedSelectedIndices->InsertInOrder( ( *selectionIndexes ) [ idx ] );
        }    
        
    return orderedSelectedIndices;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::DeleteSelectedListBoxItemsL()
// Delete the selected item or items from the list box.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::DeleteSelectedListBoxItemsL( 
	CEikTextListBox* aListBox )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::DeleteSelectedListBoxItemsL()" );
    
    CAknFilteredTextListBoxModel* model = 
        static_cast< CAknFilteredTextListBoxModel *> ( aListBox->Model() );
    if ( model->NumberOfItems() == 0 )
        return;
    
    RArray< TInt >* orderedSelectedIndices = GetSelectedListBoxItemsLC( aListBox );
    if ( !orderedSelectedIndices )
        return;
        
    // Delete selected items from bottom up so indices don't change on us
    CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
    TInt currentItem = 0;
    
    for ( TInt idx = orderedSelectedIndices->Count(); idx-- > 0; ) 
        {
        currentItem = ( *orderedSelectedIndices )[ idx ];
        itemArray->Delete ( currentItem );
        }
    
    // Dispose the array resources
    CleanupStack::PopAndDestroy();
    
    // Dispose the array pointer
    CleanupStack::PopAndDestroy( orderedSelectedIndices );
    
    // Refresh listbox's cursor now that items are deleted
    AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
        aListBox, currentItem, ETrue );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::ListBox()
// Get the listbox.
// -----------------------------------------------------------------------------
//
CAknDoubleStyleListBox* CATConfigurationAppGuiListBox::ListBox()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::ListBox()" );
    
    return iListBox;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::CreateListBoxItemL()
// Create a list box item with the given column values.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::CreateListBoxItemL( TDes& aBuffer, 
        const TDesC& aMainText,
        const TDesC& aSecondaryText )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::CreateListBoxItemL()" );
    
    _LIT ( KStringHeader, "\t%S\t%S" );
    
    aBuffer.Format( KStringHeader(), &aMainText, &aSecondaryText );
    } 

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::AddListBoxResourceArrayItemL()
// Add an item to the list by reading the text items from the array resource.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::AddListBoxResourceArrayItemL( 
	TInt aResourceId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::AddListBoxResourceArrayItemL()" );
    
    CDesCArray* array = iCoeEnv->ReadDesCArrayResourceL( aResourceId );
    CleanupStack::PushL( array );
    // This is intended to be large enough, but if you get 
    // a USER 11 panic, consider reducing string sizes.
    TBuf<512> listString; 
    CreateListBoxItemL( listString, ( *array ) [ 0 ], ( *array ) [ 1 ] );
    AddListBoxItemL( iListBox, listString );
    CleanupStack::PopAndDestroy( array );
    } 

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::SetupListBoxIconsL()
// Set up the list's icon array.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::SetupListBoxIconsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::SetupListBoxIconsL()" );
    
    CArrayPtr< CGulIcon >* icons = NULL;
    
    if ( icons != NULL )
        {
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::HandleMarkableListCommandL()
// Handle commands relating to markable lists.
// -----------------------------------------------------------------------------
//
TBool CATConfigurationAppGuiListBox::HandleMarkableListCommandL( 
	TInt /*aCommand*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::HandleMarkableListCommandL()" );
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::GetCurrentItemIndex()
// Returns current listbox index.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiListBox::GetCurrentItemIndex()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::GetCurrentItemIndex()" );
    
    return iListBox->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::UpdateProcessesL()
// Updates processes array.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::UpdateProcessesL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::UpdateProcessesL()" );
    
    // Update processes array from the storage server
    CATConfigurationAppGuiAppUi* appUi = 
		( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
    appUi->StorageServer().GetProcessesL( iProcesses );
    // TEST
    /*TATProcessInfo pr1;
    pr1.iProcessId = 1;
    TBuf8<KMaxProcessName> pr1Name;
    pr1Name.Copy( _L("MemoryLeaker.exe") );
    pr1.iProcessName.Copy( pr1Name );
    pr1.iStartTime = 1234556789;
    iProcesses.Append( pr1 );

    TATProcessInfo pr2;
    pr2.iProcessId = 2;
    TBuf8<KMaxProcessName> pr2Name;
    pr2Name.Copy( _L("ConsoleApp.exe") );
    pr2.iProcessName.Copy( pr2Name );
    pr2.iStartTime = 2234556789;
    iProcesses.Append( pr2 );
    
    TATProcessInfo pr3;
    pr3.iProcessId = 3;
    TBuf8<KMaxProcessName> pr3Name;
    pr3Name.Copy( _L("MediaPlayer.exe") );
    pr3.iProcessName.Copy( pr3Name );
    pr3.iStartTime = 3234556789;
    iProcesses.Append( pr3 );*/
    // TEST END
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiListBox::UpdateViewL()
// Updates views controls.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiListBox::UpdateViewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiListBox::UpdateViewL()" );
    
    CTextListBoxModel* model = iListBox->Model();
    CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
    itemArray->Reset();
    
    // Update processes array
    UpdateProcessesL();
    for ( TInt count = 0 ; count < iProcesses.Count() ; count++ )
        {
        // This is intended to be large enough, but if you get 
        // a USER 11 panic, consider reducing string sizes.
        TATProcessInfo processInfo = iProcesses[ count ];
        TUint processId = processInfo.iProcessId;
        TBuf<KMaxProcessName> processName;
        processName.Copy( processInfo.iProcessName );
        TBuf<512> listString;
        // Check is if this process's subtest is running
        CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();
        
        TATLogOption loggingMode;//lint !e644
        appUi->StorageServer().GetLoggingModeL( processId, loggingMode );
        if ( EATLoggingOff != loggingMode )
            {
            // Check is if this process's subtest is running
            TInt subTest = appUi->IsSubTestRunning( processId );
            if ( subTest > KErrNotFound )
                {
                HBufC* running = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LISTBOX_SUBTEST );
                CreateListBoxItemL( listString, processName, running->Des() );
                CleanupStack::PopAndDestroy( running );
                }
            else
                {
                CreateListBoxItemL( listString, processName, KNullDesC );
                // TEST START
                /*if ( count == KErrNone )
                    CreateListBoxItemL( listString, processName, KNullDesC );
                else if ( count == 1 )
                    CreateListBoxItemL( listString, processName, _L("logging cancelled") );
                else if ( count == 2 )
                    CreateListBoxItemL( listString, processName, _L("subtest running") );
                */
                //TEST END
                }                
            }
        else
            {
            HBufC* cancelled = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LISTBOX_LOGGING_CANCELLED );
            CreateListBoxItemL( listString, processName, cancelled->Des() );
            CleanupStack::PopAndDestroy( cancelled );
            }            
        AddListBoxItemL( iListBox, listString );
        }    
    iListBox->DrawNow();
    }

// End of File
