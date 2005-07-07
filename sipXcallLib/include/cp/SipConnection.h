// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
//////////////////////////////////////////////////////////////////////////////

#ifndef _SipConnection_h_
#define _SipConnection_h_

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include <cp/Connection.h>

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class SipUserAgent;
class SipMessage;
class SdpCodec;
class SdpCodecFactory;


//:Class short description which may consist of multiple lines (note the ':')
// Class detailed description which may extend to multiple lines
class SipConnection : public Connection
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

	enum ReinviteStates
	{
		ACCEPT_INVITE = 0,
		REINVITED,
		REINVITING
	};

/* ============================ CREATORS ================================== */

   SipConnection(const char* outboundLineAddress,
                 UtlBoolean isEarlyMediaFor180Enabled = TRUE,
				 CpCallManager* callMgr = NULL,
                 CpCall* call = NULL,
                 CpMediaInterface* mediaInterface = NULL, 
                 SipUserAgent* sipUA = NULL, 
                 int offeringDelayMilliSeconds = IMMEDIATE,
                 int sessionReinviteTimer = 0,
                 int availableBehavior = RING, 
                 const char* forwardUnconditionalUrl = NULL,
                 int busyBehavior = BUSY, 
                 const char* forwardOnBusy = NULL);
     //:Default constructor

   virtual
   ~SipConnection();
     //:Destructor

/* ============================ MANIPULATORS ============================== */

   virtual UtlBoolean dequeue(UtlBoolean callInFocus);

   virtual UtlBoolean dial(const char* dialString,
						  const char* callerId,
						  const char* callId,
                          const char* callController = NULL,
                          const char* originalCallConnection = NULL,
                          UtlBoolean requestQueuedCall = FALSE);
   //! param: requestQueuedCall - indicates that the caller wishes to have the callee queue the call if busy

   virtual UtlBoolean originalCallTransfer(UtlString& transferTargetAddress,
						           const char* transferControllerAddress,
                                   const char* targetCallId);
   // Initiate transfer on transfer controller connection in 
   // the original call.
   // If fromAddress or toAddress are NULL it is assumed to
   // be a blind transfer.

   virtual UtlBoolean targetCallBlindTransfer(const char* transferTargetAddress,
						           const char* transferControllerAddress);
   // Communicate blind transfer on transfer controller connection in 
   // the target call.  This is signaled by the transfer controller in the
   // original call.

   virtual UtlBoolean transfereeStatus(int connectionState, int response);
   // Method to communicate status to original call on transferee side

   virtual UtlBoolean transferControllerStatus(int connectionState, int response);
   // Method to communicate status to target call on transfer
   // controller side

   virtual UtlBoolean answer();

   virtual UtlBoolean hangUp();

   virtual UtlBoolean hold();

   virtual UtlBoolean offHold();

   virtual UtlBoolean renegotiateCodecs();

   virtual UtlBoolean accept(int forwardOnNoAnswerTimeOut);

   virtual UtlBoolean reject();

   virtual UtlBoolean redirect(const char* forwardAddress);

   virtual UtlBoolean processMessage(OsMsg& eventMessage,
                                    UtlBoolean callInFocus, UtlBoolean onHook);
									//int numCodecs,
									//SdpCodec* rtpCodecsArray[]);

   void setCallerId();

   virtual UtlBoolean getRemoteAddress(UtlString* remoteAddress) const;
   virtual UtlBoolean getRemoteAddress(UtlString* remoteAddress,
                                      UtlBoolean leaveFieldParmetersIn) const;

   static UtlBoolean processNewFinalMessage(SipUserAgent* sipUa,
                                             OsMsg* eventMessage);

   void setContactType(CONTACT_TYPE eType) ;

   


/* ============================ ACCESSORS ================================= */

    virtual UtlBoolean getSession(SipSession& session);
    
	virtual OsStatus getFromField(UtlString* fromField);

	virtual OsStatus getToField(UtlString* toField);

    int getNextCseq();

/* ============================ INQUIRY =================================== */

   static UtlBoolean shouldCreateConnection(SipUserAgent& sipUa,
                                            OsMsg& eventMessage, 
                                            SdpCodecFactory* codecFactory = NULL);

   virtual UtlBoolean willHandleMessage(OsMsg& eventMessage) const;

   virtual UtlBoolean isConnection(const char* callId, 
                                  const char* toTag,
                                  const char* fromTag,
                                  UtlBoolean  strictCompare) const;

   virtual UtlBoolean isSameRemoteAddress(Url& remoteAddress) const;
   virtual UtlBoolean isSameRemoteAddress(Url& remoteAddress,
                                         UtlBoolean tagsMustMatch) const;

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:
    
   CONTACT_TYPE selectCompatibleContactType(const SipMessage& request) ;
     //: Select a compatible contact given the URI

   void updateContact(Url* pContactUrl, CONTACT_TYPE eType) const;

   static UtlBoolean requestShouldCreateConnection(const SipMessage* sipMsg,
                                            SipUserAgent& sipUa,
                                            SdpCodecFactory* codecFactory);

   UtlBoolean doOffHold(UtlBoolean forceReInvite);

   UtlBoolean doHangUp(const char* dialString = NULL,
				      const char* callerId = NULL);

   void SipConnection::buildFromToAddresses(const char* dialString,
                                            const char* callerId,
                                            const char* callerDisplayName,
                                            UtlString& fromAddress, 
                                            UtlString& goodToAddress) const;

   void buildLocalContact(Url fromAddress,
                          UtlString& localContact) const;//for outbound call
   void buildLocalContact(UtlString& localContact) const;//when getting inbound calls

   UtlBoolean extendSessionReinvite();

   // SIP Request handlers
   UtlBoolean processRequest(const SipMessage* request,
                            UtlBoolean callInFocus, UtlBoolean onHook);
   void processInviteRequest(const SipMessage* request);
   void processReferRequest(const SipMessage* request);
   void processAckRequest(const SipMessage* request);
   void processByeRequest(const SipMessage* request);
   void processCancelRequest(const SipMessage* request);
   void processNotifyRequest(const SipMessage* request);

   // SIP Response handlers
   UtlBoolean processResponse(const SipMessage* response,
                            UtlBoolean callInFocus, UtlBoolean onHook);
   void processInviteResponse(const SipMessage* request);
   void processReferResponse(const SipMessage* request);
   void processOptionsResponse(const SipMessage* request);
   void processByeResponse(const SipMessage* request);
   void processCancelResponse(const SipMessage* request);
   void processNotifyResponse(const SipMessage* request);

   /* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

	SipUserAgent* sipUserAgent;
    UtlString mFromTag;
	SipMessage* inviteMsg;
	UtlBoolean	mbCancelling;

    SipMessage* mReferMessage;
    UtlBoolean inviteFromThisSide;
    UtlString mLastRequestMethod;
	UtlString mRemoteContact; //last contact frield from the other side
    Url mFromUrl; // SIP address for the local side
    Url mToUrl;  //  SIP address for the remote side
    UtlString mRemoteUriStr;  //  SIP uri string for the remote side
    UtlString mLocalUriStr;  //  SIP uri string for the local side

	int lastLocalSequenceNumber;
    int lastRemoteSequenceNumber;
	int reinviteState;
    UtlString mRouteField;
    int mDefaultSessionReinviteTimer;
    int mSessionReinviteTimer;
    UtlString mAllowedRemote;  // Methods supported by the otherside
	UtlBoolean mIsReferSent;   // used to determine whether to send ack when sequence number is smaller
	UtlBoolean mIsAcceptSent;   // used to determine whether to accept ack when sequence number is smaller
    int mHoldCompleteAction;
	UtlBoolean mIsEarlyMediaFor180;
    UtlString mLineId; //line identifier for incoming calls. 
    UtlString mLocalContact;
    CONTACT_TYPE mContactType ;

    UtlBoolean getInitialSdpCodecs(const SipMessage* sdpMessage, 
                                        SdpCodecFactory& supportedCodecsArray,
                                        int& numCodecsInCommon,
                                        SdpCodec** &codecsInCommon,
                                        UtlString& remoteAddress,
                                        int& remotePort) const;

    virtual void proceedToRinging(const SipMessage* inviteMessage,
                                 SipUserAgent* sipUserAgent,
                                 int tagNum,
                                 //UtlBoolean callInFocus,
                                 int availableBehavior,
                                 CpMediaInterface* mediaInterface);

    UtlBoolean isMethodAllowed(const char* method);

    void doBlindRefer();



	SipConnection& operator=(const SipConnection& rhs);
     //:Assignment operator (disabled)

	SipConnection(const SipConnection& rSipConnection);
     //:Copy constructor (disabled)

};

/* ============================ INLINE METHODS ============================ */

#endif  // _SipConnection_h_
