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



#ifndef PROFILER_GUI_VALUESCONTAINER_H
#define PROFILER_GUI_VALUESCONTAINER_H

// INCLUDES
#include <coecntrl.h>
#include <eiklbo.h> 
#include <aknlists.h> 
#include <akninfopopupnotecontroller.h> 

// FORWARD DECLARATIONS
class CProfilerGuiModel;
class CAknInfoPopupNoteController;

// CLASS DECLARATIONS

class CProfilerGuiMainContainer : public CCoeControl, MCoeControlObserver, MEikListBoxObserver
    {
public:
    void ConstructL(const TRect& aRect);
    ~CProfilerGuiMainContainer();

private:
    void SizeChanged();  
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void HandleResourceChange(TInt aType);
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);  // From MEikListBoxObserver
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);  // From MCoeControlObserver
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

public:
    const CArrayFix<TInt>* ListBoxSelectionIndexes();
    TInt ListBoxSelectionIndexesCount();
    void SetDefaultTitlePaneTextL();
    TInt CurrentListBoxItemIndex();
    void SetListBoxTextArrayL(CDesCArray* aTextArray);
    inline CAknSingleGraphicStyleListBox* ListBox() { return iListBox; }
    void ShowWriterInfoPopupL(const TDesC& aNote);
private:
    CProfilerGuiModel*                              iModel;
    CAknSingleGraphicStyleListBox*      			iListBox;
    CAknInfoPopupNoteController*        			iInfoPopup;
    };

#endif

// End of File
