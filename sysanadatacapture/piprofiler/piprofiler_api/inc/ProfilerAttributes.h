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


#ifndef PROFILER_ATTRIBUTES_H
#define PROFILER_ATTRIBUTES_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <s32mem.h>

// LITERALS
_LIT8(KDefaultTraceFilePrefix, "PIProfiler_#");
_LIT8(KDefaultTraceOutput, "file_system");
_LIT8(KDefaultTraceFileSaveDrive, "C:\\data\\");

// CONSTANTS
const TUint KPrefixMaxLength = 64;

/*
 * 
 * TGeneralAttributes class definition, internal settings format
 *  
 */
class TGeneralAttributes
    {
public:
    TBuf8<KPrefixMaxLength> iTraceOutput;
    TBuf8<KPrefixMaxLength> iTraceFilePrefix;
    TBuf8<KPrefixMaxLength> iSaveFileDrive;
    };



/*
 * 
 * TSettingItem class definition, internal settings format
 *  
 */
class TSettingItem
    {
public:
    enum 
    {
        ESettingItemTypeInt = 0,
        ESettingItemTypeBool,
        ESettingItemTypeHex,
        ESettingItemTypeText
    };
    
public:
    TBuf<64>   iSettingText;
    TUint32    iType;
    TBuf<128>  iValue;
    TBuf<256>  iSettingDescription;
    TBuf<64>   iUIText;
    };
/*
 * 
 * TSamplerAttributes class definition, internal settings format
 *  
 */
class TSamplerAttributes
    {
public:
    // default constructor
    TSamplerAttributes();
    // constructor
    TSamplerAttributes(TInt32 aUid,
            const TDesC8& aShortName,
            const TDesC8& aName,
            const TDesC8& aDescription,
            TInt aSampleRate,
            TBool aEnabled,
            TBool aHidden,
            TInt aItemCount);
public:
    TInt32      iUid;
    TBuf8<8>    iShortName;     // name of the plugin, short name
    TBuf8<64>   iName;          // name of the plugin, long name
    TBuf8<256>  iDescription;   // sampler description, info about HW/SW dependencies etc.
    TInt        iSampleRate;    // sample rate of the plugin
    TBool       iEnabled;       // previous state of plugin
    TBool       iIsHidden;      
    TInt        iItemCount;     // plugin specific setting item count
    
    // plugin specific settings, plugin implementation dependent
    TSettingItem    iSettingItem1;
    TSettingItem    iSettingItem2;
    TSettingItem    iSettingItem3;
    TSettingItem    iSettingItem4;
    TSettingItem    iSettingItem5;
    TSettingItem    iSettingItem6;
    };

inline TSamplerAttributes::TSamplerAttributes()
    {}

inline TSamplerAttributes::TSamplerAttributes(TInt32 aUid,
            const TDesC8& aShortName,
            const TDesC8& aName,
            const TDesC8& aDescription,
            TInt aSampleRate,
            TBool aEnabled,
            TBool aHidden,
            TInt aItemCount)
    {
    iUid = aUid;
    iShortName.Copy(aShortName);
    iName.Copy(aName);
    iDescription.Copy(aDescription);
    iSampleRate = aSampleRate;
    iEnabled = aEnabled;
    iIsHidden = aHidden;
    iItemCount = aItemCount;
    }

#endif
