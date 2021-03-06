#include "ConstantNode.h"
#include "Container.h"
#include "Environment.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RlString.h"
#include "SyntaxForLoop.h"

#include <cassert>
#include <sstream>

using namespace RevLanguage;


/** Standard constructor */
SyntaxForLoop::SyntaxForLoop( const std::string& identifier, SyntaxElement* inExpr ) :
    SyntaxElement(),
    varName( identifier ),
    inExpression( inExpr ),
    stateSpace( NULL ),
    nextOneoffsetElementIndex( 0 )
{
    if ( inExpression == NULL )
        throw RbException("The 'in' expression of for loop is empty");
}


/** Deep copy constructor */
SyntaxForLoop::SyntaxForLoop( const SyntaxForLoop& x ) :
    SyntaxElement(x)
{
    varName                     = x.varName;
    inExpression                = x.inExpression->clone();
    stateSpace                  = NULL;
    nextOneoffsetElementIndex   = 0;
}


/** Destructor deletes members */
SyntaxForLoop::~SyntaxForLoop()
{
    delete inExpression;
    delete stateSpace;
}


/** Assignment operator */
SyntaxForLoop& SyntaxForLoop::operator=( const SyntaxForLoop& x )
{
    if ( &x != this )
    {
        SyntaxElement::operator=(x);

        delete inExpression;
        delete stateSpace;

        varName                     = x.varName;
        inExpression                = x.inExpression->clone();
        stateSpace                  = NULL;
        nextOneoffsetElementIndex   = 0;
    }

    return *this;
}


/** Clone syntax element */
SyntaxElement* SyntaxForLoop::clone( void ) const
{
    return new SyntaxForLoop( *this );
}


/** Get semantic value (not applicable so return NULL) */
RevPtr<Variable> SyntaxForLoop::evaluateContent( Environment& env )
{
    return NULL;
}


/** Finalize loop. */
void SyntaxForLoop::finalizeLoop( void )
{
    nextOneoffsetElementIndex = 0;
}


/** Get the name of the index variable */
const std::string& SyntaxForLoop::getIndexVarName( void ) const
{
    return varName;
}


/**
 * Get the next loop state. We simply assign a new value to
 * the loop variable using a control variable assignment.
 * We rely on the caller to make sure that the element index
 * is not out of range (or the container will throw an error).
 */
void SyntaxForLoop::getNextLoopState( void )
{
    // Get the next value from the container
    RevObject* elm = stateSpace->getElement( nextOneoffsetElementIndex )->getRevObject().clone();

    // Set the loop variable to the next value using a control variable assignment
    loopVariable->setRevObject( elm );
    loopVariable->setControlVarState( true );

    // Increment the element index
    nextOneoffsetElementIndex++;
}


/**
 * Initialize loop state. Here we evaluate the in-expression and
 * make sure it is a container with at least one dimension. We will
 * use the first dimension of the container to get the values for
 * the loop variable. We also add the loop variable to the environment.
 */
void SyntaxForLoop::initializeLoop( Environment& env )
{
    assert ( nextOneoffsetElementIndex == 0 );  // Check that we are not running already

    // Evaluate expression and check that we get a vector
    const RevPtr<Variable>&      theVar      = inExpression->evaluateContent(env);
    const RevObject&             theValue    = theVar->getRevObject();

    // Check that it is a container (the first dimension of which we will use)
    if ( theValue.isTypeSpec( Container::getClassTypeSpec() ) == false )
       throw RbException( "The 'in' expression does not evaluate to a container" );

    stateSpace = dynamic_cast<Container*>(theValue.clone());

    // Add the loop variable to the environment, if it is not already there
    if ( !env.existsVariable( varName ) )
        env.addVariable( varName, new Variable( NULL) );
    
    // Set the local smart pointer to the loop variable
    loopVariable = env.getVariable( varName );
    
    // Initialize nextValue
    nextOneoffsetElementIndex = 1;
}


/**
 * Have we already reached the end of the loop? We return true
 * if we have reached the end.
 */
bool SyntaxForLoop::isFinished( void ) const
{
    return nextOneoffsetElementIndex > stateSpace->size();
}


/** Print info about syntax element */
void SyntaxForLoop::printValue( std::ostream& o ) const
{
    o << "SyntaxForLoop:" << std::endl;
    o << "varName      = " << varName << std::endl;
    o << "inExpression = ";
    inExpression->printValue( o );
    o << std::endl;
}

