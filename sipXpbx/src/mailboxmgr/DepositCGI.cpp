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
#include "net/HttpMessage.h"
#include "mailboxmgr/VXMLDefs.h"
#include "mailboxmgr/ActiveGreetingHelper.h"
#include "mailboxmgr/ValidateMailboxCGIHelper.h"
#include "mailboxmgr/MailboxManager.h"
#include "mailboxmgr/DepositCGI.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS

/* ============================ CREATORS ================================== */

DepositCGI::DepositCGI( const Url& from,
                        const UtlString& mailboxIdentity ) :
   m_from ( from ),
   m_mailboxIdentity ( mailboxIdentity )
{}

DepositCGI::~DepositCGI()
{}

OsStatus
DepositCGI::execute(UtlString* out)
{
   // Contains the dynamically generated VXML script.
   UtlString dynamicVxml = UtlString( VXML_BODY_BEGIN ) + "<form> ";

   // Get the base URL of the mediaserver - "http://localhost:8090"
   MailboxManager* pMailboxManager = MailboxManager::getInstance();

   UtlString ivrPromptUrl;
   pMailboxManager->getIvrPromptURL( ivrPromptUrl );

   UtlString mediaserverUrl;
   pMailboxManager->getMediaserverURL( mediaserverUrl );

   UtlString secureMediaserverUrl;
   pMailboxManager->getMediaserverSecureURL( secureMediaserverUrl );

   // Validate the mailbox id and extension.
   // If valid, lazily create the physical folders for the mailbox if necessary.

   OsSysLog::add(FAC_MEDIASERVER_CGI, PRI_DEBUG,
                 "DepositCGI::execute: attempting to validate mailbox '%s'",
                 m_mailboxIdentity.data());
   ValidateMailboxCGIHelper validateMailboxHelper ( m_mailboxIdentity );
   OsStatus result = validateMailboxHelper.execute( out );
   OsSysLog::add(FAC_MEDIASERVER_CGI, PRI_DEBUG,
                 "DepositCGI::execute: validating mailbox '%s', result %d",
                 m_mailboxIdentity.data(), result);
    
   if ( result == OS_SUCCESS ) 
   {
      // Get the fully qualified mailbox id.
      validateMailboxHelper.getMailboxIdentity( m_mailboxIdentity );
      OsSysLog::add(FAC_MEDIASERVER_CGI, PRI_DEBUG,
                    "DepositCGI::execute: getMailboxIdentity returned m_mailboxIdentity = '%s'",
                    m_mailboxIdentity.data());

      // URL of user's greeting
      UtlString greetingUrl;
      ActiveGreetingHelper greetingHelper;

      // Retrieve the active greeting snippet and embed into dynamic VXML snippet
      if ( greetingHelper.getActiveGreetingUrl( m_mailboxIdentity, greetingUrl, FALSE ) == OS_SUCCESS )
      {
         dynamicVxml +=	
            "<var name=\"msgurl\" expr=\"'" + greetingUrl + "'\" />\n" \
            "<var name=\"msgurltype\" expr=\"'url'\"/>\n";		
      } else if ( greetingHelper.getRecordedName( m_mailboxIdentity, greetingUrl, FALSE ) == OS_SUCCESS )
      {
         // Greeting was not found. However, recorded name was found.
         // Play <fred> is not available
         dynamicVxml +=	
            "<var name=\"msgurl\" expr=\"'" + greetingUrl + "'\" />\n" \
            "<var name=\"msgurltype\" expr=\"'name'\"/>\n";
      } else
      {
         // Send the user id so that it can be spelt out.
         UtlString useridOrExtension;
         validateMailboxHelper.getExtension( useridOrExtension );
         // extension is optional, if not set use the userid
         if ( useridOrExtension.isNull() )
         {
            // parse the m_mailboxIdentity for the userid piece
            validateMailboxHelper.getUserId ( 
               m_mailboxIdentity, 
               useridOrExtension );
         } else 
         {
            // Extract the leading numeric id.
            if( useridOrExtension.first( '@' ) != UTL_NOT_FOUND )
            {
               useridOrExtension = 
                  useridOrExtension(0, useridOrExtension.first( '@' ));
            }
         }
         useridOrExtension.toLower();
         dynamicVxml +=	
            "<var name=\"msgurl\" expr=\"'" + useridOrExtension + "'\" />\n" \
            "<var name=\"msgurltype\" expr=\"'alphanumeric'\"/>\n";
      }

      // Be careful here as the the vxmlFriendlyFrom will be sent back to us again
      UtlString vxmlFriendlyFrom = m_from.toString();
      MailboxManager::convertUrlStringToXML( vxmlFriendlyFrom );
      // HttpMessage::escape( vxmlFriendlyFrom );

      dynamicVxml += 
         "<subdialog name=\"send_msg\" src=\"" + mediaserverUrl + "/vm_vxml/savemessage.vxml\">\n" +
         "<param name=\"called_by\" value=\"incoming\"/>\n" \
         "<param name=\"mailbox\" value=\"" + m_mailboxIdentity + "\"/>\n" \
         "<param name=\"from\" value=\"" + vxmlFriendlyFrom + "\"/>\n" \
         "<param name=\"msgurl\" expr=\"msgurl\" />\n" \
         "<param name=\"msgurltype\" expr=\"msgurltype\" />\n" \
         "<param name=\"mediaserverurl\" expr=\"'" + ivrPromptUrl + "'\"/>\n" \
         "<param name=\"securemediaserverurl\" expr=\"'" + secureMediaserverUrl + "'\"/>\n" \
         "</subdialog>";
   } else
   {
      dynamicVxml +=	"<subdialog src=\"" + mediaserverUrl + "/vm_vxml/error_handler.vxml\" >\n" \
         "<param name=\"errortype\" expr=\"'invalidextn'\" />\n" \
         "<param name=\"mediaserverurl\" expr=\"'" + ivrPromptUrl + "'\" />\n" \
         "<param name=\"securemediaserverurl\" expr=\"'" + secureMediaserverUrl + "'\"/>\n" \
         "</subdialog>\n";
   }

   dynamicVxml += "</form>" + UtlString( VXML_END );

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
