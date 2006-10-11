// 
// 
// Copyright (C) 2005 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2005 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef REDIRECTSUSPEND_H
#define REDIRECTSUSPEND_H

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "os/OsMsg.h"
#include "utl/UtlContainableAtomic.h"
#include "net/SipMessage.h"
#include "SipRedirector.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
struct redirector
{
   // TRUE if this redirector has requested suspending this request.
   int suspended;
   // TRUE if this redirector has ever requested suspending this
   // request, and so needs to have cancel() called.
   int needsCancel;
   // Where the pointer to the redirector's private storage is kept.
   SipRedirectorPrivateStorage* privateStorage;
};

// TYPEDEFS
// FORWARD DECLARATIONS

// The "suspend object", which contains the information about a
// redirection request that the redirectors and the redirect server
// need to complete its processing and ultimately return the 302
// response for it.

class RedirectSuspend : public UtlContainableAtomic
{
public:

   // Constructor
   RedirectSuspend(int noRedirectors);

   // Destructor
   ~RedirectSuspend();

   // The request sequence number does not need to be stored, as these
   // objects are always looked up via their sequence numbers.

   // The incoming message that we are processing.
   // Note this field is a SipMessage, not a SipMessage*, so that when
   // we store the incoming message in it, we save a copy.
   SipMessage mMessage;

   // The count of the number of redirectors that have requested
   // suspension of this request.
   int mSuspendCount;

   // Number of redirector slots.
   int mNoRedirectors;

   // Pointer to an array of information kept for each redirector.
   struct redirector* mRedirectors;

   virtual const char* const getContainableType() const;
   static const UtlContainableType TYPE;
};

#endif /*  REDIRECTSUSPEND_H */
