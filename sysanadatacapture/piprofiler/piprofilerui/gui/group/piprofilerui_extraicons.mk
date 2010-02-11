#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:   
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\piprofilerui_extraicons.mif

HEADERDIR=$(EPOCROOT)epoc32\include
HEADERFILENAME=$(HEADERDIR)\piprofilerui_extraicons.mbg


do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	@if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME)
	@if exist $(HEADERFILENAME) erase $(HEADERFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : $(ICONTARGETFILENAME)

$(ICONTARGETFILENAME) (HEADERFILENAME) : ..\icons\qgn_prob_piprofilerui_status_disabled.svg ..\icons\qgn_prob_piprofilerui_status_enabled.svg
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 ..\icons\qgn_prob_piprofilerui_status_disabled.svg \
		/c8,8 ..\icons\qgn_prob_piprofilerui_status_enabled.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME) && \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
