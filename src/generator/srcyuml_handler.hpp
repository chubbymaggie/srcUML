/**
 * @file srcyuml_handler.hpp
 *
 * @copyright Copyright (C) 2016 srcML, LLC. (www.srcML.org)
 *
 * This file is part of srcYUML.
 *
 * srcYUML is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * srcYUML is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with srcYUML.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_SRCYUML_HANDLER_HPP
#define INCLUDED_SRCYUML_HANDLER_HPP

#include <srcSAXEventDispatchUtilities.hpp>
#include <srcSAXController.hpp>

#include <srcyuml_dispatcher.hpp>
#include <ClassPolicySingleEvent.hpp>

#include <srcyuml_class.hpp>
#include <srcyuml_relationship.hpp>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <map>

/**
 * srcyuml_handler
 *
 * Base class that provides hooks for processing.
 */
class srcyuml_handler : public srcSAXEventDispatch::PolicyListener {

private:

    std::ostream & out;
    std::vector<std::shared_ptr<srcyuml_class>> classes;

public:

    srcyuml_handler(const std::string & input_str, std::ostream & out) : out(out) {

        srcSAXController controller(input_str);
        run(controller);

    }

    srcyuml_handler(const char * input_filename, std::ostream & out) : out(out) {

        srcSAXController controller(input_filename);
        run(controller);

    }

    ~srcyuml_handler() {}

    void run(srcSAXController & controller) {

        srcyuml_dispatcher<ClassPolicy> dispatcher(this);
        controller.parse(&dispatcher);
        output_yuml();

    }

    void output_yuml() {//counter for classes. Relationship use. Rewrite to functions. 

        out << "digraph hierarchy {\nsize=\"5, 5\"\n";
        out << "node[shape=record,style=filled,fillcolor=gray95]\n";
        out << "edge[dir=\"both\", arrowtail=\"empty\", arrowhead=\"empty\", labeldistance=\"2.0\"]\n";

        std::map class_number_map;

        srcyuml_relationships relationships(classes);
        for(const std::shared_ptr<srcyuml_class> & aclass : classes) {
            //out << *aclass; // rewrote
            srcyuml_class::output_aclass(&out, &aclass, &class_number_map);
        }
        //out << relationships; // rewrote
        srcyuml_relationships::output_relations(&out, &relationships, &class_number_map);

    }

    virtual void Notify(const srcSAXEventDispatch::PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {

        if(typeid(ClassPolicy) == typeid(*policy)) {

            ClassPolicy::ClassData * class_data = policy->Data<ClassPolicy::ClassData>();
            if(class_data && class_data->name)
                classes.emplace_back(std::make_shared<srcyuml_class>(class_data));

        }

    }

private:

};

#endif
