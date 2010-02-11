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



// INCLUDE FILES
#include <e32base.h>
#include <e32cons.h>
#include <e32svr.h>
#include <utf.h>
#include <apgtask.h>
#include "atconsoleui.h"
#include "atconsoleviews.h"

// LOCAL FUNCTION PROTOTYPES
LOCAL_C void LimitedAppend( TDes& aOriginal, const TDesC& aAppend);

// FORWARD DECLARATIONS

// ==================== LOCAL FUNCTIONS =======================================
// -----------------------------------------------------------------------------
// LimitedAppend()
// Appends texts.
// -----------------------------------------------------------------------------
//
LOCAL_C void LimitedAppend( TDes& aOriginal, const TDesC& aAppend )
    {

    TInt spaceLeft = aOriginal.MaxLength() - aOriginal.Length();
    
    if ( spaceLeft > aAppend.Length() )
        {
        aOriginal.Append ( aAppend );
        }
    else
        {       
        aOriginal.Append ( aAppend.Left ( spaceLeft ) );
        }
    }

// -----------------------------------------------------------------------------
// CView::NewL()
// First phase constructor.
// -----------------------------------------------------------------------------
//
CView* CView::NewL( CConsoleMain* aConsole,
                    CView* aParent,
                    const TDesC& aName,
                    TInt64 aProcessId
                  )
    {

    CView* self = new ( ELeave ) CView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aProcessId );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CView::ConstructL()
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CView::ConstructL( CConsoleMain* aConsole,
                        CView* aParent,
                        const TDesC& aName, TInt64 aProcessId
                      )
    {
    // Store the parameters
    iMain = aConsole;
    iConsole = aConsole->GetConsole();
    iParent = aParent;
    iViewName = aName;
    iProcessId = aProcessId;

    // Get display size
    const TInt KOverHead = 5;
    iSize = iConsole->ScreenSize();
    iScreenSize = iSize.iHeight - KOverHead;   // Size available for menus
    }

// -----------------------------------------------------------------------------
// CView::CView()
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CView::CView()
    {
    iDirection = 1;
    iPrevPos  = -1;     // Invalid starting value
    }

// -----------------------------------------------------------------------------
// CView::~CView()
// Destructor.
// -----------------------------------------------------------------------------
//
CView::~CView()
    {    
    iItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CView::TimerUpdate()
// Handles screen updates.
// -----------------------------------------------------------------------------
//
void CView::TimerUpdate()
    {   
    RArray<TDesC> texts;
    if( ItemTexts( texts ) != KErrNone )
        {
        texts.Close();
        return;
        }
    TInt count = texts.Count();
    // If list is empty, do not scroll.
    if ( count < 1 )
        {
        texts.Close();
        return;
        }
    else if( iFirstItem + iPositionOnScreen > count )
        {
        iFirstItem = iPositionOnScreen = 0;
        }

    // If menu selection is updated, then restart scrolling
    if (iPrevPos != iFirstItem + iPositionOnScreen)
        {
        iPrevPos = iFirstItem + iPositionOnScreen;
        iStart = 0;
        iDirection = 1;
        }

    // If menu item have not been changed after last timer, then
    // start scrolling  
    const TDesC& name = texts[ iFirstItem + iPositionOnScreen ]; 
    if ( name.Length() > iSize.iWidth)
        {
        TInt y = iConsole->WhereY();
        TInt x = iConsole->WhereX();
        TBuf<80> iTmp;

        iStart = iStart + iDirection;

        // "Right end"
        if ( iStart + iSize.iWidth > name.Length() + KViewOverhead )
            {
            iStart--;
            iDirection = -1;
            }
        
        // "Left end"
        if ( iStart == -1 )
            {
            iStart++;
            iDirection = 1;
            }
        
        if( iStart > name.Length() )
            {
            iStart = 0;
            }

        iTmp=_L(" *");
        LimitedAppend( iTmp, name.Mid ( iStart ) );
            
        iConsole->SetPos( 0, iPositionOnScreen + 1 );
        iConsole->Printf ( iTmp.Left( iSize.iWidth -2 )  );

        iConsole->SetPos(x,y);
        }
    texts.Close();
    }

// -----------------------------------------------------------------------------
// CView::AppendBefore()
// Appends cursor to the line.
// -----------------------------------------------------------------------------
//
void CView::AppendBefore( TInt aLineNum, TDes& aLine )
    {    
    if( ( aLine.MaxLength() - aLine.Length() ) < 2 )
        {
        return;
        }        
    
    // If printing active line, print the marker
    if ( aLineNum == iPositionOnScreen + iFirstItem )
        {
        aLine.Append( _L(" *") );
        }
    else
        {
        aLine.Append( _L("  ") );
        }        
    }

// -----------------------------------------------------------------------------
// CView::PrintViewL()
// Prints view.
// -----------------------------------------------------------------------------
//
void CView::PrintViewL( TUpdateType aType )
    {
    iUpdateType = aType;
    TBuf<KMaxLineLength> line;

    // Clear display
    iConsole->ClearScreen();

    // Print menu title
    Print( Name() );
    
    RArray<TDesC> texts;
    if( ItemTexts( texts ) != KErrNone )
        {
		texts.Close();
        return;
        }
    
    TInt oldItemCount = iItemCount;
     
    iItemCount = texts.Count();

    // If first time in menu, update start and end positions
    if( ( iFirstItem == iLastItem ) ||        // First time here..
        ( iLastItem >= iItemCount ) ||
        ( oldItemCount != iItemCount ) )      // Menu size changed
        {
        iLastItem = iItemCount - 1;
        iPositionOnScreen = 0;

        // If "overflow", then adjust the end
        if (iLastItem > iScreenSize )
            {
            iLastItem = iScreenSize;
            }
        }
    
    // Print items
    for ( TInt i = iFirstItem; i <= iLastItem; i++ )
        {
        line.Zero();
        
        // Append text before line
        AppendBefore( i, line );
        TBuf<2*KMaxName> temp;
        if ( i < iItems.Count() )
            {
            temp.Copy( iItems[ i ]->Name() );
            TATLogOption loggingMode;
            iMain->StorageServer().GetLoggingModeL( iMain->Processes()[ i ].iProcessId, loggingMode );
            if ( EATLoggingOff != loggingMode )
                {
                if ( iMain->IsSubTestRunning( iMain->Processes()[ i ].iProcessId ) > KErrNotFound )
                    {
                    temp.Append( _L(": ") );
                    temp.Append( _L("subtest running") );
                    }                   
                }
            else
                {
                temp.Append( _L(": ") );
                temp.Append( _L("logging cancelled") );
                }
            // TEST START
            /*if ( i == iFirstItem )
                {
                temp.Append( _L(": ") );
                temp.Append( _L("subtest running") );
                }
            else if ( i == iFirstItem + 1 )
                {
                temp.Append( _L(": ") );
                temp.Append( _L("logging cancelled") );
                }
            else if ( i == iFirstItem + 2 )
                {
                temp.Append( _L(": ") );
                temp.Append( _L("subtest running") );
                }*/
            // TEST END
            LimitedAppend ( line, temp /*texts[i]*/ );
            }
        else
            LimitedAppend ( line, KExitTxt );

        // Print the line
        Print(line);
        }
    texts.Close();
    }

// -----------------------------------------------------------------------------
// CView::SelectL()
// Handles line selections.
// -----------------------------------------------------------------------------
//
CView* CView::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    MapKeyCode( aSelection );
    
    switch ( ( TInt )aSelection )
        {
        // Exit
        case EKeyEscape:
            aContinue = EFalse;
            return this;
        // SelectL item
        case EKeyEnter:
        case EKeyRightArrow:
            {
            // Print items
            for ( TInt i = iFirstItem; i <= iLastItem; i++ )
                {
                if ( i < iItems.Count() )
                    {
                    TATLogOption loggingMode;
                    TInt processCount ( iMain->Processes().Count() );
                    if ( i <  processCount )
                        {
                        iMain->StorageServer().GetLoggingModeL( iMain->Processes()[ i ].iProcessId, loggingMode );
                        if ( EATLoggingOff != loggingMode )
                            {
                            return iItems[ iPositionOnScreen ];
                            }
                        else
                            {
                            return this;
                            }
                        }
                    }
                }                    
            //return iItems[ iPositionOnScreen ];
            } 
        // Going back
        case EKeyLeftArrow:
            {
            TBool tmp = iShowLibraries | iSetLoggingMode;
            iShowLibraries = EFalse;
            iSetLoggingMode = EFalse;
            if ( tmp )
                return this;
            else
                return iParent;
            }
        // Go down
        case EKeyDownArrow:
            {
            if ( iFirstItem + iPositionOnScreen == iItemCount - 1 )
                {
                // If end of the list, go to beginning
                iLastItem = iLastItem - iFirstItem;
                iFirstItem = 0;
                iPositionOnScreen = 0;
                }
            else 
                {
                if ( iPositionOnScreen == iScreenSize )
                    {
                    // If in end of screen, update items
                    if ( iLastItem != ( iItemCount - 1 ) )
                        {
                        // If not end of the list, then update first and last
                        // but do not update position in screen.
                        iLastItem++;
                        iFirstItem++;
                        }
                    }
                else
                    {
                    // Going down "in-screen", no need to update items
                    iPositionOnScreen++;
                    }
                }
            break;
            }   
        // Go Up
        case EKeyUpArrow:
            {
            if ( iFirstItem + iPositionOnScreen == 0 )
                {
                // If in the beginning of the list
    
                if ( iItemCount <= iScreenSize ) 
                    {
                    // Wrap around when the list is not full
                        iPositionOnScreen = iItemCount-1;
                    }
                else
                    {
                    // Wrap around when there are more items than
                    // can be shown at once.            
                    iPositionOnScreen = iScreenSize;
                    iLastItem = iItemCount-1;
                    iFirstItem = iItemCount - iPositionOnScreen - 1;
                    }
                }
            else if ( iPositionOnScreen == 0 )
                {
                // If not at the beginning of the list, then update first and
                // last  but do not update position in screen.
                if ( iFirstItem != 0 )
                    {
                    iLastItem--;
                    iFirstItem--;
                    }
                }
            else
                {
                // Going up "in-screen", no need to update items
                iPositionOnScreen--;
                }
            
            break;
            }                

        // Additional keys
        case EKeyHome:
        case '3':
            iPositionOnScreen = 0;
            iFirstItem = 0;
            iLastItem = iScreenSize;
    
            if ( iLastItem > iItemCount-1 )
                {
                iLastItem = iItemCount-1;
                }
            break;
        case EKeyEnd:
        case '9':
            iPositionOnScreen = iScreenSize;
            iLastItem = iItemCount-1;
            iFirstItem = iLastItem - iScreenSize;
    
            if ( iFirstItem < 0 )
                {
                iFirstItem = 0;
                iPositionOnScreen = iLastItem-1;
                }
            break;
        case EKeyPageUp:
        case '1':
    
            iFirstItem = iFirstItem - iScreenSize - 1;
            iLastItem = iLastItem - iScreenSize - 1;
    
            if ( iFirstItem < 0 )
                {
                iFirstItem = 0;
                iPositionOnScreen = 0;
                iLastItem = iScreenSize;
                if ( iLastItem > iItemCount-1 )
                    {
                    iLastItem = iItemCount-1;
                    }
                }
            break;
        case EKeyPageDown:
        case '7':
            iFirstItem = iFirstItem + iScreenSize + 1;
            iLastItem = iLastItem + iScreenSize + 1;
    
            // Going too far
            if ( iLastItem > iItemCount - 1 )
                {
                iLastItem = iItemCount-1;
                iFirstItem = iLastItem - iScreenSize;
                iPositionOnScreen = iScreenSize;
                }
    
            // Ok, list is smaller than screen
            if ( iFirstItem < 0 )
                {
                iFirstItem = 0;
                iLastItem = iItemCount-1;
                iPositionOnScreen = iLastItem;
                }
            break;
        default:  // Bypass the keypress
            break;
        }

    // Continue normally and keep in the same menu
    aContinue = ETrue;
    return this;

    }

// -----------------------------------------------------------------------------
// CView::ItemTexts()
// Formats menu texts
// -----------------------------------------------------------------------------
//
TInt CView::ItemTexts( RArray<TDesC>& aArray )
    {
    
    TInt count = iItems.Count();
    for( TInt i=0; i<count; i++ )
        {
        TBuf<50> temp;
        temp.Copy( iItems[i]->Name() );
        if( aArray.Append( iItems[i]->Name() ) != KErrNone )
            {
            return KErrNoMemory;
            }
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CView::SetParent()
// Sets menu parent menu
// -----------------------------------------------------------------------------
//
void CView::SetParent ( CView* aMenu )
    {

    iParent = aMenu;

    }

// -----------------------------------------------------------------------------
// CView::Print()
// Prints line to the screen.
// -----------------------------------------------------------------------------
//
void CView::Print( const TDesC& aPrint )
    {    
    iConsole->Printf ( aPrint.Left( iSize.iWidth - KViewOverhead ) );   
    iConsole->Printf(_L("\n"));
    }

// -----------------------------------------------------------------------------
// CView::AddItemL()
// Adds item to the list.
// -----------------------------------------------------------------------------
//
void CView::AddItemL( CView* aMenu )
    {

    User::LeaveIfError ( iItems.Append( aMenu ) );
    
    }

// -----------------------------------------------------------------------------
// CView::ResetItems()
// Resets the item list.
// -----------------------------------------------------------------------------
//
void CView::ResetItems()
    {
    iItems.ResetAndDestroy();
    }
// -----------------------------------------------------------------------------
// CView::Name()
// Returns the name of view.
// -----------------------------------------------------------------------------
//
const TDesC& CView::Name( ) const
    {

    return iViewName;

    }

// -----------------------------------------------------------------------------
// CView::MapKeyCode()
// Maps numeric keycodes to proper TKeyCode values.
// -----------------------------------------------------------------------------
//
void CView::MapKeyCode(TKeyCode &aSelection)
    {
    
    TInt asciiCode = (TInt) aSelection;
    
    // Handling numeric keys 2,4,6 and 8
    
    if(asciiCode == KMyKeyUpAsciiCode)
        {
        aSelection = EKeyUpArrow;
        }
        
    if(asciiCode == KMyKeyLeftAsciiCode)
        {
        aSelection = EKeyLeftArrow;
        }

    if(asciiCode == KMyKeyRightAsciiCode)
        {
        aSelection = EKeyEnter;
        }

    if(asciiCode == KMyKeyDownAsciiCode)
        {
        aSelection = EKeyDownArrow;
        }
    }

// -----------------------------------------------------------------------------
// CMainView::NewL()
// First phase constructor.
// -----------------------------------------------------------------------------
//
CMainView* CMainView::NewL( CConsoleMain* aConsole,
                            CView* aParent,
                            const TDesC& aName )
    {

    CMainView* self = new ( ELeave ) CMainView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName );
    CleanupStack::Pop( self );
    return self;

    }

// -----------------------------------------------------------------------------
// CMainView::ItemTexts()
// Format menu texts.
// -----------------------------------------------------------------------------
//
TInt CMainView::ItemTexts( RArray<TDesC>& aArray )
    {
    
    CView::ItemTexts( aArray );
    // Add Exit to last one in menu
    if( aArray.Append( KExitTxt ) != KErrNone )
        {
        return KErrNoMemory;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMainView::ConstructL()
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CMainView::ConstructL( CConsoleMain* aConsole, 
                            CView* aParent,
                            const TDesC& aName
                          )
    {

    CView::ConstructL( aConsole, aParent, aName, NULL );

    }

// -----------------------------------------------------------------------------
// CMainView::SelectL()
// Process keypresses in menu.
// -----------------------------------------------------------------------------
//
CView* CMainView::SelectL( TKeyCode aSelection, TBool& aContinue )
    {

    MapKeyCode( aSelection );
    
    // If SelectLion == "exit" and right or enter
    if ( aSelection == EKeyEnter || aSelection == EKeyRightArrow )
        {
        if ( iPositionOnScreen == iItemCount - 1 )
            {
            // Exit
            aContinue = EFalse;
            return this;
            }
        }

    // Normal menu handling
    CView* tmp = CView::SelectL( aSelection, aContinue );
    return tmp;

    }

// -----------------------------------------------------------------------------
// CProcessInfoView::NewL()
// First phase constructor.
// -----------------------------------------------------------------------------
//
CProcessInfoView* CProcessInfoView::NewL( CConsoleMain* aConsole,
                                        CView* aParent,
                                        const TDesC& aName, TInt64 aProcessId )
    {    

        CProcessInfoView* self = new ( ELeave ) CProcessInfoView();
    CleanupStack::PushL( self );
    self->ConstructL( aConsole, aParent, aName, aProcessId );
    CleanupStack::Pop( self );
    return self;
    
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::~CProcessInfoView()
// Destructor.
// -----------------------------------------------------------------------------
//
CProcessInfoView::~CProcessInfoView()
    {
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::ConstructL()
// First phase constructor.
// -----------------------------------------------------------------------------
//
void CProcessInfoView::ConstructL( CConsoleMain* aConsole,
                                  CView* aParent,
                                  const TDesC& aName, TInt64 aProcessId
                                 )
    {
    CView::ConstructL( aConsole, aParent, aName, aProcessId );
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::ItemTexts()
// Formats menu item texts.
// -----------------------------------------------------------------------------
//
TInt CProcessInfoView::ItemTexts( RArray<TDesC>& /*aArray*/ )
    {      
    return KErrNone; 
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::PrintViewL()
// Prints process info view.
// -----------------------------------------------------------------------------
//
void CProcessInfoView::PrintViewL( TUpdateType /*aType*/ )
    {
    if ( !iShowLibraries && !iSetLoggingMode )
        {
        // Get module names
        RArray<TATProcessInfo> processes;
        // Clear display
        iConsole->ClearScreen();
        // Print menu title
        TBuf<2*KMaxName> title;
        title.Copy( Name() );
        TInt subTest = iMain->IsSubTestRunning( iProcessId );
        if ( subTest > KErrNotFound )
            {
            // Append subtest running text
            title.Append( _L(": ") );
            title.Append( _L( "subtest running" ) );
            }
        
        Print( title /*Name()*/ );
        // Update positions
        RArray<TDesC> texts;
        if( ItemTexts( texts ) != KErrNone )
            {
            return;
            }
        
        TInt oldItemCount = iItemCount;
        iItemCount = texts.Count();

        // If first time in menu, update start and end positions
        if( ( iFirstItem == iLastItem ) ||        // First time here..
            ( iLastItem >= iItemCount ) ||
            ( oldItemCount != iItemCount ) )      // Menu size changed
            {
            iLastItem = iItemCount - 1;
            iPositionOnScreen = 0;

            // If "overflow", then adjust the end
            if ( iLastItem > iScreenSize )
                {
                iLastItem = iScreenSize;
                }
            }
        // Update pos screen
        if ( iPositionOnScreen > ( TInt )ECancelLoggingTxtItem - 1 )
            iPositionOnScreen = 0;
        else if ( iPositionOnScreen < ( TInt )EStartSubtestTxtItem )
            iPositionOnScreen = ( TInt )ECancelLoggingTxtItem - 1;
        
        // Get processes
        processes = iMain->Processes();
        // Get current opened process
        for ( TInt num = 0 ; num < processes.Count() ; num++ )
            {
            TATProcessInfo tmpProcessInfo;
            tmpProcessInfo = processes[ num ];
            if ( tmpProcessInfo.iProcessId == iProcessId )
                {
                iCurrentProcessInfo = tmpProcessInfo;
                break;
                }    
            }

        TBuf<KMaxLineLength> line;
        // Get curr. and max. allocations
        TUint32 number( KErrNone );
        TUint32 size( KErrNone );
        TUint32 maxNumber( KErrNone );
        TUint32 maxSize( KErrNone );
        TInt currErr = iMain->StorageServer().GetCurrentAllocsL( iProcessId, number, size );
        TInt maxErr = iMain->StorageServer().GetMaxAllocsL( iProcessId, maxNumber, maxSize );
        // Get logging mode
        TATLogOption loggingMode;
        TInt loggingErr = iMain->StorageServer().GetLoggingModeL( iProcessId, loggingMode );
        
        // Print curr. allocations
        line = _L("    ");
        LimitedAppend ( line, _L( "Curr. allocations: " ) );
        // Append curr. allocs
        if ( currErr == KErrNone )
            line.AppendNum( number );
        
        Print( line );
        line = _L("        ");
        if ( currErr == KErrNone )
            {
            if ( size >= KATThousand && size < KATMillion ) // kiloByte range
                {
                TReal value( size );
                value = value / KATThousand;
                line.Format( _L("     %.1f"), value );
                line.Append( _L(" ") );
                line.Append( KATKB );  
                }
            else if ( size >= KATMillion ) // megaByte range
                {
                TReal value( size );
                value = value / KATMillion;
                line.Format( _L("     %.1f"), value );
                line.Append( _L(" ") );
                line.Append( KATMB );
                }
            else
                {
                line.Copy( _L("     ") );
                line.AppendNum( size );
                line.Append( _L(" ") );
                line.Append( KATB );
                }
            }
        else
            {
            LimitedAppend ( line, _L( "unavailable" ) );
            }       
        Print( line );
        
        // Print max. allocations
        line = _L("    ");
        LimitedAppend ( line, _L( "Max. allocations: " ) );
        // Append curr. allocs
        if ( maxErr == KErrNone )
            line.AppendNum( maxNumber );
        
        Print( line );
        line = _L("        ");
        if ( maxErr == KErrNone )
            {
            if ( size >= KATThousand && size < KATMillion ) // kiloByte range
                {
                TReal value( maxSize );
                value = value / KATThousand;
                line.Format( _L("     %.1f"), value );
                line.Append( _L(" ") );
                line.Append( KATKB );  
                }
            else if ( size >= KATMillion ) // megaByte range
                {
                TReal value( maxSize );
                value = value / KATMillion;
                line.Format( _L("     %.1f"), value );
                line.Append( _L(" ") );
                line.Append( KATMB );
                }
            else
                {
                line.Copy( _L("     ") );
                line.AppendNum( maxSize );
                line.Append( _L(" ") );
                line.Append( KATB );
                }
            }
        else
            {
            LimitedAppend ( line, _L( "unavailable") );
            }
        Print( line ); 
        
        // Print starting time
        line = _L("    ");
        LimitedAppend ( line, _L( "Process started:") );
        Print( line );
        line = _L("     ");
        TTime time( iCurrentProcessInfo.iStartTime );
        TBuf<50> dateString;
        _LIT( KDateString3,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B" );
        time.FormatL( dateString, KDateString3 );
        LimitedAppend ( line, dateString );
        Print( line );
        
        // Print logging mode
        line = _L("    ");
        LimitedAppend ( line, _L( "Logging mode:") );
        Print( line );
        line = _L("     ");
        if ( KErrNone == loggingErr )
            {
            if ( EATUseDefault == loggingMode )
                {
                LimitedAppend ( line, _L( "External" ) );
                }
            else if ( EATLogToFile == loggingMode )
                {
                LimitedAppend ( line, _L( "Internal" ) );
                }
            else if ( EATLogToXti == loggingMode )
                {
                LimitedAppend ( line, _L( "External" ) );
                }
            else if( EATLoggingOff == loggingMode )
                {
                LimitedAppend ( line, _L( "None" ) );
                }
            }
        else
            {
            LimitedAppend ( line, _L( "unavailable" ) );
            }
        Print(line);
        
        // Print mode information (UDEB/UREL)
        line = _L("    ");
        LimitedAppend ( line, _L( "Mode:") );
        Print( line );
        line = _L("     ");
        TUint32 isUdeb;
        TInt udebErr = iMain->StorageServer().GetUdebL( iProcessId, isUdeb );
        if ( KErrNone == udebErr )
            {
            if ( isUdeb > KErrNone )
                {
                LimitedAppend ( line, _L( "UDEB" ) );
                }
            else
                {
                LimitedAppend ( line, _L( "UREL" ) );
                }
            }
        else
            {
            LimitedAppend ( line, _L( "unavailable" ) );
            }
        Print( line );

        // Print logging file information
        line = _L("    ");
        LimitedAppend ( line, _L( "Logging file:") );
        Print( line );
        line = _L("     ");  
        TBuf8<KMaxFileName> fileName;
        TInt fileErr = iMain->StorageServer().GetLoggingFileL( iProcessId, fileName );
        if ( KErrNone == fileErr )
            {
            if ( fileName.Length() > KErrNone )
                {
                TBuf<KMaxFileName> unicodeFile;
                CnvUtfConverter::ConvertToUnicodeFromUtf8( unicodeFile, fileName );
                LimitedAppend ( line, unicodeFile );
                }
            else
                {               
                LimitedAppend ( line, _L( "None" ) );
                }
            }
        else
            {
            LimitedAppend ( line, _L( "unavailable" ) );
            }        
        Print( line );
        
        line = _L("\n");
        Print( line );
        
        // Print process info menu
        TInt cursor( KErrNone );
        while ( cursor <= ( TInt )ECancelLoggingTxtItem - 1 ) // -1, dyninitmenupane
            {
            line.Zero();
            if ( cursor == 0 && iMain->IsSubTestRunning( iProcessId ) == KErrNotFound )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "Start subtest") );
                // Print the line
                Print( line );
                }
            else if ( cursor == 0 && iMain->IsSubTestRunning( iProcessId ) > KErrNotFound )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "Stop subtest") );
                // Print the line
                Print( line );
                } 
            else if ( cursor == 1 )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "View libraries") );
                // Print the line
                Print( line );
                }
            else if ( cursor == 2 )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "End process") );
                // Print the line
                Print( line );
                }
            else if ( cursor == 3 )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "Kill process") );
                // Print the line
                Print( line );
                }
            else if ( cursor == 4 )
                {
                AppendBefore( cursor, line );
                LimitedAppend ( line, _L( "Cancel logging") );
                // Print the line
                Print( line );
                }
            cursor++;
            }
        texts.Close();
        }
    else if ( iShowLibraries && !iSetLoggingMode )
        {
        ViewLibrariesL();
        }
    else if ( !iShowLibraries && iSetLoggingMode )
        {
        SetLoggingModeL();
        }

    }

// -----------------------------------------------------------------------------
// CProcessInfoView::SelectL()
// Process keypresses in menu.
// -----------------------------------------------------------------------------
//
CView* CProcessInfoView::SelectL( TKeyCode aSelection, TBool& aContinue )
    {
    MapKeyCode( aSelection );
    
    if( ( aSelection == EKeyRightArrow ||
            aSelection == EKeyEnter ) )
        {
        if ( !iSetLoggingMode )
            {
            if( iPositionOnScreen == EStartSubtestTxtItem )
                {
                if ( iMain->IsSubTestRunning( iProcessId ) == KErrNotFound )
                    iMain->SetProcessSubTestStart( iProcessId );
                else
                    iMain->SetProcessSubTestStop( iProcessId );
                return NULL;
                }
            else if( iPositionOnScreen == EViewLibrariesTxtItem - 1 )
                {
                iShowLibraries = ETrue;
                return NULL;
                }
            else if( iPositionOnScreen == EEndProcessTxtItem - 1 )
                {
                EndProcessL( ETryToEnd );
                // Trying to end process -> go processes view
                return iParent;
                }
            else if( iPositionOnScreen == EKillProcessTxtItem - 1 )
                {
                EndProcessL( ETryToKill );
                // Trying to kill process -> go processes view
                return iParent;
                }
            else if( iPositionOnScreen == ECancelLoggingTxtItem - 1 )
                {
                iMain->StorageServer().CancelLogging( iProcessId );
                return iParent;
                }
            else
                {
                return this;
                }               
            }
        else if ( iSetLoggingMode )
            {
            return NULL;
            }
        }

    return CView::SelectL( aSelection, aContinue );
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::ViewLibrariesL()
// Prints libraries loaded by current process.
// -----------------------------------------------------------------------------
//
void CProcessInfoView::ViewLibrariesL()
    { 
    // Clear display
    iConsole->ClearScreen();
    // Print menu title
    Print( _L( "Loaded libraries:") );

    TBuf<KMaxLineLength> line;  
    // Loaded libraries for this process
    RArray< TBuf8<KMaxLibraryName> > libraries;
    iMain->StorageServer().GetLoadedDllsL( iCurrentProcessInfo.iProcessId, libraries );
    TInt lCount = libraries.Count();
    for ( TInt count = 0 ; count < lCount ; count++ )
        {
        TBuf<KMaxLibraryName> library;
        library.Copy( libraries[ count ] );
        line = _L("    ");
        LimitedAppend ( line, library );
        Print(line);
        }
    libraries.Close();
    }


// -----------------------------------------------------------------------------
// CProcessInfoView::SetLoggingModeL()
// Prints logging modes which user wants to select to current process.
// -----------------------------------------------------------------------------
//
void CProcessInfoView::SetLoggingModeL()
    { 
    // Clear display
    iConsole->ClearScreen();
    // Print menu title
    Print( _L( "Logging mode:") );

    // Update pos screen
    if ( iPositionOnScreen > 2 )
        iPositionOnScreen = 0;
    else if ( iPositionOnScreen < 0 )
        iPositionOnScreen = 2;
    
    TBuf<KMaxLineLength> line;  

    // Print logging modes
    TInt cursor( KErrNone );
    while ( cursor <= 2 ) // Three items
        {
        line.Zero();
        if ( cursor == 0 )
            {
            AppendBefore( cursor, line );
            LimitedAppend ( line, _L( "External") );
            // Print the line
            Print( line );
            }
        else if ( cursor == 1 )
            {
            AppendBefore( cursor, line );
            LimitedAppend ( line, _L( "Internal") );
            // Print the line
            Print( line );
            } 
        else if ( cursor == 2 )
            {
            AppendBefore( cursor, line );
            LimitedAppend ( line, _L( "None") );
            // Print the line
            Print( line );
            }
        cursor++;
        }  
    }

// -----------------------------------------------------------------------------
// CProcessInfoView::EndProcessL()
// Ends process with a specific method.
// -----------------------------------------------------------------------------
//
void CProcessInfoView::EndProcessL( TInt aCommand )
    {
     RProcess endProcess;
     TUint processId = iProcessId;
     TBuf<KMaxProcessName> processName;
     processName.Copy( Name() );
     TInt find = processName.Find( _L( "." ) );
     if ( find > KErrNotFound )
         processName.Delete( find, processName.Length() - find );
     
     TInt openErr = endProcess.Open( TProcessId( processId ) );
     if ( openErr ==KErrNone )
         {
         if ( aCommand == ETryToEnd )
             {
             endProcess.Close();
             RWsSession wsSession;
             TInt wsErr = wsSession.Connect();
             if ( KErrNone == wsErr )
                 {
                 TApaTaskList apList = TApaTaskList( wsSession );
                 TApaTask apTask = apList.FindApp( processName );
                 if ( apTask.Exists() )
                     {
                     apTask.EndTask();
                     }
                 wsSession.Close();
                 }

             }
         else if ( aCommand == ETryToKill )
             {               
             endProcess.Kill( KErrNone );
             endProcess.Close();
             }
         }
    }

// End of file
