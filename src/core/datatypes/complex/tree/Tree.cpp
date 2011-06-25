/*
 *  Tree.cpp
 *  revbayes_xcode
 *
 *  Created by John Huelsenbeck on 8/24/10.
 *  Copyright 2010 University of California, Berkeley. All rights reserved.
 *
 */

#include "ArgumentRule.h"
#include "ConstantNode.h"
#include "ContainerNode.h"
#include "Integer.h"
#include "MemberFunction.h"
#include "RbException.h"
#include "RbNames.h"
#include "Tree.h"
#include "TreeNode.h"
#include "VectorString.h"

/** Default constructor */
Tree::Tree(void) : MemberObject( getMemberRules() ) {
    
}

/** constructor */
Tree::Tree(const MemberRules& memberRules) : MemberObject( memberRules ) {
    
}

/** copy constructor */
Tree::Tree(const Tree& t) : MemberObject( getMemberRules() ) {
    
}


void Tree::allocateNodes(int nNodes) {
    
    for (int i=0; i<nNodes; i++)
        nodes.push_back( new TreeNode );
}

MemberObject* Tree::convertTo(std::string const &type, size_t dim) const {
    return NULL;
}

void Tree::deleteNodes(void) {
    
    for (std::vector<TreeNode*>::iterator it=nodes.begin(); it != nodes.end(); it++)
        delete (*it);
    nodes.clear();
}

const VectorString& Tree::getClass(void) const {
    
    static VectorString rbClass = VectorString(Tree_name) + MemberObject::getClass();
    return rbClass;
}

bool Tree::isConvertibleTo(const std::string& type, size_t dim, bool once) const {
    return false;
}

/** Do not allow the user to set the index */
void Tree::setVariable(const std::string& name, DAGNode* var) {
    
    MemberObject::setVariable(name, var);
}

/** Map calls to member methods */
DAGNode* Tree::executeOperation(const std::string& name, ArgumentFrame& args) {
    
    if (name == "getNodes") 
    {
        ContainerNode* plate = new ContainerNode(TreeNode_name, int(nodes.size()));
        for (size_t i=0; i<nodes.size(); i++) 
        {
//            plate[i].setValue(nodes[i]);
        }
        return plate;
    }
    else if (name == "getNumberOfNodes") 
    {
        return new ConstantNode( new Integer((int)nodes.size()));
    }
    else if (name == "getRoot") 
    {
        return new ConstantNode( root );
    }
    else if (name == "getTips") 
    {
        ContainerNode* plate = new ContainerNode(TreeNode_name,numberOfTips);
        int j = 0;
        for (size_t i=0; i<nodes.size(); i++) 
        {
            if ( nodes[i]->isTip() ) 
            {
 //               plate[j].setValue(nodes[i]);
                j++;
            }
        }
        return plate;
    }
    else
        throw RbException("No member method called '" + name + "' for object of type Tree");
    
    return NULL;
}

/** Get method specifications */
const MethodTable& Tree::getMethods(void) const {
    
    static MethodTable      methodInits;
    static ArgumentRules    isTipRules;
    static bool             initsSet = false;
    
    if (!initsSet) 
    {
        
        // add a function returning all nodes of the tree
        methodInits.addFunction("getNodes", new MemberFunction(TreeNode_name, ArgumentRules()));
        
        // add a function returning the number of nodes
        methodInits.addFunction("getNumberOfNodes", new MemberFunction(Boolean_name, ArgumentRules()));
        
        // add a function returning the root of the tree
        methodInits.addFunction("getRoot", new MemberFunction(TreeNode_name, ArgumentRules()));
        
        // add a function returning all the tips
        methodInits.addFunction("getTips", new MemberFunction(Boolean_name, ArgumentRules()));
        
        
        //        methodInits.addFunction("getNumberOfNodes", new MemberFunction(Boolean_name, ArgumentRules()));
        
        initsSet = true;
    }
    
    return methodInits;
}

const MemberRules& Tree::getMemberRules(void) const {
    
    static MemberRules memberRules;
    static bool        rulesSet = false;
    
    if (!rulesSet) 
    {
        
        rulesSet = true;
    }
    
    return memberRules;
}

void Tree::printValue(std::ostream& o) const {
    o << newick;
}

std::string Tree::richInfo(void) const {
    return newick;
}