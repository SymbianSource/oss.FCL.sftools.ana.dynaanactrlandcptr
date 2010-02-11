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

#ifndef ATCONFIGURATIONAPPGUIDOCUMENT_H
#define ATCONFIGURATIONAPPGUIDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CEikAppUi;

/**
* @class CATConfigurationAppGuiDocument ATConfigurationAppGuiDocument.h
* @brief A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CATConfigurationAppGuiDocument : public CAknDocument
    {
public: 
    /**
    * Creates new document object.
    * @param aApp Reference to the CEikApplication
    * @return CATConfigurationAppGuiDocument* pointer to the
    * newly created document class.
    */
    static CATConfigurationAppGuiDocument* NewL( CEikApplication& aApp );

private: 
    /**
    * C++ default constructor.
    * @param aApp Reference to the CEikApplication
    */
    CATConfigurationAppGuiDocument( CEikApplication& aApp );
    
    /**
    * ConstructL.
    */
    void ConstructL();
    
public: 

    /**
    * From CEikDocument.
    */
    CEikAppUi* CreateAppUiL();
    };
#endif // ATCONFIGURATIONAPPGUIDOCUMENT_H
