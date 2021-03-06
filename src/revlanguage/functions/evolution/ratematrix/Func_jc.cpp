//
//  Func_exp.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 8/7/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "Func_jc.h"
#include "JcRateMatrixFunction.h"
#include "Natural.h"
#include "RateMatrix_JC.h"
#include "Real.h"
#include "RealPos.h"
#include "RlDeterministicNode.h"
#include "RlRateMatrix.h"
#include "RlSimplex.h"
#include "TypedDagNode.h"

using namespace RevLanguage;

/** default constructor */
Func_jc::Func_jc( void ) : Function( ) {
    
}


/** Clone object */
Func_jc* Func_jc::clone( void ) const {
    
    return new Func_jc( *this );
}


RevPtr<Variable> Func_jc::execute() {
    
    int ns = static_cast<const Natural &>( this->args[0].getVariable()->getRevObject() ).getValue();
    RevBayesCore::JcRateMatrixFunction* f = new RevBayesCore::JcRateMatrixFunction( size_t(ns) );
    
    DeterministicNode<RevBayesCore::RateMatrix> *detNode = new DeterministicNode<RevBayesCore::RateMatrix>("", f, this->clone());
    
    RateMatrix* value = new RateMatrix( detNode );
    
    return new Variable( value );
}


/* Get argument rules */
const ArgumentRules& Func_jc::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) {
        
        argumentRules.push_back( new ArgumentRule( "numStates", true, Natural::getClassTypeSpec() ) );
        
        rulesSet = true;
    }
    
    return argumentRules;
}


const std::string& Func_jc::getClassType(void) { 
    
    static std::string revType = "Func_jc";
    
	return revType; 
}

/* Get class type spec describing type of object */
const TypeSpec& Func_jc::getClassTypeSpec(void) { 
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/* Get return type */
const TypeSpec& Func_jc::getReturnType( void ) const {
    
    static TypeSpec returnTypeSpec = RateMatrix::getClassTypeSpec();
    
    return returnTypeSpec;
}


const TypeSpec& Func_jc::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}
