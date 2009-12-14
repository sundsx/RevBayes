/**
 * @file
 * This file contains the declaration of SyntaxClassDef, which is
 * used to hold the definitions of user-defined types.
 *
 * @brief Declaration of SyntaxClassDef
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 *
 * $Id$
 */

#ifndef SyntaxClassDef_H
#define SyntaxClassDef_H

#include "RbString.h"
#include "SyntaxElement.h"
#include "SyntaxFormal.h"
#include "SyntaxVariable.h"

#include <ostream>
#include <list>
#include <string>


class SyntaxClassDef : public SyntaxElement {

    public:
            SyntaxClassDef(     RbString* name,
                                RbString* base,
                                std::list<SyntaxElement*>* defs);   //!< Standard constructor
            SyntaxClassDef(const SyntaxClassDef& x);                //!< Copy constructor
	        virtual ~SyntaxClassDef();                              //!< Destructor

        // Basic utility functions
        std::string     briefInfo() const;                          //!< Brief info about object
        SyntaxElement*  clone() const;                              //!< Clone object
        bool            equals(const SyntaxElement* elem) const;    //!< Equals comparison
        void            print(std::ostream& o) const;               //!< Print info about object

        // Regular functions
        DAGNode*        getDAGNode(Frame* frame=NULL) const;        //!< Convert to DAG node
        RbObject*       getValue(Frame* frame=NULL) const;          //!< Get semantic value

    protected:
        RbString*                       className;                  //!< The name of the class
        RbString*                       baseClass;                  //!< The name of the base class
        std::list<SyntaxElement*>*      definitions;                //!< Defined member variables and functions
};

#endif
