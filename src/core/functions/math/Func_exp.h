/**
 * @file
 * This file contains the declaration of Func_exp, which 
 * calculates 'e' to the power of 'b'.
 *
 * @brief Declaration of Func_exp
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 *
 * $Id$
 */

#ifndef Func_exp_H
#define Func_exp_H

#include "RbFunction.h"

#include <map>
#include <string>
#include <vector>

class DAGNode;
class VectorString;

class Func_exp :  public RbFunction {
    
public:
    // Basic utility functions
    Func_exp*                   clone(void) const;                                          //!< Clone the object
    const VectorString&         getClass(void) const;                                       //!< Get class vector
    
    // Regular functions
    DAGNode*                    execute(void);                                              //!< Execute function
    const ArgumentRules&        getArgumentRules(void) const;                               //!< Get argument rules
    const TypeSpec              getReturnType(void) const;                                  //!< Get type of return value
    
};

#endif
