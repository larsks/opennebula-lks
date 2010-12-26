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

#include "HostHook.h"
#include "Host.h"
#include "Nebula.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void HostAllocateHook::do_hook(void *arg)
{
    Host *  host;

    string  parsed_args = args;
    size_t  found;

    host = static_cast<Host *>(arg);

    if ( host == 0 )
    {
        return;
    }

    found = args.find("$HID");

    if ( found !=string::npos )
    {
        ostringstream oss;
        oss << host->get_oid();

        parsed_args.replace(found,4,oss.str());
    }

    Nebula& ne                    = Nebula::instance();
    HookManager * hm              = ne.get_hm();
    const HookManagerDriver * hmd = hm->get();

    if ( hmd != 0 )
    {
        if ( remote == true )
        {
            hmd->execute(host->get_oid(),
                         name,
                         host->get_hostname(),
                         cmd,
                         parsed_args);
        }
        else
        {
            hmd->execute(host->get_oid(),name,cmd,parsed_args);
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

map<int,Host::HostState> HostStateMapHook::host_states;

// -----------------------------------------------------------------------------

int HostStateMapHook::get_state(int id, Host::HostState &state)
{
    map<int,Host::HostState>::iterator it;

    it = host_states.find(id);

    if ( it == host_states.end() )
    {
        return -1;
    }

    state = it->second;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void HostStateMapHook::update_state (int id, Host::HostState state)
{
    map<int,Host::HostState>::iterator it;

    it = host_states.find(id);

    if ( it == host_states.end() )
    {
        host_states.insert(make_pair(id,state));
    }
    else
    {
        it->second = state;
    }
}

// -----------------------------------------------------------------------------

void HostStateMapHook::remove_host (int id)
{
    map<int,Host::HostState>::iterator it;

    it = host_states.find(id);

    if ( it != host_states.end() )
    {
        host_states.erase(it);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void HostStateHook::do_hook(void *arg)
{
    Host * host;
    int    rc;

    Host::HostState prev_state, cur_state;

    host = static_cast<Host *>(arg);

    if ( host == 0 )
    {
        return;
    }

    rc = get_state(host->get_oid(), prev_state);

    if ( rc != 0 )
    {
        return;
    }

    cur_state = host->get_state();

    if ( prev_state == cur_state ) //Still in the same state
    {
        return;
    }

    if ( cur_state == this->state )
    {
        string  parsed_args = args;
        size_t  found;

        found = args.find("$HID");

        if ( found !=string::npos )
        {
            ostringstream oss;
            oss << host->get_oid();

            parsed_args.replace(found,4,oss.str());
        }

        Nebula& ne        = Nebula::instance();
        HookManager * hm  = ne.get_hm();

        const HookManagerDriver * hmd = hm->get();

        if ( hmd != 0 )
        {
            if ( remote == true)
            {
                hmd->execute(host->get_oid(),
                             name,
                             host->get_hostname(),
                             cmd,
                             parsed_args);
            }
            else
            {
                hmd->execute(host->get_oid(),name,cmd,parsed_args);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void HostUpdateStateHook::do_hook(void *arg)
{
    Host * host = static_cast<Host *>(arg);

    if ( host == 0 )
    {
        return;
    }

    update_state(host->get_oid(), host->get_state());
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
