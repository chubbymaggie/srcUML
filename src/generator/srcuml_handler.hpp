/**
 * @file srcuml_handler.hpp
 *
 * @copyright Copyright (C) 2016 srcML, LLC. (www.srcML.org)
 *
 * This file is part of srcUML.
 *
 * srcUML is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * srcUML is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with srcUML.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_SRCUML_HANDLER_HPP
#define INCLUDED_SRCUML_HANDLER_HPP

#include <srcSAXEventDispatchUtilities.hpp>
#include <srcSAXController.hpp>

#include <srcuml_dispatcher.hpp>
#include <ClassPolicySingleEvent.hpp>

#include <srcuml_class.hpp>
#include <srcuml_relationship.hpp>
#include <dot_outputter.hpp>
#include <yuml_outputter.hpp>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <map>

/**
 * srcuml_handler
 *
 * Base class that provides hooks for processing.
 */
class srcuml_handler : public srcSAXEventDispatch::PolicyListener {

private:

    std::vector<std::shared_ptr<srcuml_class>> classes;

public:

    srcuml_handler(const std::string & input_str, std::ostream & out) {

        srcSAXController controller(input_str);
        run(controller, out);

    }

    srcuml_handler(const char * input_filename, std::ostream & out) {

        srcSAXController controller(input_filename);
        run(controller, out);

    }

    ~srcuml_handler() {}

    void run(srcSAXController & controller, std::ostream & out) {

        srcuml_dispatcher<ClassPolicy> dispatcher(this);
        controller.parse(&dispatcher);
        dot_outputter outputter;
        outputter.output(out, classes);

    }

    virtual void Notify(const srcSAXEventDispatch::PolicyDispatcher * policy, const srcSAXEventDispatch::srcSAXEventContext & ctx) override {

        if(typeid(ClassPolicy) == typeid(*policy)) {

            ClassPolicy::ClassData * class_data = policy->Data<ClassPolicy::ClassData>();
            if(class_data && class_data->name)
                classes.emplace_back(std::make_shared<srcuml_class>(class_data));

        }

    }

private:

};

#endif
