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

#include "atconfigurationappguiapplication.h"
#include "atconfigurationappguidocument.h"
#include "atlog.h"
#include <eikstart.h>

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiApplication::AppDllUid()
// Returns UID for this application (KUidATConfigurationAppGuiApplication)
// -----------------------------------------------------------------------------
//
TUid CATConfigurationAppGuiApplication::AppDllUid() const
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiApplication::AppDllUid()" );
    
    return KUidATConfigurationAppGuiApplication;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiApplication::CreateDocumentL()
// Returns Pointer to the created document object (CATConfigurationAppGuiDocument)
// -----------------------------------------------------------------------------
//
CApaDocument* CATConfigurationAppGuiApplication::CreateDocumentL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiApplication::CreateDocumentL()" );
    
    return CATConfigurationAppGuiDocument::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiApplication::NewApplication()
// Returns The application (CATConfigurationAppGuiApplication)
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiApplication::NewApplication()" );
    
    return new CATConfigurationAppGuiApplication;
    }

// -----------------------------------------------------------------------------
// GLDEF_C TInt E32Main()
// This standard export is the entry point for all Series 60 applications.
// Returns error code
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiApplication::E32Main()" );
    
    return EikStart::RunApplication( NewApplication );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiApplication::OpenIniFileLC()
// To support .ini files for storing application settings
// -----------------------------------------------------------------------------
//
/*CDictionaryStore* CATConfigurationAppGuiApplication::OpenIniFileLC( RFs& aFs ) const
    {
    return CEikApplication::OpenIniFileLC( aFs );
    }*/

// End of File

