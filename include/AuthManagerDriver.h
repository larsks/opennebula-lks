/* -------------------------------------------------------------------------- */
/* Copyright 2002-2010, OpenNebula Project Leads (OpenNebula.org)             */
/*                                                                            */
/* Licensed under the Apache License, Version 2.0 (the "License"); you may    */
/* not use this file except in compliance with the License. You may obtain    */
/* a copy of the License at                                                   */
/*                                                                            */
/* http://www.apache.org/licenses/LICENSE-2.0                                 */
/*                                                                            */
/* Unless required by applicable law or agreed to in writing, software        */
/* distributed under the License is distributed on an "AS IS" BASIS,          */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   */
/* See the License for the specific language governing permissions and        */
/* limitations under the License.                                             */
/* -------------------------------------------------------------------------- */

#ifndef AUTH_MANAGER_DRIVER_H_
#define AUTH_MANAGER_DRIVER_H_

#include <map>
#include <string>
#include <sstream>

#include "Mad.h"

using namespace std;

//Forward definition of the AuthManager Class
class AuthManager;

/**
 *  AuthManagerDriver provides a base class to implement TM
 *  Drivers. This class implements the protocol and recover functions
 *  from the Mad interface.
 */
class AuthManagerDriver : public Mad
{
public:

    AuthManagerDriver(
        int                         userid,
        const map<string,string>&   attrs,
        bool                        sudo,
        AuthManager *               _authm):
            Mad(userid,attrs,sudo), authm(_authm){};

    virtual ~AuthManagerDriver(){};

    /**
     *  Implements the VM Manager driver protocol.
     *    @param message the string read from the driver
     */
    void protocol(
        string&     message);

    /**
     *  Re-starts the driver
     */
    void recover();

private:
    friend class AuthManager;

    /**
     *  The AuthManager to notify results.
     */
    AuthManager * authm;

    /**
     *  Sends an authorization request to the MAD:
     *    "AUTHORIZE  OPERATION_ID USER_ID REQUEST1 REQUEST2..."
     *    @param oid an id to identify the request.
     *    @param uid the user id.
     *    @param requests space separated list of requests in the form OP:OBJ:ID
     */
    void authorize(int oid, int uid, const string& requests) const;

    /**
     *  Sends an authorization request to the MAD:
     *    "AUTHENTICATE  OPERATION_ID USER_ID CHALLENGE"
     *    @param oid an id to identify the request.
     *    @param uid the user id.
     *    @param challenge to authenticate the user
     */
    void authenticate(int oid, int uid, const string& ch) const;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif /*AUTH_MANAGER_DRIVER_H_*/

