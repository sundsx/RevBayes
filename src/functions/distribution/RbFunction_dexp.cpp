/**
 * @file
 * This file contains the implementation of RbFunction_dexp, the
 * dexp() function.
 *
 * @brief Implementation of RbFunction_dexp
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author Fredrik Ronquist and the REvBayes core team
 * @license GPL version 3
 * @version 1.0
 * @since Version 1.0, 2009-08-26
 *
 * $Id$
 */

#include "DAGNode.h"
#include "RbFunction_dexp.h"
#include "RbDouble.h"
#include "RbException.h"
#include "RbNames.h"
#include "RbObject.h"
#include "RbStatistics.h"
#include "RbUndefined.h"
#include <cmath>

const StringVector RbFunction_dexp::rbClass = StringVector("dexp") + RbFunction::rbClass;

/** Define the argument rules */

/** Add to symbol table */
//static bool fxn_dexp = SymbolTable::globalTable().add("dexp", new RbFunction_dexp());


/** Default constructor, allocate workspace */
RbFunction_dexp::RbFunction_dexp(void)
    : RbFunction(), value(new RbDouble(0)) {
    RbUndefined undef;
    RbDouble x_min = RbDouble(0.0);
    RbDouble r_def = RbDouble(1.0);
    RbDouble r_min = RbDouble(0.0);
	argRules.push_back( ArgumentRule("x", RbNames::Double::name, undef, x_min, undef ) );
	argRules.push_back( ArgumentRule("rate", RbNames::Double::name, r_def, r_min, undef) );
	returnType = RbNames::Double::name;
} 

/** Copy constructor */
RbFunction_dexp::RbFunction_dexp(const RbFunction_dexp& s)
    : RbFunction(s), value(new RbDouble(0)) {
    RbUndefined undef;
    RbDouble x_min = RbDouble(0.0);
    RbDouble r_def = RbDouble(1.0);
    RbDouble r_min = RbDouble(0.0);
    argRules.push_back( ArgumentRule("x", RbNames::Double::name, undef, x_min, undef ) );
    argRules.push_back( ArgumentRule("rate", RbNames::Double::name, r_def, r_min, undef) );
    returnType = RbNames::Double::name;
}

/** Destructor, delete workspace */
RbFunction_dexp::~RbFunction_dexp() {

    delete value;
}

/**
 * @brief clone function
 *
 * This function creates a deep copy of this object.
 *
 * @see RbObject.clone()
 * @returns           return a copy of this object
 *
 */
RbObject* RbFunction_dexp::clone(void) const {

    RbObject *x = new RbFunction_dexp( *this );
    return x;
}

RbObject& RbFunction_dexp::operator=(const RbObject& obj) {

    try {
        // Use built-in fast down-casting first
        const RbFunction_dexp& x = dynamic_cast<const RbFunction_dexp&> (obj);

        RbFunction_dexp& y = (*this);
        y = x;
        return y;
    } catch (std::bad_cast & bce) {
        try {
            // Try converting the value to an argumentRule
            const RbFunction_dexp& x = dynamic_cast<const RbFunction_dexp&> (*(obj.convertTo("dexp")));

            RbFunction_dexp& y = (*this);
            y = x;
            return y;
        } catch (std::bad_cast & bce) {
            RbException e("Not supported assignment of " + obj.getClass()[0] + " to dexp");
            throw e;
        }
    }

    // dummy return
    return (*this);
}

RbFunction_dexp& RbFunction_dexp::operator=(const RbFunction_dexp& obj) {

    argRules = obj.argRules;
    returnType = obj.returnType;
    (*value) = (*obj.value);
    return (*this);
}


/**
 * @brief print function
 *
 * This function prints this object.
 *
 * @see RbObject.print()
 * @param c           the stream where to print to
 *
 */
void RbFunction_dexp::printValue(std::ostream &o) const {

    o << value << std::endl;
}

/**
 * @brief dump function
 *
 * This function dumps this object.
 *
 * @see RbObject.dump()
 * @param c           the stream where to dump to
 *
 */
void RbFunction_dexp::dump(std::ostream& c){
    //TODO implement

    std::string message = "Dump function of RbFunction_dexp not fully implemented!";
    RbException e;
    e.setMessage(message);
    throw e;
}

/**
 * @brief resurrect function
 *
 * This function resurrects this object.
 *
 * @see RbObject.resurrect()
 * @param x           the object from which to resurrect
 *
 */
void RbFunction_dexp::resurrect(const RbDumpState& x){
    //TODO implement
    std::string message = "Resurrect function of RbFunction_dexp not fully implemented!";
    RbException e;
    e.setMessage(message);
    throw e;
}

std::string RbFunction_dexp::toString(void) const {

	char temp[30];
	sprintf(temp, "%1.6lf", value->getValue());
	std::string tempStr = temp;
    return "Value = " + tempStr;
}


/**
 * @brief overloaded == operators
 *
 * This function compares this object
 *
 * @param o           the object to compare to
 *
 */
bool RbFunction_dexp::equals(const RbObject* o) const {

    return false;
}

/** Execute function */
RbObject* RbFunction_dexp::executeOperation(const std::vector<DAGNode*>& arguments) {

    /* Get actual argument */
    RbDouble *x      = (RbDouble*) arguments[0]->getValue();
    RbDouble *lambda = (RbDouble*) arguments[1]->getValue();

    value->setValue(RbStatistics::Exponential::pdf(*lambda,*x));

    return value;
}

RbObject* RbFunction_dexp::convertTo(const std::string& type) const {

    return NULL;
}

/**
 * @brief is convertible to
 *
 * This function checks if this data type can be converted into the given data type.
 *
 * @param dt         the data type we want to convert to
 * @returns          true, if it can be converted
 *
 */
bool RbFunction_dexp::isConvertibleTo(const std::string& type) const {

    return false;
}
