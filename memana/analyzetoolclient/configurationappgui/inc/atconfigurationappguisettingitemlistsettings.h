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

#ifndef ATCONFIGURATIONAPPGUISETTINGITEMLISTSETTINGS_H
#define ATCONFIGURATIONAPPGUISETTINGITEMLISTSETTINGS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

/**
 * @class	CATConfigurationAppGuiSettingItemListSettings
 */
class CATConfigurationAppGuiSettingItemListSettings: public CBase
	{
public:

    /**
    * Two-phased constructor that can leave.
    * @return A new instance of this class
    */
	static CATConfigurationAppGuiSettingItemListSettings* NewL();
	
    /**
    *  Symbian 2nd phase constructor.
    */
	void ConstructL();
		
private:
    
    /**
    * C++ default constructor.
    */
	CATConfigurationAppGuiSettingItemListSettings();
public:
    
    /**
    * Returns value of binary setting 1.
    * @return TBool& reference to selected binary setting
    */
	TBool& BinarySetting1();
	
    /**
    * Sets binary 1 setting value
    * @param aValue binary value to be set
    */
	void SetBinarySetting1( const TBool& aValue );
	
    /**
    * Returns enumerated text value from first popup setting.
    * @return TDes& reference to selected text
    */
	TDes& EnumeratedTextPopup1();
	
    /**
    * Returns the value of application's update interval.
    * @return TInt update interval
    */
	TInt UpdateInterval();
	
    /**
    * Sets value of the update interval
    * @param aUpdate update interval value to be set
    */
	void SetUpdateIntervalL( TInt aUpdate );
	
    /**
    * Sets value of the enumerated text value for
    * first popup settting.
    * @param aValue text value to be set
    */
	void SetEnumeratedTextPopup1L( const TDesC& aValue );
	
protected:
    // First binary setting value
	TBool iBinarySetting1;
	// Value of the update interval
	TInt iUpdateInterval;
	// Temporary buffer holding setting text shown
	// to the user
	TBuf<50> iTemp;
	
	};
#endif // ATCONFIGURATIONAPPGUISETTINGITEMLISTSETTINGS_H
