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

#ifndef ATCONSOLE_UI_H
#define ATCONSOLE_UI_H

//  INCLUDE
#include <e32std.h>
#include <e32base.h>

#include <analyzetool/atstorageserverclnt.h>
#include <analyzetool/analyzetool.h>

// CONSTANTS
const TInt KMaxLineLength = 80;            // Max. line length
const TInt KScrollPeriod = 300000;		   // scrolling period
const TUint32 KATHundred = 100;
const TUint32 KATThousand = 1000;
const TUint32 KATMillion = 1000000;
_LIT( KATB, "B" );
_LIT( KATKB, "KB" );
_LIT( KATMB, "MB" );

// MACROS

// FORWARD DECLARATIONS
class CView;
class CConsoleMain;
class CConsoleReader;

// CLASS DECLARATION

/**
 * @class CScrollerTimer atconsoleui.h
 * @brief The CSrollerTimer is used for updating the application
 * user interface.
 */
class CScrollerTimer
        :public CActive
    {       
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CScrollerTimer* NewL( CConsoleMain* aMain );

        /**
        * Destructor of CModule.
        */
        ~CScrollerTimer();

    public: // New functions

		/** 
        * StartL
        */
		void StartL();

		/** 
        * RunL
        */
		void RunL();

		/** 
        * DoCancel
        */
		void DoCancel();

		/** 
        * RunError
        */
		TInt RunError ( TInt aError );

    private:

        /** 
        * C++ default constructor.
        */
        CScrollerTimer();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( CConsoleMain* aMain );

    protected:  // Data
        // None

    private:    // Data
        CConsoleMain*     iMain;            // Pointer to main console
		RTimer            iTimer;
    };

/**
 * @class CConsoleMain atconsoleui.h
 * @brief The CConsoleMain is main class for constructing the console
 * application.
 */
class CConsoleMain: public CBase
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CConsoleMain* NewL();

        /**
        * Start
        */
        TInt StartL();

        /**
        * Returns the array of processes under testing
        */
        RArray<TATProcessInfo> Processes();

        /**
        * Returns handle to the storage server
        * @return RATStorageServer handle to the storage server
        */  
        inline RATStorageServer StorageServer()
            {
            return iStorageServer;    
            }

        /**
        * Starts a subtest for a process.
        * @param aProcessId ID of the process
        */
        void SetProcessSubTestStart( TUint aProcessId );
        
        /**
        * Stops a subtest for a process.
        * @param aProcessId ID of the process
        */
        
        void SetProcessSubTestStop( TUint aProcessId );
        
        /**
        * Checks if subtest is running for a process.
        * @param aProcessId ID of the process
        * @return The index of the first matching aProcessId 
        * within the array. KErrNotFound, if no matching 
        * aProcessId can be found.
        */
        TInt IsSubTestRunning( TUint aProcessId );
        
        /**        
        * Stops all the subtests currently running.
        */   
        void StopAllSubtestsL();
        
        /**
        * Destructor of CConsoleMain.
        */
        ~CConsoleMain();
        
        /**
        * Updates processes.
        */       
        void UpdateProcessesL();


    public: // New functions

        /**
        * Close instance.
        */
        void Close( TInt aHandle );

        /**
        * Return pointer to console
        */
        CConsoleBase* GetConsole();

        /**
        * Called from timer to perform scrolling
        */
        void TimerUpdate();
                
        /**
        * Process keystokes.
        */
        void KeyPressedL();

    private:

        /** 
        * C++ default constructor.
        */
        CConsoleMain();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();
        
    protected:  // Data
        // None

    private:    // Data
        CConsoleBase*       iConsole;       // Pointer to console

        CView*              iMainView;      // Root menu
        CView*              iCurrentView;   // Current menu

        CScrollerTimer*     iScroller;
        
        CConsoleReader*     iReader;        // Console reader                   
        // Handle to the RATStorageServer
        RATStorageServer iStorageServer;
        // Array of processes under testing
        RArray<TATProcessInfo> iProcesses;
        // array containing all the processes which subtest is running
        RArray<TUint> iSubTestProcesses;
        TBool             iStorageServerOpen;
    };
    
/**
 * @class CConsoleReader atconsoleui.h
 * @brief The CConsoleReader is reading keystrokes from console.
 */
class CConsoleReader
    :public CActive
    {
    public:  // Constructors and destructor

        /**
        * NewL is first phase of two-phased constructor.
        */
        static CConsoleReader* NewL( CConsoleMain* aMain, 
                                     CConsoleBase* aConsole );

        /**
        * Start
        */
        void StartL();

        /**
        * Destructor of CConsoleReader.
        */
        ~CConsoleReader();

    public: // Functions from base classes

        /**
        * RunL derived from CActive handles the completed requests.
        */
        void RunL();

        /**
        * DoCancel derived from CActive handles the Cancel
        */
        void DoCancel();
        /**
        * RunError derived from CActive handles errors from active object
        */        
        virtual TInt RunError(TInt aError);

    private:

        /** 
        * C++ default constructor.
        */
        CConsoleReader( CConsoleMain* aMain, 
                        CConsoleBase* iConsole );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

    public:   //Data
        // None
    
    protected:  // Data
        // None

    private:    // Data
        CConsoleBase*   iConsole;   // Pointer to console
        CConsoleMain*   iMain;      // Pointer to the CConsoleMain
    };

#endif // ATCONSOLEUI_H

// End of File
