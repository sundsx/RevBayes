/**
 * @file
 * This file contains the declaration of Func_normalize,
 * which is used to normalize vectors, creating a
 * simplex.
 *
 * @brief Declaration of Func_normalize
 *
 * (c) Copyright 2009- under GPL version 3
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 */

#ifndef Func_normalize_H
#define Func_normalize_H

#include "RlFunction.h"
#include <map>
#include <string>

namespace RevLanguage {
    
    class Func_normalize :  public Function {
        
    public:
        Func_normalize();
        
        // Basic utility functions
        Func_normalize*             clone(void) const;                                          //!< Clone the object
        static const std::string&   getClassType(void);                                         //!< Get Rev type
        static const TypeSpec&      getClassTypeSpec(void);                                     //!< Get class type spec
        const TypeSpec&             getTypeSpec(void) const;                                    //!< Get language type of the object
        
        // Regular functions
        const ArgumentRules&        getArgumentRules(void) const;                               //!< Get argument rules
        const TypeSpec&             getReturnType(void) const;                                  //!< Get type of return value
        
        
        RevPtr<Variable>            execute(void);                                              //!< Execute function
        
    };
    
}


#endif


