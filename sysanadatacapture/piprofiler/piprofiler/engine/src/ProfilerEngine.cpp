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


#include <e32cons.h>
#include <e32base.h>
#include <f32file.h>
#include <c32comm.h>
#include <s32file.h>
#include <pathinfo.h>
#include <s32mem.h>
#include <bautils.h>
#include <sysutil.h>
#include <piprofiler/ProfilerConfig.h>
#include "ProfilerEngine.h"
#include <piprofiler/ProfilerTraces.h>
#include <piprofiler/EngineUIDs.h>

// properties
const TUid KEngineStatusPropertyCat={0x2001E5AD};
enum TEnginePropertyKeys
	{
	EProfilerEngineStatus = 8,
	EProfilerErrorStatus
	};

static _LIT_SECURITY_POLICY_PASS( KAllowAllPolicy );

// CONSTANTS 
const TInt KStreamBufferSize = 32768;
const TInt KSavedLineCount = 64;
const TInt KFileNameBufSize = 128;

// LITERALS
_LIT8(KGenericTraceOutput, "output_type");
_LIT8(KGenericTraceFilePrefix, "file_prefix");
_LIT8(KGenericTraceFileSaveDrive, "save_file_location");
_LIT8(KEquals, "=");
_LIT8(KNewLineSeparator, "\n");
_LIT8(KProfilerVersionTag, "version");
_LIT8(KEndMark, "[end]");
_LIT8(KOutputToDebugOutput, "debug_output");

/** 
 * 
 * The controller class used to provide the Profiler functions. 
 * This runs as a server in the engine thread
 * 
 */
class CPServer : public CServer2, public MProfilerController
    {
public:
    static MProfilerController*		NewL(TInt aPriority, MProfilerEngine& aEngine);

private:
                        CPServer(TInt aPriority, MProfilerEngine& aEngine);
    void				Release();
    CSession2*			NewSessionL(const TVersion& aVersion,const RMessage2& aMessage) const;
    };

// The session class used by the server controller
class CPSession : public CSession2
    {
private:
    inline const CPServer&	Server() const;
    void					ServiceL(const RMessage2& aMessage);
    };

/*
 *
 *	CProfiler class implementation
 *
 */
// --------------------------------------------------------------------------------------------
CProfiler* CProfiler::NewLC(const TDesC& aSettingsFile)
    {
	CProfiler* self = new(ELeave) CProfiler(aSettingsFile);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

// --------------------------------------------------------------------------------------------
CProfiler::CProfiler(const TDesC& aSettingsFile) : 
    iSettingsFileLocation(aSettingsFile)
	{
	// define property for Profiler Engine status, UI may read it for control purposes
	if ( RProperty::Define(KEngineStatusPropertyCat, 
	        EProfilerEngineStatus, 
	        RProperty::EInt, 
	        KAllowAllPolicy, 
	        KAllowAllPolicy, 
	        0) != KErrAlreadyExists )
	    {
	    LOGTEXT(_L("CProfiler::CProfiler - status property already defined"));
	    }

	if ( RProperty::Define(KEngineStatusPropertyCat, 
	        EProfilerErrorStatus, 
            RProperty::EInt, 
            KAllowAllPolicy, 
            KAllowAllPolicy, 
            0) != KErrAlreadyExists )
        {
        LOGTEXT(_L("CProfiler::CProfiler - status property already defined"));
        }

	// attach to own property
	iEngineStatus.Attach(KEngineStatusPropertyCat, EProfilerEngineStatus);
	// set status idle
	iEngineStatus.Set(KEngineStatusPropertyCat, EProfilerEngineStatus, RProfiler::EIdle);
	
    // attach to own property
    iUpdateStatus.Attach(KEngineStatusPropertyCat, EProfilerErrorStatus);
    // set status idle
    iUpdateStatus.Set(KEngineStatusPropertyCat, EProfilerErrorStatus, EFalse);
	}

// --------------------------------------------------------------------------------------------
CProfiler::~CProfiler()
    {
	LOGTEXT(_L("CProfiler::~CProfiler - Enter"));

	// delete error checker
    if(iErrorChecker)
        {
        iErrorChecker->Cancel();
        delete iErrorChecker;
        iErrorChecker = NULL;
        }
	
	// delete settings array
	if(iDefaultSamplerAttributesArray)
	    {
	    iDefaultSamplerAttributesArray->Reset();
	    delete iDefaultSamplerAttributesArray;
	    iDefaultSamplerAttributesArray = NULL;
	    }

    // delete settings file raw line array
    if(iSavedLineArray)
        {
        iSavedLineArray->Reset();
        delete iSavedLineArray;
        iSavedLineArray = NULL;
        }
		
	// delete sampler controller, cleans up the sampler plugin instances
	if(iSamplerHandler)
		{
		delete iSamplerHandler;
		iSamplerHandler = NULL;
		}
	// delete writer controller, cleans up the writer plugin instances
	if(iWriterHandler)
		{
		delete iWriterHandler;
		iWriterHandler = NULL;
		}

    // delete user side sampler stream 
	if(iUserStream)
		{
		delete iUserStream;
		iUserStream = NULL;
		}

	// close engine status property
	iEngineStatus.Close();
	if (RProperty::Delete(KEngineStatusPropertyCat, EProfilerEngineStatus) != KErrNotFound)
	    {
	    LOGTEXT(_L("CProfiler::~CProfiler - cannot close status property"));
	    }
    // close engine update property
    iUpdateStatus.Close();
    if (RProperty::Delete(KEngineStatusPropertyCat, EProfilerErrorStatus) != KErrNotFound)
        {
        LOGTEXT(_L("CProfiler::~CProfiler - cannot close update property"));
        }
    
    // close server process
    if (iServer)
        {
        LOGTEXT(_L("CProfiler::~CProfiler - Releasing server"));
        iServer->Release();
        }
    
	LOGTEXT(_L("CProfiler::~CProfiler - Finished"));
    }

// --------------------------------------------------------------------------------------------
void CProfiler::ConstructL()
    {
	LOGTEXT(_L("CProfiler::ConstructL - Enter"));
	TInt err(0);

    // create new sampler stream instance
    iUserStream = CProfilerSampleStream::NewL(KStreamBufferSize);
    if(!iUserStream)
        {
        LOGTEXT(_L("Profiler engine cannot reserve memory"));
        User::Leave(KErrCancel);   // operation cancelled
        }
	
    // engine status checker
    iErrorChecker = CProfilerErrorChecker::NewL();
    iErrorChecker->SetObserver(this);

	// create and initiate plug-in controller instances
    iSamplerHandler = CSamplerController::NewL(*iUserStream);
    iWriterHandler = CWriterController::NewL(*iUserStream);
    
    iWriterHandler->InitialiseWriterListL();
    
    // set engine as an observer to sampler controller to get the notification of plugin load has ended
    iSamplerHandler->SetObserver(this);
    
    // default settings from sampler plugins, maximum 20 sampler plugins
    iDefaultSamplerAttributesArray = new(ELeave) CArrayFixFlat<TSamplerAttributes>(20); 
    
    // set profiler status to initializing
    iState = RProfiler::EInitializing;
	iEngineStatus.Set(RProfiler::EInitializing);
	
	// set default general settings, will be overdriven if changed in settings file
	iGeneralAttributes.iTraceOutput.Copy(KDefaultTraceOutput);
	iGeneralAttributes.iTraceFilePrefix.Copy(KDefaultTraceFilePrefix);
	iGeneralAttributes.iSaveFileDrive.Copy(KDefaultTraceFileSaveDrive);
	
	RThread me;
	
	me.SetPriority(EPriorityRealTime);

	err = KErrGeneral;
	TInt count = 0;

	while(err != KErrNone && count < 30)
	    {
		err = User::RenameThread(KProfilerName);
		if(err != KErrNone)
		    {
			User::Leave(err);
		    }
		else break;
	    }

	if(err != KErrNone) 
	    {
		User::Leave(err);
	    }

	// set settings file loading preferences
	iSettingsFileLoaded = EFalse;

	// change status property to idle since initialization successfull
	iState = RProfiler::EIdle;
	if( iEngineStatus.Set((TInt)RProfiler::EIdle) != KErrNone )
	    {
	    LOGTEXT(_L("CProfiler::ConstructL - engine status property change failed"));
	    }

    if( iUpdateStatus.Set(EFalse) != KErrNone )
        {
        LOGTEXT(_L("CProfiler::ConstructL - engine status property change failed"));
        }

	// create a server instance for clients to communicate with 
	iServer = CPServer::NewL(10,*this);
	
	// close the handle 
	me.Close();
	
	LOGTEXT(_L("CProfiler::ConstructL - Exit"));
	
    }

CProfilerSampleStream* CProfiler::GetSamplerStream()
    {
    return iUserStream;
    }

void CProfiler::HandleSamplerControllerReadyL()
    {
    // load settings
    // check if settings file already loaded
    if(!iSettingsFileLoaded)
        {
        // load default settings file
        LoadSettingsL();

        iSettingsFileLoaded = ETrue;
        }
    
    // notify engine's launcher(UI or PIProfilerLauncher) to continue launch
    RProcess::Rendezvous(KErrNone); 
    }

void CProfiler::NotifyRequesterForSettingsUpdate()
    {
    // set update status P&S property true => update needed on UI side
    iUpdateStatus.Set(ETrue);
    }

void CProfiler::HandleProfilerErrorChangeL(TInt aError)
    {
    LOGSTRING2("CProfiler::HandleProfilerErrorChangeL() - error received, %d", aError);
    
    // check if profiler running
    if(iState == RProfiler::ERunning)
        {
        // stop profiler if error occurred during the trace
        iEngineStatus.Set(aError);
        
        // stop samplers, NOTE! Writer plugins not stopped since 
        iSamplerHandler->StopSamplerPlugins();

        // stop debug output plugin and write the rest of the trace data to output
        if(iGeneralAttributes.iTraceOutput.CompareF(KOutputToDebugOutput) == 0)   
            {
            // write the rest of trace data only if debug output selected
            iWriterHandler->StopSelectedPlugin();
            }
        LOGSTRING2("CProfiler::HandleProfilerErrorChangeL - sampling stopped, going to state %d", RProfiler::EIdle);
        }
    }

// ----------------------------------------------------------------------------
// Gets a value from settings file for certain attribute.
// ----------------------------------------------------------------------------
void CProfiler::DoGetValueFromSettingsArray(CDesC8ArrayFlat* aLineArray, const TDesC8& aAttribute, TDes8& aValue)
    {
    LOGTEXT(_L("CProfiler::DoGetValueFromSettingsFile()"));
    _LIT8(KSettingItemSeparator, "=");
    
    // read a line of given array
    for (TInt i=0; i<aLineArray->MdcaCount(); i++)
        {
        // check if this line has a separator
        TInt sepPos = aLineArray->MdcaPoint(i).Find(KSettingItemSeparator);
        if (sepPos > 0)
            {
            // check that the element matches
            if (aLineArray->MdcaPoint(i).Left(sepPos).CompareF(aAttribute) == 0)
                {
                // get the value
                aValue.Copy(aLineArray->MdcaPoint(i).Right(aLineArray->MdcaPoint(i).Length()-sepPos-1));
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::GetSamplerAttributesL(const RMessage2& aMessage)
    {
    TInt err(KErrNone);
    TInt pos(0);

    // get sampler count
    TInt count(iDefaultSamplerAttributesArray->Count());

    // write each of the default sampler plugin setting attributes over client-server session 
    for (TInt i(0); i<count; ++i)
       {
       TSamplerAttributes attr = iDefaultSamplerAttributesArray->At(i);
       TPckgC<TSamplerAttributes> attrPckg(attr);
       
       // write a TSamplerAttributes container at a time
       aMessage.WriteL(0, attrPckg, pos);
       pos += attrPckg.Length();
       }

    aMessage.Complete(err);
    return err;
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::SetSamplerAttributesL(const RMessage2& aMessage)
    {
    TSamplerAttributes attr;
    TPckg<TSamplerAttributes> inAttr(attr);
    
    TInt err = aMessage.Read(0, inAttr, 0);    
    
    // apply the changes directly to a plugin
    iSamplerHandler->SetSamplerSettingsL(attr.iUid, attr);
    
    aMessage.Complete(err);
    return err;
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::GetGeneralAttributesL(const RMessage2& aMessage)
    {
    TPckgBuf<TGeneralAttributes> generalSettings( iGeneralAttributes );
    
    // write general attributes over client-server session
    TInt err = aMessage.Write(0, generalSettings);
    
    aMessage.Complete(err);
    return err;
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::SetGeneralAttributesL(const RMessage2& aMessage)
    {
    // read the general settings from message
    TGeneralAttributes attr;
    TPckg<TGeneralAttributes> inPckg(attr);
    TInt err = aMessage.Read(0, inPckg, 0);
    
    // copy to the general attributes
    iGeneralAttributes.iSaveFileDrive.Copy(attr.iSaveFileDrive);
    iGeneralAttributes.iTraceFilePrefix.Copy(attr.iTraceFilePrefix);
    iGeneralAttributes.iTraceOutput.Copy(attr.iTraceOutput);
    
    aMessage.Complete(err);
    return err;
    }

TInt CProfiler::GetSamplerAttributeCountL(const RMessage2& aMessage)
    {
    // get the plugin array count and wrap it to TPckgBuf<>
    TPckgBuf<TInt> attributeCount(iDefaultSamplerAttributesArray->Count());
    
    // write general attributes over client-server session
    TInt err = aMessage.Write(0, attributeCount);
    
    aMessage.Complete(err);
    return err;
    }

TInt CProfiler::RefreshStatus(const RMessage2& aMessage)
    {
    TInt err(KErrNone);
    
    // update profiler status for requester
    iEngineStatus.Set(iState);
    
    aMessage.Complete(err);
    return err;
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::LoadSettingsL(/*const TDesC& configFile*/)
    {
	RFs fileServer;
	RFile file;
	TInt err(KErrNone);
    
	// connect to file server 
	err = fileServer.Connect();
	
	// check if file server can be connected
	if (err != KErrNone)
	    {
		// file server couldn't be connected
		return KErrGeneral;
	    }

	// check if settings file location length reasonable
	if ( iSettingsFileLocation.CompareF(KNullDesC) == 0 )
	    {
		// open the file with the default path and name
		TBuf<256> pathAndName;
		pathAndName.Append(PathInfo::PhoneMemoryRootPath());
		pathAndName.Append(KProfilerSettingsFileName);
		iSettingsFileLocation.Copy(pathAndName);
		LOGTEXT(_L("CProfiler::LoadSettings - Opening settings file with name (with the default path)"));
		LOGTEXT(pathAndName);
	    }

    // open the file with the given path and name
    err = file.Open(fileServer,iSettingsFileLocation,EFileRead);

	
	// check if RFile::Open() returned error
	if (err != KErrNone)
	    {
		// file couldn't be opened
		LOGTEXT(_L("CProfiler::LoadSettings - Failed to open settings, using default"));

		// check if settings already loaded
		if(iDefaultSamplerAttributesArray->Count() > 0)
		    {
		    // reset default settings array
		    iDefaultSamplerAttributesArray->Reset();
		    }
		
		// load default settings, instead of settings file ones
		iSamplerHandler->GetSamplerAttributesL(iDefaultSamplerAttributesArray);
		
		fileServer.Close();
		return KErrNone;
	    }
	
	// initialize iSavedLineArray, initial settings file lines 64
	if(iSavedLineArray)
	    {
        iSavedLineArray->Reset();
	    }
	else
	    {
        iSavedLineArray = new (ELeave) CDesC8ArrayFlat(KSavedLineCount);
	    }
	
	iSavedLinesCount = KSavedLineCount;
	
    // get size of the file
    TInt fileSize(0);
    err = file.Size(fileSize);
    // check if an error occurred reading the file size
    if(err != KErrNone)
        {
        return KErrGeneral; // could not find the size
        }
        
    // sanity check for the file size
    if (fileSize < 3 || fileSize > 20000)
        {
        fileSize = KSettingsFileSize;
        return KErrNotSupported;
        }
    
	// read the contents of the file to buffer. 
	iSettingsBuffer.Zero();
	file.Read(iSettingsBuffer, fileSize);
	file.Close();
	fileServer.Close();
	LOGSTRING2("CProfiler::LoadSettings: read %d bytes",iSettingsBuffer.Length());

	// append end mark "[end]"
    iSettingsBuffer.Append(KEndMark);
	// force an ending newline
	iSettingsBuffer.Append('\n');

	// next fill the saved settings array (CDesC8ArrayFlat) for further comparison with changes and default values
    TBuf8<384> tmpBuf;
    TInt lineCount(0);
    TBool commentFound(EFalse);
    for (TInt i(0); i<iSettingsBuffer.Length(); i++)  // loop all chars
        {
        // if new line char found, create a new text line
        if (iSettingsBuffer[i]=='\r' || iSettingsBuffer[i]=='\n')
            {
            // check if collected string has reasonable length
            if (tmpBuf.Length() > 0)
                {
                // remove extra spaces
                tmpBuf.TrimAll();
                // check if the size of the array too small
                if(lineCount >= iSavedLinesCount)
                    {
                    iSavedLineArray->ExpandL(20);   // expand by 20 lines
                    iSavedLinesCount += 20;
                    }
                iSavedLineArray->AppendL(tmpBuf);
                tmpBuf.Copy(KNullDesC8);
                lineCount++;
                }
            commentFound = EFalse;
            }
        // check if comment mark ';' is found on the line, skip the rest of the line
        else if(iSettingsBuffer[i]==';')
            {
            commentFound = ETrue;
            }
        // otherwise append a char to the temp line buffer if it is a wanted ASCII char
        else if (iSettingsBuffer[i]>=32 && iSettingsBuffer[i]<=127 && !commentFound)
            {
            tmpBuf.Append(iSettingsBuffer[i]);
            }
        }
    
    // empty tmpBuf
    tmpBuf.Copy(KNullDesC8);
    // check settings file version
    DoGetValueFromSettingsArray(iSavedLineArray, KProfilerVersionTag, tmpBuf); 

    TBuf8<32> version;
    version.Copy(PROFILER_VERSION_SHORT);
    
    // check if settings file version is 
    if(tmpBuf.CompareF(version) >= 0)
        {
        // update general attributes
        UpdateSavedGeneralAttributes(iSavedLineArray);
        
        // update settings to sampler plugins and save the attributes to default array
        iSamplerHandler->UpdateSavedSamplerAttributesL(iSavedLineArray, iDefaultSamplerAttributesArray);
        }
    else
        {
        // check if settings already loaded
        if(iDefaultSamplerAttributesArray)
            {
            // reset default settings array
            iDefaultSamplerAttributesArray->Reset();
            }
        // get the default settings if settings file version too old
        iSamplerHandler->GetSamplerAttributesL(iDefaultSamplerAttributesArray);
        }
    
	return err; 
    }

// --------------------------------------------------------------------------------------------
void CProfiler::UpdateSavedGeneralAttributes(CDesC8ArrayFlat* aSavedAttributes)
    {
    // get saved general settings
    DoGetValueFromSettingsArray(aSavedAttributes, KGenericTraceOutput, iGeneralAttributes.iTraceOutput);
    DoGetValueFromSettingsArray(aSavedAttributes, KGenericTraceFilePrefix, iGeneralAttributes.iTraceFilePrefix);
    DoGetValueFromSettingsArray(aSavedAttributes, KGenericTraceFileSaveDrive, iGeneralAttributes.iSaveFileDrive);

    }

TBool CProfiler::CheckLocationSanity(RFs& fs, const TDesC8& aLocation)
    {
    TBool ret(EFalse);

    TBuf<32> drive;
    
    CnvUtfConverter::ConvertToUnicodeFromUtf8(drive, aLocation);
    
    // check that aLocation contains some of reasonable drives 
    if(aLocation.Find(_L8("C:\\")) != KErrNotFound)
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveC))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("D:\\")) != KErrNotFound) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveD))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("E:\\")) != KErrNotFound) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveE))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("F:\\")) != KErrNotFound) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveF))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("G:\\")) != KErrNotFound) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveG))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("H:\\")) != KErrNotFound ) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
            !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveH))
            ret = ETrue;
        }
    else if(aLocation.Find(_L8("I:\\")) != KErrNotFound) 
        {
        if(BaflUtils::CheckFolder(fs, drive) == KErrNone && 
                !SysUtil::DiskSpaceBelowCriticalLevelL(&fs, 0, EDriveI))
            ret = ETrue;
        }
    else
        {
        // otherwise return false
        ret = EFalse;
        }
    
    return ret;
    }

TInt CProfiler::HandleGeneralSettingsChange()
    {
    // local literals
    _LIT8(KBackSlash, "\\");
    _LIT8(KTraceFileExtension, ".dat");
    
    TBuf8<KFileNameBufSize> fileNameBuf;
    TBuf8<10> number;
    TInt result(0);
    TInt index(1);
    TInt hashLocation(0);
    TParse parse;

    // check if plugin writer changed
    if(iGeneralAttributes.iTraceOutput.CompareF(KOutputToDebugOutput) == 0)
        {
        iWriterHandler->SetPluginActive( KDebOutWriterPluginUid, EWriterPluginEnabled );
        }
    else
        {
        RFs fileServer;
        RFile file;
        
        // connect to the file server
        result = fileServer.Connect();
        if(result == KErrNone)
            {
            // disk writer plugin will be activated
            iWriterHandler->SetPluginActive( KDiskWriterPluginUid, EWriterPluginEnabled );
            
            // fix the trace data file location as well
            iTotalPrefix.Zero();
            iTotalPrefix.Append(iGeneralAttributes.iSaveFileDrive);
            
            // check that trace file location sane
            if(!CProfiler::CheckLocationSanity(fileServer, iTotalPrefix))
                {
//                // empty location data
//                iTotalPrefix.Zero();
//                // use default location instead
//                iTotalPrefix.Append(KProfilerDefaultDrive);
                fileServer.Close();
                return KErrPathNotFound;
                }
            
            // remove extra spaces
            iTotalPrefix.TrimAll();
            
            // check if trace data location directory => string ends with '\' 
            if(iTotalPrefix.LocateReverse('\\') != iTotalPrefix.Length()-1 && 
                    iTotalPrefix.LocateReverse('/') != iTotalPrefix.Length()-1)
                {
                // append backslash to end
                iTotalPrefix.Append(KBackSlash);
                }
            
            // append trace file name prefix e.g. PIProfiler_#
            iTotalPrefix.Append(iGeneralAttributes.iTraceFilePrefix);
    
            // locate '#' mark for finding the next free trace file name, e.g. E:\data\PIProfiler_4.dat
            hashLocation = iTotalPrefix.Locate('#');
            if( hashLocation == KErrNotFound )
                {
                // append simply at the end of the trace file prefix, no need to inform user
                iTotalPrefix.Append('#');
                // get new hash mark location
                hashLocation = iTotalPrefix.Locate('#');
                }
    
            // add the file extension
            iTotalPrefix.Append(KTraceFileExtension);

            // search for files with different indices
            // until a free filename is found
            while(result != KErrNotFound)
                {
                fileNameBuf.Zero();
                // start with the original prefix
                fileNameBuf.Append(iTotalPrefix);
                // convert the number to a descriptor
                number.Num(index);
                // replace the hashmark with the real number
                fileNameBuf.Replace(hashLocation,1,number);
                
                // make a copy to the iFileNameStream descriptor
                iFileNameStream.Zero();
                CnvUtfConverter::ConvertToUnicodeFromUtf8(iFileNameStream, fileNameBuf);
                
                LOGSTRING2("CProfiler::HandleGeneralSettingsChange() - trying to open files %S ",&iFileNameStream);

                if((result = parse.Set(iFileNameStream, NULL, NULL)) != KErrNone)
                    {
                    // break loop if fails, problems in file name => change to log into debug output
                    break;
                    }
                
                // create directory for trace files if not exists
                result = fileServer.MkDirAll(parse.FullName());

                // check that file server responded with KErrNone or KErrAlreadyExists
                if( result != KErrNone && result != KErrAlreadyExists)
                    {
                    // if some other result, e.g. memory full => break
                    break;
                    }

                // attempt opening the file
                result = file.Open(fileServer,parse.FullName(),EFileShareReadersOnly);
                if(result != KErrNotFound)
                    {
                    if( result != KErrNotReady && 
                        result != KErrServerBusy ) 
                        {
                        // close the file if it could be opened
                        LOGSTRING2("Found STREAM file with index %d",index);
                        index++;
                        }
                    else 
                        {
                        // in boot measurements the file system might not be ready yet.
                        LOGSTRING2("Problem in opening STREAM file %d",index);
                        }
                    file.Close();
                    }
                } // while
            }
        else
            {
            // return error code
            return result;
            }
        
        TUint32 id(iWriterHandler->GetActiveWriter()->GetWriterType());
        
        // check if a file name is one that does not exist and selected plugin is disk writer
        if(result == KErrNotFound && id == KDiskWriterPluginUid.iUid)
            {
            // write right trace data file name to disk writer plugin
            iWriterHandler->SetPluginSettings( KDiskWriterPluginUid, iFileNameStream );
            }
        else
            {
            // return error if could not create trace log file
            return result;
            }
        // close file server
        fileServer.Close();
        }   // if output == KOutputToDebugOutput
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------
void CProfiler::SaveSettingsL()
    {
    LOGTEXT(_L("CProfiler::SaveSettings()"));
    
    // local literal
    _LIT(KGeneralHeader, "[general]");
    _LIT(KVersionHeader, "version");
    _LIT8(KPIProfilerSettingsHeader, "; PI Profiler Settings File");
    _LIT8(KGeneralSettingsHeader, "; general settings");
    _LIT8(KOutputFileDescription,"; \"output_type=file_system\" writes *.dat file to external memory");
    _LIT8(KOutputDebugDescription,"; \"output_type=debug_output\" writes *.dat file to debug port");
    _LIT8(KOutputFilePrefixDescription,"; if writing to file, prefix of the *.dat file\r\n; first '#' in the prefix is replaced with an integer");
    _LIT8(KOutputSaveDriveDescription,"; if writing to file, the location to store the *.dat file");
    
    RFs fs;
    RFile settingsFile;
    TInt err(KErrNone);
    TBuf8<384> line;
    
    // connect to file server
    err = fs.Connect();
    if( err != KErrNone )
        {
        // failed to write settings to settings file
        return;
        }
    
    // create and set the private path
    fs.CreatePrivatePath(EDriveC);
    fs.SetSessionToPrivate(EDriveC);
  
    // create the new settings file
    err = settingsFile.Replace(fs, iSettingsFileLocation, EFileWrite);
    if(err != KErrNone)
        return;
    
    CleanupClosePushL(settingsFile);  

    // write the header
    line.Copy(KPIProfilerSettingsHeader);
    line.Append(KNewLineSeparator);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);
    
    // write the header
    line.Copy(KGeneralSettingsHeader);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);

    // write all generic settings
    line.Copy(KGeneralHeader);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);

    // write version info
    line.Copy(KVersionHeader);
    line.Append(KEquals);
    line.Append(PROFILER_VERSION_SHORT);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);

    // output explanation
    line.Copy(KOutputFileDescription);
    line.Append(KNewLineSeparator);
    line.Append(KOutputDebugDescription);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);
    
    // write trace output
    line.Copy(KGenericTraceOutput);
    line.Append(KEquals);
    line.Append(iGeneralAttributes.iTraceOutput);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);
    
    // file prefix explanation
    line.Copy(KOutputFilePrefixDescription);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);

    // write trace file prefix
    line.Copy(KGenericTraceFilePrefix);
    line.Append(KEquals);
    line.Append(iGeneralAttributes.iTraceFilePrefix);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);
    
    // file prefix explanation
    line.Copy(KOutputSaveDriveDescription);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);

    // write trace file location
    line.Copy(KGenericTraceFileSaveDrive);
    line.Append(KEquals);
    line.Append(iGeneralAttributes.iSaveFileDrive);
    line.Append(KNewLineSeparator);
    settingsFile.Write(line);
    
    // reset the default attributes array
    iDefaultSamplerAttributesArray->Reset();
    
    // update the latest changes from plugins
    iSamplerHandler->GetSamplerAttributesL(iDefaultSamplerAttributesArray);
    
    // call CSamplerController to write all sampler settings
    iSamplerHandler->ComposeAttributesToSettingsFileFormat(settingsFile, iDefaultSamplerAttributesArray);
    
    CleanupStack::PopAndDestroy(); //settingsFile
    // close file
    fs.Close();
    }

TInt CProfiler::CheckOldProfilerRunning()
    {
    TFindProcess procName;
    procName.Find(_L("BappeaProf.exe*"));
    TFullName aResult;
    TInt err(KErrNone);
    RProcess proc;    
    
    // now check if old Profiler is still running on
    err = procName.Next(aResult);
    // check if old profiler process found 
    if(err == KErrNone)
        {
        // other process found, i.e. right process to communicate with, in case started from eshell
        err = proc.Open(procName);
        if(err == KErrNone)
            {
            if(proc.ExitCategory().Length() > 0)
                {
                proc.Close();
                // process already exited => create a new one
                return KErrNotFound;
                }
            proc.Close();
            }
        // return error for error handling
        return KErrAlreadyExists;
        }
    return err;
    }

// --------------------------------------------------------------------------------------------
void CProfiler::HandleError(TInt aErr)
    {
    // write error to status property to inform requester
    TInt err(iEngineStatus.Set(KEngineStatusPropertyCat, EProfilerEngineStatus, aErr));
    if(err != KErrNone)
        RDebug::Print(_L("CProfiler::HandleError() - error setting status: %d"), err);
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::ControlDataL(TInt aCommand,TAny* value1,TAny* /*value2*/)
    {
	LOGSTRING3("CProfiler::ControlData %d, 0x%x",aCommand,value1);

	_LIT(KDebugOutput, "debug_output");
	_LIT(KFileOutput, "file_system");
	_LIT8(KOutputToDebugOutput, "debug_output");
	
	TDes* desc;
	TPtrC ptrDesc;
	
	switch(aCommand)
	    {
		// new controls
	    case RProfiler::EGetFileName:
	        {
            LOGTEXT(_L("Profiler::EGetFileName - start"));
            LOGSTRING2(_L("Profiler::EGetFileName - total file name is: %S"),(TDes*)value1);
            desc = (TDes*)value1;
            desc->Zero();
            desc->Append(iFileNameStream);
            LOGSTRING2(_L("Profiler::EGetFileName - now total file name is: %S"),(TDes*)value1);
            return KErrNone;
	        }
        case RProfiler::EGetActiveWriter:
            {
            LOGTEXT(_L("Profiler::EGetActiveWriter - start"));
            desc = (TDes*)value1;
            desc->Zero();
            if(iGeneralAttributes.iTraceOutput.CompareF(KOutputToDebugOutput) == 0)
                {
                desc->Append(KDebugOutput);
                }
            else
                {
                desc->Append(KFileOutput);
                }
            return KErrNone;
            }
	    }

	return KErrNone;
    }

// --------------------------------------------------------------------------------------------
TInt CProfiler::ControlL(TInt aCommand)
    {
	LOGSTRING2("CProfiler::Control - Controlling ProfilerEngine %d",aCommand);
	TInt err(KErrNone);
	
	switch (aCommand)
	    {
		case RProfiler::EStartSampling:
		    {
		    // check first if old Profiler already running
		    err = CheckOldProfilerRunning();
		    if(err == KErrAlreadyExists)
		        {
		        // if exists do not start a profiling process since corrupts the collected trace data  
		        HandleError(err);
		        err = KErrNone;
		        return err;
		        }
		    
		    // save settings before launching the profiler
		    // reason: the profiling may have set to the background => need for get right settings
		    SaveSettingsL();
		    
		    // set the general settings to writer plugins to reflect the latest changes
		    err = HandleGeneralSettingsChange();
		    if(err == KErrNone)
		        {
                // reset the buffers before new profiling
                iUserStream->ResetBuffers();
    
                // give the CProfilerSampleStream a handle to current writer
                iUserStream->SetWriter(*iWriterHandler->GetActiveWriter());
                
                // set initially debug output writer active
                err = iWriterHandler->StartSelectedPlugin();
    
                // check that writer plugin started
                if(err != KErrNone)
                    {
                    // if not started handle error
                    HandleError(err);
                    }
                else
                    {
                    // start activated sampler plug-in, NOTE: plugins check if errors occur in startup for some reason
                    iSamplerHandler->StartSamplerPluginsL();
        
                    // set engine state P&S property to running, e.g. for PIProfiler UI to read
                    iState = RProfiler::ERunning;
        
                    // set the engine into running mode
                    iEngineStatus.Set(iState);
                    }
                }
		    else
		        {
		        // handle error and notify requester
		        HandleError(err);
		        }
		    
		    LOGTEXT(_L("CProfiler::Control - Finished processing EStartSampling!"));

			return err;
		    }
		case RProfiler::EStopSampling:
			LOGTEXT(_L("CProfiler::Control - Starting to stop sampling..."));
			// stop sampler plugins
			if(iState == RProfiler::ERunning)
				{
				iState = RProfiler::EStopping;
				iEngineStatus.Set(RProfiler::EStopping);
				
				iSamplerHandler->StopSamplerPlugins();
	
				// finalize the filled buffer writing
				iUserStream->Finalise();
				
				// stop output plugin and write the rest of the trace data to output
				LOGTEXT(_L("CProfiler::Control - stopping writer"));
				iWriterHandler->StopSelectedPlugin();
	
				// set engine state P&S property idle 
				iState = RProfiler::EIdle;
				iEngineStatus.Set(RProfiler::EIdle);
				
				LOGSTRING2("CProfiler::Control - sampling stopped, going to state %d", RProfiler::EIdle);
				}
			return KErrNone;

		case RProfiler::EExitProfiler:
		    {
		    // save settings into settings file when exiting
		    SaveSettingsL();

            if(iUserStream)
                {
                delete iUserStream;
                iUserStream = NULL;
                }
		    
            // set engine state P&S property idle 
            iState = RProfiler::EIdle;
            iEngineStatus.Set(RProfiler::EIdle);
               
            LOGTEXT(_L("Stopping Activer Scheduler"));
            CActiveScheduler::Stop();
            LOGTEXT(_L("Stopped Activer Scheduler"));

            return KErrNone;
		    }
	    }

	LOGTEXT(_L("CProfiler::Control - returning"));

	return err;
    }

// --------------------------------------------------------------------------------------------
void CProfiler::Finalise()
    {	
	LOGTEXT(_L("CProfiler::Finalise - Finished processing EStopSampling!"));
    }

// --------------------------------------------------------------------------------------------
RProfiler::TSamplerState CProfiler::State() const
    {
	return iState;
    }

/*
 *
 *	Class CPServer definition
 *
 */
// --------------------------------------------------------------------------------------------
inline const CPServer& CPSession::Server() const
    {
	return *static_cast<const CPServer*>(CSession2::Server());
    }

// --------------------------------------------------------------------------------------------
void CPSession::ServiceL(const RMessage2& aMessage)
    {
	LOGTEXT(_L("CPSession::ServiceL - Starting to process message"));
	TInt err(KErrNone);
	
	if(aMessage.Function() == RProfiler::EGetGeneralAttributes)
	    {
	    Server().GetGeneralAttributesL(aMessage);
	    }
	else if(aMessage.Function() == RProfiler::ESetGeneralAttributes)
	    {
        Server().SetGeneralAttributesL(aMessage);
	    }
	else if(aMessage.Function() == RProfiler::EGetSamplerAttributes)
	    {
        Server().GetSamplerAttributesL(aMessage);
	    }
    else if(aMessage.Function() == RProfiler::EGetSamplerAttributeCount)
        {
        Server().GetSamplerAttributeCountL(aMessage);
        }
    else if(aMessage.Function() == RProfiler::ESetSamplerAttributes)
        {
        Server().SetSamplerAttributesL(aMessage);
        }
    else if(aMessage.Function() == RProfiler::ERefreshProfilerStatus)
        {
        Server().RefreshStatus(aMessage);
        }
	else if(aMessage.Ptr0() == 0 && aMessage.Ptr1() == 0 && aMessage.Ptr2() == 0)
		{	
		LOGTEXT(_L("Ptr0 && Ptr1 == 0 && Ptr2 == 0"));
		aMessage.Complete(Server().ControlL(RProfiler::TCommand(aMessage.Function())));
		LOGTEXT(_L("CPSession::ServiceL - Message completed"));
		} 
	else if(aMessage.Ptr0() != 0 && aMessage.Ptr1() != 0 && aMessage.Ptr2() != 0)
		{
		LOGTEXT(_L("Error with message, all pointers contain data!"));
		}
		
	else if (aMessage.Ptr0() != 0)
		{
		if(aMessage.Ptr1() == 0)
			{
			LOGTEXT(_L("ServiceL: Ptr0 != 0 && Ptr1 == 0"));
			// provided value is a descriptor
			TBuf<64>* dst = new TBuf<64>;
			aMessage.ReadL(0,*dst,0);
	
			err = Server().ControlDataL(aMessage.Function(),(TAny*)dst);
			delete dst;
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));
			}
		else
			{
			LOGTEXT(_L("ServiceL: Ptr0 != 0 && Ptr1 != 0"));
			// provided value is a descriptor
			TBuf<64>* dst = new TBuf<64>;
			aMessage.ReadL(0,*dst,0);
			
			TUint32 num1 = (TUint32)aMessage.Ptr1();
			
			err = Server().ControlDataL(aMessage.Function(),(TAny*)dst, (TAny*)num1);
			delete dst;
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));
			}
		}
	else if (aMessage.Ptr1() != 0)
		{
		LOGTEXT(_L("ServiceL: Ptr1 != 0"));
		// provided value is a TUint32
		if( ((TUint32)aMessage.Ptr3()) == 0xffffffff)
			{
			TUint32 num = (TUint32)aMessage.Ptr1();
			err = Server().ControlDataL(aMessage.Function(),(TAny*)num);
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));
			}
		else
			{
			LOGTEXT(_L("ServiceL: Ptr3 != 0xffffffff"));
			TUint32 num1 = (TUint32)aMessage.Ptr1();
			TUint32 num2 = (TUint32)aMessage.Ptr3();
			err = Server().ControlDataL(aMessage.Function(),(TAny*)num1,(TAny*)num2);
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));
			}
		}
	else if (aMessage.Ptr2() != 0)
		{
		// command requests for data, provided 
		// value should be a descriptor
		if( ((TUint32)aMessage.Ptr3()) == 0xffffffff)
			{
			LOGTEXT(_L("ServiceL: Ptr2 != 0 && Ptr3 == 0xffffffff"));
	
			TBuf<256>* src = new TBuf<256>;
			src->Zero();
			err = Server().ControlDataL(aMessage.Function(),(TAny*)src);
	
			LOGSTRING2("Got sampler data %S",src);
			
			aMessage.WriteL(2, *src, 0);
	
			delete src;
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));
			}
		else 
			{
			LOGTEXT(_L("ServiceL: Ptr2 != 0 && Ptr3 != 0xffffffff"));
			
			TUint32 num1 = (TUint32)aMessage.Ptr2();	// containing id
			TBuf<256>* buffer = new TBuf<256>;		// Text data, e.g. plug-in name or description

			LOGSTRING3("Getting data for sampler: 0x%X, buffer max len %d",num1, aMessage.GetDesMaxLength(3));

			err = Server().ControlDataL(aMessage.Function(), (TAny*)num1, (TAny*)buffer);

			LOGSTRING2("Got sampler data %S",&buffer);
			
			// write back to same parameter
			aMessage.WriteL(3, *buffer, 0);
			aMessage.Complete(err);
			LOGTEXT(_L("CPSession::ServiceL - Message completed"));			
			}
		}
	LOGTEXT(_L("CPSession::ServiceL - Message processed"));
    }

// --------------------------------------------------------------------------------------------
MProfilerController* CPServer::NewL(TInt aPriority, MProfilerEngine& aEngine)
    {
	LOGTEXT(_L("CPServer::NewL - Enter"));
	CPServer* self = new(ELeave) CPServer(aPriority, aEngine);
	CleanupStack::PushL(self);
	self->StartL(KProfilerName);
	CleanupStack::Pop();
	LOGTEXT(_L("CPSession::NewL - Exit"));
	return self;
    }

// --------------------------------------------------------------------------------------------
CPServer::CPServer(TInt aPriority, MProfilerEngine& aEngine)
	: CServer2(aPriority), MProfilerController(aEngine)
    {

    }

// --------------------------------------------------------------------------------------------
void CPServer::Release()
    {
	delete this;
    }

// --------------------------------------------------------------------------------------------
CSession2* CPServer::NewSessionL(const TVersion&,const RMessage2&) const
    {
	return new(ELeave) CPSession();
    }

/*
 *
 * Static methods for controlling the profiler
 * through command line
 *
 */
// --------------------------------------------------------------------------------------------
static void RunEngineServerL(const TDesC& aSettingsFile)
    {
	RDebug::Print(_L("Profiler: RunEngineServerL() - Install active scheduler"));
	CActiveScheduler* pS = new CActiveScheduler;
	CActiveScheduler::Install(pS);
	CProfiler* p = CProfiler::NewLC(aSettingsFile);
	CActiveScheduler::Start();
	p->Finalise();
	CleanupStack::PopAndDestroy(p);
	delete pS;
    }

// --------------------------------------------------------------------------------------------
static TInt TestSettingsFile(const TDesC& configFile)
    {
    RFs fs;
    TBuf8<256> configFile8;
    
    // check if file server can be connected
    if (fs.Connect() != KErrNone)
        {
        // file server couldn't be connected, return false
        return KErrNotFound;
        }

    // check if config file name length is > 0
    if (configFile.Length() > 0)
        {
        // check sanity of settings file location
        CnvUtfConverter::ConvertFromUnicodeToUtf8(configFile8, configFile);
        if(!CProfiler::CheckLocationSanity(fs, configFile8))
            {
            fs.Close();
            return KErrGeneral;
            }
        }
    else
        {
        // configFile length 0, return false
        fs.Close();
        return KErrNotFound;
        }
    // return true if tests passed
    fs.Close();
    return KErrNone;
    }

// --------------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    // parse command line arguments 
    TBuf<256> c;

    // copy the full command line with arguments into a buffer
    User::CommandLine(c);

    TBuf<256> fileName;
    fileName.Append(c); // only one settings param should be
    LOGSTRING2("Filename is %S", &fileName);
    if(TestSettingsFile(fileName) != KErrNone)
        {
        // settings file does not exist, copy null desc to file name
        fileName.Copy(KNullDesC);
        }
    
    // if no command line arguments found just start the profiler process
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt ret(KErrNoMemory);
    if( cleanup )
        {
        TRAPD( ret, RunEngineServerL(fileName) );
        RDebug::Print(_L("Profiler: E32Main() - ret %d"), ret);
        delete cleanup;
        } 
    __UHEAP_MARKEND;

    return ret;
    } 


