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

#ifndef ATCONSOLE_VIEWS_H
#define ATCONSOLE_VIEWS_H


//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS
const TInt KViewOverhead = 4;

// Numeric key ascii code values for ATConsoleUI's menu
const TInt KMyKeyDownAsciiCode = 56; // Ascii code for number '8'
const TInt KMyKeyLeftAsciiCode = 52; // Ascii code for number '4'
const TInt KMyKeyRightAsciiCode = 54; // Ascii code for number '6'
const TInt KMyKeyUpAsciiCode = 50;  // Ascii code for number '2'

// FORWARD DECLARATIONS
class CView;
class CConsoleMain;

// CLASS DECLARATION

/**
 * @class CView atconsoleviews.h
 * @brief The CView is used for making views needed in the console
 * application.
 */
class CView
        :public CBase
    {
    public:  // Enumerations
        enum TUpdateType
            {
            EViewPrint,
            EViewRefresh,
            };

    public:  // Constructors and destructor
        // None

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CView* NewL( CConsoleMain* aConsole,
                            CView* aParent,
                            const TDesC& aName, TInt64 aProcessId );

       /**
        * Destructor of CConsoleMain.
        */
        virtual ~CView();

    public: // New functions
    
        /** 
        * Return menu name
        */
        virtual const TDesC& Name() const;

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RArray<TDesC>& aArray );

        /** 
        * Prints the view
        */
        virtual void PrintViewL( TUpdateType  aType );

        /** 
        * Process keypresses on this menu
        */
        virtual CView* SelectL( TKeyCode aSelection, TBool& aContinue );

        /** 
        * Set parent menu
        */
        virtual void SetParent ( CView* aMenu );

        /** 
        * Print line
        */
        virtual void Print( const TDesC& aPrint );

        /** 
        * Add item to menu
        */
        virtual void AddItemL ( CView* aItem );

        /** 
        * Resets the item list
        */
        virtual void ResetItems();

        /** 
        * Update display from timer
        */
		virtual void TimerUpdate();
		
		/**
		* Append text before original text.
		*/
		virtual void AppendBefore(  TInt aLineNum, TDes& aLine );
		
		/**
		* Get menu type.
		*/
		inline TInt Type(){ return iType; };
		
		/**
		* Map KeyCode 
		*/
		void MapKeyCode( TKeyCode &aSelection );

    protected:  // Functions from base classes
        // None    

        /** 
        * C++ default constructor.
        */
        CView();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole,
                         CView* aParent,
                         const TDesC& aName, TInt64 aProcessId );

    protected:  // Data    
    
        CConsoleBase*           iConsole;      // Pointer to console
        CConsoleMain*           iMain;         // Pointer to main console
        CView*                  iParent;       // Pointer to parent menu
        TName                   iViewName;         // Menu name
        TInt64                  iProcessId;    // Process id of the menu

        RPointerArray<CView>    iItems;        // Menu items

        TInt                    iPositionOnScreen;  // Position on display
        TInt                    iFirstItem;        // First displayed item
        TInt                    iLastItem;         // Last displayed item
        TInt                    iItemCount;    // Last Item
        TSize                   iSize;         // Display size
        TInt                    iScreenSize;   // "Work area" size
		TInt                    iDirection;    // Scrolling direction
		TInt                    iStart;        // Scrolling position
		TInt                    iPrevPos;      // Previous position in scrolling
        // Menu type (which updates must cause console update) 
        TInt                    iType;
		TUpdateType             iUpdateType;
		// Determines if libraries are to be shown
		TBool   iShowLibraries;
		// Determines if set logging mode query is to be shown
		TBool iSetLoggingMode;
    };

/**
 * @class CProcessInfoView atconsoleviews.h
 * @brief The CProcessInfoView is used for showing information about
 * some process and starting/stopping subtests.
 */
class CProcessInfoView 
    :public CView
  
    {

    private: // Enumerations
        enum TProcessInfoView
            {
            EStartSubtestTxtItem,
            EStopSubtestTxtItem,
            EViewLibrariesTxtItem,
            EEndProcessTxtItem,
            EKillProcessTxtItem,
            ECancelLoggingTxtItem
            };

        enum TProcessEnds
            {
            ETryToEnd,
            ETryToKill
            };
            
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CProcessInfoView* NewL( CConsoleMain* aConsole,
                                      CView* aParent,
                                      const TDesC& aName, TInt64 aProcessId );

       /**
        * Destructor
        */
        ~CProcessInfoView();

    public: // Functions from base classes

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RArray<TDesC>& aArray );
        
        /** 
        * Process keypresses on this menu
        */
        virtual CView* SelectL( TKeyCode aSelectLion, 
                                TBool& aContinue );
 
        /** 
        * Prints the view
        */
        virtual void PrintViewL( TUpdateType  aType );
        
        /** 
        * Prints libraries
        */
        void ViewLibrariesL();
 
        /** 
        * Prints logging modes
        */
        void SetLoggingModeL();
        
        /**
        * Ends a process with a certain method.
        * @param aCommand command id
        */
        void EndProcessL( TInt aCommand );
        
    protected:  // New functions
        // None

    protected:  // Functions from base classes
        // None
       
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CView* aParent,
                         const TDesC& aName, TInt64 aProcessId );

    private:    // Data
        // Current opened process's info
        TATProcessInfo iCurrentProcessInfo;
        
    };

// CONSTANTS
_LIT( KExitTxt,     "Exit" );

/**
 * @class CMainView atconsoleviews.h
 * @brief The CMainView is used for showing all the processes
 * currently under testing.
 */
class CMainView 
    :public CView
  
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CMainView* NewL( CConsoleMain* aConsole, 
                                CView* aParent, 
                                const TDesC& aName );

    public: // Functions from base classes    

        /** 
        * Return item texts.
        */
        virtual TInt ItemTexts( RArray<TDesC>& aArray );

        /** 
        * Process keypresses on this menu
        */
        virtual CView* SelectL( TKeyCode aSelectLion, TBool& aContinue );

    protected:  // Functions from base classes

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aConsole, 
                         CView* aParent,
                         const TDesC& aName );

    };
 
#endif // ATCONSOLE_VIEWS_H

// End of File
