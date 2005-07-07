// 
// 
// Copyright (C) 2004 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
// 
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
// 
// $$
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
//#include <...>

// APPLICATION INCLUDES
#include "mailboxmgr/VXMLDefs.h"
#include "mailboxmgr/MailboxManager.h"
#include "mailboxmgr/SaveSystemPromptsCGI.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/* ============================ CREATORS ================================== */
SaveSystemPromptsCGI::SaveSystemPromptsCGI(	const UtlString& promptType,
									        const char* data, 
									        int   datasize ) :
    m_promptType ( promptType ),
    m_datasize ( datasize )
{
    if (data && datasize > 0)
    {
      m_data = new char[datasize + 1];
      if (m_data)
      {
		  memcpy(m_data, data, datasize);
		  m_data[datasize] = 0;
      }
    }

}

SaveSystemPromptsCGI::~SaveSystemPromptsCGI()
{
  if (m_data)
    delete[] m_data;

}

OsStatus
SaveSystemPromptsCGI::execute(UtlString* out) 
{
    OsStatus result = OS_SUCCESS;
    MailboxManager* pMailboxManager =
        MailboxManager::getInstance();

    // Forward call to Mailbox Manager 
    result = pMailboxManager->saveSystemPrompts(
                m_promptType, m_data, m_datasize );

	UtlString dynamicVxml (VXML_BODY_BEGIN);

	if( result == OS_SUCCESS )
	{
		dynamicVxml += VXML_SUCCESS_SNIPPET;
	}
	else
	{
		dynamicVxml += VXML_FAILURE_SNIPPET;
	}

	dynamicVxml += VXML_END;

	// Write out the dynamic VXML script to be processed by OpenVXI
	if (out) 
	{
        out->remove(0);
        UtlString responseHeaders;
        MailboxManager::getResponseHeaders(dynamicVxml.length(), responseHeaders);

        out->append(responseHeaders.data());
        out->append(dynamicVxml.data());
	}
    return OS_SUCCESS;
}
