//  
// Copyright (C) 2006 SIPfoundry Inc. 
// Licensed by SIPfoundry under the LGPL license. 
//  
// Copyright (C) 2006 Hector Izquierdo Seliva. 
// Licensed to SIPfoundry under a Contributor Agreement. 
//  
// $$ 
////////////////////////////////////////////////////////////////////////////// 

#ifdef HAVE_SPEEX /* [ */


// APPLICATION INCLUDES
#include "mp/MpAudioConnection.h"
#include "mp/MpdSipxSpeex.h"
#include "mp/MprDejitter.h"
#include "os/OsSysLog.h"
#include "os/OsSysLogFacilities.h"


const MpCodecInfo MpdSipxSpeex::smCodecInfo(
         SdpCodec::SDP_CODEC_SPEEX,    // codecType
         "Speex codec",                // codecVersion
         false,                        // usesNetEq
         8000,                         // samplingRate
         8,                            // numBitsPerSample (not used)
         1,                            // numChannels
         38,                           // interleaveBlockSize
         15000,                        // bitRate
         1*8,                          // minPacketBits
         38*8,                         // avgPacketBits
         63*8,                         // maxPacketBits
         160,                          // numSamplesPerFrame
         5);                           // preCodecJitterBufferSize (should be adjusted)

              

MpdSipxSpeex::MpdSipxSpeex(int payloadType)
: MpDecoderBase(payloadType, &smCodecInfo)
, mpJBState(NULL)
, mpDecoderState(NULL)
, mDecbits()
, mNumSamplesPerFrame(0)
{   
}

MpdSipxSpeex::~MpdSipxSpeex()
{
}

OsStatus MpdSipxSpeex::initDecode(MpAudioConnection* pConnection)
{
   mpJBState = pConnection->getJBinst();

   JB_initCodepoint(mpJBState, "SPEEX", 8000, getPayloadType());

   if (mpDecoderState == NULL) {
      int tmp;
   
      // Init decoder
      mpDecoderState = speex_decoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));   

      // It makes the decoded speech deviate further from the original,
      // but it sounds subjectively better.
      tmp = 1;
      speex_decoder_ctl(mpDecoderState,SPEEX_SET_ENH,&tmp);

      // Get number of samples in one frame
      speex_decoder_ctl(mpDecoderState,SPEEX_GET_FRAME_SIZE,&mNumSamplesPerFrame);

      speex_bits_init(&mDecbits);
   }

   return OS_SUCCESS;
}

OsStatus MpdSipxSpeex::freeDecode(void)
{
   if (mpDecoderState != NULL) {
      speex_decoder_destroy(mpDecoderState);
      mpDecoderState = NULL;

      speex_bits_destroy(&mDecbits);
   }

   return OS_SUCCESS;
}

int MpdSipxSpeex::decode(const MpRtpBufPtr &pPacket, unsigned decodedBufferLength, MpAudioSample *samplesBuffer)
{
   // Assert that available buffer size is enough for the packet.
   assert(mNumSamplesPerFrame <= decodedBufferLength);

   // Prepare data for Speex decoder
   speex_bits_read_from(&mDecbits,(char*)pPacket->getDataPtr(),pPacket->getPayloadSize());

   // Decode frame
   speex_decode_int(mpDecoderState,&mDecbits,samplesBuffer);   

   return mNumSamplesPerFrame;
}

#endif /* HAVE_SPEEX ] */
