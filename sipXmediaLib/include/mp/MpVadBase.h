//
// Copyright (C) 2008 SIPez LLC.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// Copyright (C) 2008 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// Author: Sergey Kostanbaev <Sergey DOT Kostanbaev AT sipez DOT com>

#ifndef _MpVadBase_h_
#define _MpVadBase_h_

// SYSTEM INCLUDES
// APPLICATION INCLUDES
#include <utl/UtlDefs.h>
#include <os/OsStatus.h>
#include <utl/UtlString.h>
#include "mp/MpTypes.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/**
*  Base class for all VAD algorithms.
*
*  To create concrete class you could directly instantiate it or use
*  MpVadBase::createVad() static method for greater flexibility.
*
*  @nosubgrouping
*/

class MpVadBase
{
/* //////////////////////////// PUBLIC //////////////////////////////////// */
public:

/* ============================ CREATORS ================================== */
///@name Creators
//@{
     /// Initialize VAD with given sample rate
   virtual OsStatus init(int samplesPerSec) = 0;
     /**<
     *  Should be called before any other class methods.
     */

     /// Factory method for VAD algorithms creation.
   static MpVadBase *createVad(const UtlString &vadName = "");
     /**<
     *  @param[in] vadName - name of VAD algorithm to use. Use empty string
     *             to get default algorithm.
     *
     *  @returns Method never returns NULL. If appropriate VAD algorithm is
     *           not found, default one is returned.
     */

     /// Destructor
   virtual ~MpVadBase() {};

     /// Reset
   virtual void reset() = 0;

//@}

/* ============================ MANIPULATORS ============================== */
///@name Manipulators
//@{
     /// Detect speech presence
   virtual MpSpeechType processFrame(uint32_t packetTimeStamp,
                                     const MpAudioSample* pBuf,
                                     unsigned inSamplesNum,
                                     const MpSpeechParams &speechParams,
                                     UtlBoolean calcEnergyOnly = FALSE) = 0;
     /**<
     * @param[in] packetTimeStamp - RTP timestamp of packet.
     * @param[in] pBuf - buffer with input data.
     * @param[in] inSamplesNum - number of samples of actual data, passed to
     *            this function.
     * @param[in] speechParams - various parameters of speech.
     * @param[in] calcEnergyOnly - if TRUE, VAD should calculate energy only.
     * @returns Method returns MP_SPEECH_ACTIVE or MP_SPEECH_SILENT mainly,
     *          but if algorithm doesn't handle some situations return value
     *          may be MP_SPEECH_UNKNOWN
     */

     /// Set algorithm parameter
   virtual OsStatus setParam(const char* paramName, void* value) = 0;
     /**<
     * @param[in] paramName - name of parameter
     * @param[in] value - value, can be any type, before call cast it to void*
     * @returns Method returns OS_SUCCESS if parameter has been set,
     *          otherwise OS_FAILED
     */ 

//@}

/* ============================ ACCESSORS ================================= */
///@name Accessors
//@{
     /// Return weighted energy of current frame
   virtual int getEnergy() const = 0;

//@}

/* ============================ INQUIRY =================================== */
///@name Inquiry
//@{

//@}

/* //////////////////////////// PROTECTED ///////////////////////////////// */
protected:

/* //////////////////////////// PRIVATE /////////////////////////////////// */
private:

};

#endif // _MpVadBase_h_
