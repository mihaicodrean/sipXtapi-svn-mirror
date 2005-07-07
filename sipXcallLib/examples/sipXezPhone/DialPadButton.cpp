// $Id$
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "stdwx.h"
#include "sipXmgr.h"
#include "DialPadButton.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS
// MACROS
BEGIN_EVENT_TABLE(DialPadButton, wxBitmapButton)
    EVT_LEFT_DOWN(OnMouseDown)
    EVT_LEFT_UP(OnMouseUp)
END_EVENT_TABLE()

// Constructor
DialPadButton::DialPadButton(wxWindow* parent, int id, const wxBitmap& bitmap, const wxPoint& pos, const wxSize& size, const enum TONE_ID toneId, const char charVal) :
   wxBitmapButton(parent, id, bitmap, pos, size, 0),
   mCharVal(charVal),
   mToneId(toneId)
{
}

// Destructor
DialPadButton::~DialPadButton()
{

}

void DialPadButton::OnMouseDown(wxMouseEvent& event)
{
    if (sipXmgr::getInstance().getCurrentCall() > 0)
    {
        sipxCallStartTone(sipXmgr::getInstance().getCurrentCall(), mToneId, true, true);
    }
    event.Skip();
}

void DialPadButton::OnMouseUp(wxMouseEvent& event)
{
    if (sipXmgr::getInstance().getCurrentCall() > 0)
    {
        sipxCallStopTone(sipXmgr::getInstance().getCurrentCall());
    }
    event.Skip();
}
