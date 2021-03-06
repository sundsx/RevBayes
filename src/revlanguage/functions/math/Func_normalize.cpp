//
//  Func_normalize.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 9/8/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "ArgumentRule.h"
#include "Ellipsis.h"
#include "Func_normalize.h"
#include "ModelVector.h"
#include "NormalizeVectorFunction.h"
#include "RbUtil.h"
#include "RealPos.h"
#include "RlDeterministicNode.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"


using namespace RevLanguage;

Func_normalize::Func_normalize() : Function() {
    
}

/* Clone object */
Func_normalize* Func_normalize::clone( void ) const {
    
    return new Func_normalize( *this );
}


/** Execute function: We rely on getValue and overloaded push_back to provide functionality */
RevPtr<Variable> Func_normalize::execute( void ) {
    
    const RevBayesCore::TypedDagNode< std::vector<double> > *params = static_cast< ModelVector<RealPos> & >( args[0].getVariable()->getRevObject() ).getDagNode();
    
    RevBayesCore::NormalizeVectorFunction *func = new RevBayesCore::NormalizeVectorFunction( params );
    
    DeterministicNode<std::vector<double> > *detNode = new DeterministicNode<std::vector<double> >("", func, this->clone());

    ModelVector<RealPos> *theNormalizedVector = new ModelVector<RealPos>( detNode );
    
    return new Variable( theNormalizedVector );
}


/** Get argument rules */
const ArgumentRules& Func_normalize::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) {
        
        argumentRules.push_back( new ArgumentRule( "x", true, ModelVector<RealPos>::getClassTypeSpec() ) );
        rulesSet = true;
    }
    
    return argumentRules;
}


/** Get Rev type of object */
const std::string& Func_normalize::getClassType(void) { 
    
    static std::string revType = "Func_normalize";
    
	return revType; 
}


/** Get class type spec describing type of object */
const TypeSpec& Func_normalize::getClassTypeSpec(void) { 
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/** Get type spec */
const TypeSpec& Func_normalize::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


/** Get return type */
const TypeSpec& Func_normalize::getReturnType( void ) const {
    
    return ModelVector<RealPos>::getClassTypeSpec();
}
