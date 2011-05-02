/* -------------------------------------------------------------------------- */
/* Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             */
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

#include "Scheduler.h"
#include "RankPolicy.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <sstream>


using namespace std;

class RankScheduler : public Scheduler
{
public:

    RankScheduler(string       url,
                  time_t       timer,
                  unsigned int machines_limit,
                  unsigned int dispatch_limit,
                  unsigned int host_dispatch_limit
                  ):Scheduler(url,
                              timer,
                              machines_limit,
                              dispatch_limit,
                              host_dispatch_limit),rp(0){};

    ~RankScheduler()
    {
        if ( rp != 0 )
        {
            delete rp;
        }
    };

    void register_policies()
    {
        rp = new RankPolicy(vmpool,hpool,1.0);

        add_host_policy(rp);
    };

private:
    RankPolicy * rp;

};

int main(int argc, char **argv)
{
    RankScheduler * ss;
    time_t          timer= 30;
    unsigned int    port = 2633;
    unsigned int    machines_limit = 300;
    unsigned int    dispatch_limit = 30;
    unsigned int    host_dispatch_limit = 1;
    unsigned int    foreground = 0;
    char            opt;
    char            *pidfile_name = NULL;

    ofstream        pidfile;
    ostringstream   oss;

    while((opt = getopt(argc,argv,"fP:p:t:m:d:h:")) != -1)
    {
        switch(opt)
        {
            case 'f':
                foreground = 1;
                break;
            case 'P':
                pidfile_name = strdup(optarg);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                timer = atoi(optarg);
                break;
            case 'm':
                machines_limit = atoi(optarg);
                break;
            case 'd':
                dispatch_limit = atoi(optarg);
                break;
            case 'h':
                host_dispatch_limit = atoi(optarg);
                break;
            default:
                cerr << "usage: " << argv[0] << " [ -f ] [ -P pidfile ] [-p port] [-t timer] ";
                cerr << "[-m machines limit] [-d dispatch limit] [-h host_dispatch_limit]\n";
                exit(-1);
                break;
        }
    };

    /* ---------------------------------------------------------------------- */

    if (! foreground) {
        pid_t pid, sid;

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
            cerr << "fork() failed.\n";
            exit(1);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
            exit(0);
        }

        if (pidfile_name) {
            pid = getpid();
            pidfile.open(pidfile_name);

            if (! pidfile.is_open()) {
                cerr << "failed to write pid file.\n";
                exit(1);
            }
            pidfile << pid << "\n";
            pidfile.close();
        }
 
        /* Change the file mode mask */
        umask(0);
 
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
            /* Log the failure */
            cerr << "setsid() failed.\n";
            exit(1);
        }
 
        /* Change the current working directory */
        if ((chdir("/")) < 0) {
            /* Log the failure */
            cerr << "chdir() failed.\n";
            exit(1);
        }
 
        /* Close out the standard file descriptors */
        close(0);
        close(1);
        close(2);
    }

    /* ---------------------------------------------------------------------- */

    oss << "http://localhost:" << port << "/RPC2";

    ss = new RankScheduler(oss.str(),
                           timer,
                           machines_limit,
                           dispatch_limit,
                           host_dispatch_limit);

    try
    {
        ss->start();
    }
    catch (exception &e)
    {
        cout << e.what() << endl;

        return -1;
    }

    delete ss;

    return 0;
}
