/**
 * @file
 * This file contains the implementation of Environment, which
 * is the abstract base class of Environments and workspaces.
 *
 * @brief Implementation of Environment
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-17, version 1.0
 *
 * $Id$
 */

#include "Environment.h"
#include "RbException.h"
#include "RbNames.h"
#include "RbOptions.h"
#include "Variable.h"
#include "VariableSlot.h"
#include "VectorString.h"


/** Destructor */
Environment::~Environment() {
    
    // delete the parent environment
    if (parentEnvironment != NULL) {
        parentEnvironment->release();
        if (parentEnvironment->isUnreferenced()){
            delete parentEnvironment;
        }
    }
    
    // clear the variable table
    clear();

}


/** Construct Environment with NULL parent */
Environment::Environment(void) : RbInternal(), parentEnvironment(NULL) {
}


/** Construct Environment with parent */
Environment::Environment(Environment* parentFr) : RbInternal(), parentEnvironment(parentFr) {
    parentEnvironment->retain();
}

/** Copy Constructor */
Environment::Environment(const Environment &x): RbInternal(x) {
    
    // make a deep copy of the parent environment
    if (x.parentEnvironment != NULL) {
        parentEnvironment = x.parentEnvironment->clone();
        parentEnvironment->retain();
    }
    else {
        parentEnvironment = NULL;
    }
    
    // make a deep copy of the variable names
    varNames = x.varNames;
    
    // make a deep copy of the variable table
    for (size_t i=0; i<x.size(); i++) {
        // get the name of the i-th variable
        const std::string &name = x.getName(i);
        
        // get a copy of the variable slot
        VariableSlot *slotCopy = x[name].clone();
        
        // retain the slot
        slotCopy->retain();
        
        // add the copy
        variableTable.insert( std::pair<std::string, VariableSlot*>( name, slotCopy ) );
        
    }
    
}

/** Index operator to variable slot from string */
VariableSlot& Environment::operator[]( const std::string& name ) {
    
    std::map<std::string, VariableSlot*>::iterator it = variableTable.find(name);
    if ( variableTable.find(name) == variableTable.end() ) {
        if ( parentEnvironment != NULL )
            return parentEnvironment->operator []( name );
        else
            throw RbException( RbException::MISSING_VARIABLE, "Variable " + name + " does not exist" );
    }
    
    PRINTF( "Retrieving \"%s\" from frame\n", name.c_str() );
    
    return *(*it).second;
}


/** Index operator (const) to variable slot from string */
const VariableSlot& Environment::operator[]( const std::string& name ) const {
    
    std::map<std::string, VariableSlot*>::const_iterator it = variableTable.find( name );
    if ( variableTable.find(name) == variableTable.end() ) {
        if ( parentEnvironment != NULL )
            return parentEnvironment->operator []( name );
        else
            throw RbException( RbException::MISSING_VARIABLE, "Variable " + name + " does not exist" );
    }
    
    PRINTF( "Retrieving %s %s from frame\n", it->second->getTypeSpec().toString().c_str(), name.c_str() );
    
    return *(*it).second;
}


/** Index operator to variable slot from int */
VariableSlot& Environment::operator[]( const size_t index ) {
    
    // get the name at the index
    const std::string &name = getName(index);
    
    return operator[](name);
}


/** Index operator (const) to variable slot from int */
const VariableSlot& Environment::operator[]( const size_t index ) const {
    
    // get the name at the index
    const std::string &name = getName(index);
    
    return operator[](name);
}

/** Add variable */
void Environment::addVariable(const std::string& n, VariableSlot *theSlot) {
    
    std::string name = n;
    // we check if the name equals the empty string
    // if so we replace it with the memory address of the slot we want to insert
    // because we cannot add variable slot with an empty string as the identifier
    // caller will not be able to retrieve this slot via it's name
    // but it is their own fault if they tried to add it without a name to identify with
    if (name == EmptyString) {
        // we do not have a name for the variable so we use the memory address
        long tmp = long(theSlot);
        std::stringstream out;
        out << tmp;
        name = out.str();
    }
    /* Throw an error if the variable exists. Note that we cannot use the function
     existsVariable because that function looks recursively in parent frames, which
     would make it impossible to hide global variables. */
    if ( variableTable.find( name ) != variableTable.end() )
        throw RbException( "Variable " + name + " already exists in frame" );
    
    // retain the slot
    theSlot->retain();
    
    /* Insert new slot in variable table */
    variableTable.insert( std::pair<std::string, VariableSlot*>( name, theSlot ) );
    
    // add the name to the variable name list
    varNames.push_back(name);
    
    PRINTF( "Inserted \"%s\" in frame\n", name.c_str() );
}


/** Add variable */
void Environment::addVariable(const std::string& name, const TypeSpec& typeSp, Variable *theVar) {
    
    // create a new slot
    VariableSlot *theSlot = new VariableSlot(name,typeSp,theVar);
    
    // call function to add the slot
    addVariable(name, theSlot);
}

/** Add variable */
void Environment::addVariable(const std::string& name, Variable *theVar) {
    
    addVariable( name, TypeSpec(RbObject_name), theVar );
}

/** Add variable to frame */
void Environment::addVariable( const std::string& name, const TypeSpec& typeSp, DAGNode* dagNode ) {
    // create a new variable object
    Variable *var = new Variable(name, dagNode);
    
    // add the object to the list
    addVariable(name, typeSp, var );
}

/** Add variable to frame */
void Environment::addVariable( const std::string& name, const TypeSpec& typeSp ) {
    // create a new variable object
    Variable *var = new Variable( name );
    
    // add the object to the list
    addVariable(name, typeSp, var );
}

/** clone */
Environment* Environment::clone() const {
    return new Environment(*this);
}


/** Clear the variable table */
void Environment::clear(void) {
    // empty the variable table
    for (size_t i=0; i<size(); i++) {
        // get the name of the i-th variable
        const std::string &name = getName(i);
        
        // get a copy of the variable slot
        VariableSlot *theSlot = &operator[](name);
        
        // release the slot
        theSlot->release();
        if (theSlot->isUnreferenced()) {
            delete theSlot;
        }
    }
    
    // empty the two vectors
    variableTable.clear();
    varNames.clear();
}


/** Erase variable */
void Environment::eraseVariable( const std::string& name ) {
    
    std::map<std::string, VariableSlot*>::iterator it = variableTable.find( name );
    if ( it == variableTable.end() )
        throw RbException( RbException::MISSING_VARIABLE, "Variable " + name + " does not exist in frame" );
    
    PRINTF( "Erasing %s %s from frame\n", name.c_str(), it->second->getTypeSpec().toString().c_str() );
    
    variableTable.erase( it );
    
    // free the memory
    if (it->second != NULL) {
        it->second->release();
        if(it->second->isUnreferenced()) {
            delete ( (*it).second );
        }
    }
    
    // remove the name from the var name list
    for (std::vector<std::string>::iterator it=varNames.begin(); it!=varNames.end(); it++) {
        if (*it == name) {
            varNames.erase(it);
            break;
        }
    }
}


/** Does variable exist in the Environment (current frame and enclosing frames)? */
bool Environment::existsVariable( const std::string& name ) const {
    
    if ( variableTable.find(name) == variableTable.end() ) {
        if ( parentEnvironment != NULL )
            return parentEnvironment->existsVariable( name );
        else
            return false;
    }
    
    return true;
}


/** Get class vector describing type of object */
const VectorString& Environment::getClass() const {
    
    static VectorString rbClass = VectorString(Environment_name) + RbInternal::getClass();
    return rbClass;
}



/** Get reference, alternative method */
DAGNode* Environment::getDagNodePtr( const std::string& name ) const {
    
    return operator[]( name ).getDagNodePtr();
}


/** Get variable, alternative method */
const DAGNode* Environment::getDagNode( const std::string& name ) const {
    
    return operator[]( name ).getDagNode();
}


/** Get value, alternative method */
const RbLanguageObject* Environment::getValue( const std::string& name ) const {
    
    // find the variable slot first
    std::map<std::string, VariableSlot*>::const_iterator it = variableTable.find( name );
    if ( variableTable.find(name) == variableTable.end() ) {
        if ( parentEnvironment != NULL )
            return parentEnvironment-> getValue( name );
        else
            throw RbException( RbException::MISSING_VARIABLE, "Variable " + name + " does not exist" );
    }
    
    // set the slot
    VariableSlot *theSlot = it->second;
    return theSlot->getValue();
}


/** 
 * Is Environment same or parent of otherEnvironment? We use this function
 * to decide when a reference from otherEnvironment to a variable in this
 * Environment is safe, and when it is not. The only time we know for sure
 * that it is safe is when this Environment is identical to, or a parent of,
 * otherEnvironment.
 */
bool Environment::isSameOrParentOf( Environment* otherEnvironment ) const {
    
    if (otherEnvironment == NULL) 
        return false;
    
    if ( this == otherEnvironment )
        return true;
    
    if ( otherEnvironment->parentEnvironment == NULL )
        return false;
    
    return isSameOrParentOf( otherEnvironment->parentEnvironment );
}


/** Print workspace */
void Environment::printValue(std::ostream& o) const {
    
    o << "Variable table:" << std::endl;
    VariableTable::const_iterator it;
    for ( it = variableTable.begin(); it != variableTable.end(); it++) {
        o << (*it).first << " = ";
        (*it).second->printValue( o );
        o << std::endl;
    }
    o << std::endl;
    
}


/** Complete info about object to string */
std::string Environment::richInfo( void ) const {
    
    std::ostringstream o;
    o << "Environment:" << std::endl;
    printValue( o );
    
    return o.str();
}


/** Replace the name of a variable */
void Environment::setName(size_t i, const std::string &name) {
    
    // get the old name
    const std::string &oldName = varNames[i];
    
    // get the variable slot associated with the old name
    VariableSlot &theSlot = operator[](oldName);
    theSlot.retain();
    
    // remove the entry with the old name
    eraseVariable(oldName);
    
    // insert the slot with its new name
    variableTable.insert( std::pair<std::string, VariableSlot*>( name, &theSlot ) );
    
    // insert the name at it's old position
    varNames.insert(varNames.begin() + i, name);
}