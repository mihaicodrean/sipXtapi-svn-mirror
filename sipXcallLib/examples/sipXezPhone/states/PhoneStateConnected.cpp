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
#include "../stdwx.h"
#include "../sipXmgr.h"
#include "PhoneStateConnected.h"
#include "PhoneStateCallHeld.h"
#include "PhoneStateIdle.h"
#include "PhoneStateDisconnectRequested.h"
#include "PhoneStateTransferRequested.h"
#include "../sipXezPhoneApp.h"


// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
extern sipXezPhoneApp* thePhoneApp;
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS
// MACRO CALLS

PhoneStateConnected::PhoneStateConnected()
{
}

PhoneStateConnected::~PhoneStateConnected(void)
{
}

PhoneState* PhoneStateConnected::OnFlashButton()
{
   return (new PhoneStateDisconnectRequested());
}

PhoneState* PhoneStateConnected::OnDisconnected()
{
   sipXmgr::getInstance().disconnect();
   return (new PhoneStateIdle());
}

PhoneState* PhoneStateConnected::OnHoldButton()
{
    return (new PhoneStateCallHeld());
}

PhoneState* PhoneStateConnected::OnTransferRequested(const wxString phoneNumber)
{
    return (new PhoneStateTransferRequested(phoneNumber));
}

PhoneState* PhoneStateConnected::Execute()
{
   thePhoneApp->setStatusMessage("Connected.");
   
   return this;
}
