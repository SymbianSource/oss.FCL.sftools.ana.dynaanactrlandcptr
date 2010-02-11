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


#ifndef ENGINEUIDS_H
#define ENGINEUIDS_H

#include <e32cmn.h>

// No item is selected in the container's listbox
const TUid KSamplerNoneSelected = { 0x00000001 };
const TUid KWriterNoneSelected = { 0x00000002 };

/*
 * Sampler plugin implementation UIDs
 * 
 */
// Use this UID if plugin is GeneralsPlugin:
const TUid KSamplerGeneralsPluginUid = { 0x2001E5B2 };

/*
 *
 * The sub samplers of GeneralsPlugin
 * NOTE: NOT real reserved UIDs!!!
 * 
 */
	// Use this UID if plugin is GppPlugin:
	const TUid KSamplerGppPluginUid = { 0x2001E570 };
	
	// Use this UID if plugin is GfcPlugin:
	const TUid KSamplerGfcPluginUid = { 0x2001E571 };
	
	// Use this UID if plugin is IttPlugin:
	const TUid KSamplerIttPluginUid = { 0x2001E572 };
	
	// Use this UID if plugin is MemPlugin:
	const TUid KSamplerMemPluginUid = { 0x2001E573 };
	
	// Use this UID if plugin is PriPlugin:
	const TUid KSamplerPriPluginUid = { 0x2001E574 };


// Use this UID if plugin is GeneralsPlugin:
const TUid KSamplerInternalsPluginUid = { 0x2001E5B4 };

/*
 *
 * The sub samplers of InternalsPlugin
 * NOTE: NOT real reserved UIDs!!!
 * 
 */
	// Use this UID if plugin is IrqPlugin:
	const TUid KSamplerIrqPluginUid = { 0x2001E575 };
	
	// Use this UID if plugin is SwiPlugin:
	const TUid KSamplerSwiPluginUid = { 0x2001E577 };
	
	// Use this UID if plugin is PecPlugin:
	const TUid KSamplerPecPluginUid = { 0x2001E579 };
	
	// Use this UID if plugin is L2IPlugin:
	const TUid KSamplerL2IPluginUid = { 0x2001E581 };

/*
 *
 * Other plugins/samplers
 *
 */
 
// Use this UID if plugin is BupPlugin:
const TUid KSamplerBupPluginUid = { 0x2001E5B6 };

// Use this UID if plugin is IsaPlugin:
const TUid KSamplerIsaPluginUid = { 0x2001E5B8 };

// Use this UID if plugin is PwrPlugin:
const TUid KSamplerPwrPluginUid = { 0x2001E5B9 };

// Use this UID if plugin is TipPlugin:
//const TUid KSamplerTipPluginUid = { 0x20201F78 };

// Use this UID if plugin is ScrPlugin:
//const TUid KSamplerScrPluginUid = { 0x20201F82 };





// NOTE: The following UIDs are reserved for own sampler plug-in development!!!
// Use this UID if plugin is Ext1Plugin:
const TUid KSamplerExt1PluginUid = { 0x20201F84 };
// Use this UID if plugin is Ext2Plugin:
const TUid KSamplerExt2PluginUid = { 0x20201F85 };
// Use this UID if plugin is Ext3Plugin:
const TUid KSamplerExt3PluginUid = { 0x20201F86 };
// Use this UID if plugin is Ext4Plugin:
const TUid KSamplerExt4PluginUid = { 0x20201F87 };
// Use this UID if plugin is Ext5Plugin:
const TUid KSamplerExt5PluginUid = { 0x20201F88 };
// Use this UID if plugin is Ext6Plugin:
const TUid KSamplerExt6PluginUid = { 0x20201F89 };


/*
// UID of Profiler Engine application.
// This UID is used for accessing the help IDs controlled by Sampler help files
const TUid KUidSampler = { 0x200058EC };
*/

/*
 * Writer plugin implementation UIDs
 * 
 */
// Use this UID if plugin belongs to DebOutWriterPlugin:
const TUid KDebOutWriterPluginUid = { 0x2001E5BA };

// Use this UID if plugin belongs to MmcOutWriterPlugin:
const TUid KDiskWriterPluginUid = { 0x2001E5BB };

#endif // ENGINEUIDS_H
// End of File
