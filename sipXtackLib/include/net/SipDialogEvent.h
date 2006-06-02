// 
// 
// Copyright (C) 2005 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// Copyright (C) 2005 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2005 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef _SipDialogEvent_h_
#define _SipDialogEvent_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include <utl/UtlHashMap.h>
#include <net/HttpBody.h>
#include <net/Url.h>
#include <os/OsDateTime.h>
#include <os/OsBSem.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS


#define DIALOG_EVENT_TYPE "dialog"

#define BEGIN_DIALOG_INFO "<dialog-info xmlns=\"urn:ietf:params:xml:ns:dialog-info\""
#define END_DIALOG_INFO "</dialog-info>\n"

#define VERSION_EQUAL " version="
#define STATE_EQUAL " state="
#define ENTITY_EQUAL " entity="

#define DOUBLE_QUOTE "\""
#define END_BRACKET ">"
#define END_LINE ">\n"

#define BEGIN_DIALOG "<dialog id="
#define CALL_ID_EQUAL " call-id="
#define LOCAL_TAG_EQUAL " local-tag="
#define REMOTE_TAG_EQUAL " remote-tag="
#define DIRECTION_EQUAL " direction="
#define END_DIALOG "</dialog>\n"

#define BEGIN_STATE "<state"
#define EVENT_EQUAL " event="
#define CODE_EQUAL " code="
#define END_STATE "</state>\n"

#define BEGIN_DURATION "<duration>"
#define END_DURATION "</duration>\n"

#define BEGIN_LOCAL "<local>\n"
#define END_LOCAL "</local>\n"

#define BEGIN_REMOTE "<remote>\n"
#define END_REMOTE "</remote>\n"

#define BEGIN_IDENTITY "<identity"
#define DISPLAY_EQUAL " display="
#define END_IDENTITY "</identity>\n"

#define BEGIN_TARTGET "<target uri=\""
#define END_TARGET "\"/>\n"


#define STATE_TRYING "trying"
#define STATE_PROCEEDING "proceeding"
#define STATE_EARLY "early"
#define STATE_CONFIRMED "confirmed"
#define STATE_TERMINATED "terminated"

// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

//! Container for dialog element in the dialog event package
/**
 * This class contains all the contents presented in a dialog element of the
 * dialog event package described in draft-ietf-sipping-dialog-package-06.txt
 * (An INVITE Initiated Dialog Event Package for SIP). This class has the
 * methods to construct and manipulate the dialog and its sub-elements.
 */

class Dialog : public UtlContainable
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
  public:

/** 
 * @name ====================== Constructors and Destructors
 * @{
 */
   /// Constructor
   Dialog(const char* dialogId,
          const char* callId, 
          const char* localTag, 
          const char* remoteTag, 
          const char* direction);

   /// Copy constructor
   Dialog(const Dialog& rDialog);

   /// Destructor
   ~Dialog();

   virtual UtlContainableType getContainableType() const;

   virtual unsigned int hash() const;

   int compareTo(const UtlContainable *b) const;

///@}
   
/**
 * @name ====================== Dialog Setting Interfaces
 *
 * These methods set/get the dialog element and sub-elements.
 *
 * @{
 */

   void getDialog(UtlString& dialogId,
                  UtlString& callId, 
                  UtlString& localTag, 
                  UtlString& remoteTag, 
                  UtlString& direction) const;
                  
   void getCallId(UtlString& callId) const;

   void setDialogId(const char* dialogId);

   void getDialogId(UtlString& dialogId) const;

   void setState(const char* state, const char* event, const char* code);

   void setTags(const char* local, const char* remote);

   void getState(UtlString& state, UtlString& event, UtlString& code) const;

   void setDuration(const unsigned long duration);

   unsigned long getDuration() const;

   void setReplaces(const char* callId,
                    const char* localTag,
                    const char* remoteTag);

   void getReplaces(UtlString& callId,
                    UtlString& localTag,
                    UtlString& remoteTag) const;

   void setReferredBy(const char* url,
                      const char* display);

   void getReferredBy(UtlString& url,
                      UtlString& display) const;

   void setLocalIdentity(const char* identity,
                         const char* display);

   void getLocalIdentity(UtlString& identity,
                         UtlString& display) const;

   void setRemoteIdentity(const char* identity,
                          const char* display);

   void getRemoteIdentity(UtlString& identity,
                          UtlString& display) const;

   void setLocalTarget(const char* url);

   void getLocalTarget(UtlString& url) const;

   void setRemoteTarget(const char* url);

   void getRemoteTarget(UtlString& url) const;

///@}
   
/* //////////////////////////// PROTECTED ///////////////////////////////// */
  protected:
   
/* //////////////////////////// PRIVATE /////////////////////////////////// */
  private:

   // Variables for dialog element
   UtlString mId;
   UtlString mCallId;
   UtlString mLocalTag;
   UtlString mRemoteTag;
   UtlString mDirection;

   // Variables for state element
   UtlString mState;
   UtlString mEvent;
   UtlString mCode;

   // Variables for duration element
   long mDuration;

   // Variables for replaces element
   UtlString mNewCallId;
   UtlString mNewLocalTag;
   UtlString mNewRemoteTag;

   // Variables for referred-by element
   UtlString mReferredBy;
   UtlString mDisplay;

   // Variables for local element
   UtlString mLocalIdentity;
   UtlString mLocalDisplay;
   UtlString mLocalTarget;
   UtlString mLocalSessionDescription;

   // Variables for remote element
   UtlString mRemoteIdentity;
   UtlString mRemoteDisplay;
   UtlString mRemoteTarget;
   UtlString mRemoteSessionDescription;

   //Assignment operator
   Dialog& operator=(const Dialog& rhs);
};


//! Container for MIME type application/dialog-info+xml.
/**
 * This class contains all the contents presented in a dialog event package
 * described in draft-ietf-sipping-dialog-package-06.txt (An INVITE Initiated
 * Dialog Event Package for SIP). This class has the methods to construct and
 * manipulate the dialog events in a dialog event package.
 */
class SipDialogEvent : public HttpBody
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
  public:

/** 
 * @name ====================== Constructors and Destructors
 * @{
 */
   //! Construct an empty body of a dialog event package
   SipDialogEvent(const char* state,
                  const char* entity);


   //! Construct from an existing dialog event package in the xml format
   SipDialogEvent(const char* bodyBytes);

   //! Destructor that will free up the memory allocated for dialog contents if it is not being deleted
   virtual
      ~SipDialogEvent();

///@}
   
/**
 * @name ====================== Dialog Event Serialization Interfaces
 *
 * @{
 */

   //! Build the body of this object
   void buildBody() const;

   //! Get the string length of this object
   virtual int getLength() const;

   //! Get the serialized char representation of this dialog event.
   /*! \param bytes - buffer space where the dialog event is written, null
    *       terminated.
    *  \param length - the number of bytes written (not including the
    *       null terminator).
    */
   virtual void getBytes(const char** bytes,
                         int* length) const;

   //! Get the serialized string representation of this dialog event.
   /*! \param bytes - buffer space where the dialog event is written, null
    *       terminated.
    *  \param length - the number of bytes written (not including the
    *       null terminator).
    */
   virtual void getBytes(UtlString* bytes,
                         int* length) const;

   void setEntity(const char* entity);

   void getEntity(UtlString& entity) const;

   void setState(const char* state);

   void getState(UtlString& state) const;


///@}

/**
 * @name ====================== Dialog Setting Interfaces
 *
 * These methods set/get the dialog element.
 *
 * @{
 */

   //! Insert a Dialog object to a hash table
   void insertDialog(Dialog* dialog);

   //! Get the Dialog object from the hash table based on the callId
   Dialog* getDialog(UtlString& callId);

   //! In the case where a empty SipDialog object is retrieved from the
   //DialogEventPublisher in handling a DISCONNECTED or FAILED message
   //the publisher still needs to find the dialog, even if it is just 
   //by the callId. Work-around for XCL-98.
   Dialog* getDialogByCallId(UtlString& callId);
   
   //! Remove the Dialog object from the hash table
   Dialog* removeDialog(Dialog* dialog);
   
   //! Check whether there is any dialog or not
   UtlBoolean isEmpty();

   //! Get the first dialog
   Dialog* getFirstDialog();

   //! Get all dialogs
   void getAllDialogs(UtlHashMap &dialogs);

///@}
   
/* //////////////////////////// PROTECTED ///////////////////////////////// */
  protected:
   
   /// Parse an existing dialog event package from xml format into the internal representation.
   void parseBody(const char* bytes);


/* //////////////////////////// PRIVATE /////////////////////////////////// */
  private:

   //! Variables for dialog-info
   int mVersion;
   UtlString mDialogState;
   UtlString mEntity;

   //! Variables for dialog element
   UtlHashMap mDialogs;

    //! reader/writer lock for synchronization
    OsBSem mLock;

   //! Disabled copy constructor
   SipDialogEvent(const SipDialogEvent& rSipDialogEvent);

   //! Disabled assignment operator
   SipDialogEvent& operator=(const SipDialogEvent& rhs);

};

/* ============================ INLINE METHODS ============================ */

#endif  // _SipDialogEvent_h_
