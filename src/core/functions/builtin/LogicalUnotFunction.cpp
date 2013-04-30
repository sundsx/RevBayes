#include "LogicalUnotFunction.h"

using namespace RevBayesCore;

LogicalUnotFunction::LogicalUnotFunction(const TypedDagNode<bool> *l) : TypedFunction<bool>( new bool(false) ), left( l ) {
    // add the parameters as parents
    this->addParameter( left );
    
    update();
}


LogicalUnotFunction::LogicalUnotFunction(const LogicalUnotFunction &n) : TypedFunction<bool>( n ), left( n.left ) {
    // no need to add parameters, happens automatically
    
    update();
}


LogicalUnotFunction::~LogicalUnotFunction( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}



LogicalUnotFunction* LogicalUnotFunction::clone( void ) const {
    return new LogicalUnotFunction( *this );
}


void LogicalUnotFunction::update( void ) {
    
    *this->value = !left->getValue();
    
}



void LogicalUnotFunction::swapParameterInternal(const DagNode *oldP, const DagNode *newP) {
    
    if ( oldP == left ) {
        left = static_cast<const TypedDagNode<bool>* >( newP );
    }
    
}
