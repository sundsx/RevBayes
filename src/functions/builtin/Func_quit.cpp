/**
 * @file
 * This file contains the implementation of Func_quit, which is
 * the function used to quit the program.
 *
 * @brief Implementation of Func_quit
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @interface RbFunction
 * @package functions
 * @since Version 1.0, 2009-09-03
 *
 * $Id$
 */

#include "ArgumentRule.h"
#include "DAGNode.h"
#include "Func_quit.h"
#include "Model.h"
#include "Parser.h"
#include "RbException.h"
#include "RbNames.h"
#include "RbString.h"
#include "StringVector.h"
#include "UserInterface.h"

#include <fstream>


/** Clone object */
RbObject* Func_quit::clone(void) const {

    return new Func_quit(*this);
}


/** Get argument rules */
const ArgumentRules& Func_quit::getArgumentRules(void) const {

    static ArgumentRules argumentRules;
    return argumentRules;
}


/** Execute function */
const RbObject* Func_quit::executeOperation(const std::vector<DAGNode*>& args) {

    /* Ask if user really wants to quit */
    bool wantToQuit = UserInterface::userInterface().ask("Do you really want to quit");
    
    if (wantToQuit)
        throw RbException(RbException::QUIT);

    return NULL;
}


/** Get class vector describing type of object */
const StringVector& Func_quit::getClass(void) const {

    static StringVector rbClass = StringVector(Func_quit_name) + RbFunction::getClass();
    return rbClass;
}


/** Get return type */
const std::string& Func_quit::getReturnType(void) const {

    return RbNULL_name;
}

