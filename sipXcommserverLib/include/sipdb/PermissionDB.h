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
#ifndef PERMISSIONDB_H
#define PERMISSIONDB_H

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
class ResultSet;
class TiXmlNode;

/**
 * This class implements the Alias database abstract class
 *
 * @author John P. Coffey
 * @version 1.0
 */
class PermissionDB
{
public:
    /**
     * Singleton Accessor
     *
     * @return
     */
    static PermissionDB* getInstance( const UtlString& name = "permission" );

    /// releaseInstance - cleans up the singleton (for use at exit)
    static void releaseInstance();

    // Domain Serialization/Deserialization
    OsStatus store();

    //set methods// values can be MAPPED or REGISTER
    UtlBoolean insertRow (
        const Url& uri,
        const UtlString& permission );

    // Query interface always used cursor now
    void getPermissions (
        const Url& identity,
        ResultSet& rResultset ) const;

    // Query the identities associated with a particular permission
    void getIdentities (
        const UtlString& permission,
        ResultSet& rResultset ) const;

    // Query top see if a this identity has the permission set
    UtlBoolean hasPermission (
        const Url& identity,
        const UtlString& permission ) const;

    // Delete methods
    UtlBoolean removeRow(
        const Url& identity,
        const UtlString& permission );

    void removeRows ( const Url& identity );

    // Flushes the entire DB
    void removeAllRows ();

    // utility method for dumping all rows
    void getAllRows ( ResultSet& rResultset) const;

protected:
    // implicit loader
    OsStatus load();

    // Singleton Constructor is private
    PermissionDB ( const UtlString& name );

    // One step closer to common load/store code
    UtlBoolean insertRow ( const UtlHashMap& nvPairs );

    // There is only one singleton in this design
    static PermissionDB* spInstance;

    // Singleton and Serialization mutex
    static OsMutex sLockMutex;

    // ResultSet column Keys
    static UtlString gIdentityKey;
    static UtlString gPermissionKey;

    // Fast DB instance
    dbDatabase* m_pFastDB;

    // the persistent filename for loading/saving
    UtlString mDatabaseName;

private:
    /**
     * Virtual Destructor
     */
    virtual ~PermissionDB();

};

#endif //PERMISSIONDB_H

