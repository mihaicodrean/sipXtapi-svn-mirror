//
// Copyright (C) 2004 SIPfoundry Inc.
// License by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004 Pingtel Corp.
// Licensed to SIPfoundry under a Contributor Agreement.
//
//////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES


// APPLICATION INCLUDES
#include "utl/UtlContainable.h"
#include "utl/UtlSortedList.h"
#include "utl/UtlSortedListIterator.h"
#include "os/OsLock.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STATIC VARIABLE INITIALIZATIONS

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

UtlSortedListIterator::UtlSortedListIterator(const UtlSortedList& list) 
   : UtlListIterator(list)
{
}


/* ============================ MANIPULATORS ============================== */

/**
 * Find the next object that isEqualTo objectToFind, and reset the iterator
 * so that it is the current position.
 * 
 * @return The found element or NULL if no more elements are available.
 */
UtlContainable* UtlSortedListIterator::findNext(const UtlContainable* objectToFind)
{
   UtlContainable* nextMatch = NULL;
   
   UtlContainer::acquireIteratorConnectionLock();
   OsLock take(mContainerRefLock);
   UtlSortedList* myList = dynamic_cast<UtlSortedList*>(mpMyContainer);
   if (myList)
   {
      OsLock container(myList->mContainerLock);
      UtlContainer::releaseIteratorConnectionLock();

      if (mpCurrentNode != UtlListIterator::OFF_LIST_END)
      {
         GList *start = (mpCurrentNode == NULL ? g_list_first(myList->mpList) : 
                         mpCurrentNode);

         GList* nextNode = myList->findNode(start, UtlSortedList::EXACTLY, objectToFind);
      
         if (nextNode)
         {
            nextMatch = (UtlContainable*)nextNode->data;
            mpCurrentNode = nextNode;
         }
      }

      if (!nextMatch) // find a match?
      {
         // no - set current position so that next will return NULL
         mpCurrentNode = g_list_last(myList->mpList);
      }
   }
   else
   {
      UtlContainer::releaseIteratorConnectionLock();
   }   

   return(nextMatch);
}


/* ============================ ACCESSORS ================================= */

/* ============================ INQUIRY =================================== */

/* //////////////////////////// PROTECTED ///////////////////////////////// */

/* //////////////////////////// PRIVATE /////////////////////////////////// */

/* ============================ FUNCTIONS ================================= */


