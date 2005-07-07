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
 

// SYSTEM INCLUDES
#include <assert.h>

// APPLICATION INCLUDES
#include <utl/UtlDListIterator.h>
#include <os/OsDatagramSocket.h>
#include <cp/CpPhoneMediaInterface.h>
#include <mp/MpMediaTask.h>
#include <mp/MpCallFlowGraph.h>
#include <mp/MpStreamPlayer.h>
#include <mp/MpStreamPlaylistPlayer.h>
#include <mp/MpStreamQueuePlayer.h>
#include <mp/dtmflib.h>
#include <net/SdpCodec.h>


#if defined(_VXWORKS)
#   include <socket.h>
#   include <resolvLib.h>
#   include <netinet/ip.h>
#elif defined(__pingtel_on_posix__)
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#endif

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
#define MINIMUM_DTMF_LENGTH 60
#define MAX_RTP_PORTS 1000

#ifdef _VXWORKS
#   define RING_TONE_FILE_NAME "/flash0/ringTone.raw"
#else
#   define RING_TONE_FILE_NAME "ringTone.raw"
#endif


//#define TEST_PRINT

// STATIC VARIABLE INITIALIZATIONS

class CpPhoneMediaConnection : public UtlInt
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:
    CpPhoneMediaConnection(int connectionId = -1) :
      UtlInt(connectionId)
    {
        mpRtpSocket = NULL;
        mpRtcpSocket = NULL;
        mRtpSendHostPort = 0;
        mRtcpSendHostPort = 0;
        mRtpReceivePort = 0;
        mRtcpReceivePort = 0;
        mDestinationSet = FALSE;       
        mRtpSending = FALSE;
        mRtpReceiving = FALSE;
        mpCodecFactory = NULL;
        mpPrimaryCodec = NULL;
    };

    virtual ~CpPhoneMediaConnection()
    {
        if(mpRtpSocket)
        {

#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "~CpPhoneMediaConnection deleting RTP socket: %p descriptor: %d",
                mpRtpSocket, mpRtpSocket->getSocketDescriptor());
#endif
            delete mpRtpSocket;
            mpRtpSocket = NULL;
        }
        if(mpRtcpSocket)
        {
#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "~CpPhoneMediaConnection deleting RTCP socket: %p descriptor: %d",
                mpRtcpSocket, mpRtcpSocket->getSocketDescriptor());
#endif
            delete mpRtcpSocket;
            mpRtcpSocket = NULL;
        }
        if(mpCodecFactory)
        {
            delete mpCodecFactory;
            mpCodecFactory = NULL;
        }

        if (mpPrimaryCodec)
        {
            delete mpPrimaryCodec;
            mpPrimaryCodec = NULL; 
        }
       
        
    }

    OsDatagramSocket* mpRtpSocket;
    OsDatagramSocket* mpRtcpSocket;
    UtlString mRtpSendHostAddress;
    int mRtpSendHostPort;
    int mRtcpSendHostPort;
    int mRtpReceivePort;
    int mRtcpReceivePort;
    UtlBoolean mDestinationSet;
    UtlBoolean mRtpSending;
    UtlBoolean mRtpReceiving;
    SdpCodecFactory* mpCodecFactory;
    SdpCodec* mpPrimaryCodec;
};

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Constructor
CpPhoneMediaInterface::CpPhoneMediaInterface(int startRtpPort, int lastRtpPort,
                                             const char* publicAddress,
                                             const char* localAddress,
                                             int numCodecs,
                                             SdpCodec* sdpCodecArray[],
                                             const char* locale,
                                             int expeditedIpTos)
{
   mpFlowGraph = new MpCallFlowGraph(locale);

   mNextRtpPort = startRtpPort;
   mLastRtpPort = lastRtpPort;

   if(publicAddress && *publicAddress)
   {
       mRtpReceiveHostAddress.append(publicAddress);
   }
   else
   {
       OsSocket::getHostIp(&mRtpReceiveHostAddress);
   }

   if(localAddress && *localAddress)
   {
       mLocalAddress.append(localAddress);
   }
   else
   {
        OsSocket::getHostIp(&mLocalAddress);
   }

   if(sdpCodecArray && numCodecs > 0)
   {
       mSupportedCodecs.addCodecs(numCodecs, sdpCodecArray);

       // Assign any unset payload types
       mSupportedCodecs.bindPayloadTypes();
   }
   else
   {
       // Temp hard code codecs
       SdpCodec mapCodecs1(SdpCodec::SDP_CODEC_PCMU, SdpCodec::SDP_CODEC_PCMU);
       mSupportedCodecs.addCodec(mapCodecs1);
       SdpCodec mapCodecs2(SdpCodec::SDP_CODEC_PCMA, SdpCodec::SDP_CODEC_PCMA);
       mSupportedCodecs.addCodec(mapCodecs2);
       //mapCodecs[2] = new SdpCodec(SdpCodec::SDP_CODEC_L16_MONO);
   }

   mExpeditedIpTos = expeditedIpTos;
}

// Copy constructor
CpPhoneMediaInterface::CpPhoneMediaInterface(const CpPhoneMediaInterface& rCpPhoneMediaInterface)
{
}

// Destructor
CpPhoneMediaInterface::~CpPhoneMediaInterface()
{
    CpPhoneMediaConnection* mediaConnection = NULL;
    while ((mediaConnection = (CpPhoneMediaConnection*) mMediaConnections.get()))
    {
        doDeleteConnection(mediaConnection);
        delete mediaConnection;
        mediaConnection = NULL;
    }

    if(mpFlowGraph)
    {
      // Free up the resources used by tone generation ASAP
      stopTone();

        // Stop the net in/out stuff before the sockets are deleted
        //mpMediaFlowGraph->stopReceiveRtp();
        //mpMediaFlowGraph->stopSendRtp();

        MpMediaTask* mediaTask = MpMediaTask::getMediaTask(0);

        // take focus away from the flow graph if it is focus
        if(mpFlowGraph == (MpCallFlowGraph*) mediaTask->getFocus())
        {
            mediaTask->setFocus(NULL);
        }
        delete mpFlowGraph;
        mpFlowGraph = NULL;
    }

    mRtpReceiveHostAddress.remove(0);
    mLocalAddress.remove(0);
}

/* ============================ MANIPULATORS ============================== */

// Assignment operator
CpPhoneMediaInterface&
CpPhoneMediaInterface::operator=(const CpPhoneMediaInterface& rhs)
{
   if (this == &rhs)            // handle the assignment to self case
      return *this;

   return *this;
}

OsStatus CpPhoneMediaInterface::createConnection(int& connectionId, int localPort)
{
    OsStatus returnCode;

    {
        connectionId = mpFlowGraph->createConnection();
        if (localPort <= 0) {
            localPort = mNextRtpPort;
        }

        CpPhoneMediaConnection* mediaConnection = new CpPhoneMediaConnection();
        mMediaConnections.append(mediaConnection);
        *mediaConnection = connectionId;

        // Create the sockets
        // Eventually this should use a specified address as this
        // host may be multi-homed
        OsDatagramSocket* rtpSocket = new OsDatagramSocket(0, NULL,
            localPort, mLocalAddress.data());
        OsDatagramSocket* rtcpSocket = new OsDatagramSocket(0, NULL,
            localPort + 1, mLocalAddress.data());

        // If either of the sockets are bad (e.g. already in use) or
        // if either have stuff on them to read (e.g. someone is
        // sending junk to the ports, look for another port pair
        while(!rtpSocket->isOk() || !rtcpSocket->isOk() ||
               rtcpSocket->isReadyToRead() ||
               rtpSocket->isReadyToRead(60))
        {
            localPort +=2;
            // This should use mLastRtpPort instead of some
            // hardcoded MAX, but I do not think mLastRtpPort
            // is set correctly in all of the products.
            if(localPort > mNextRtpPort + MAX_RTP_PORTS) 
            {
                OsSysLog::add(FAC_CP, PRI_ERR, 
                    "No available ports for RTP and RTCP in range %d - %d",
                    mNextRtpPort, mNextRtpPort + MAX_RTP_PORTS);
                break;  // time to give up
            }

            delete rtpSocket;
            delete rtcpSocket;
            rtpSocket = new OsDatagramSocket(0, NULL, localPort,
               mLocalAddress.data());
            rtcpSocket = new OsDatagramSocket(0, NULL, localPort + 1,
               mLocalAddress.data());
/*
            osPrintf("*** CpPhoneMediaInterface::createConnection port = %d\n",
               localPort);
*/

        }

        // Set a maximum on the buffers for the sockets so
        // that the network stack does not get swamped by early media
        // from the other side;
        {
            int sRtp, sRtcp, oRtp, oRtcp, optlen;

            sRtp = rtpSocket->getSocketDescriptor();
            sRtcp = rtcpSocket->getSocketDescriptor();

            optlen = sizeof(int);
            oRtp = 2000;
            setsockopt(sRtp, SOL_SOCKET, SO_RCVBUF, (char *) (&oRtp), optlen);
            oRtcp = 500;
            setsockopt(sRtcp, SOL_SOCKET, SO_RCVBUF, (char *) (&oRtcp), optlen);

            // Set the type of service (DiffServ code point) to low delay
            int tos = mExpeditedIpTos;
            
            setsockopt (sRtp, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int));
            setsockopt (sRtcp, IPPROTO_IP, IP_TOS, (char *)&tos, sizeof(int));
        }

#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "CpPhoneMediaConnection created RTP socket: %p descriptor: %d interface: %p connection: %d",
                rtpSocket, rtpSocket->getSocketDescriptor(), this, mediaConnection->value());
#endif
        (mediaConnection->mpRtpSocket) = rtpSocket;

#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "CpPhoneMediaConnection created RTP socket: %p descriptor: %d interface: %p connection: %d",
                rtcpSocket, rtcpSocket->getSocketDescriptor(), this, mediaConnection->value());
#endif
        (mediaConnection->mpRtcpSocket) = rtcpSocket;

        mediaConnection->mRtpReceivePort = localPort;
        mediaConnection->mRtcpReceivePort = localPort + 1;

        mediaConnection->mpCodecFactory = new SdpCodecFactory(mSupportedCodecs);
        mediaConnection->mpCodecFactory->bindPayloadTypes();
        returnCode = OS_SUCCESS;
    }

    return(returnCode);
}

int CpPhoneMediaInterface::getNumCodecs(int connectionId)
{
    CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);
    int numCodecs = 0;

    if(mediaConnection && mediaConnection->mpCodecFactory)
    {
       numCodecs = mediaConnection->mpCodecFactory->getCodecCount();
    }

    return(numCodecs);
}

OsStatus CpPhoneMediaInterface::getCapabilities(int connectionId,
                                                UtlString& rtpHostAddress,
                                                int& rtpPort,
                                                SdpCodecFactory& supportedCodecs)
{
   OsStatus returnCode = OS_INVALID;
   rtpHostAddress.remove(0);
   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);

   if(mediaConnection)
   {
      rtpHostAddress.append(mRtpReceiveHostAddress.data());
      rtpPort = mediaConnection->mRtpReceivePort;
      supportedCodecs = *(mediaConnection->mpCodecFactory);
      returnCode = OS_SUCCESS;
   }

   return(returnCode);
}

CpPhoneMediaConnection* CpPhoneMediaInterface::getMediaConnection(int connectionId)
{
   UtlInt matchConnectionId(connectionId);
   return((CpPhoneMediaConnection*) mMediaConnections.find(&matchConnectionId));
}

OsStatus CpPhoneMediaInterface::setConnectionDestination(int connectionId,
                                         const char* remoteRtpHostAddress,
                                         int remoteRtpPort)
{
   OsStatus returnCode = OS_NOT_FOUND;
   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);


   if(mediaConnection &&
       remoteRtpHostAddress && *remoteRtpHostAddress)
   {
       mediaConnection->mDestinationSet = TRUE;
       mediaConnection->mRtpSendHostAddress.remove(0);
       mediaConnection->mRtpSendHostAddress.append(remoteRtpHostAddress);
       mediaConnection->mRtpSendHostPort = remoteRtpPort;
       mediaConnection->mRtcpSendHostPort = remoteRtpPort + 1;

       if(mediaConnection && mediaConnection->mpRtpSocket)
       {
          mediaConnection->mpRtpSocket->doConnect(remoteRtpPort, remoteRtpHostAddress, TRUE);
          returnCode = OS_SUCCESS;
#ifdef TEST_PRINT
          OsSysLog::add(FAC_CP, PRI_DEBUG, "Setting RTP socket destination id: %d address: %s port:"
             " %d socket: %p descriptor: %d\n", 
             connectionId, remoteRtpHostAddress, remoteRtpPort, 
             mediaConnection->mpRtpSocket, mediaConnection->mpRtpSocket->getSocketDescriptor());
          //assert( strcmp(remoteRtpHostAddress, "0.0.0.0"));
#endif
       }

       if(mediaConnection->mpRtcpSocket)
       {
          mediaConnection->mpRtcpSocket->doConnect(remoteRtpPort + 1, remoteRtpHostAddress, TRUE);
#ifdef TEST_PRINT
          OsSysLog::add(FAC_CP, PRI_DEBUG, "Setting RTCP socket destination id: %d address: %s port:"
             " %d socket: %p descriptor: %d\n", 
             connectionId, remoteRtpHostAddress, remoteRtpPort, 
             mediaConnection->mpRtcpSocket, mediaConnection->mpRtcpSocket->getSocketDescriptor());
          //assert( strcmp(remoteRtpHostAddress, "0.0.0.0"));
#endif
       }
       else
       {
           OsSysLog::add(FAC_CP, PRI_ERR, "ERROR: no rtp socket in setConnectionDestination\n");
       }
   }
   else
   {
       OsSysLog::add(FAC_CP, PRI_ERR, "CpPhoneMediaInterface::setConnectionDestination with"
                   " zero length host address\n");
   }

   return(returnCode);
}

OsStatus CpPhoneMediaInterface::startRtpSend(int connectionId,
                                             int numCodecs,
                                             SdpCodec* sendCodecs[])
{
   // need to set default payload types in get capabilities

   int i;
   SdpCodec* primaryCodec = NULL;
   SdpCodec* dtmfCodec = NULL;
   OsStatus returnCode = OS_NOT_FOUND;
   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);
/*
   osPrintf("CpPhoneMediaInterface::startRtpSend(%d, %d, { ",
                  connectionId, numCodecs);
*/
   for (i=0; i<numCodecs; i++) {
      if (SdpCodec::SDP_CODEC_TONES == sendCodecs[i]->getValue()) {
         if (NULL == dtmfCodec) {
            dtmfCodec = sendCodecs[i];
         }
      } else if (NULL == primaryCodec) {
         primaryCodec = sendCodecs[i];
      }
/*
      osPrintf("%d%s", sendCodecs[i]->getValue(),
                     ((numCodecs-1)==i) ? " })\n":", ");
*/
   }
   if(mpFlowGraph && mediaConnection)
   {
#ifdef TEST_PRINT
       OsSysLog::add(FAC_CP, PRI_DEBUG, "Start Sending RTP/RTCP codec: %d sockets: %p/%p descriptors: %d/%d\n",
           primaryCodec ? primaryCodec->getCodecType() : -2,
           (mediaConnection->mpRtpSocket), (mediaConnection->mpRtcpSocket),
           mediaConnection->mpRtpSocket->getSocketDescriptor(),
           mediaConnection->mpRtcpSocket->getSocketDescriptor());
#endif

       // Store the primary codec for cost calculations later
       if (mediaConnection->mpPrimaryCodec != NULL)
       {
           delete mediaConnection->mpPrimaryCodec ;
           mediaConnection->mpPrimaryCodec = NULL ;
       }
       if (primaryCodec != NULL)
       {
           mediaConnection->mpPrimaryCodec = new SdpCodec();
           *mediaConnection->mpPrimaryCodec = *primaryCodec ;
       }

       // Make sure we use the same payload types as the remote
       // side.  Its the friendly thing to do.
       if(mediaConnection->mpCodecFactory)
       {
           mediaConnection->mpCodecFactory->copyPayloadTypes(numCodecs,
                                                            sendCodecs);
       }

       if(mediaConnection->mRtpSending)
       {
//           osPrintf("stop/restarting RTP send\n");
           mpFlowGraph->stopSendRtp(connectionId);
       }

#ifdef TEST_PRINT
      UtlString dtmfCodecString;
      if(dtmfCodec) dtmfCodec->toString(dtmfCodecString);
      OsSysLog::add(FAC_CP, PRI_DEBUG, "CpPhoneMediaInterface::startRtpSend %susing DTMF codec: %s\n",
         dtmfCodec ? "" : "NOT ",
         dtmfCodecString.data());
#endif

      if (!mediaConnection->mRtpSendHostAddress.isNull() && mediaConnection->mRtpSendHostAddress.compareTo("0.0.0.0"))
      {
         // This is the new interface for parallel codecs
         mpFlowGraph->startSendRtp(*(mediaConnection->mpRtpSocket),
                                   *(mediaConnection->mpRtcpSocket),
                                   connectionId,
                                   primaryCodec,
                                   dtmfCodec,
                                   NULL); // no redundant codecs

         mediaConnection->mRtpSending = TRUE;
      }
      returnCode = OS_SUCCESS;
   }
   return(returnCode);
}

#if 1 /* [ */
extern "C" {
extern int setCpStartRtpOptions(int);
}

static int doCallStop = 0;

int setCpStartRtpOptions(int v)
{
   int save = doCallStop;
   doCallStop = (0 != v);
   return save;
}
#endif /* ] */

OsStatus CpPhoneMediaInterface::startRtpReceive(int connectionId,
                                                int numCodecs,
                                                SdpCodec* receiveCodecs[])
{
   OsStatus returnCode = OS_NOT_FOUND;

   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);

   if(mpFlowGraph && mediaConnection)
   {
#ifdef TEST_PRINT
      int i;

      OsSysLog::add(FAC_CP, PRI_DEBUG, "Start Receiving RTP/RTCP, %d codec%s; sockets: %p/%p descriptors: %d/%d\n",
           numCodecs, ((1==numCodecs)?"":"s"),
           (mediaConnection->mpRtpSocket),
           (mediaConnection->mpRtcpSocket),
           mediaConnection->mpRtpSocket->getSocketDescriptor(),
           mediaConnection->mpRtcpSocket->getSocketDescriptor());
      for (i=0; i<numCodecs; i++) {
          osPrintf("   %d:  i:%d .. x:%d\n", i+1,
                   receiveCodecs[i]->getCodecType(),
                   receiveCodecs[i]->getCodecPayloadFormat());
      }
#endif

      // Make sure we use the same payload types as the remote
      // side.  It's the friendly thing to do.
      if(mediaConnection->mpCodecFactory)
      {
          mediaConnection->mpCodecFactory->copyPayloadTypes(numCodecs,
                                                           receiveCodecs);
      }

#if 1 /* [ */
      if(mediaConnection->mRtpReceiving)
      {
         // This is not supposed to be necessary and may be
         // causing an audible glitch when codecs are changed
         if (doCallStop) {
            mpFlowGraph->stopReceiveRtp(connectionId);
         }
      }
#endif /* ] */

      mpFlowGraph->startReceiveRtp(receiveCodecs, numCodecs,
           *(mediaConnection->mpRtpSocket), *(mediaConnection->mpRtcpSocket),
           connectionId);
      mediaConnection->mRtpReceiving = TRUE;



      returnCode = OS_SUCCESS;
   }
   return(returnCode);
}

OsStatus CpPhoneMediaInterface::stopRtpSend(int connectionId)
{
   OsStatus returnCode = OS_NOT_FOUND;
   CpPhoneMediaConnection* mediaConnection =
       getMediaConnection(connectionId);

   if(mpFlowGraph && mediaConnection &&
       mediaConnection->mRtpSending)
   {
      mpFlowGraph->stopSendRtp(connectionId);
      mediaConnection->mRtpSending = FALSE;
      returnCode = OS_SUCCESS;
   }
   return(returnCode);
}

OsStatus CpPhoneMediaInterface::stopRtpReceive(int connectionId)
{
   OsStatus returnCode = OS_NOT_FOUND;
   CpPhoneMediaConnection* mediaConnection =
       getMediaConnection(connectionId);

   if(mpFlowGraph && mediaConnection &&
       mediaConnection->mRtpReceiving)
   {
      mpFlowGraph->stopReceiveRtp(connectionId);
      mediaConnection->mRtpReceiving = FALSE;
      returnCode = OS_SUCCESS;
   }
   return(returnCode);
}

OsStatus CpPhoneMediaInterface::deleteConnection(int connectionId)
{
   OsStatus returnCode = OS_NOT_FOUND;
   CpPhoneMediaConnection* mediaConnection =
       getMediaConnection(connectionId);

   returnCode = doDeleteConnection(mediaConnection);
   return(returnCode);
}

OsStatus CpPhoneMediaInterface::doDeleteConnection(CpPhoneMediaConnection* mediaConnection)
{
   OsStatus returnCode = OS_NOT_FOUND;
   if(mediaConnection)
   {
      returnCode = OS_SUCCESS;
      mediaConnection->mDestinationSet = FALSE;
#ifdef TEST_PRINT
      if (mediaConnection && mediaConnection->mpRtpSocket && mediaConnection->mpRtcpSocket)
    OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "stopping RTP/RTCP send & receive sockets %p/%p descriptors: %d/%d",
                mediaConnection->mpRtpSocket,
                mediaConnection->mpRtcpSocket,
                mediaConnection->mpRtpSocket->getSocketDescriptor(),
                mediaConnection->mpRtcpSocket->getSocketDescriptor());
     else if (!mediaConnection)
    OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "CpPhoneMediaInterface::doDeleteConnection mediaConnection is NULL!");
     else 
    OsSysLog::add(FAC_CP, PRI_DEBUG, 
                "CpPhoneMediaInterface::doDeleteConnection NULL socket: mpRtpSocket=0x%08x, mpRtpSocket=0x%08x",
                mediaConnection->mpRtpSocket,
                                mediaConnection->mpRtcpSocket);
#endif

      returnCode = stopRtpSend(mediaConnection->getValue());
      returnCode = stopRtpReceive(mediaConnection->getValue());

      if(mediaConnection->getValue() >= 0)
      {
          mpFlowGraph->deleteConnection(mediaConnection->getValue());
          mediaConnection->setValue(-1);
          mpFlowGraph->synchronize();
      }
      if(mediaConnection->mpRtpSocket)
      {
#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, "deleting RTP socket: %p descriptor: %d",
                mediaConnection->mpRtpSocket,
                mediaConnection->mpRtpSocket->getSocketDescriptor());
#endif

         delete mediaConnection->mpRtpSocket;
         mediaConnection->mpRtpSocket = NULL;
      }
      if(mediaConnection->mpRtcpSocket)
      {
#ifdef TEST_PRINT
            OsSysLog::add(FAC_CP, PRI_DEBUG, "deleting RTCP socket: %p descriptor: %d",
                mediaConnection->mpRtcpSocket,
                mediaConnection->mpRtcpSocket->getSocketDescriptor());
#endif

         delete mediaConnection->mpRtcpSocket;
         mediaConnection->mpRtcpSocket = NULL;
      }
   }
   return(returnCode);
}

OsStatus CpPhoneMediaInterface::startDtmf(char toneId,
                                          UtlBoolean local,
                                          UtlBoolean remote)
{
   OsStatus returnCode = OS_SUCCESS;
   int toneDestination = 0 ;

   if(mpFlowGraph)
   {      
      if (local)
      {
         toneDestination |= MpCallFlowGraph::TONE_TO_SPKR;
      }                  

      if(remote)
      {
         toneDestination |= MpCallFlowGraph::TONE_TO_NET;
      }
      
      mpFlowGraph->startTone(toneId, toneDestination);
      
      // Make sure the DTMF tone is on the minimum length
      OsTask::delay(MINIMUM_DTMF_LENGTH);
   }

   return(returnCode);
}

OsStatus CpPhoneMediaInterface::stopDtmf()
{
   OsStatus returnCode = OS_SUCCESS;
   if(mpFlowGraph)
   {
      mpFlowGraph->stopTone();
        // Make sure the DTMF tone is on the minimum length
      OsTask::delay(MINIMUM_DTMF_LENGTH);
   }

   return(returnCode);
}

OsStatus CpPhoneMediaInterface::playAudio(const char* url,
                                          UtlBoolean repeat,
                                          UtlBoolean local,
                                          UtlBoolean remote)
{
    OsStatus returnCode = OS_NOT_FOUND;
    UtlString urlString;
    if(url) urlString.append(url);
    int fileIndex = urlString.index("file://");
    if(fileIndex == 0) urlString.remove(0, 6);

    if(mpFlowGraph && !urlString.isNull())
    {

        // Start playing the audio file
        returnCode = mpFlowGraph->playFile(urlString.data(),
            repeat,
            remote ? MpCallFlowGraph::TONE_TO_NET :
                MpCallFlowGraph::TONE_TO_SPKR);
    }

    if(returnCode != OS_SUCCESS)
    {
        osPrintf("Cannot play audio file: %s\n", urlString.data());
    }

    return(returnCode);
}

OsStatus CpPhoneMediaInterface::playBuffer(char* buf,
                                           unsigned long bufSize,
                                           int type, 
                                           UtlBoolean repeat,
                                           UtlBoolean local,
                                           UtlBoolean remote, 
                                           OsNotification* event)
{
    OsStatus returnCode = OS_NOT_FOUND;
    if(mpFlowGraph && buf)
    {

        // Start playing the audio file
        returnCode = mpFlowGraph->playBuffer(buf, bufSize, type, 
               repeat,
               remote ? MpCallFlowGraph::TONE_TO_NET : MpCallFlowGraph::TONE_TO_SPKR,
               event);
    }

    if(returnCode != OS_SUCCESS)
    {
        osPrintf("Cannot play audio buffer: %10p\n", buf);
    }

    return(returnCode);
}

OsStatus CpPhoneMediaInterface::pauseAudio()
{
    return(OS_NOT_FOUND);
}

OsStatus CpPhoneMediaInterface::stopAudio()
{
    OsStatus returnCode = OS_NOT_FOUND;
    if(mpFlowGraph)
    {
        mpFlowGraph->stopFile(TRUE);
        returnCode = OS_SUCCESS;
    }
    return(returnCode);
}


OsStatus CpPhoneMediaInterface::createPlayer(MpStreamPlayer** ppPlayer, 
                                             const char* szStream, 
                                             int flags, 
                                             OsMsgQ *pMsgQ, 
                                             const char* szTarget)
{
   OsStatus returnCode = OS_NOT_FOUND;

   if ((pMsgQ == NULL) && (mpFlowGraph != NULL))
      pMsgQ = mpFlowGraph->getMsgQ() ;


   if(pMsgQ != NULL)
   {
      Url url(szStream) ;

      *ppPlayer = new MpStreamPlayer(pMsgQ, url, flags, szTarget) ;

      returnCode = OS_SUCCESS;
   }

   return(returnCode);
}


OsStatus CpPhoneMediaInterface::destroyPlayer(MpStreamPlayer* pPlayer)
{
   if (pPlayer != NULL)
   {
      pPlayer->destroy() ;
   }

   return OS_SUCCESS;   
}


OsStatus CpPhoneMediaInterface::createPlaylistPlayer(MpStreamPlaylistPlayer** ppPlayer, 
                                                     OsMsgQ *pMsgQ, 
                                                     const char* szTarget)
{
   OsStatus returnCode = OS_NOT_FOUND;

   if ((pMsgQ == NULL) && (mpFlowGraph != NULL))
      pMsgQ = mpFlowGraph->getMsgQ() ;

   if(pMsgQ != NULL)   
   {
      *ppPlayer = new MpStreamPlaylistPlayer(pMsgQ, szTarget) ;
      returnCode = OS_SUCCESS;
   }

   return(returnCode);
}


OsStatus CpPhoneMediaInterface::destroyPlaylistPlayer(MpStreamPlaylistPlayer* pPlayer)
{
   if (pPlayer != NULL)
   {
      pPlayer->destroy() ;
   }

   return OS_SUCCESS;   
}


OsStatus CpPhoneMediaInterface::createQueuePlayer(MpStreamQueuePlayer** ppPlayer, 
                                                  OsMsgQ *pMsgQ, 
                                                  const char* szTarget)
{
   OsStatus returnCode = OS_NOT_FOUND;
   
   if ((pMsgQ == NULL) && (mpFlowGraph != NULL))
      pMsgQ = mpFlowGraph->getMsgQ() ;

   if(pMsgQ != NULL)   
   {
      *ppPlayer = new MpStreamQueuePlayer(pMsgQ, szTarget) ;
      returnCode = OS_SUCCESS;
   }

   return(returnCode);
}


OsStatus CpPhoneMediaInterface::destroyQueuePlayer(MpStreamQueuePlayer* pPlayer)
{
   if (pPlayer != NULL)
   {
      pPlayer->destroy() ;
   }

   return OS_SUCCESS;   
}


OsStatus CpPhoneMediaInterface::startTone(int toneId,
                                          UtlBoolean local,
                                          UtlBoolean remote)
{
   OsStatus returnCode = OS_SUCCESS;
   int toneDestination = 0 ;

   if(mpFlowGraph)
   {
      if (local)
      {
         toneDestination |= MpCallFlowGraph::TONE_TO_SPKR;
      }                  
      
      if(remote)
      {
         toneDestination |= MpCallFlowGraph::TONE_TO_NET;
      }
      
      mpFlowGraph->startTone(toneId, toneDestination);

      // Make sure the DTMF tone is on the minimum length
      OsTask::delay(MINIMUM_DTMF_LENGTH);
   } 

   return(returnCode);
}

OsStatus CpPhoneMediaInterface::stopTone()
{
   OsStatus returnCode = OS_SUCCESS;
   if(mpFlowGraph)
   {
      mpFlowGraph->stopTone();
   }

   return(returnCode);
}

void CpPhoneMediaInterface::giveFocus()
{
    if(mpFlowGraph)
    {
        // There should probably be a lock here
        // Set the flow graph to have the focus
        MpMediaTask* mediaTask = MpMediaTask::getMediaTask(0);
        mediaTask->setFocus(mpFlowGraph);
        // osPrintf("Setting focus for flow graph\n");
   }
}

void CpPhoneMediaInterface::defocus()
{
    if(mpFlowGraph)
    {
        MpMediaTask* mediaTask = MpMediaTask::getMediaTask(0);

        // There should probably be a lock here
        // take focus away from the flow graph if it is focus
        if(mpFlowGraph == (MpCallFlowGraph*) mediaTask->getFocus())
        {
            mediaTask->setFocus(NULL);
            // osPrintf("Setting NULL focus for flow graph\n");
        }
   }
}


// Limits the available codecs to only those within the designated limit.
void CpPhoneMediaInterface::setCodecCPULimit(int iLimit)
{
   mSupportedCodecs.setCodecCPULimit(iLimit) ;

   CpPhoneMediaConnection* mediaConnection = NULL;
   UtlDListIterator connectionIterator(mMediaConnections);
   while ((mediaConnection = (CpPhoneMediaConnection*) connectionIterator()))
   {
      mediaConnection->mpCodecFactory->setCodecCPULimit(iLimit) ;
   }
}

OsStatus CpPhoneMediaInterface::stopRecording()
{
   OsStatus ret = OS_UNSPECIFIED;
   if (mpFlowGraph)
   {
#ifdef TEST_PRINT
     osPrintf("CpPhoneMediaInterface::stopRecording() : calling flowgraph::stoprecorders\n");
     OsSysLog::add(FAC_CP, PRI_DEBUG, "CpPhoneMediaInterface::stopRecording() : calling flowgraph::stoprecorders");
#endif
     mpFlowGraph->closeRecorders();
     ret = OS_SUCCESS;
   }
   
   return ret;
}


OsStatus CpPhoneMediaInterface::ezRecord(int ms, 
                                         int silenceLength, 
                                         const char* fileName, 
                                         double& duration,
                                         int& dtmfterm,
                                         OsProtectedEvent* ev)
{
   OsStatus ret = OS_UNSPECIFIED;
   if (mpFlowGraph && fileName)
   {
     if (!ev) // default behavior
        ret = mpFlowGraph->ezRecord(ms, 
                                 silenceLength, 
                                 fileName, 
                                 duration, 
                                 dtmfterm, 
                                 MprRecorder::WAV_PCM_16);
     else
        ret = mpFlowGraph->mediaRecord(ms, 
                                 silenceLength, 
                                 fileName, 
                                 duration, 
                                 dtmfterm, 
                                 MprRecorder::WAV_PCM_16,
                                 ev);
   }
   
   return ret;
}

void CpPhoneMediaInterface::addToneListener(OsNotification *pListener, int connectionId)
{
    if ((mpFlowGraph) && (connectionId >= 0))
    {
        mpFlowGraph->addToneListener(pListener, (MpConnectionID) connectionId);
    }
}

void CpPhoneMediaInterface::removeToneListener(int connectionId)
{
    if ((mpFlowGraph) && (connectionId >= 0))
    {
        mpFlowGraph->removeToneListener((MpConnectionID) connectionId) ;
    }
}


/* ============================ ACCESSORS ================================= */

void CpPhoneMediaInterface::setPremiumSound(UtlBoolean enabled)
{
    if(mpFlowGraph)
    {
        if(enabled)
        {
            mpFlowGraph->enablePremiumSound();
        }
        else
        {
            mpFlowGraph->disablePremiumSound();
        }
    }
}


// Calculate the current cost for our sending/receiving codecs
int CpPhoneMediaInterface::getCodecCPUCost()
{   
   int iCost = SdpCodec::SDP_CODEC_CPU_LOW ;   

   if (mMediaConnections.entries() > 0)
   {      
      CpPhoneMediaConnection* mediaConnection = NULL;

      // Iterate the connections and determine the most expensive supported 
      // codec.
      UtlDListIterator connectionIterator(mMediaConnections);
      while ((mediaConnection = (CpPhoneMediaConnection*) connectionIterator()))
      {
         // If the codec is null, assume LOW.
         if (mediaConnection->mpPrimaryCodec != NULL)
         {
            int iCodecCost = mediaConnection->mpPrimaryCodec->getCPUCost();
            if (iCodecCost > iCost)
               iCost = iCodecCost;
         }

         // Optimization: If we have already hit the highest, kick out.
         if (iCost == SdpCodec::SDP_CODEC_CPU_HIGH)
            break ;
      }
   }
   
   return iCost ;
}


// Calculate the worst case cost for our sending/receiving codecs
int CpPhoneMediaInterface::getCodecCPULimit()
{   
   int iCost = SdpCodec::SDP_CODEC_CPU_LOW ;   
   int         iCodecs = 0 ;
   SdpCodec**  codecs ;


   //
   // If have connections; report what we have offered
   //
   if (mMediaConnections.entries() > 0)
   {      
      CpPhoneMediaConnection* mediaConnection = NULL;

      // Iterate the connections and determine the most expensive supported 
      // codec.
      UtlDListIterator connectionIterator(mMediaConnections);
      while ((mediaConnection = (CpPhoneMediaConnection*) connectionIterator()))
      {
         mediaConnection->mpCodecFactory->getCodecs(iCodecs, codecs) ;      
         for(int i = 0; i < iCodecs; i++)
         {
            // If the cost is greater than what we have, then make that the cost.
            int iCodecCost = codecs[i]->getCPUCost();
            if (iCodecCost > iCost)
               iCost = iCodecCost;

             delete codecs[i];
         }
         delete[] codecs;

         // Optimization: If we have already hit the highest, kick out.
         if (iCost == SdpCodec::SDP_CODEC_CPU_HIGH)
            break ;
      }
   }
   //
   // If no connections; report what we plan on using
   //
   else
   {
      mSupportedCodecs.getCodecs(iCodecs, codecs) ;  
      for(int i = 0; i < iCodecs; i++)
      {
         // If the cost is greater than what we have, then make that the cost.
         int iCodecCost = codecs[i]->getCPUCost();
         if (iCodecCost > iCost)
            iCost = iCodecCost;

          delete codecs[i];
      }
      delete[] codecs;
   }

   return iCost ;
}

// Returns the flowgraph's message queue
   OsMsgQ* CpPhoneMediaInterface::getMsgQ()
{
   return mpFlowGraph->getMsgQ() ;
}

/* ============================ INQUIRY =================================== */
UtlBoolean CpPhoneMediaInterface::isSendingRtp(int connectionId)
{
   UtlBoolean sending = FALSE;
   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);

   if(mediaConnection)
   {
       sending = mediaConnection->mRtpSending;
   }
   else
   {
       osPrintf("CpPhoneMediaInterface::isSendingRtp invalid connectionId: %d\n",
          connectionId);
   }

   return(sending);
}

UtlBoolean CpPhoneMediaInterface::isReceivingRtp(int connectionId)
{
   UtlBoolean receiving = FALSE;
   CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);

   if(mediaConnection)
   {
      receiving = mediaConnection->mRtpReceiving;
   }
   else
   {
       osPrintf("CpPhoneMediaInterface::isReceivingRtp invalid connectionId: %d\n",
          connectionId);
   }
   return(receiving);
}

UtlBoolean CpPhoneMediaInterface::isDestinationSet(int connectionId)
{
    UtlBoolean isSet = FALSE;
    CpPhoneMediaConnection* mediaConnection = getMediaConnection(connectionId);

    if(mediaConnection)
    {
        isSet = mediaConnection->mDestinationSet;
    }
    else
    {
       osPrintf("CpPhoneMediaInterface::isDestinationSet invalid connectionId: %d\n",
          connectionId);
    }
    return(isSet);
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */

