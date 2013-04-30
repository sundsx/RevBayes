//
//  RlRateMatrix.cpp
//  RevBayesCore
//
//  Created by Sebastian Hoehna on 9/10/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "RlRateMatrix_JC.h"

#include "ArgumentRule.h"
#include "MemberFunction.h"
#include "Natural.h"
#include "RlBoolean.h"
#include "RlTaxonData.h"
#include "Vector.h"


using namespace RevLanguage;

RateMatrix_JC::RateMatrix_JC(void) : RlModelVariableWrapper<RevBayesCore::RateMatrix_JC>() {
    
}


RateMatrix_JC::RateMatrix_JC( RevBayesCore::RateMatrix_JC *v) : RlModelVariableWrapper<RevBayesCore::RateMatrix_JC>( v ) {
    
}


RateMatrix_JC::RateMatrix_JC( RevBayesCore::TypedDagNode<RevBayesCore::RateMatrix_JC> *m) : RlModelVariableWrapper<RevBayesCore::RateMatrix_JC>( m ) {
    
}


RateMatrix_JC* RateMatrix_JC::clone() const {
    return new RateMatrix_JC( *this );
}


/* Map calls to member methods */
RbLanguageObject* RateMatrix_JC::executeMethod(std::string const &name, const std::vector<Argument> &args) {
    
    if (name == "[]") {
        // get the member with give index
        const Natural& index = static_cast<const Natural&>( args[0].getVariable()->getValue() );
        
        if (this->value->getValue().getNumberOfStates() < (size_t)(index.getValue()) ) {
            throw RbException("Index out of bounds in []");
        }
        
        const std::vector<double>& element = this->value->getValue()[index.getValue() - 1];
        std::vector<double> elementVector;
        for (size_t i=0; i < this->value->getValue().size(); ++i) {
            elementVector.push_back( element[i] );
        }
        return new Vector<RealPos>( elementVector );
    }
    else if (name == "size") {
        int n = (int)this->value->getValue().getNumberOfStates();
        return new Natural(n);
    }
    
    return RlModelVariableWrapper<RevBayesCore::RateMatrix_JC>::executeMethod( name, args );
}


/* Get class name of object */
const std::string& RateMatrix_JC::getClassName(void) { 
    
    static std::string rbClassName = "RateMatrix(JC)";
    
	return rbClassName; 
}

/* Get class type spec describing type of object */
const TypeSpec& RateMatrix_JC::getClassTypeSpec(void) { 
    
    static TypeSpec rbClass = TypeSpec( getClassName(), new TypeSpec( RbLanguageObject::getClassTypeSpec() ) );
    
	return rbClass; 
}


/** Get the methods for this vector class */
/* Get method specifications */
const MethodTable& RateMatrix_JC::getMethods(void) const {
    
    static MethodTable    methods                     = MethodTable(); 
    static bool           methodsSet                  = false;
    
    if ( methodsSet == false ) {
        
        // add method for call "x[]" as a function
        ArgumentRules* squareBracketArgRules = new ArgumentRules();
        squareBracketArgRules->push_back( new ArgumentRule( "index" , true, Natural::getClassTypeSpec() ) );
        methods.addFunction("[]",  new MemberFunction( Vector<RealPos>::getClassTypeSpec(), squareBracketArgRules) );
        
        
        // add method for call "x[]" as a function
        ArgumentRules* sizeArgRules = new ArgumentRules();
        methods.addFunction("size",  new MemberFunction( Natural::getClassTypeSpec(), sizeArgRules) );
        
        // necessary call for proper inheritance
        methods.setParentTable( &RlModelVariableWrapper<RevBayesCore::RateMatrix_JC>::getMethods() );
        methodsSet = true;
    }
    
    
    return methods;
}



/** Get the type spec of this class. We return a member variable because instances might have different element types. */
const TypeSpec& RateMatrix_JC::getTypeSpec(void) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    return typeSpec;
}