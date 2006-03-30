//
// Copyright (C) 2004-2006 SIPfoundry Inc.
// Licensed by SIPfoundry under the LGPL license.
//
// Copyright (C) 2004-2006 Pingtel Corp.  All rights reserved.
// Licensed to SIPfoundry under a Contributor Agreement.
//
// $$
///////////////////////////////////////////////////////////////////////////////

// SYSTEM INCLUDES
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// APPLICATION INCLUDES
#include "os/OsConfigDb.h"
#include "os/OsReadLock.h"
#include "os/OsWriteLock.h"
#include "os/OsStatus.h"
#include "os/OsUtil.h"
#include "os/OsFS.h"
#include "os/OsSysLog.h"
#include "os/OsConfigEncryption.h"
#include "utl/UtlSortedListIterator.h"
#include "utl/UtlSList.h"
#include "utl/UtlSListIterator.h"

// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
#define MAX_FILELINE_SIZE 256
#define DB_LINE_FORMAT "%s : %s\r\n"

// STATIC VARIABLE INITIALIZATIONS
static OsConfigEncryption *gEncryption = NULL;

/* //////////////////////////// PUBLIC //////////////////////////////////// */

/* ============================ CREATORS ================================== */

// Default Constructor
OsConfigDb::OsConfigDb()
    :  mRWMutex(OsRWMutex::Q_PRIORITY),
       mDb(),
       mCapitalizeName(FALSE)
{
}

// Destructor
OsConfigDb::~OsConfigDb()
{
    OsWriteLock lock(mRWMutex);    // take lock for writing
    mDb.destroyAll();
}

/* ============================ MANIPULATORS ============================== */

OsStatus OsConfigDb::loadFromFile(const char *filename)
{
   if (filename == NULL)
   {
       return OS_INVALID_ARGUMENT;
   }

   OsStatus retval = OS_UNSPECIFIED;
   OsWriteLock lock(mRWMutex);    // take lock for writing while the database
                                  //  is loaded

   setIdentityLabel(filename);

   OsConfigEncryption *encryption = getEncryption();
   if (encryption != NULL && encryption->isNormallyEncrypted(this))
   {
       retval = loadFromEncryptedFile(filename);
   }
   else
   {
       FILE *fp = fopen(filename,"rb");
       if (fp)
       {
           retval = loadFromUnencryptedFile(fp);
           fclose(fp);
       }
   }

   return retval;
}

void OsConfigDb::dump()
{
    for (unsigned int i = 0; i < mDb.entries(); i++)
    {
        DbEntry *e = (DbEntry *)mDb.at(i);
        osPrintf(DB_LINE_FORMAT, e->key.data(), e->value.data());
    }
}

// Buffer CANNOT be encrypted
OsStatus OsConfigDb::loadFromBuffer(const char *buff)
{
    if (buff == NULL)
        return OS_INVALID_ARGUMENT;

    OsWriteLock lock(mRWMutex);    // take lock for writing while the database
                                  //  is loaded

    return loadFromUnencryptedBuffer(buff);
}

OsStatus OsConfigDb::loadFromFile(FILE* fp)
{
    OsWriteLock lock(mRWMutex);     // take lock for reading while the database
                                   //  is stored

    // TODO: Support encrypted files loaded via FILE instance. Note however, this is
    // of little use because no file name is generally known and encryption/decryption
    // rules generally rely on this.  If callers know the filename, try calling
    // loadFromFile with filename
    return loadFromUnencryptedFile(fp);
}


   // Undo the previous hack
#  if defined(_VXWORKS)
#  undef OK
#  endif

void OsConfigDb::insertEntry(const char* fileLine)
{
        char* name;
        char* nameEnd;
        char* value;
        char* valueEnd;

     // Get rid of initial white space
     name = (char*)fileLine;
     value = (char*)fileLine + strlen(fileLine);
     valueEnd = value;
     if(name < value)
     {
        valueEnd--;
     }

     while(isspace(*name))
     {
        name++;
     }

     // Find the end of the name
     nameEnd = name;

     while(!isspace(*nameEnd) && *nameEnd != '\0')
     {
        nameEnd++;
     }

     *nameEnd = '\0';

     if(nameEnd < value)
     {
        value = nameEnd + 1;
     }

#if 0 /* [ */
     // Get rid of terminators
     if(*valueEnd == '\n' || *valueEnd == '\r')
     {
        *valueEnd = '\0';
        if(valueEnd > value)
        {
           valueEnd--;
           if(*valueEnd == '\n' || *valueEnd == '\r')
           {
              *valueEnd = '\0';
           }
        }
     }
#endif /* ] */

     // Get rid of separators at the begining
     while(isspace(*value) || *value == ':')
     {
        value++;
     }

     //if the value has any leading space characters, get rid of them
     while (isspace(*value))
        value++;

     // Get rid of separators and terminators at the end
     while(value <= valueEnd && isspace(*valueEnd))
     {
        *valueEnd = '\0';
        valueEnd--;
     }

     // Insert key/value if key is not null
     if (*name!='\0') {
                 if (mCapitalizeName)
                 {
                         UtlString capName(name);
                         capName.toUpper();
                         insertEntry(capName.data(), value);
                 }
                 else
                        insertEntry(name, value);
        // osPrintf("Read Name: \"%s\" Value: \"%s\"\n", name, value);
     }
}

void OsConfigDb::setCapitalizeName(UtlBoolean capitalizeName)
{
    mCapitalizeName = capitalizeName;
}


OsStatus OsConfigDb::storeToFile(const char *filename)
{
   if (filename == NULL)
   {
       return OS_INVALID_ARGUMENT;
   }

   OsStatus retval = OS_UNSPECIFIED;
   OsReadLock lock(mRWMutex);    // take lock for writing while the database
                                  //  is loaded

   setIdentityLabel(filename);
   OsConfigEncryption *encryption = getEncryption();
   if (encryption != NULL && encryption->isWriteEncryptedEnabled() &&
           encryption->isNormallyEncrypted(this))
   {
       retval = storeToEncryptedFile(filename);
   }
   else
   {
       FILE *fp = fopen(filename,"wb");
       if (!fp)
       {
           osPrintf("Could not open %s.  errno = %d\n",filename,errno);
           return retval;
       }
       retval = storeToFile(fp);

       fclose(fp);
   }

   return retval;
}


// Remove the key/value pair associated with rKey.
// Return OS_SUCCESS if the key was found in the database, return
// OS_NOT_FOUND otherwise.
OsStatus OsConfigDb::remove(const UtlString& rKey)
{
   OsWriteLock lock(mRWMutex);
   DbEntry    lookupPair(rKey);
   DbEntry*   pEntryToRemove;
   unsigned int i = mDb.index(&lookupPair);
   if (i == UTL_NOT_FOUND)
   {
      return OS_NOT_FOUND;
   }
   else
   {
      pEntryToRemove = (DbEntry *)mDb.at(i);
      mDb.removeAt(i);
      delete pEntryToRemove;

      return OS_SUCCESS;
   }
}

// Remove all the key/value pairs starting with the designated prefix
OsStatus OsConfigDb::removeByPrefix(const UtlString& rPrefix) 
{
   DbEntry* pEntry ;

   UtlSortedListIterator itor(mDb) ;
   while (pEntry = (DbEntry*) itor())
   {
       if (pEntry->key.length() >= rPrefix.length())
       {
           UtlString keyPrefix = pEntry->key ;
           keyPrefix.remove(rPrefix.length()) ;
           if (keyPrefix.compareTo(rPrefix, UtlString::ignoreCase) == 0)
           {
               remove(pEntry->key) ;
           }
       }      
   }
   
   return OS_SUCCESS ;
}

// Insert the key/value pair into the config database.
// If the database already contains an entry for this key, then replace it
// with the new key/value pair.
void OsConfigDb::set(const UtlString& rKey, const UtlString& rNewValue)
{
   OsWriteLock lock(mRWMutex);

   if (rKey.length() > 0) {
      insertEntry(rKey, rNewValue);
   }
}

// Insert the key/value pair into the config database If the
// database already contains an entry for this key, then set the
// value for the existing entry to iNewValue.
void OsConfigDb::set(const UtlString& rKey, const int iNewValue) 
{
    UtlString newValue ;

    // Convert to String
    char cTemp[64] ;
    sprintf(cTemp, "%d", iNewValue);

    // Set
    newValue = cTemp ;    
    set(rKey, newValue) ;
}

/* ============================ ACCESSORS ================================= */

void OsConfigDb::setIdentityLabel(const char *idLabel)
{
    if (idLabel != NULL)
    {
        mIdentityLabel = idLabel;
        //strncpy(mIdentityLabel, idLabel, sizeof(mIdentityLabel));
    }
    else
    {
        mIdentityLabel.resize(0);
    }
}

// filename, url, etc that would help identity the source of this config
// return NULL if no idenity was set
const char *OsConfigDb::getIdentityLabel()
{
    return mIdentityLabel.data();
}

// Encryption Rules for this instance.
// SUBCLASS NOTE:  no setter method, requires subclassing to override instance
// only rules
OsConfigEncryption *OsConfigDb::getEncryption()
{
    return getStaticEncryption();
}

// Set encryption Rules for all instances
void OsConfigDb::setStaticEncryption(OsConfigEncryption *encryption)
{
    gEncryption = encryption;
}

// Get encryption Rules for all instances
OsConfigEncryption *OsConfigDb::getStaticEncryption()
{
    return gEncryption;
}

// Sets rValue to the value in the database associated with rKey.
// If rKey is found in the database, returns OS_SUCCESS.  Otherwise,
// returns OS_NOT_FOUND and sets rValue to the empty string.
OsStatus OsConfigDb::get(const UtlString& rKey, UtlString& rValue)
{
   OsReadLock lock(mRWMutex);
   DbEntry   lookupPair(rKey);
   unsigned int        i;
   DbEntry*  pEntry;
   i = mDb.index(&lookupPair);
   if (i == UTL_NOT_FOUND)
   {
      rValue = "";     // entry not found
      return OS_NOT_FOUND;
   }
   else
   {
      pEntry = (DbEntry *)mDb.at(i);
      rValue = pEntry->value;
   }

   return OS_SUCCESS;
}

// Gets a db subset of name value pairs which have the given hashSubKey as
// a prefix for the key
OsStatus OsConfigDb::getSubHash(const UtlString& rHashSubKey, OsConfigDb& rSubDb)
{
   UtlString dummy;
   UtlString previousKey(rHashSubKey);
   UtlString key;
   UtlString value;
   UtlBoolean bAddedSubKey = false ;
   int keyOffset = strlen(rHashSubKey);
   int keyPrefixIndex;
#ifdef TEST
   osPrintf("Key prefix: \"%s\"\n", rHashSubKey.data());
#endif

   // Need to have the prefix as a key so that there is a next
   if(get(rHashSubKey, dummy) != OS_SUCCESS)
   {
       set(rHashSubKey, "");
       bAddedSubKey = true ;
   }

   while(getNext(previousKey, key, value) == OS_SUCCESS)
   {
      //osPrintf("key: \"%s\" value: \"%s\"\n",
      //         key.data(), value.data());
      keyPrefixIndex = key.index(rHashSubKey);
      previousKey = key;

      // The prefix should start at the first char
      if(keyPrefixIndex == 0)
      {
#ifdef TEST
         osPrintf("sub-key: \"%s\" value: \"%s\"\n",
                  &((key.data())[keyOffset]), value.data());
#endif
         rSubDb.insertEntry(&((key.data())[keyOffset]),
                      value.data());
      }
      else
      {
         break;
      }

      //osPrintf("Previous key: \"%s\"\n", previousKey.data());
   }
#ifdef TEST
   osPrintf("last key found: \"%s\" value: \"%s\"\n",
            key.data(), value.data());
#endif
   if (bAddedSubKey)
      remove(rHashSubKey);
   dummy.remove(0);
   previousKey.remove(0);
   key.remove(0);
   value.remove(0);
   return(OS_SUCCESS);
}

// Sets rValue to the value in the database associated with rKey.
// If rKey is found in the database, returns OS_SUCCESS.  Otherwise,
// returns OS_NOT_FOUND and sets rValue to -1.
OsStatus OsConfigDb::get(const UtlString& rKey, int& rValue)
{
   UtlString value;
   OsStatus returnStatus = get(rKey, value);

   if(returnStatus == OS_SUCCESS)
   {
      rValue = atoi(value.data());
   }
   else
   {
      rValue = -1;
   }

   value.remove(0);
   return(returnStatus);
}

// Relative to rKey, return the key and value associated with
// next (lexicographically ordered) key/value pair stored in the
// database.  If rKey is the empty string, key and value associated
// with the first entry in the database will be returned.
// Returns
//   OS_SUCCESS if there is a "next" entry
//   OS_NOT_FOUND if rKey is not found in the database and is not the
//      empty string
//   OS_NO_MORE_DATA if there is no "next" entry
OsStatus OsConfigDb::getNext(const UtlString& rKey,
                         UtlString& rNextKey, UtlString& rNextValue)
{
   OsReadLock lock(mRWMutex);
   UtlBoolean  foundMatch;
   int        nextIdx = 0;
   DbEntry   lookupPair(rKey);
   unsigned int        idx;
   DbEntry*  pEntry;

   foundMatch = FALSE;
   if (rKey.compareTo("") == 0)
   {
      foundMatch = TRUE;         // if the key is the empty string, then
      nextIdx = 0;               // return the first entry in the database
   }
   else
   {
      idx = mDb.index(&lookupPair);
      if (idx != UTL_NOT_FOUND)
      {
         foundMatch = TRUE;
         nextIdx = idx + 1;
      }
   }

   if (foundMatch && (nextIdx < numEntries()))
   {
      pEntry     = (DbEntry *)mDb.at(nextIdx);
      rNextKey   = pEntry->key;
      rNextValue = pEntry->value;

      return OS_SUCCESS;
   }

   rNextKey   = "";
   rNextValue = "";

   if (!foundMatch)
      return OS_NOT_FOUND;
   else
      return OS_NO_MORE_DATA;
}


// Stores a list of strings to the configuration datadase using the 
// designated prefix as the base for the list items. 
void OsConfigDb::addList(const UtlString& rPrefix,
                         UtlSList& rList) 
{
    // First remove all items start with the specified prefix
    removeByPrefix(rPrefix) ;
    int iNumEntries ;
    UtlString key ;
    UtlString* pValue ;


    // Next add all of the new items
    iNumEntries = rList.entries() ;
    if (iNumEntries > 0)
    {
        key = rPrefix ;
        key.append(".COUNT") ;
        set(key, iNumEntries) ;

        UtlSListIterator itor(rList) ;        
        int iCount = 1 ;
        char cTemp[64] ;
        while (pValue = (UtlString*) itor())
        {
            sprintf(cTemp, "%d", iCount++);   
            key = rPrefix ;
            key.append(".") ;
            key.append(cTemp) ;

            set(key, *pValue) ;            
        }
    }
}


// Loads a list of strings from the configuration datadase using the 
// designated prefix as the base for the list items.
int OsConfigDb::loadList(const UtlString& rPrefix,
                         UtlSList& rList) 
{
    OsReadLock lock(mRWMutex);
    int iNumEntries ;
    int rc = 0 ; 
    UtlString key ;
    UtlString value ;
    char cTemp[64] ;

    // Get number of items
    key = rPrefix ;
    key.append(".COUNT") ;    
    if (get(key, iNumEntries) == OS_SUCCESS)
    {
        for (int i = 0; i < iNumEntries; i++)
        {            
            sprintf(cTemp, "%d", i+1);
            key = rPrefix ;
            key.append(".") ;
            key.append(cTemp) ;

            if (get(key, value) == OS_SUCCESS)
            {
                rList.append(new UtlString(value)) ;
                rc++ ;
            }
        }        
    }

    return rc ;
}


// Get a port number from the configuration database
int OsConfigDb::getPort(const char* szKey)
{
    assert(szKey) ;

    UtlString value ;
    int port = PORT_NONE ;

    if (get(szKey, value) == OS_SUCCESS)
    {
        // If the value is null, leave port == PORT_NONE.
        if (value.length())
        {
           if (value.compareTo("DEFAULT", UtlString::ignoreCase) == 0)
           {
              port = PORT_DEFAULT;
           }
           else if (value.compareTo("NONE", UtlString::ignoreCase) == 0)
           {
              port = PORT_NONE;
           }
           else
           {
              port = atoi(value.data());
              if (!portIsValid(port))
              {
                 port = PORT_NONE;
                 OsSysLog::add(FAC_KERNEL, PRI_CRIT,
                               "Invalid port number value '%s' for config variable '%s'.",
                               value.data(), szKey);
              }
           }
        }
    }

    return port ;
}

// Delete all entries from the configuration database
void OsConfigDb::clear() 
{
   OsReadLock lock(mRWMutex);
   
    while (mDb.entries() > 0)   
    {
        DbEntry *e = (DbEntry *)mDb.at(0);
        mDb.removeAt(0) ;
        delete e ;
    }
}


/* ============================ INQUIRY =================================== */

// Return TRUE if the database is empty, otherwise FALSE.
UtlBoolean OsConfigDb::isEmpty(void)
{
   OsReadLock lock(mRWMutex);

   return (mDb.entries() == 0);
}

// Return the number of entries in the config database
int OsConfigDb::numEntries(void)
{
   OsReadLock lock(mRWMutex);

   return mDb.entries();
}

void OsConfigDb::storeToBuffer(char *buff)
{
    char *p = buff;
    int n = numEntries();
    for (int i = 0; i < n; i++)
    {
        DbEntry *pEntry = (DbEntry *)mDb.at(i);
        removeChars(&pEntry->key, '\r');
        removeChars(&pEntry->value, '\n');

        sprintf(p, DB_LINE_FORMAT, (char *)pEntry->key.data(),
                (char *)pEntry->value.data());

        p = buff + strlen(buff);
    }
}

// important that it be big enough, not so important it's exact
int OsConfigDb::calculateBufferSize()
{
    int n = numEntries();
    int size = n * strlen(DB_LINE_FORMAT);
    for (int i = 0; i < n; i++)
    {
        DbEntry *pEntry = (DbEntry *)mDb.at(i);
        size += pEntry->key.length() + pEntry->value.length();
    }
    return size;
}

void OsConfigDb::removeChars(UtlString *s, char c)
{
    size_t x = 0;
    while (x != UTL_NOT_FOUND)
    {
        x = s->first(c);
        if (x != UTL_NOT_FOUND)
        {
            s->remove(x, 1);
        }
    }
}

/* //////////////////////////// PROTECTED ///////////////////////////////// */

// write buffer to disk, replace existing file
OsStatus OsConfigDb::storeBufferToFile(const char *filename, const char *buff, unsigned long buffLen)
{
    OsStatus retval = OS_FAILED;

    if (buff != NULL && buffLen > 0)
    {
        UtlString filepath(filename);
        if (OsFileSystem::exists(filepath))
        {
            OsFileSystem::remove(filepath);

            OsFile file(filepath);
            file.open(OsFile::CREATE);
            unsigned long writtenLen;
            file.write(buff, buffLen, writtenLen);
            file.close();
            if (writtenLen == buffLen)
            {
                retval = OS_SUCCESS;
            }
        }
    }

    return retval;
}

OsStatus OsConfigDb::storeToEncryptedFile(const char *filename)
{
    OsStatus retval = OS_SUCCESS;

    // store to buffer
    int buffLen = calculateBufferSize();
    char *buff = new char[buffLen];
    storeToBuffer(buff);
    buffLen = strlen(buff);
    OsEncryption e;
    retval = getEncryption()->encrypt(this, &e, buff, buffLen);
    if (retval == OS_SUCCESS)
    {
        retval = storeBufferToFile(filename, (const char *)e.getResults(), e.getResultsLen());
    }

    return retval;
}

// Store the config database to a file
OsStatus OsConfigDb::storeToFile(FILE* fp)
{
   OsStatus retval = OS_SUCCESS;
   int        i;
   int        cnt;
   DbEntry*  pEntry;

   // step through the database writing out one entry per line
   // each entry is of the form "%s: %s\n"
   cnt = numEntries();
   for (i=0; i < cnt; i++)
   {
      pEntry = (DbEntry *)mDb.at(i);

      //remove any  \n or \r at the ends of the lines
      unsigned int remove_char_loc = 0;
      while (remove_char_loc != UTL_NOT_FOUND)
      {
      remove_char_loc = pEntry->key.first('\r');
      if (remove_char_loc != UTL_NOT_FOUND)
         pEntry->key.remove(remove_char_loc,1);
      }

      remove_char_loc = 0;
      while (remove_char_loc != UTL_NOT_FOUND)
      {
         remove_char_loc = pEntry->value.first('\n');
         if (remove_char_loc != UTL_NOT_FOUND)
            pEntry->value.remove(remove_char_loc,1);
      }


      fprintf(fp, "%s : %s\r\n",
              (char*) pEntry->key.data(),
              (char*) pEntry->value.data());
   }

   fflush(fp);
   // The following line probably should never have been here.
   // It was causing segfaults under Linux, and it should have
   // on VxWorks and Windows NT as well.
   //fclose(fp);
   return retval;
}


/* //////////////////////////// PRIVATE /////////////////////////////////// */

OsStatus OsConfigDb::loadFromEncryptedFile(const char *file)
{
    OsStatus retval = OS_UNSPECIFIED;
    if (file == NULL)
    {
        return retval;
    }

    OsFile osfile(file);
    retval = osfile.open(OsFile::READ_ONLY);
    if (retval == OS_SUCCESS)
    {
        unsigned long buffLen = 0;
        osfile.getLength(buffLen);
        char *buff = new char[buffLen + 1];
        memset(buff, 0, buffLen + 1);

        unsigned long bytesRead;
        retval = osfile.read(buff, buffLen, bytesRead);
        if (bytesRead != buffLen || retval != OS_SUCCESS)
        {
            OsSysLog::add(FAC_KERNEL, PRI_ERR, "Error reading config file or \
mismatch in expected size  %s\n", getIdentityLabel());
            retval = OS_FAILED;
        }
        else
        {
            retval = loadFromEncryptedBuffer(buff, buffLen);
        }

        delete [] buff;
        osfile.close();
    }

    return retval;
}

// Load the configuration database from a file
OsStatus OsConfigDb::loadFromUnencryptedFile(FILE* fp)
{
   //int  result;
   //char name[81];
   //char value[81];
   char fileLine[MAX_FILELINE_SIZE + 1];
   OsStatus retval = OS_SUCCESS;

   // The following #define is needed in order for the feof() macro to work
   // properly under VxWorks
#  if defined(_VXWORKS)
#  define OK VX_OK
#  endif

   // step through the file writing out one entry per line
   // each entry is of the form "%s: %s\n"
   while (!feof(fp))
   {
      //result = fscanf(fp, "%80s : %80s", name, value);
      //if (result == 2)
      if(fgets(fileLine, MAX_FILELINE_SIZE, fp))
      {
         insertEntry(fileLine);
      }
      else if(ferror(fp))
      {
         perror("OsConfigDb::loadFromFile read error");
         retval = OS_UNSPECIFIED;
         break;
      }
      //else if (result == EOF)
      //   break;                   // end of file, exit the loop
      //else
      //   assert(FALSE);           // unexpected return code
   }

   return retval;
}

OsStatus OsConfigDb::loadFromEncryptedBuffer(char *buf, int bufLen)
{
    OsStatus retval = OS_SUCCESS;

    if (getEncryption()->isEncrypted(this, (const char *)buf, bufLen))
    {
        OsEncryption e;
        getEncryption()->decrypt(this, &e, buf, bufLen);
        if (getEncryption()->isEncrypted(this, (const char *)e.getResults(), e.getResultsLen()))
        {
            // if decryption did not produce an unencrypted version
            // then probably bad password, or test for encryption is bad
            // to distinguish internal errors from bad password, use unauthorized
            retval = OS_UNAUTHORIZED;
        }
        else
        {
            // yes it's null terminated
            retval = loadFromUnencryptedBuffer((const char*)e.getResults());
        }
    }
    else if (retval == OS_SUCCESS)
    {
        retval = loadFromUnencryptedBuffer((const char*)buf);
    }

    return retval;
}

OsStatus OsConfigDb::loadFromUnencryptedBuffer(const char *buf)
{
   if (buf == NULL)
      return OS_INVALID_ARGUMENT;

   char configLine[MAX_FILELINE_SIZE + 1];
   OsStatus retval = OS_SUCCESS;
   UtlString config = UtlString(buf);

   // The following #define is needed in order for the feof() macro to work
   // properly under VxWorks
#  if defined(_VXWORKS)
#  define OK VX_OK
#  endif

   // step through the file writing out one entry per line
   // each entry is of the form "%s: %s\n"
   int start = 0;
   int size ;
   size_t pos;
   while (1)
   {
      pos = config.first('\n');
      if ((pos != UTL_NOT_FOUND) || (pos == UTL_NOT_FOUND && !config.isNull()))
      {
         if (pos == UTL_NOT_FOUND)
         {
            pos = config.length();
         }

         // Do not allow lines greater than MAX_FILELINE_SIZE
         size = pos;
         if (size > MAX_FILELINE_SIZE)
         {
#ifdef TEST
             osPrintf("Warning: max line length exceeded in config db file");
#endif //TEST
             size = MAX_FILELINE_SIZE;
         }

         strncpy(configLine, config.data(), size) ;
         configLine[size] = 0 ;

         start = pos + 1;
         int len = config.length();
         if (len > start)
            config = config(start, len - start);
         else
            config = OsUtil::NULL_OS_STRING;

         if (strlen(configLine) == 0) continue;

         insertEntry(configLine);
      }
      else
      {
         break;         // end of buffer reached.
      }
   }

   return retval;
}


// Helper method for inserting a key/value pair into the database.
// The write lock for the database should be taken before calling this
// method. If the database already contains an entry for this key, then
// set the value for the existing entry to rNewValue.
void OsConfigDb::insertEntry(const UtlString& rKey,
                              const UtlString& rNewValue)
{
   DbEntry  tempEntry(rKey, rNewValue);
   DbEntry* pOldEntry;
   unsigned int       i;
   i = mDb.index(&tempEntry);
   if (i != UTL_NOT_FOUND)
   {                             // we already have an entry with this key
          pOldEntry = (DbEntry *)mDb.at(i);        //  just change its value
      // osPrintf("OsConfigDb::iNVP(%X,%s) - FOUND %s replaces %s\n",
                  // this, rKey.data(), pOldEntry->value.data(), rNewValue.data());
      pOldEntry->value = rNewValue;
   }
   else
   {
      DbEntry* pNewEntry = new DbEntry(rKey, rNewValue);

      // osPrintf("OsConfigDb::iNVP(%X,%s) - ADDING (%s)\n",
                  // this, rKey.data(), rNewValue.data());
      mDb.insert(pNewEntry);
   }
}

//----------------DbEntry ------------------------

DbEntry::DbEntry(const UtlString &rKey, const UtlString &rValue)
{
    key = rKey;
    value = rValue;
}

DbEntry::DbEntry(const UtlString &rKey)
{
    key = rKey;
}

DbEntry::~DbEntry()
{
}

int DbEntry::compareTo(const UtlContainable *b) const
{
   return key.compareTo(((DbEntry *)b)->key);
}

unsigned int DbEntry::hash() const
{
    return key.hash();
}

static UtlContainableType DB_ENTRY_TYPE = "DbEntry";

const UtlContainableType DbEntry::getContainableType() const
{
    return DB_ENTRY_TYPE;
}

/* ============================ FUNCTIONS ================================= */
