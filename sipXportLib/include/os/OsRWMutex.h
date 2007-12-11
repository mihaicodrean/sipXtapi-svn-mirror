//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////


#ifndef _OsRWMutex_h_
#define _OsRWMutex_h_

// SYSTEM INCLUDES

// APPLICATION INCLUDES
#include "os/OsDefs.h"
#include "os/OsStatus.h"
#include "utl/UtlDefs.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

//:Mutual exclusion semaphore handling multiple readers and writers
// Two kinds of concurrent tasks, called "readers" and "writers", share a
// single resource. The readers can use the resource simultaneously, but each
// writer must have exclusive access to it. When a writer is ready to use the
// resource, it should be enabled to do so as soon as possible.
class OsRWMutexBase
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

   enum QueueOptions
   {
      Q_FIFO     = 0x0, // queue blocked tasks on a first-in, first-out basis
      Q_PRIORITY = 0x1  // queue blocked tasks based on their priority
   };
     //!enumcode: Q_FIFO - queues blocked tasks on a first-in, first-out basis
     //!enumcode: Q_PRIORITY - queues blocked tasks based on their priority

/* ============================ CREATORS ================================== */


/* ============================ MANIPULATORS ============================== */

   virtual OsStatus acquireRead(void)=0;
     //:Block (if necessary) until the task acquires the resource for reading
     // Multiple simultaneous readers are allowed.

   virtual OsStatus acquireWrite(void)=0;
     //:Block (if necessary) until the task acquires the resource for writing
     // Only one writer at a time is allowed (and no readers).

   virtual OsStatus tryAcquireRead(void)=0;
     //:Conditionally acquire the resource for reading (i.e., don't block)
     // Multiple simultaneous readers are allowed.
     // Return OS_BUSY if the resource is held for writing by some other task

   virtual OsStatus tryAcquireWrite(void)=0;
     //:Conditionally acquire the resource for writing (i.e., don't block).
     // Only one writer at a time is allowed (and no readers).
     // Return OS_BUSY if the resource is held for writing by some other task
     // or if there are running readers.

   virtual OsStatus releaseRead(void)=0;
     //:Release the resource for reading

   virtual OsStatus releaseWrite(void)=0;
     //:Release the resource for writing

/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
     /// Default constructor
   OsRWMutexBase() { };

     /// Destructor
   virtual
   ~OsRWMutexBase() { };

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

     /// Copy constructor (not implemented for this class)
   OsRWMutexBase(const OsRWMutexBase& rhs);

     /// Assignment operator (not implemented for this class)
   OsRWMutexBase& operator=(const OsRWMutexBase& rhs);

};

/* ============================ INLINE METHODS ============================ */

/// Depending on the native OS that we are running on, we include the class
/// declaration for the appropriate lower level implementation and use a
/// "typedef" statement to associate the OS-independent class name (OsRWMutex)
/// with the OS-dependent realization of that type (e.g., OsRWMutexWnt).
#if defined(_WIN32)
#  include "os/shared/OsRWMutexShared.h"
   typedef class OsRWMutexShared OsRWMutex;
#elif defined(_VXWORKS)
#  include "os/shared/OsRWMutexShared.h"
   typedef class OsRWMutexShared OsRWMutex;
#elif defined(__pingtel_on_posix__)
#  include "os/linux/OsRWMutexLinux.h"
   typedef class OsRWMutexLinux OsRWMutex;
#else
#  error Unsupported target platform.
#endif

#endif  // _OsRWMutex_h_
