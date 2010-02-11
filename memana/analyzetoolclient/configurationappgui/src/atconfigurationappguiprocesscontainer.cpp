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


// INCLUDES
#include <barsread.h>
#include <stringloader.h>
#include <aknlists.h>
#include <eikenv.h>
#include <akniconarray.h>
#include <eikclbd.h>
#include <aknviewappui.h>
#include <eikappui.h>
#include <utf.h>
#include <atconfigurationappgui.rsg>
#include <analyzetool/atstorageserverclnt.h>

#include "atconfigurationappguiprocesscontainer.h"
#include "atconfigurationappguiprocessview.h"
#include "atconfigurationappgui.hrh"
#include "atconfigurationappguiprocesscontainer.hrh"
#include "atconfigurationappguiappui.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::CATConfigurationAppGuiProcessContainer()
// First phase of Symbian two-phase construction.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessContainer::CATConfigurationAppGuiProcessContainer()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::CATConfigurationAppGuiProcessContainer()" );
    
    iListBox = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::~CATConfigurationAppGuiProcessContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessContainer::~CATConfigurationAppGuiProcessContainer()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::~CATConfigurationAppGuiProcessContainer()" );
    
    delete iListBox;
    iListBox = NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::NewL()
// Construct the control (first phase).
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessContainer* CATConfigurationAppGuiProcessContainer::NewL( 
    const TRect& aRect, CATConfigurationAppGuiProcessView* aParent, 
    MEikCommandObserver* aCommandObserver, TUint aProcessId, 
    TInt64 aProcessStartTime )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::NewL()" );
    
    CATConfigurationAppGuiProcessContainer* self = 
		CATConfigurationAppGuiProcessContainer::NewLC( 
            aRect, 
            aParent, 
            aCommandObserver, aProcessId, aProcessStartTime );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::NewLC()
// Construct the control (first phase).
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiProcessContainer* CATConfigurationAppGuiProcessContainer::NewLC( 
    const TRect& aRect, CATConfigurationAppGuiProcessView* aParent, 
    MEikCommandObserver* aCommandObserver, TUint aProcessId, 
    TInt64 aProcessStartTime )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::NewLC()" );
    
    CATConfigurationAppGuiProcessContainer* self = 
		new ( ELeave ) CATConfigurationAppGuiProcessContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect, 
				      aParent, 
				      aCommandObserver, 
				      aProcessId, aProcessStartTime );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::ConstructL()
// Construct the control (second phase).
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::ConstructL( 
    const TRect& aRect, CATConfigurationAppGuiProcessView* aParent, 
    MEikCommandObserver* aCommandObserver, TUint aProcessId, 
    TInt64 aProcessStartTime )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::ConstructL()" );
    
    //if ( aParent == NULL )
        //{
    CreateWindowL();
        //}
    /*else
        {
        SetContainerWindowL( *aParent );
        }*/
    iFocusControl = NULL;
    iParent = aParent;
    iCommandObserver = aCommandObserver;
    iProcessId = aProcessId;
    iProcessStartTime = aProcessStartTime;
    InitializeControlsL();
    SetRect( aRect );
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::CountComponentControls()
// Returns the number of controls in the container.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiProcessContainer::CountComponentControls() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::CountComponentControls()" );
    return ( int ) ELastControl;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::ComponentControl()
// Get the control with the given index.
// -----------------------------------------------------------------------------
//
CCoeControl* CATConfigurationAppGuiProcessContainer::ComponentControl( 
	TInt aIndex ) const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::ComponentControl()" );
    
    switch ( aIndex )
        {
    case EListBox:
        return iListBox;
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::SizeChanged()
// Handle resizing of the container.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::SizeChanged()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::SizeChanged()" );
    
    CCoeControl::SizeChanged();
    LayoutControls();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::LayoutControls()
// Layout components.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::LayoutControls()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::LayoutControls()" );
    
    iListBox->SetExtent( TPoint( 0, 0 ), iListBox->MinimumSize() );
    }
    
// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::OfferKeyEventL()
// Handle key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CATConfigurationAppGuiProcessContainer::OfferKeyEventL( 
        const TKeyEvent& aKeyEvent, 
        TEventCode aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::OfferKeyEventL()" );
    
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
// CATConfigurationAppGuiProcessContainer::InitializeControlsL()
// Initialize each control upon creation.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::InitializeControlsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::InitializeControlsL()" );
    
    iListBox = new ( ELeave ) CAknDoubleStyleListBox;
    iListBox->SetContainerWindowL( *this );
        {
        TResourceReader reader;
        iEikonEnv->CreateResourceReaderLC( reader, R_ATCONFIGURATION_APP_GUI_LIST_BOX );
        iListBox->ConstructFromResourceL( reader );
        CleanupStack::PopAndDestroy(); // Reader internal state
        }
    // The listbox owns the items in the list and will free them
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->
         SetScrollBarVisibilityL( CEikScrollBarFrame::EOn, CEikScrollBarFrame::EOn );

    // Enable marquee
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        
    HBufC* noInfo = StringLoader::LoadLC( R_ATCONFIGURATION_APP_GUI_LBX_NO_INFO );
    iListBox->View()->SetListEmptyTextL( noInfo->Des() );
    CleanupStack::PopAndDestroy( noInfo );

    // Setup the icon array so graphics-style boxes work
    SetupListBoxIconsL();

    // Add list items
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM1 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM2 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM3 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM4 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM5 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM6 );

    iListBox->SetFocus( ETrue );
    iFocusControl = iListBox;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::HandleResourceChange()
// Handle global resource changes.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::HandleResourceChange( TInt aType )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::HandleResourceChange()" );
    
    CCoeControl::HandleResourceChange( aType );
    SetRect( iAvkonViewAppUi->View( TUid::Uid( EATConfigurationAppGuiProcessViewId ) )->ClientRect() );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::Draw()
// Draw container contents.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::Draw( const TRect& aRect ) const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::Draw()" );
    
    CWindowGc& gc = SystemGc();
    gc.Clear( aRect );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::AddListBoxItemL()
// Add a list box item to a list.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::AddListBoxItemL( 
    CEikTextListBox* aListBox, const TDesC& aString )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::AddListBoxItemL()" );
    
    CTextListBoxModel* model = aListBox->Model();
    CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
    itemArray->AppendL( aString );
    aListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::GetSelectedListBoxItemsLC()
// Get the array of selected item indices.
// -----------------------------------------------------------------------------
//
RArray< TInt >* CATConfigurationAppGuiProcessContainer::GetSelectedListBoxItemsLC( 
	CEikTextListBox* aListBox )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::GetSelectedListBoxItemsLC()" );
    
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
// CATConfigurationAppGuiProcessContainer::DeleteSelectedListBoxItemsL()
// Delete the selected item or items from the list box.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::DeleteSelectedListBoxItemsL( 
	CEikTextListBox* aListBox )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::DeleteSelectedListBoxItemsL()" );
    
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
// CATConfigurationAppGuiProcessContainer::ListBox()
// Get the listbox.
// -----------------------------------------------------------------------------
//
CAknDoubleStyleListBox* CATConfigurationAppGuiProcessContainer::ListBox()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::ListBox()" );
    
    return iListBox;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::CreateListBoxItemL()
// Create a list box item with the given column values.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::CreateListBoxItemL( TDes& aBuffer, 
    const TDesC& aMainText, const TDesC& aSecondaryText )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::CreateListBoxItemL()" );
    
    _LIT ( KStringHeader, "\t%S\t%S" );

    aBuffer.Format( KStringHeader(), &aMainText, &aSecondaryText );
    } 

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::AddListBoxResourceArrayItemL()
// Add an item to the list by reading the text items from the array resource.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::AddListBoxResourceArrayItemL( 
	TInt aResourceId )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::AddListBoxResourceArrayItemL()" );
    
    CDesCArray* array = iCoeEnv->ReadDesCArrayResourceL( aResourceId );
    CleanupStack::PushL( array );
    _LIT( KSpace, " " );
    TBuf<KATHundred> main;
    TBuf<KMaxFileName + 1> secondary;
    // Get handle to appUi
    CATConfigurationAppGuiAppUi* appUi = ( CATConfigurationAppGuiAppUi* )iEikonEnv->AppUi();

    switch ( aResourceId )
        {
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM1:
            {
            TUint32 number( KErrNone );
            TUint32 size( KErrNone );
            TInt currErr = appUi->StorageServer().GetCurrentAllocsL( iProcessId, number, size );
            if ( currErr == KErrNone )
                {
                main.Copy( KSpace );
                main.AppendNum( number );
                if ( size >= KATThousand && size < KATMillion ) // kiloByte range
                    {
                    TReal value( size );
                    value = value / KATThousand;
                    secondary.Format( _L(" %.1f"), value );
                    secondary.Append( KSpace );
                    secondary.Append( KATKB );  
                    }
                else if ( size >= KATMillion ) // megaByte range
                    {
                    TReal value( size );
                    value = value / KATMillion;
                    secondary.Format( _L(" %.1f"), value );
                    secondary.Append( KSpace );
                    secondary.Append( KATMB );
                    }
                else
                    {
                    secondary.Copy( KSpace );
                    secondary.AppendNum( size );
                    secondary.Append( KSpace );
                    secondary.Append( KATB );
                    }
                }
            else
                {
                main.Copy( KSpace );
                main.Append( _L("unavailable") );
                secondary.Copy( KSpace );
                secondary.Append( _L("unavailable") );
                }
            break;
            }
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM2:
            {
            TUint32 number( KErrNone );
            TUint32 size( KErrNone );
            TInt maxErr = appUi->StorageServer().GetMaxAllocsL( iProcessId, number, size );
            if ( maxErr == KErrNone )
                {
                main.Copy( KSpace );
                main.AppendNum( number );
                if ( size >= KATThousand && size < KATMillion ) // kiloByte range
                    {
                    TReal value( size );
                    value = value / KATThousand;
                    secondary.Format( _L(" %.1f"), value );
                    secondary.Append( KSpace );
                    secondary.Append( KATKB );  
                    }
                else if ( size >= KATMillion ) // megaByte range
                    {
                    TReal value( size );
                    value = value / KATMillion;
                    secondary.Format( _L(" %.1f"), value );
                    secondary.Append( KSpace );
                    secondary.Append( KATMB );
                    }
                else
                    {
                    secondary.Copy( KSpace );
                    secondary.AppendNum( size );
                    secondary.Append( KSpace );
                    secondary.Append( KATB );
                    }                   
                }
            else
                {
                main.Copy( KSpace );
                main.Append( _L("unavailable") );
                secondary.Copy( KSpace );
                secondary.Append( _L("unavailable") );
                }
            break;
            }
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM3:
            {
            secondary.Copy( KSpace );
            TTime time( iProcessStartTime );
            TBuf<50> dateString;
            _LIT( KDateString3,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
            time.FormatL( dateString, KDateString3 );
            secondary.Append( dateString );
            break;
            }
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM4:
            {
            secondary.Copy( KSpace );
            TATLogOption loggingMode;
            TInt loggingErr = appUi->StorageServer().GetLoggingModeL( iProcessId, loggingMode );
            if ( KErrNone == loggingErr )
                {
                if ( EATUseDefault == loggingMode )
                    {
                    HBufC* defaultLogging = StringLoader::LoadLC( R_ATCONFIGURATION_APP_XTI_LOGGING );
                    secondary.Append( defaultLogging->Des() );
                    CleanupStack::PopAndDestroy( defaultLogging );
                    }
                else if ( EATLogToFile == loggingMode )
                    {
                    HBufC* fileLogging = StringLoader::LoadLC( R_ATCONFIGURATION_APP_FILE_LOGGING );
                    secondary.Append( fileLogging->Des() );
                    CleanupStack::PopAndDestroy( fileLogging );
                    }
                else if ( EATLogToXti == loggingMode )
                    {
                    HBufC* xtiLogging = StringLoader::LoadLC( R_ATCONFIGURATION_APP_XTI_LOGGING );
                    secondary.Append( xtiLogging->Des() );
                    CleanupStack::PopAndDestroy( xtiLogging );
                    }
                else if( EATLoggingOff == loggingMode )
                    {
                    HBufC* noneLogging = StringLoader::LoadLC( R_ATCONFIGURATION_APP_NONE_LOGGING );
                    secondary.Append( noneLogging->Des() );
                    CleanupStack::PopAndDestroy( noneLogging );
                    }
                }
            else
                {
                secondary.Append( _L("unavailable") );
                }
            break;
            }
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM5:
            {
            secondary.Copy( KSpace );
            TUint32 isUdeb;
            TInt udebErr = appUi->StorageServer().GetUdebL( iProcessId, isUdeb );
            if ( KErrNone == udebErr )
                {
                if ( isUdeb > KErrNone )
                    {
                    _LIT( KUdeb, "UDEB" );
                    secondary.Append( KUdeb() );
                    }
                else
                    {
                    _LIT( KUrel, "UREL" );
                    secondary.Append( KUrel() );   
                    }
                }
            else
                {
                secondary.Append( _L("unavailable") );
                }
            break;
            }
        case R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM6:
            {
            secondary.Copy( KSpace );
            TBuf8<KMaxFileName> fileName;
            TInt fileErr = appUi->StorageServer().GetLoggingFileL( iProcessId, fileName );
            if ( KErrNone == fileErr )
                {
                if ( fileName.Length() > KErrNone )
                    {
                    TBuf<KMaxFileName> unicodeFile;
                    CnvUtfConverter::ConvertToUnicodeFromUtf8( unicodeFile, fileName );
                    secondary.Append( unicodeFile );
                    }
                else
                    {               
                    _LIT( KNone, "None" );
                    secondary.Append( KNone() );
                    }
                }
            else
                {
                _LIT( KUnavail, "unavailable" );
                secondary.Append( KUnavail() );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    HBufC* mainText = HBufC::NewLC( ( *array ) [ 0 ].Length() + main.Length() );
    HBufC* secondaryText = HBufC::NewLC(( *array ) [ 1 ].Length() + secondary.Length() );
    mainText->Des().Copy( ( *array ) [ 0 ] );
    mainText->Des().Append( main );
    secondaryText->Des().Copy( ( *array ) [ 1 ] );
    secondaryText->Des().Append( secondary );
    // This is intended to be large enough, but if you get 
    // a USER 11 panic, consider reducing string sizes.
    TBuf<512> listString; 
    CreateListBoxItemL( listString, mainText->Des(), secondaryText->Des() );
    AddListBoxItemL( iListBox, listString );
    CleanupStack::PopAndDestroy( secondaryText );
    CleanupStack::PopAndDestroy( mainText );
    CleanupStack::PopAndDestroy( array );
    } 

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::SetupListBoxIconsL()
// Set up the list's icon array.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::SetupListBoxIconsL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::SetupListBoxIconsL()" );
    
    CArrayPtr< CGulIcon >* icons = NULL;

    if ( icons != NULL )
        {
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
        }
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::HandleMarkableListCommandL()
// Handle commands relating to markable lists.
// -----------------------------------------------------------------------------
//
TBool CATConfigurationAppGuiProcessContainer::HandleMarkableListCommandL( 
	TInt /*aCommand*/ )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::HandleMarkableListCommandL()" );
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::GetCurrentItemIndex()
// Returns current listbox index.
// -----------------------------------------------------------------------------
//
TInt CATConfigurationAppGuiProcessContainer::GetCurrentItemIndex()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::GetCurrentItemIndex()" );
    
    return iListBox->CurrentItemIndex();
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiProcessContainer::UpdateViewL()
// Update view's controls.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiProcessContainer::UpdateViewL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiProcessContainer::UpdateViewL()" );
    
    CTextListBoxModel* model = iListBox->Model();
    CDesCArray* itemArray = static_cast< CDesCArray* > ( model->ItemTextArray() );
    itemArray->Reset();
    
    // add list items
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM1 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM2 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM3 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM4 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM5 );
    AddListBoxResourceArrayItemL( R_ATCONFIGURATION_APP_GUI_PROCESS_LISTBOX_ITEM6 );
    
    iListBox->DrawNow();
    }

// End of File
