/**
 * @file
 * This file contains the declaration of MemberNode, an abstract base
 * class for complex objects with member variables and methods.
 *
 * @todo For now, we do not allow function overloading. Overriding is manual.
 *
 * @brief Declaration of MemberNode
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (M�n, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberNode.h 194 2009-12-14 11:43:32Z ronquist $
 */

#ifndef MemberNode_H
#define MemberNode_H

#include "Frame.h"
#include "MethodTable.h"
#include "RbComplex.h"

#include <set>
#include <string>

class ArgumentRule;
class DAGNode;
class VectorString;

class MemberNode: public RbComplex {

    public:
        virtual                    ~MemberNode(void) {}                                                         //!< Destructor

        // Basic utility functions
        virtual MemberNode*         clone(void) const = 0;                                                        //!< Clone object
        virtual bool                equals(const RbObject* x) const;                                              //!< Equals comparison
        virtual const VectorString& getClass(void) const;                                                         //!< Get class vector
        virtual void                printValue(std::ostream& o) const;                                            //!< Print value for user
        virtual std::string         toString(void) const;                                                         //!< Complete info

        // Member variable functions
        const MemberTable&          getMembers(void) const { return members; }                                    //!< Get members
        virtual const MemberRules&  getMemberRules(void) const = 0;                                               //!< Get member rules
        const RbObject*             getValue(const std::string& name);                                            //!< Get member value
        const RbObject*             getValue(const std::string& name) const;                                      //!< Get member value (const)
        const DAGNode*              getVariable(const std::string& name) const;                                   //!< Get member variable
        void                        setName(const std::string& name);                                             //!< Set base name of variables
        void                        setValue(const std::string& name, RbObject* val);                             //!< Set member value
        virtual void                setVariable(const std::string& name, DAGNode* var);                           //!< Set member variable

        // Member method functions
        DAGNode*                    executeMethod(const std::string& name, int funcId);                           //!< Execute method with preprocessed args
        DAGNode*                    executeMethod(const std::string& name, std::vector<Argument>& args);          //!< Execute method
        virtual const MethodTable&  getMethodInits(void) const = 0;                                               //!< Get method specifications
        const MethodTable&          getMethods(void) const { return methods; }                                    //!< Get methods
        int                         setArguments(const std::string& name, std::vector<Argument>& args);           //!< Set arguments of method

	protected:
									MemberNode(const MemberRules& memberRules, const MethodTable& methodInits); //!< Constructor

        // Protected functions
        virtual DAGNode*            executeOperation(const std::string& name, std::vector<DAGNode*>& args) = 0;   //!< Execute method
        DAGNode*                    getVariable(const std::string& name);                                         //!< Get member variable (non-const)

        // Members keep track of variables and functions belonging to the object
        MemberTable                 members;                                                                      //!< Member variables
        MethodTable                 methods;                                                                      //!< Member methods

        // Friend class
        friend class                DistributionFunction;                                                         //!< Give distribution function access
};

#endif
