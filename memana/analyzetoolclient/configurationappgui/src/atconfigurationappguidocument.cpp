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

#include "atconfigurationappguidocument.h"
#include "atconfigurationappguiappui.h"
#include "atlog.h"

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiDocument::CATConfigurationAppGuiDocument()
// Constructs the document class for the application.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiDocument::CATConfigurationAppGuiDocument( CEikApplication& anApplication )
    : CAknDocument( anApplication )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiDocument::CATConfigurationAppGuiDocument()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiDocument::ConstructL()
// Completes the second phase of Symbian object construction.
// -----------------------------------------------------------------------------
//
void CATConfigurationAppGuiDocument::ConstructL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiDocument::CATConfigurationAppGuiDocument()" );
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiDocument::NewL()
// Creates an instance of CATConfigurationAppGuiDocument, constructs it, and
// returns it.
// -----------------------------------------------------------------------------
//
CATConfigurationAppGuiDocument* CATConfigurationAppGuiDocument::NewL( 
	CEikApplication& aApp )
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiDocument::NewL()" );
    
    CATConfigurationAppGuiDocument* self = 
		new ( ELeave ) CATConfigurationAppGuiDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CATConfigurationAppGuiDocument::CreateAppUiL()
// Creates the application UI object for this document.
// -----------------------------------------------------------------------------
//
CEikAppUi* CATConfigurationAppGuiDocument::CreateAppUiL()
    {
    LOGSTR1( "ATCU CATConfigurationAppGuiDocument::CreateAppUiL()" );
    
    return new ( ELeave ) CATConfigurationAppGuiAppUi;
    }

// End of File

