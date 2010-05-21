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

/* ************************************************************************** */
/* Image Pool                                                                 */
/* ************************************************************************** */

#include "ImagePool.h"

int ImagePool::allocate (
        int            uid,
        const  string& stemplate,
        int *          oid)
{
        Image * img;
        string  name;
        string  type;
        string  original_path;
        string  target;
        string  bus;

        char *  error_msg;
        int     rc;

        // ---------------------------------------------------------------------
        // Build a new Image object
        // ---------------------------------------------------------------------
        img = new Image(uid);

        // ---------------------------------------------------------------------
        // Parse template 
        // ---------------------------------------------------------------------
        rc = img->image_template.parse(stemplate, &error_msg);

        if ( rc != 0 )
        {
            goto error_parse;
        }
        
        // ---------------------------------------------------------------------
        // Check default image attributes
        // ---------------------------------------------------------------------               
        img->get_template_attribute("NAME", name);

        if ( name.empty() == true )
        {
            goto error_name;
        }
            
        img->get_template_attribute("TYPE", type);

        if ( type.empty() == true )
        {
            goto error_type;
        }        
        
        img->get_template_attribute("ORIGINAL_PATH", original_path);
        
        if  ( type == "OS" || type == "CDROM" )
        {
            if ( original_path.empty() == true )
            {
                goto error_original_path;
            }
        }
        
        img->get_template_attribute("TARGET", target);

        if ( target.empty() == true )
        {
            target = "hda"; // TODO add to oned.configuration
        }
        
        img->get_template_attribute("BUS", bus);

        if ( bus.empty() == true )
        {
            bus = "IDE"; // TODO add to oned.configuration
        }
        
        
        
        // generatesource
        
        img->name   = name;
        img->type   = type;
        img->target = target;
        img->bus    = bus;
        

        // ---------------------------------------------------------------------
        // Insert the Object in the pool
        // ---------------------------------------------------------------------

        *oid = PoolSQL::allocate(img);

        if ( *oid == -1 )
        {
            return -1;
        }

        // Add the image name to the map of image_names
        image_names.insert(make_pair(name, *oid));

        return *oid;

error_name:
    NebulaLog::log("IMG", Log::ERROR, "NAME not present in image template");
    goto error_common;
error_type:
    NebulaLog::log("IMG", Log::ERROR, "TYPE not present in image template");
    goto error_common;
error_original_path:
    NebulaLog::log("IMG", Log::ERROR, 
    "ORIGINAL_PATH compulsory and not present in image template of this type.");
    goto error_common;
error_common:
    delete img;

    *oid = -1;
    return -1;
error_parse:
    ostringstream oss;

    oss << "ImagePool template parse error: " << error_msg;
    NebulaLog::log("IMG", Log::ERROR, oss);
    free(error_msg);

    delete img;

    *oid = -2;
    return -2;    
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int ImagePool::dump_cb(void * _oss, int num, char **values, char **names)
{
    ostringstream * oss;

    oss = static_cast<ostringstream *>(_oss);

    return Image::dump(*oss, num, values, names);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

int ImagePool::dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;

    oss << "<IMAGE_POOL>";

    set_callback(static_cast<Callbackable::Callback>(&ImagePool::dump_cb),
                  static_cast<void *>(&oss));

    cmd << "SELECT * FROM " << Image::table;

    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }

    rc = db->exec(cmd, this);

    oss << "</IMAGE_POOL>";

    return rc;
}
