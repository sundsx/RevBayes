/**
 * @file
 * This file contains the implementation of FileMonitor, used to save
 * information to file about the monitoring of a variable DAG node.
 *
 * @brief Implementation of FileMonitor
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id$
 */

#include "DagNodeContainer.h"
#include "Ellipsis.h"
#include "Integer.h"
#include "FileMonitor.h"
#include "RbException.h"
#include "RbUtil.h"
#include "ValueRule.h"
#include "VariableNode.h"
#include "VectorString.h"

#include <sstream>


// Definition of the static type spec member
const TypeSpec FileMonitor::typeSpec(FileMonitor_name);

/** Constructor */
FileMonitor::FileMonitor(void) : Monitor(getMemberRules() ) {
    
}

/** Copy Constructor */
FileMonitor::FileMonitor(const FileMonitor &x) : Monitor(x) {
    
    // shallow copy
    separator = x.separator;
    
}

FileMonitor::~FileMonitor() {
   
}


/** Clone the object */
FileMonitor* FileMonitor::clone(void) const {

    return new FileMonitor(*this);
}


/** Get class vector describing type of object */
const VectorString& FileMonitor::getClass() const {

    static VectorString rbClass = VectorString(FileMonitor_name) + ConstantMemberObject::getClass();
    return rbClass;
}

/** Return member rules */
RbPtr<const MemberRules> FileMonitor::getMemberRules( void ) const {
    
    static RbPtr<MemberRules> memberRules( new MemberRules() );
    static bool        rulesSet = false;
    
    if (!rulesSet) 
    {
        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "filename"  , TypeSpec(RbString_name)         ) ) );
        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "printgen"  , TypeSpec(Integer_name)          ) ) );
        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "separator" , TypeSpec(RbString_name), RbPtr<RbLanguageObject>(new RbString("\t") ) ) ) );
        memberRules->push_back( RbPtr<ArgumentRule>( new ValueRule( "variable"  , TypeSpec(RbLanguageObject_name) ) ) );
        memberRules->push_back( RbPtr<ArgumentRule>( new Ellipsis (               TypeSpec(RbLanguageObject_name) ) ) );
        rulesSet = true;
    }
    
    return RbPtr<const MemberRules>( memberRules );
}


/** Get the type spec of this class. We return a static class variable because all instances will be exactly from this type. */
const TypeSpec& FileMonitor::getTypeSpec(void) const {
    return typeSpec;
}


/** Monitor value unconditionally */
void FileMonitor::monitor(void) {

    for (std::vector<RbPtr<VariableNode> >::const_iterator it=nodes.begin(); it!=nodes.end(); it++) {
        // add a separator before every new element except the first element
        if ( it != nodes.begin() )
            outStream << separator;
        
        // print the value
        (*it)->printValue(outStream);
    }
    
    outStream << std::endl;
    
    
}


/** Monitor value at generation gen */
void FileMonitor::monitor(int gen) {

    // get the printing frequency
    int samplingFrequency = dynamic_cast<const Integer*>( (const RbObject*)getMemberValue("printgen") )->getValue();
    
    if (gen % samplingFrequency == 0) {
        // print the iteration number first
        outStream << gen;
        
        for (std::vector<RbPtr<VariableNode> >::const_iterator it=nodes.begin(); it!=nodes.end(); it++) {
            // add a separator before every new element
            outStream << separator;
            
            // print the value
            (*it)->printValue(outStream);
        }

	outStream << std::endl;
    }
}


/** open the file stream for printing */
void FileMonitor::openStream(void) {

    // get the filename
    std::string filename = dynamic_cast<const RbString*>( (const RbObject*)getMemberValue("filename") )->getValue();
    
    // open the stream to the file
    outStream.open(filename.c_str());
    
}

/** Print header for monitored values */
void FileMonitor::printHeader() {
  
    // print one column for the iteration number
    outStream << "Sample";
    
    for (std::vector<RbPtr<VariableNode> >::const_iterator it=nodes.begin(); it!=nodes.end(); it++) {
        // add a separator before every new element
        outStream << separator;
        
         RbPtr<VariableNode> theNode = *it;
        
        // print the header
        if (theNode->getName() != "")
            outStream << theNode->getName();
        else
            outStream << "Unnamed " << theNode->getType();
    }
    
    outStream << std::endl;
}


/** Print value for user */
void FileMonitor::printValue(std::ostream& o) const {
    
    // get the printing frequency
    int samplingFrequency = dynamic_cast<const Integer*>( (const RbObject*)getMemberValue("printgen") )->getValue();
    
    o << "Monitor: interval = " << samplingFrequency;
}


/** Complete info about object */
std::string FileMonitor::richInfo(void) const {
    
    // get the printing frequency
    int samplingFrequency = dynamic_cast<const Integer*>( (const RbObject*)getMemberValue("printgen") )->getValue();
    
    std::ostringstream o;
    o << "Monitor: interval = " << samplingFrequency;

    return o.str();
}

void FileMonitor::setMemberVariable(std::string const &name, RbPtr<Variable> var) {
    
    // catch setting of the variables 
    if (name == "variable" || name == "") {
        RbPtr<DAGNode> theNode = var->getDagNode();
        if (theNode->getValue()->isType(DagNodeContainer_name)) {
            RbPtr<DagNodeContainer> theContainer( static_cast<DagNodeContainer*>( (RbLanguageObject*)theNode->getValue() ) );
            for (size_t i = 0; i < theContainer->size(); i++) {
                theNode = static_cast<VariableSlot*>( (RbObject*)theContainer->getElement(i) )->getDagNode();
                if (theNode->isType(VariableNode_name)) {
                    nodes.push_back( RbPtr<VariableNode>( static_cast<VariableNode*>( (DAGNode*)theNode ) ) );
//                } else {
//                    throw RbException("Cannot monitor a constant node!");
                }
            }
        }
        else {
            if (theNode->isType(VariableNode_name)) {
                nodes.push_back( RbPtr<VariableNode>( static_cast<VariableNode*>( (DAGNode*)theNode ) ) );
//            } else {
//                throw RbException("Cannot monitor a constant node!");
            }
        }
    } 
    else if (name == "separator") {
        separator = static_cast<RbString*>( (RbObject*)var->getValue() )->getValue();
        
        // call parent class to set member variable
        ConstantMemberObject::setMemberVariable( name, var );
    } 
    else {
        // call parent class to set member variable
        ConstantMemberObject::setMemberVariable( name, var );
    }
}

