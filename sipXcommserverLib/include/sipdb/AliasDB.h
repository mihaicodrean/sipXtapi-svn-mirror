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
#ifndef ALIASDB_H
#define ALIASDB_H

// SYSTEM INCLUDES


// APPLICATION INCLUDES
#include "os/OsMutex.h"

// DEFINES
// MACROS
// EXTERNAL FUNCTIONS
// EXTERNAL VARIABLES
// CONSTANTS
// STRUCTS
// TYPEDEFS
// FORWARD DECLARATIONS
class Url;
class dbDatabase;
class dbFieldDescriptor;
class UtlHashMap;
class TiXmlNode;
class ResultSet;

/**
 * This class implements the Alias database abstract class
 *
 * @author John P. Coffey
 * @version 1.0
 */
class AliasDB
{
public:
    /**
     * Singleton Accessor
     *
     * @return
     */
    static AliasDB* getInstance( const UtlString& name = "alias" );

    /// releaseInstance - cleans up the singleton (for use at exit)
    static void releaseInstance();

    // Domain Serialization/Deserialization
    OsStatus store();

    // Insert or update a row in the Aliases database.
    // Note: since because aliasIdentities can have multiple
    // contacts, we need to pass an update hint default false
    // parameter to the insert
    UtlBoolean insertRow (
        const Url& aliasIdentity,
        const Url& contact,
        bool updateContact = FALSE);

    // Delete methods
    UtlBoolean removeRow (const Url& aliasIdentity );

    // Flushes the entire DB
    void removeAllRows ();

    // utility method for dumping all rows
    void getAllRows(ResultSet& rResultSet) const;

    // Query interface to return a set of mapped full URI
    // contacts associated with the alias
    void getContacts (
        const Url& aliasIdentity,
        ResultSet& rResultSet) const;

    // Query interface to return aliases associated with a sipIdentity
    void getAliases (
        const Url& contactIdentity,
        ResultSet& rResultSet ) const;

protected:
    // implicit loader
    OsStatus load();

    // Singleton Constructor is private
    AliasDB ( const UtlString& name );

    // One step closer to common load/store code
    UtlBoolean insertRow ( const UtlHashMap& nvPairs );

    // There is only one singleton in this design
    static AliasDB* spInstance;

    // Singleton and Serialization mutex
    static OsMutex sLockMutex;

    // ResultSet column Keys
    static UtlString gIdentityKey;
    static UtlString gContactKey;
    // @JC Remove this after beta sites upgrade
    static UtlString gLegacyIdentityKey;

    // Fast DB instance
    dbDatabase* m_pFastDB;

    // the persistent filename for loading/saving
    UtlString mDatabaseName;

private:
    /**
     * Virtual Destructor
     */
    virtual ~AliasDB();

};

#endif //ALIASDB_H

