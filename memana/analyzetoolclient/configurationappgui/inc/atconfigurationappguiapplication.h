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

#ifndef ATCONFIGURATIONAPPGUIAPPLICATION_H
#define ATCONFIGURATIONAPPGUIAPPLICATION_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS
const TUid KUidATConfigurationAppGuiApplication = { 0x20012432/*0xEBE42C45*/ };

/**
 *
 * @class CATConfigurationAppGuiApplication ATConfigurationAppGuiApplication.h
 * @brief A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CATConfigurationAppGuiApplication : public CAknApplication
    {
public:
    /**
    * From CEikApplication.
    */
    //CDictionaryStore* OpenIniFileLC( RFs& aFs ) const;
private:
    /**
    * From CApaApplication.
    */
    TUid AppDllUid() const;
    /**
    * From CEikApplication.
    */
    CApaDocument* CreateDocumentL();
    };
            
#endif // ATCONFIGURATIONAPPGUIAPPLICATION_H
