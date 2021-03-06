/**
 * @file
 * This file contains the implementation of some of the functions
 * in the abstract base class for language objects, RevObject.
 *
 * @brief Partial implementation of RevObject
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-02, version 1.0
 * @extends RevObject
 *
 * $Id$
 */

#include "MemberProcedure.h"
#include "MethodTable.h"
#include "ModelVector.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RevObject.h"
#include "RlUtils.h"
#include "RlString.h"
#include "TypeSpec.h"
#include "Workspace.h"

#include <sstream>
#include <stdio.h>

using namespace RevLanguage;




/** Constructor: we set member variables here from member rules */
RevObject::RevObject(const MemberRules& memberRules) {
    
}


/** 
 * Destructor. 
 * Nothing to do here because we don't own anything.
 */
RevObject::~RevObject( void )
{
    
}


/**
 * Addition operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 *
 * \param[in]   rhs     The right hand side value of the division operation.
 *
 * \return              NULL
 */
RevObject* RevObject::add(const RevObject &rhs) const
{
    throw RbException("Cannot add a value of type '" + this->getType() + "' to a value of type '" + rhs.getType() + "'.");
    
    return NULL;
}

/** The default implementation does nothing because we don't have an internal object */
void RevObject::constructInternalObject( void ) {
    // nothing to do
}



/** Convert to type and dim. The caller manages the returned object. */
RevObject* RevObject::convertTo(const TypeSpec& typeSpec) const {
        
    throw RbException("Failed conversion from type '" + getType() + "' to type '" + typeSpec.getType() + "'" );
    
    return NULL;
}


/**
 * Decrement operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 */
void RevObject::decrement( void ) 
{
    throw RbException("Cannot decrement a value of type '" + this->getType() + "'.");

}


/**
 * Division operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 *
 * \param[in]   rhs     The right hand side value of the division operation.
 *
 * \return              NULL
 */
RevObject* RevObject::divide(const RevObject &rhs) const
{
    throw RbException("Cannot divide a value of type '" + this->getType() + "' to a value of type '" + rhs.getType() + "'.");
    
    return NULL;
}


/** 
 * Execute simple method. 
 */
RevPtr<Variable> RevObject::executeMethod(std::string const &name, const std::vector<Argument> &args) {
    
    if (name == "get") 
    {
        // get the member with give name
        const std::string &varName = static_cast<const RlString &>( args[0].getVariable()->getRevObject() ).getValue();
        
        // check if a member with that name exists
        if ( hasMember( varName ) ) 
        {
            return getMember(varName);
        }
        
        // there was no variable with the given name
        return NULL;
        
    }
    else if ( name == "members" ) 
    {
        
        return NULL;
    }
    else if ( name == "methods" ) 
    {
        // just print the method names (including inherited methods)
        const MethodTable &m = getMethods();
        m.printValue(std::cout, true);
        
        return NULL;
    }
    else 
    {
        throw RbException( "No mapping from member method '" + name + "' to internal function call provided" );
    }
}


/**
 * Find or create an element variable of a container. Default implementation throws
 * an error. Override in container objects.
 */
RevPtr<Variable> RevObject::findOrCreateElement( const std::vector<size_t>& indices )
{
    throw RbException( "Object of type '" + getType() + "' does not have any elements" );
}


/** Get class vector describing type of object */
const std::string& RevObject::getClassType(void)
{
    
    static std::string revType = "RevObject";
	return revType; 
}


/**
 * Get class vector describing type of object. We are
 * the base class of all other classes, so parent is NULL.
 */
const TypeSpec& RevObject::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), NULL );
	
    return revTypeSpec; 
}



/** Return member rules (no members) */
const MemberRules& RevObject::getMemberRules(void) const
{
    
    static const MemberRules rules = MemberRules();
    
    return rules;
}


/** Get a member variable */
RevPtr<Variable> RevObject::getMember(const std::string& name) const
{
    throw RbException("No Member named '" + name + "' available.");
}


/** 
 * Get method specifications of all member objects. 
 * We support two methods:
 * 1) memberNames()
 * 2) get("name")
 */
const MethodTable& RevObject::getMethods(void) const
{
    
    static MethodTable methods = MethodTable();
    static bool        methodsSet = false;
    
    if ( methodsSet == false ) {
        
        ArgumentRules* getMembersArgRules = new ArgumentRules();
        ArgumentRules* getMethodsArgRules = new ArgumentRules();
        ArgumentRules* getArgRules = new ArgumentRules();
        
        // add the 'members()' method
        methods.addFunction("members", new MemberProcedure(RlUtils::Void, getMembersArgRules) );
        
        // add the 'members()' method
        methods.addFunction("methods", new MemberProcedure(RlUtils::Void, getMethodsArgRules) );
        
        // add the 'memberNames()' method
        getArgRules->push_back( new ArgumentRule( "name", true, RlString::getClassTypeSpec() ) );
        methods.addFunction("get", new MemberProcedure(RevObject::getClassTypeSpec(), getArgRules) );
        
        methodsSet = true;
    }   
    
    return methods;
}


/**
 * Get the Rev type of the object
 */
const std::string& RevObject::getType( void ) const
{
    
    return getTypeSpec().getType();
}


/**
 * Get the value as a DAG node. This default implementation throws an error.
 */
RevBayesCore::DagNode* RevObject::getDagNode( void ) const
{
    
    throw RbException("RevLanguage only objects cannot be used inside DAG's!");
    
    return NULL;
}


/**
 * Get the number of dimensions. We use 0 for scalars, 1 for vectors,
 * 2 for matrices, etc.
 */
size_t RevObject::getDim( void ) const
{
    return 0;
}


/** Get a constant element value. Default implementation throws an error */
RevPtr<Variable> RevObject::getElement( const std::vector<size_t>& indices )
{
    throw RbException( "Object of type '" + this->getType() + "' does not have elements");
}


/** Does this object have a member called "name" */
bool RevObject::hasMember(std::string const &name) const
{
    
    return false;
}


/** Does this object have an internal DAG node? Default implementation returns false. */
bool RevObject::hasDagNode(void) const
{

    return false;
}


/**
 * Increment operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 */
void RevObject::increment( void ) 
{
    
    throw RbException("Cannot increment a value of type '" + this->getType() + "'.");
    
}


/**
 * The default implementation is that the object is not an abstract type object.
 * Only abstract type objects need to override this.
 */
bool RevObject::isAbstract( void ) const
{
    return false;
}


/**
 * The default implementation is that the variable is constant. Only variables which actually store
 * internally DAG nodes have to ask the DAG nodes if they are constant.
 */
bool RevObject::isConstant( void ) const
{

    return true;
}


/** Is convertible to type? */
bool RevObject::isConvertibleTo(const TypeSpec& type) const
{
    
    return false;
}


/* Are we of specified language type? */
bool RevObject::isTypeSpec(const TypeSpec& typeSpec) const
{
    
    return getTypeSpec().isDerivedOf( typeSpec );
}


/**
  * Converting the value of the internal variable to a constant. The default implementation does nothing because we don't have a DAG node as our internal variable.
  * Note, RevLanguage types which can be used as types in the DAG should overwrite this method.
  */
void RevObject::makeConstantValue( void )
{
    // do nothing

}


/**
 * Make a new object that is an indirect deterministic reference to the object.
 * The default implementation throws an error.
 */
RevObject* RevObject::makeIndirectReference(void)
{
    std::ostringstream msg;
    msg << "The type '" << getClassType() << "' not supported in indirect reference assignments (yet)";
    throw RbException( msg );
}


/**
 * Convert the object to a deterministic object with a userdefined Rev function inside it.
 */
void RevObject::makeDeterministicValue( UserFunctionCall* call, UserFunctionArgs* args )
{
    std::ostringstream msg;
    msg << "The type '" << getClassType() << "' not supported in deterministic nodes (yet)";
    throw RbException( msg );
}


/** Get a deterministic lookup of an element. Default implementation throws an error */
RevObject* RevObject::makeElementLookup( const std::vector< RevPtr<Variable> >& indices )
{
    throw RbException( "Object of type '" + this->getType() + "' does not have elements");
}


/**
 * Multiplication operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 *
 * \param[in]   rhs     The right hand side value of the division operation.
 *
 * \return              NULL
 */
RevObject* RevObject::multiply(const RevObject &rhs) const
{
    throw RbException("Cannot multiply a value of type '" + this->getType() + "' to a value of type '" + rhs.getType() + "'.");
    
    return NULL;
}


/**
 * Print the structural information for this object. Here we print the
 * type and type spec, as well as the value. Objects that have more
 * complex structure need to override this function, best by calling
 * it first and then provide the additional information.
 */
void RevObject::printStructure( std::ostream &o ) const
{
    o << "_RevType      = " << getType() << std::endl;
    o << "_RevTypeSpec  = " << getTypeSpec() << std::endl;
    o << "_value        = ";
    
    std::ostringstream o1;
    printValue( o1 );
    o << StringUtilities::oneLiner( o1.str(), 54 ) << std::endl;
}


/**
 * Replace the variable. This default implementation does nothing.
 */
void RevObject::replaceVariable(RevObject *newVar)
{
    
}


/** Set a member variable */
void RevObject::setConstMember(const std::string& name, const RevPtr<const Variable> &var)
{
    
    // here, we might want to do some general stuff like catching all members so that we can provide general functions as
    // 1) getNames()
    // 2) getMember(name)
    
    setConstMemberVariable(name, var );
    
}


/** Set a member variable */
void RevObject::setConstMemberVariable(const std::string& name, const RevPtr<const Variable> &var)
{
    
    throw RbException("No constant member with name \"" + name + "\" found to set.");
}


/* Set a member variable.
 * In this default implementation, we delegate to setConstMemberVariable.
 * Derived classes of MemberObject who need non-const variable should overwrite this function.
 * If you don't care if the variable is const, then you should only overwrite the setConstMemberVariable.
 */
void RevObject::setMember(const std::string& name, const RevPtr<Variable> &var)
{
    
    // here, we might want to do some general stuff like catching all members so that we can provide general functions as
    // 1) getNames()
    // 2) getMember(name)
    
    setMemberVariable(name, var );
    
}


/* Set a member variable.
 * In this default implementation, we delegate to setConstMemberVariable.
 * Derived classes of MemberObject who need non-const variable should overwrite this function.
 * If you don't care if the variable is const, then you should only overwrite the setConstMemberVariable.
 */
void RevObject::setMemberVariable(const std::string& name, const RevPtr<Variable> &var)
{
    
    setConstMemberVariable(name, RevPtr<const Variable>( var ) );
    
}



/** 
 * Setting the name of the internal variable. The default implementation does nothing.
 * Note: Rev types that can be used as types in the DAG should override this method.
 */
void RevObject::setName(std::string const &n)
{
}


/**
 * Subtraction operation.
 * Since we don't know the types and thus don't know the special behavior we simply throw and error.
 *
 * \param[in]   rhs     The right hand side value of the division operation.
 *
 * \return              NULL
 */
RevObject* RevObject::subtract(const RevObject &rhs) const
{
    throw RbException("Cannot subtract a value of type '" + rhs.getType() + "' from a value of type '" + this->getType() + "'.");
    
    return NULL;
}


/**
 * Get the info about this object as a string.
 * By default we simply call the overloaded operator<<.
 *
 */
std::string RevObject::toString( void ) const
{
    std::stringstream o;
    printValue(o);
    
    return o.str();
}

/** Make sure we can print the value of the object easily */
std::ostream& operator<<(std::ostream& o, const RevObject& x) {
    
    x.printValue(o);
    return o;
}

