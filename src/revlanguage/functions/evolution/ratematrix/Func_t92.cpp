/* 
 * File:   Func_t92.cpp
 * Author: nl
 * 
 * Created on 25 juillet 2014, 20:12
 */

#include "Func_t92.h"

#include "HkyRateMatrixFunction.h"
#include "RateMatrix_HKY.h"
#include "Real.h"
#include "RealPos.h"
#include "RlDeterministicNode.h"
#include "RlRateMatrix.h"
#include "RlSimplex.h"
#include "TypedDagNode.h"

using namespace RevLanguage;

/** default constructor */
Func_t92::Func_t92( void ) : Function( ) {
    
}


/** Clone object */
Func_t92* Func_t92::clone( void ) const {
    
    return new Func_t92( *this );
}


RevPtr<Variable> Func_t92::execute() {
    
    RevBayesCore::TypedDagNode< double >* ka = static_cast<const RealPos &>( this->args[0].getVariable()->getRevObject() ).getDagNode();
    RevBayesCore::TypedDagNode<std::vector<double> >* bf = static_cast<const Simplex &>( this->args[1].getVariable()->getRevObject() ).getDagNode();
    RevBayesCore::HkyRateMatrixFunction* f = new RevBayesCore::HkyRateMatrixFunction( ka, bf );

    DeterministicNode<RevBayesCore::RateMatrix> *detNode = new DeterministicNode<RevBayesCore::RateMatrix>("", f, this->clone());
    
    RateMatrix* value = new RateMatrix( detNode );
    
    return new Variable( value );
}


/* Get argument rules */
const ArgumentRules& Func_t92::getArgumentRules( void ) const {
    
    static ArgumentRules argumentRules = ArgumentRules();
    static bool          rulesSet = false;
    
    if ( !rulesSet ) {
        
        argumentRules.push_back( new ArgumentRule( "kappa", true, RealPos::getClassTypeSpec() ) );
        argumentRules.push_back( new ArgumentRule( "baseFrequencies", true, Simplex::getClassTypeSpec() ) );
        
        rulesSet = true;
    }
    
    return argumentRules;
}


const std::string& Func_t92::getClassType(void) { 
    
    static std::string revType = "Func_t92";
    
	return revType; 
}

/* Get class type spec describing type of object */
const TypeSpec& Func_t92::getClassTypeSpec(void) { 
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Function::getClassTypeSpec() ) );
    
	return revTypeSpec; 
}


/* Get return type */
const TypeSpec& Func_t92::getReturnType( void ) const {
    
    static TypeSpec returnTypeSpec = RateMatrix::getClassTypeSpec();
    
    return returnTypeSpec;
}


const TypeSpec& Func_t92::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}
