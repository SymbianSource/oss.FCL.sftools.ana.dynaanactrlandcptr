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

#ifndef ATCONFIGURATIONAPPGUI_PAN_H
#define ATCONFIGURATIONAPPGUI_PAN_H

/** ATConfigurationAppGui application panic codes */
enum TATConfigurationAppGuiPanics
    {
    EATConfigurationAppGuiUi = 1
    // add further panics here
    };

inline void Panic(TATConfigurationAppGuiPanics aReason)
    {
    _LIT(applicationName,"ATConfigurationAppGui");
    User::Panic(applicationName, aReason);
    }

#endif // ATCONFIGURATIONAPPGUI_PAN_H
