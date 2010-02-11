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

#ifndef ATCONFIGURATIONAPPGUIAPPUI_H
#define ATCONFIGURATIONAPPGUIAPPUI_H

// INCLUDES
#include <aknviewappui.h>
#include <analyzetool/atstorageserverclnt.h>
#include <analyzetool/atcommon.h>
#include <analyzetool/analyzetool.h>

// FORWARD DECLARATIONS
class CApaApplication;
class CATConfigurationAppGuiListBoxView;
class CATConfigurationAppGuiProcessView;
//class CATConfigurationAppGuiSettingItemListView;

// CONSTANTS
const TUint32 KATHundred = 100;
const TUint32 KATThousand = 1000;
const TUint32 KATMillion = 1000000;
_LIT( KATB, "B" );
_LIT( KATKB, "KB" );
_LIT( KATMB, "MB" );

/**
 * @class CATConfigurationAppGuiAppUi ATConfigurationAppGuiAppUi.h
 * @brief The AppUi class handles application-wide aspects of the user interface, including
 *        view management and the default menu, control pane, and status pane.
 */
class CATConfigurationAppGuiAppUi : public CAknViewAppUi
    {
public: 
    /**
    * C++ default constructor.
    */
    CATConfigurationAppGuiAppUi();
    
    /**
    * Destructor.
    */
    virtual ~CATConfigurationAppGuiAppUi();
    
    /**
    * ConstructL.
    */
    void ConstructL();

public:
    /**
    * From CCoeAppUi.
    */
    TKeyResponse HandleKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    /**
    * from CEikAppUi.
    */
    void HandleCommandL( TInt aCommand );
    
    /**
    * From CEikAppUi.
    */
    void HandleResourceChangeL( TInt aType );

    /**
    * From CAknAppUi.
    */
    void HandleViewDeactivation( 
            const TVwsViewId& aViewIdToBeDeactivated, 
            const TVwsViewId& aNewlyActivatedViewId );
    
    /**
    * Returns handle to the storage server
    * @return RATStorageServer handle to the storage server
    */  
    inline RATStorageServer StorageServer()
        {
        return iStorageServer;    
        }
    
    /**
    * Sets current process's process ID.
    * @param aProcessId current process's process ID
    */
    inline void SetCurrentProcessId( TUint aProcessId )
        {
        iCurrentProcessId = aProcessId;
        }
    
    /**
    * Gets current process ID.
    * @return TUint process ID
    */
    inline TUint GetCurrentProcessId()
        {
        return iCurrentProcessId;    
        }
    
    /**
    * Sets member variable's process start time.
    * @param aProcessStartTime current process's start time
    */
    inline void SetCurrentProcessStartTime( TInt64 aProcessStartTime )
        {
        iCurrentProcessStartTime = aProcessStartTime;
        }
    
    /**
    * Get current opened process's start time.
    * @return TInt64 start time presented in microseconds
    */
    inline TInt64 GetCurrentProcessStartTime()
        {
        return iCurrentProcessStartTime;    
        }

    /**
    * Starts a subtest for a process.
    * @param aProcessId ID of the process
    */
    void SetProcessSubTestStartL( TUint aProcessId );
    
    /**
    * Stops a subtest for a process.
    * @param aProcessId ID of the process
    */
    
    void SetProcessSubTestStopL( TUint aProcessId );
    
    /**
    * Checks if subtest is running for a process.
    * @param aProcessId ID of the process
    * @return The index of the first matching aProcessId 
    * within the array. KErrNotFound, if no matching 
    * aProcessId can be found.
    */
    TInt IsSubTestRunning( TUint aProcessId );

    /**
    * Loads application settings.
    * @return TInt KErrNone if loading succeeds.
    */
    TInt LoadSettingsL();
    
    /**
    * Shows error note.
    * @param aResourceId resource id of the text which
    * is to be shown to the user.
    */    
    void ShowErrorNoteL( TInt aResourceId );

    /**        
    * From CEikAppUi.
    */      
    void HandleForegroundEventL( TBool aForeground );
    

    /**        
    * Starts periodic timer with an interval.
    */      
    void StartTimerL();

    /**        
    * Periodic timer's callback function.
    * @param ptr TAny pointer.
    * @return TInt KErrNone.
    */      
    static TInt CallBackFunctionL( TAny* ptr )
        {
        ( static_cast<CATConfigurationAppGuiAppUi*>( ptr ) )->TimerNotifyL( ETrue );
        return KErrNone;
        };

    /**        
    * Periodic timer's notify function.
    * @param aPeriod ETrue if certain time interval is reached, EFalse otherwise.
    */        
    void TimerNotifyL( TBool aPeriod );
 
    /**        
    * Stops all the subtests currently running.
    */   
    void StopAllSubtestsL();

private:
    /**
    * Constructs needed components in the AppUi.
    */
    void InitializeContainersL();
    
private:
    // Processes view pointer
    CATConfigurationAppGuiListBoxView* iATConfigurationAppGuiListBoxView;
    // Process view pointer
    CATConfigurationAppGuiProcessView* iATConfigurationAppGuiProcessView;
    // Settings view pointer
    //CATConfigurationAppGuiSettingItemListView* iATConfigurationAppGuiSettingItemListView; 
    // Handle to the RATStorageServer
    RATStorageServer iStorageServer;
    // Current process ID
    TUint iCurrentProcessId;
    // Current process's start time
    TInt64 iCurrentProcessStartTime;
    // Warning tones setting
    TInt iWarningTones;
    // Update interval setting
    TInt iUpdateInterval;
    // array containing all the processes which subtest is running
    RArray<TUint> iSubTestProcesses;
    // Periodic timer to handle timer events
    CPeriodic*  iPeriodicTimer;
    // Deactivated view's Id
    TVwsViewId  iDeactivatedId;
    };

#endif // ATCONFIGURATIONAPPGUIAPPUI_H
