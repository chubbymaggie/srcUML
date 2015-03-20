/**
 * @file srcYUMLHandler.hpp
 *
 * @copyright Copyright (C) 2015 srcML, LLC. (www.srcML.org)
 *
 * srcYUML is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcYUML is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCLUDED_SRCYUML_HANDLER_HPP
#define INCLUDED_SRCYUML_HANDLER_HPP

#include <srcSAXHandler.hpp>
#include <map>
#include <string>
#include <list>
#include <iostream>
#include <vector>

struct AttributeDeclaration {
    
    std::string type;
    std::string name;
    
    AttributeDeclaration(std::string t, std::string n) : type(t), name(n) {};
};

class srcYUMLClass {
public:
    // attributes
    // functions
    // inheritance list
    // Inside a class
    // Inside public
    // inside private
    // inside protected
    // inside inheritance list
    // public, private, protected

    
    // Keys can be public, private, or protected
    std::map<std::string, std::list<struct AttributeDeclaration> > class_data_members;
    std::map<std::string, std::list<std::string> > class_functions;
    
    srcYUMLClass() {}
    
    void printData() const {
        for(const auto& itr : class_data_members) {
            std::cout << "\t" << itr.first << ":" << "\n";
            for(const auto& inner_itr : itr.second) {
                std::cout << "\t\t" << inner_itr.type << " " << inner_itr.name << "\n";
            }
        }
        
        for(const auto& itr : class_functions) {
            std::cout << "\t" << itr.first << ":" << "\n";
            for(const auto& inner_itr : itr.second) {
                std::cout << "\t\t" << inner_itr << "\n";
            }
        }
    }
    
private:

    

    
};


/**
 * srcYUMLHandler
 *
 * Base class that provides hooks for SAX processing.
 */
class srcYUMLHandler : public srcSAXHandler {

private:
    
    // Map representing <className, dataInsideClass>
    std::map<std::string, srcYUMLClass> classes_in_source;
    
    // bool variables to determine program states
    bool consuming_class,
         consuming_data_member,
         consuming_function,
         data_member_type_consumed,
         in_public,
         in_private,
         in_protected,
         in_inheritance_list,
         class_name_consumed;
    
    int multiple_class_in_class_count;
    
    
    // This could be a function or an attribute
    std::string current_recorded_data_in_class;
    std::string current_data_member_type;
    
    /*
     This is to be used to store class name until we hit the end tag
     so that we know what class key to map the data to.
     */
    std::string current_class;
    
    /* 
       This holds what visibility layer we are in for the class
       so that we can properly map where we got data from in our class
     */
    std::string current_class_visibility;
    

    
protected:

public:
    
    void printClassesInSource() const {
        for(const auto& itr : classes_in_source) {
            std::cout << itr.first << "\n";
            itr.second.printData();
        }
    }
    /**
     * srcYUMLHandler
     *
     * Default constructor default values to everything
     */
    srcYUMLHandler() :  consuming_class(false),
                        consuming_data_member(false),
                        consuming_function(false),
                        data_member_type_consumed(false),
                        in_public(false),
                        in_private(false),
                        in_protected(false),
                        in_inheritance_list(false),
                        class_name_consumed(false),
                        multiple_class_in_class_count(0) {};


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

    
    /**
     * startDocument
     *
     * SAX handler function for start of document.
     * Overide for desired behaviour.
     */
    virtual void startDocument() {}

    /**
     * endDocument
     *
     * SAX handler function for end of document.
     * Overide for desired behaviour.
     */
    virtual void endDocument() {}

    /**
     * startRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of the root element.
     * Overide for desired behaviour.
     */
    virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {}

    /**
     * startUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of an unit.
     * Overide for desired behaviour.
     */
    virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {}

    /**
     * startElement
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of an element.
     * Overide for desired behaviour.
     */
    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes) {
        std::string lname = localname;
        // We have started reading a class
        if(lname == "class") {
            consuming_class = true;
        }
        else if(consuming_class && lname == "name" && srcml_element_stack[srcml_element_stack.size() - 2] == "class") {
            current_recorded_data_in_class = "";
        }
        // We are now in the public part of the class
        else if(lname == "public" && consuming_class) {
            in_public = true;
            current_class_visibility = "public";
        }
        // We are now in the private part of the class
        else if(lname == "private" && consuming_class) {
            in_private = true;
            current_class_visibility = "private";
        }
        // We are now in the protected part of the class
        else if(lname == "protected" && consuming_class) {
            in_protected = true;
            current_class_visibility = "protected";
        }
        // We are at a class attribute
        else if(lname == "decl_stmt" && consuming_class) {
            consuming_data_member = true;
            current_recorded_data_in_class = "";
        }
        // We are at one of the classes functions
        else if(lname == "function" && consuming_class) {
            consuming_function = true;
            current_recorded_data_in_class = "";
        }
        // if we hit the block of a function and its parent is a function we have consumed all needed data to record
        else if(lname == "block" && consuming_function && srcml_element_stack[srcml_element_stack.size() - 2] == "function") {
            classes_in_source[current_class].class_functions[current_class_visibility].push_back(current_recorded_data_in_class);
            current_recorded_data_in_class = "";
            consuming_function = false;
        }
    }

    /**
     * endRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of the root element.
     * Overide for desired behaviour.
     */
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {}

    /**
     * endUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an unit.
     * Overide for desired behaviour.
     */
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {}

    /**
     * endElement
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an element.
     * Overide for desired behaviour.
     */
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {
        std::string lname = localname;
        // If we hit the </name> tag and the parent is the Class tag we have consumed the class' name
        if(consuming_class && lname == "name" && srcml_element_stack[srcml_element_stack.size() - 1] == "class" && !class_name_consumed) {
            current_class = current_recorded_data_in_class;
            classes_in_source[current_class];
            class_name_consumed = true;
        }
        // If we hit this we have consumed the WHOLE type
        else if(consuming_class && consuming_data_member && lname == "type" && !data_member_type_consumed) {
            current_data_member_type = current_recorded_data_in_class;
            current_recorded_data_in_class = "";
            data_member_type_consumed = true;
        }
        // We hit an </decl_stmt> tag in the class so we now have all of the declaration information
        else if(consuming_class && lname == "decl_stmt" && consuming_data_member && data_member_type_consumed) {
            struct AttributeDeclaration temp(current_data_member_type, current_recorded_data_in_class);
            
            classes_in_source[current_class].class_data_members[current_class_visibility].push_back(temp);
            // attribute has been fully consumed
        
            consuming_data_member = false;
            data_member_type_consumed = false;
            
        }
        // We are no longer in public visibility
        else if(consuming_class && lname == "public") {
            in_public = false;
        }
        // We are no longer in private visibility
        else if(consuming_class && lname == "private") {
            in_private = false;
        }
        // We are no longer in protected visibility
        else if(consuming_class && lname == "protected") {
            in_protected = false;
        }
        else if(consuming_class && lname == "class") {
            consuming_class = false;
            class_name_consumed = false;
        }
        else {
            // Do nothing
        }
    }
    
    /**
     * charactersRoot
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling at the root level.
     * Overide for desired behaviour.
     */
    virtual void charactersRoot(const char * ch, int len) {}

    /**
     * charactersUnit
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling within a unit.
     * Overide for desired behaviour.
     */
    virtual void charactersUnit(const char * ch, int len) {
        std::string text_parsed(ch, len);
        if(consuming_class) {
            if(text_parsed == ";gt") {
                text_parsed = ">";
            }
            else if(text_parsed == ";lt") {
                text_parsed = "<";
            }
            else {
                
            }
            current_recorded_data_in_class.append(text_parsed);
        }
        
    }

    /**
     * metaTag
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes\
     *
     * SAX handler function for a meta tags.
     * Overide for desired behaviour.
     */
    virtual void metaTag(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {}

    /**
     * comment
     * @param value the comment content
     *
     * A comment has been parsed.
     * Overide for desired behaviour.
     */
    virtual void comment(const char * value) {}

    /**
     * cdataBlock
     * @param value the pcdata content
     * @param len the block length
     *
     * Called when a pcdata block has been parsed.
     * Overide for desired behaviour.
     */
    virtual void cdataBlock(const char * value, int len) {}

    /**
     * processingInstruction
     * @param target the processing instruction target.
     * @param data the processing instruction data.
     *
     * Called when a processing instruction has been parsed.
     * Overide for desired behaviour.
     */
    virtual void processingInstruction(const char * target, const char * data) {}

#pragma GCC diagnostic pop

};

#endif