# -------------------------------------------------------------------------- #
# Copyright 2002-2009, Distributed Systems Architecture Group, Universidad   #
# Complutense de Madrid (dsa-research.org)                                   #
#                                                                            #
# Licensed under the Apache License, Version 2.0 (the "License"); you may    #
# not use this file except in compliance with the License. You may obtain    #
# a copy of the License at                                                   #
#                                                                            #
# http://www.apache.org/licenses/LICENSE-2.0                                 #
#                                                                            #
# Unless required by applicable law or agreed to in writing, software        #
# distributed under the License is distributed on an "AS IS" BASIS,          #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   #
# See the License for the specific language governing permissions and        #
# limitations under the License.                                             #
#--------------------------------------------------------------------------- #

require 'OpenNebula'


include OpenNebula

class ImagePoolOCCI
    OCCI_IMAGE_POOL = %q{
        <STORAGE><% 
            for image in @images do %>
            <DISK href="<%= base_url%>/storage/<%= image[:id] %>"\><%  
            end  %>
        </STORAGE>
    }.gsub(/^        /, '')
        
    def initialize(user_id)
        @images=Image.filter(:owner => user_id)     
    end 
    
    def to_occi(base_url)
        occi = ERB.new(OCCI_IMAGE_POOL)
        return occi.result(binding)       
    end
end

