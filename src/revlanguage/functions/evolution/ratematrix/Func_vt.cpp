//
//  Func_exp.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 8/7/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "DeterministicNode.h"
#include "Func_vt.h"
#include "Natural.h"
#include "RateMatrix_Vt.h"
#include "Real.h"
#include "RealPos.h"
#include "RlRateMatrix.h"
#include "RlSimplex.h"
#include "TypedDagNode.h"

using namespace RevLanguage;

/** default constructor */
Func_vt::Func_vt( void ) : Function( ) {
    
}


/** Clone object */
Func_vt* Func_vt::clone( void ) const {
    
    return new Func_vt( *this );
}


RevPtr<Variable> Func_vt::execute() {
    
    
    RevBayesCore::RateMatrix_Vt *rmj = new RevBayesCore::RateMatrix_Vt();
    RateMatrix* value = new RateMatrix( rmj );
    
    return new Variable( value );
}


/* Get argument rules */
const ArgumentRules& Func_vt::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    
    return argumentRules;
}


const std::string& Func_vt::getClassType(void) { 
    
    static std::string revType = "Func_vt";
    
	return revType; 
}

/* Get class type spec describing type of object */
const TypeSpec& Func_vt::getClassTypeSpec(void) { 
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/* Get return type */
const TypeSpec& Func_vt::getReturnType( void ) const {
    
    static TypeSpec returnTypeSpec = RateMatrix::getClassTypeSpec();
    
    return returnTypeSpec;
}


const TypeSpec& Func_vt::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}
