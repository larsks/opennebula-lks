/* -------------------------------------------------------------------------- */
/* Copyright 2002-2008, Distributed Systems Architecture Group, Universidad   */
/* Complutense de Madrid (dsa-research.org)                                   */
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

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include <string>
#include <map>
#include <sstream>

using namespace std;

/**
 *  Attribute base class for name-value pairs. This class provides a generic
 *  interface to implement 
 */
class Attribute
{
public:

    Attribute(string& aname):attribute_name(aname)
    {
        transform (
            attribute_name.begin(),
            attribute_name.end(),
            attribute_name.begin(),
            (int(*)(int))toupper);
    };

    Attribute(const char * aname)
    {
        ostringstream name;
        
        name << uppercase << aname;
        attribute_name = name.str();
    };
    
    virtual ~Attribute(){};

    enum AttributeType
    {
        SIMPLE = 0,
        VECTOR = 1
    };
    
    /**
     *  Gets the name of the attribute.
     *    @return the attribute name
     */
    const string& name() const
    {
        return attribute_name;
    };

    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */
    virtual string * marshall() = 0;

    /**
     *  Builds a new attribute from a string.
     */
    virtual void unmarshall(string& sattr) = 0;
    
    /**
     *  Returns the attribute type
     */
    virtual AttributeType type() = 0;

private:

    /**
     *  The attribute name.
     */
    string attribute_name;
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 *  The SingleAttribute class represents a simple attribute in the form 
 *  NAME = VALUE. 
 */

class SingleAttribute : public Attribute
{
public:

    SingleAttribute(string& name):Attribute(name){};

    SingleAttribute(string& name, string& value):
        Attribute(name),attribute_value(value){};
    
    SingleAttribute(const char * name, string& value):
        Attribute(name),attribute_value(value){};

    ~SingleAttribute(){};
    
    /**
     *  Returns the attribute value, a string.
     */
    const string& value() const
    {
        return attribute_value;
    };
    
    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function.
     *    @return a string (allocated in the heap) holding the attribute value.
     */    
    string * marshall()
    {
        string * rs = new string;
        
        *rs = attribute_value;
        
        return rs;   
    };

    /**
     *  Builds a new attribute from a string.
     */    
    void unmarshall(string& sattr)
    {
        attribute_value = sattr;
    };
    
    /**
     *  Returns the attribute type
     */    
    AttributeType type()
    {
        return SIMPLE;
    };
    
private:

    string attribute_value;    
};

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/**
 *  The VectorAttribute class represents an array attribute in the form 
 *  NAME = [ VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N]. 
 */

class VectorAttribute : public Attribute
{
public:

    VectorAttribute(string& name):Attribute(name){};

    VectorAttribute(string& name, map<string,string>& value):
            Attribute(name),attribute_value(value){};

    ~VectorAttribute(){};
    
    /**
     *  Returns the attribute value, a string.
     */
    const map<string,string>& value() const
    {
        return attribute_value;
    };
    
    /**
     *
     */
    string vector_value(const char *name) const;
    
    /**
     *  Marshall the attribute in a single string. The string MUST be freed
     *  by the calling function. The string is in the form: 
     *  "VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N".
     *    @return a string (allocated in the heap) holding the attribute value.
     */    
    string * marshall();

    /**
     *  Builds a new attribute from a string of the form:
     *  "VAL_NAME_1=VAL_VALUE_1,...,VAL_NAME_N=VAL_VALUE_N".
     */        
    void unmarshall(string& sattr);

    /**
     *  Returns the attribute type
     */    
    AttributeType type()
    {
        return VECTOR;
    };

private:

    map<string,string> attribute_value;    
};

#endif /*ATTRIBUTE_H_*/