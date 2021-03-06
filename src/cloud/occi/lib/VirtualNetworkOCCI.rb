# -------------------------------------------------------------------------- #
# Copyright 2002-2011, OpenNebula Project Leads (OpenNebula.org)             #
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

class VirtualNetworkOCCI < VirtualNetwork
    OCCI_NETWORK = %q{
        <NETWORK href="<%= base_url %>/network/<%= self.id.to_s  %>">
            <ID><%= self.id.to_s %></ID>
            <NAME><%= self.name %></NAME>
            <% if self['TEMPLATE/DESCRIPTION'] != nil %>
            <DESCRIPTION><%= self['TEMPLATE/DESCRIPTION'] %></DESCRIPTION>
            <% end %>
            <ADDRESS><%= self['TEMPLATE/NETWORK_ADDRESS'] %></ADDRESS>
            <% if self['TEMPLATE/NETWORK_SIZE'] %>
            <SIZE><%= self['TEMPLATE/NETWORK_SIZE'] %></SIZE>
            <% end %>
            <PUBLIC><%= self['PUBLIC'] == "0" ? "NO" : "YES"%></PUBLIC>
        </NETWORK>
    }

    ONE_NETWORK = %q{
        NAME            = "<%= @vnet_info['NAME'] %>"
        TYPE            = RANGED
        <% if @vnet_info['DESCRIPTION'] != nil %>
        DESCRIPTION     = "<%= @vnet_info['DESCRIPTION'] %>"
        <% end %>
        <% if @vnet_info['PUBLIC'] != nil %>
        PUBLIC     = "<%= @vnet_info['PUBLIC'] %>"
        <% end %>
        <% if @bridge %>
        BRIDGE          = <%= @bridge %>
        <% end %>
        NETWORK_ADDRESS = <%= @vnet_info['ADDRESS'] %>
        NETWORK_SIZE    = <%= @vnet_info['SIZE']%>
    }.gsub(/^        /, '')

    # Class constructor
    def initialize(xml, client, xml_info=nil, bridge=nil)
        super(xml, client)
        @bridge    = bridge
        @vnet_info = nil

        if xml_info != nil
            xmldoc     = XMLElement.build_xml(xml_info, 'NETWORK')
            @vnet_info = XMLElement.new(xmldoc) if xmldoc != nil
        end
    end

    # Creates the OCCI representation of a Virtual Network
    def to_occi(base_url)
        begin
            occi = ERB.new(OCCI_NETWORK)
            occi_text = occi.result(binding)
        rescue Exception => e
            error = OpenNebula::Error.new(e.message)
            return error
        end

        return occi_text.gsub(/\n\s*/,'')
    end

    def to_one_template()
        if @vnet_info == nil
            error_msg = "Missing NETWORK section in the XML body"
            error = OpenNebula::Error.new(error_msg)
            return error
        end

        one = ERB.new(ONE_NETWORK)
        return one.result(binding)
    end
end
