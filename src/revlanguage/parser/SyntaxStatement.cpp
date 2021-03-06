#include "RlBoolean.h"
#include "Environment.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RevNullObject.h"
#include "Signals.h"
#include "SyntaxForLoop.h"
#include "SyntaxStatement.h"
#include "RlUserInterface.h"
#include "Workspace.h"

#include <cassert>
#include <iostream>
#include <list>
#include <sstream>

using namespace RevLanguage;


/** Static vector of string giving names of statement types */
std::string SyntaxStatement::stmtName[] = { "IF", "IF_ELSE", "FOR", "WHILE", "NEXT", "BREAK", "RETURN" }; 


/** Construct from statement type */
SyntaxStatement::SyntaxStatement( statementT type ) :
    SyntaxElement(),
    statementType( type ),
    expression( NULL ),
    statements1( NULL ),
    statements2( NULL )
{
}


/** Construct from statement type and expression (RETURN expression) */
SyntaxStatement::SyntaxStatement( statementT type, SyntaxElement* expr ) :
    SyntaxElement(),
    statementType( type ),
    expression( expr ),
    statements1( NULL ),
    statements2( NULL )
{
}


/** Construct from statement type, condition and statement list */
SyntaxStatement::SyntaxStatement( statementT type, SyntaxElement* cond, std::list<SyntaxElement*>* stmts ) :
    SyntaxElement(),
    statementType( type ),
    expression( cond ),
    statements1( stmts ),
    statements2( NULL )
{
}


/** Construct from statement type, condition and two statement lists */
SyntaxStatement::SyntaxStatement(statementT                   type,
                                 SyntaxElement*               cond,
                                 std::list<SyntaxElement*>*   stmts1,
                                 std::list<SyntaxElement*>*   stmts2) :
    SyntaxElement(),
    statementType( type ),
    expression( cond ),
    statements1( stmts1 ),
    statements2( stmts2 )
{
}


/** Deep copy constructor */
SyntaxStatement::SyntaxStatement(const SyntaxStatement& x) :
    SyntaxElement( x )
{

    statementType   = x.statementType;
    expression      = x.expression->clone();

    statements1 = new std::list<SyntaxElement*>();
    if ( x.statements1 != NULL )
    {
        for ( std::list<SyntaxElement*>::const_iterator it = x.statements1->begin(); it != x.statements1->end(); ++it )
            statements1->push_back( (*it)->clone() );
    }
    
    statements2 = new std::list<SyntaxElement*>();
    if ( x.statements2 != NULL )
    {
        for ( std::list<SyntaxElement*>::const_iterator it = x.statements2->begin(); it != x.statements2->end(); ++it )
            statements2->push_back( (*it)->clone() );
    }
}


/** Destructor deletes expression and statements */
SyntaxStatement::~SyntaxStatement() {
    
    delete expression;
    
    if (statements1 != NULL)
    {
        for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
            delete *it;
        delete statements1;
    }
    
    if (statements2 != NULL)
    {
        for ( std::list<SyntaxElement*>::iterator it = statements2->begin(); it != statements2->end(); ++it )
            delete *it;
        delete statements2;
    }
}


/** Assignment operator */
SyntaxStatement& SyntaxStatement::operator=( const SyntaxStatement& x )
{
    if ( &x != this )
    {
        SyntaxElement::operator=( x );

        if ( expression != NULL )
        {
            delete expression;
            expression = NULL;
        }
        
        if ( statements1 != NULL )
        {
            for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
                delete *it;
            delete statements1;
            statements1 = NULL;
        }
        
        if ( statements2 != NULL )
        {
            for ( std::list<SyntaxElement*>::iterator it = statements2->begin(); it != statements2->end(); ++it )
                delete *it;
            delete statements2;
            statements2 = NULL;
        }

        statementType   = x.statementType;

        if ( x.expression != NULL )
            expression = x.expression->clone();

        if ( x.statements1 != NULL )
        {
            for ( std::list<SyntaxElement*>::const_iterator it = x.statements1->begin(); it != x.statements1->end(); ++it )
                statements1->push_back( (*it)->clone() );
        }

        if ( x.statements2 != NULL )
        {
            for ( std::list<SyntaxElement*>::const_iterator it = x.statements2->begin(); it != x.statements2->end(); ++it )
                statements2->push_back( (*it)->clone() );
        }
    }

    return ( *this );
}


/** Type-safe clone of syntax element */
SyntaxStatement* SyntaxStatement::clone( void ) const
{
    return new SyntaxStatement( *this );
}


/**
 * Get semantic value: it is here that we execute the statement.
 *
 * @todo Return statements do not appear to be correctly handled in
 *       for loops. The return variable is discarded and the loop
 *       just continues.
 */
RevPtr<Variable> SyntaxStatement::evaluateContent(Environment& env) {

    RevPtr<Variable> result = NULL;
    
    if (statementType == For)
    {
        // Convert expression to for condition
        SyntaxForLoop* forLoop = dynamic_cast<SyntaxForLoop*>( expression );
        assert ( forLoop != NULL );

        // Initialize for loop. We use current environment for the loop variables (as in R)
        Signals::getSignals().clearFlags();
        forLoop->initializeLoop( env );

        // Now loop over statements inside the for loop
        while ( !forLoop->isFinished() )
        {
            // Get next loop state. This will update the value of the loop variable
            forLoop->getNextLoopState();

            for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
            {
                // Get a convenient pointer to the syntax element
                SyntaxElement* theSyntaxElement = *it;
                
                // Execute statement
                result = theSyntaxElement->evaluateContent( env );

                // We do not print the result (as in R). Uncomment the code below to change this behavior

#if 0
                // Print result if it is not an assign expression (== NULL)
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && !theSyntaxElement->isAssignment() &&
                     result != NULL && result->getRevObject() != RevNullObject::getInstance())
                {
                    std::ostringstream msg;
                    result->getRevObject().printValue(msg);
                    RBOUT( msg.str() );
                }
#endif

                // Catch a return signal
                // TODO: This appears to inappropriately discard the return value of a return statement
				if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL)
                {
                    result = NULL;  // discard result
                }

                // Catch signal
                if ( !Signals::getSignals().isGood() )
                    break;
            }
            
            // Catch signals
            // TODO Return statement not handled correctly
            if ( Signals::getSignals().isSet(Signals::BREAK) )
            {
                Signals::getSignals().clearFlags();
                break;
            }
            else if ( Signals::getSignals().isSet(Signals::CONTINUE) )
            {
                Signals::getSignals().clearFlags();  // Just continue with next loop state
            }
        }
        
        // Finalize the loop (resets the forloop state for next execution round)
        forLoop->finalizeLoop();
        
    }
    else if ( statementType == Break )
    {
        // Set BREAK signal
        Signals::getSignals().set(Signals::BREAK);
    }
    else if (statementType == Next)
    {
        // Set CONTINUE signal
        Signals::getSignals().set(Signals::CONTINUE);
    }
    else if ( statementType == While )
    {
        // Loop over statements inside the while loop, first checking the expression
        while ( isTrue( expression, env ) ) {

            for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
            {
                SyntaxElement* theSyntaxElement = *it;

                // Execute statement
	            result = theSyntaxElement->evaluateContent( env );
                
                // Print result if it is not an assign expression (==NULL)
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && !theSyntaxElement->isAssignment()
                        && result != NULL && result->getRevObject() != RevNullObject::getInstance() )
                {
                    std::ostringstream msg;
                    result->getRevObject().printValue(msg);
                    RBOUT( msg.str() );
                }
	 
	            // Free memory
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    result = NULL;  // discard result
                }
	 
	            // Catch signal
	            if ( !Signals::getSignals().isGood() )
                    break;
            }

            // Catch signals
            if ( Signals::getSignals().isSet(Signals::BREAK) )
            {
	                 Signals::getSignals().clearFlags();
	                 break;
            }
            else if ( Signals::getSignals().isSet(Signals::CONTINUE) )
            {
	                 Signals::getSignals().clearFlags();  // Just continue with next loop state
            }
        }
    }
    else if ( statementType == Return )
    {
        // Set RETURN signal and return expression value
        Signals::getSignals().set(Signals::RETURN);
        return expression->evaluateContent(env);
    }
    else if ( statementType == If )
    {
        // Process statements inside the if clause if expression is true
        if ( isTrue( expression, env ) )
        {
            for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
            {
                // Execute statement
                result = (*it)->evaluateContent(env);
                
                // Print result if it is not an assign expression (==NULL)
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    std::ostringstream msg;
                    result->getRevObject().printValue(msg);
                    RBOUT( msg.str() );
                }

                // Free memory
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    result = NULL;  // discard result
                }
            }
        }
    }
    else if ( statementType == IfElse )
    {
        // Process statements inside the if clause if expression is true,
        // otherwise process statements in else clause
        if ( isTrue( expression, env ) )
        {
            for ( std::list<SyntaxElement*>::iterator it = statements1->begin(); it != statements1->end(); ++it )
            {
                // Execute statement
                result = (*it)->evaluateContent( env );
                
                // Print result if it is not an assign expression (==NULL)
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    std::ostringstream msg;
                    result->getRevObject().printValue(msg);
                    RBOUT( msg.str() );
                }
                
                // Free memory
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    result = NULL;  // discard result
                }
            }
        }
        else
        {
            for ( std::list<SyntaxElement*>::iterator it = statements2->begin(); it != statements2->end(); ++it )
            {
                // Execute statement
                result = (*it)->evaluateContent( env );
                
                // Print result if it is not an assign expression (==NULL)
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    std::ostringstream msg;
                    result->getRevObject().printValue(msg);
                    RBOUT( msg.str() );
                }
                    
                // Free memory
                if ( !Signals::getSignals().isSet( Signals::RETURN ) && result != NULL )
                {
                    result = NULL;  // discard result
                }
            }
        }
    }

    return result;
}


/**
 * This is a help function that evaluates the expression and then checks
 * whether the result is true or false, or can be interpreted as a RlBoolean
 * true or false value.
 */
bool SyntaxStatement::isTrue( SyntaxElement* expr, Environment& env ) const
{
    RevPtr<Variable> temp = expr->evaluateContent( env );
    
    if ( temp == NULL )
        return false;
    
    if ( temp->getRevObject().isTypeSpec( RlBoolean::getClassTypeSpec() ) )
    {
        bool retValue = static_cast<const RlBoolean&>( temp->getRevObject() ).getValue();
        
        return retValue;
    }
    else
    {
        RevObject *tempObject = temp->getRevObject().convertTo( RlBoolean::getClassTypeSpec() );
        RlBoolean* tempBool = static_cast<RlBoolean*>( tempObject );
        bool     retValue = tempBool->getValue();
        
        delete tempBool;
        
        return   retValue;
    }
}


/** Print info about the syntax element */
void SyntaxStatement::printValue(std::ostream& o) const
{
    o << "SyntaxStatement:" << std::endl;
    o << "statementType = " << stmtName[ statementType ] << std::endl;
    if (expression == NULL)
        o << "expression    = NULL" << std::endl;
    else 
    {
        o << "expression    = [" << expression << "] ";
        expression->printValue(o);
        o << std::endl;
    }
    if ( statements1 == NULL )
        o << "statements1   = NULL" << std::endl;
    else 
    {
        o << "statements1   = <" << statements1->size() << " statements>" << std::endl;
        int count = 1;
        for ( std::list<SyntaxElement*>::const_iterator it = statements1->begin(); it !=statements1->end(); ++it, ++count )
        {
            o << "   stmt " << count << " = [" << (*it) << "] ";
            (*it)->printValue(o);
            o << std::endl;
        }
    }
    if ( statements2 == NULL )
        o << "statements2   = NULL" << std::endl;
    else 
    {
        o << "statements2   = <" << statements2->size() << " statements>" << std::endl;
        int count = 1;
        for ( std::list<SyntaxElement*>::const_iterator it = statements2->begin(); it != statements2->end(); ++it, ++count )
        {
            o << "   stmt " << count << " = [" << (*it) << "] ";
            (*it)->printValue(o);
            o << std::endl;
        }
    }
    o << std::endl;
}

