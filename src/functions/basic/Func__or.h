/**
 * @file
 * This file contains the declaration of Func__or, which is used
 * to perform the logical OR on two variables (ints or bools)
 *
 * @brief Declaration of Func__or
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef Func__or_H
#define Func__or_H

#include "RbFunction.h"

#include <map>
#include <string>

class DAGNode;
class VectorString;

template <typename firstValType, typename secondValType>
class Func__or :  public RbFunction {

    public:
        // Basic utility functions
        Func__or*                   clone(void) const;                                      //!< Clone the object
    	const VectorString&         getClass(void) const;                                   //!< Get class vector

        // Regular functions
        const ArgumentRules&        getArgumentRules(void) const;                           //!< Get argument rules
        int                         getReturnDim(void) const;                               //!< Get dim of return value
        const std::string&          getReturnType(void) const;                              //!< Get type of return value

	protected:
        RbObject*                   executeOperation(const std::vector<DAGNode*>& args);    //!< Execute operation
};

#endif

#include "ArgumentRule.h"
#include "Boolean.h"
#include "DAGNode.h"
#include "Integer.h"
#include "RbException.h"
#include "RbNames.h"
#include "Real.h"
#include "MatrixReal.h"
#include "VectorString.h"


/** Clone object */
template <typename firstValType, typename secondValType>
Func__or<firstValType, secondValType>* Func__or<firstValType, secondValType>::clone(void) const {

    return new Func__or(*this);
}


/** Execute function: Boolean <- Integer | Integer */
template <>
RbObject* Func__or<Integer,Integer>::executeOperation(const std::vector<DAGNode*>& args) {

    bool val1 = (bool)(((Integer*)(args[0])->getValue())->getValue());
    bool val2 = (bool)(((Integer*)(args[1])->getValue())->getValue());
    bool comp = (val1 || val2);
    return new Boolean(comp);
}


/** Execute function: Boolean <- Integer | Boolean */
template <>
RbObject* Func__or<Real,Real>::executeOperation(const std::vector<DAGNode*>& args) {

    bool val1 = (bool)(((Integer*)(args[0])->getValue())->getValue());
    bool val2 = ((Boolean*)(args[1])->getValue())->getValue();
    bool comp = (val1 || val2);
    return new Boolean(comp);
}


/** Execute function: Boolean <- Boolean | Integer */
template <>
RbObject* Func__or<Integer,Real>::executeOperation(const std::vector<DAGNode*>& args) {

    bool val1 = ((Boolean*)(args[0])->getValue())->getValue();
    bool val2 = (bool)(((Integer*)(args[1])->getValue())->getValue());
    bool comp = (val1 || val2);
    return new Boolean(comp);
}


/** Execute function: Boolean <- Boolean | Boolean */
template <>
RbObject* Func__or<Real,Integer>::executeOperation(const std::vector<DAGNode*>& args) {

    bool val1 = ((Boolean*)(args[0])->getValue())->getValue();
    bool val2 = ((Boolean*)(args[1])->getValue())->getValue();
    bool comp = (val1 || val2);
    return new Boolean(comp);
}


/** Get argument rules */
template <typename firstValType, typename secondValType>
const ArgumentRules& Func__or<firstValType, secondValType>::getArgumentRules(void) const {

    static ArgumentRules argumentRules;
    static bool          rulesSet = false;

    if (!rulesSet) 
        {
        firstValType*  dummy1 = new firstValType();
        secondValType* dummy2 = new secondValType();
        argumentRules.push_back(new ArgumentRule("", dummy1->getType()));
        argumentRules.push_back(new ArgumentRule("", dummy2->getType()));
        rulesSet = true;
        delete dummy1;
        delete dummy2;
        }

    return argumentRules;
}


/** Get class vector describing type of object */
template <typename firstValType, typename secondValType>
const VectorString& Func__or<firstValType, secondValType>::getClass(void) const {

    firstValType*  dummy1 = new firstValType();
    secondValType* dummy2 = new secondValType();
    
    std::string funcAddName = "Func__or<" + dummy1->getType() + "," + dummy2->getType() + ">"; 
    static VectorString rbClass = VectorString(funcAddName) + RbFunction::getClass();
    
    delete dummy1;
    delete dummy2;
    
    return rbClass;
}


/** Get return dimension */
template <typename firstValType, typename secondValType>
int Func__or<firstValType, secondValType>::getReturnDim(void) const {

    return 1;
}


/** Get return type */
template <typename firstValType, typename secondValType>
const std::string& Func__or<firstValType, secondValType>::getReturnType(void) const {

    return Boolean_name;
}

