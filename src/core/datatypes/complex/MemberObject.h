/**
 * @file
 * This file contains the declaration of MemberObject, an abstract base
 * class for complex objects with member variables and methods.
 *
 * @todo For now, we do not allow function overloading. Overriding is manual.
 *
 * @brief Declaration of MemberObject
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-14 12:43:32 +0100 (Mån, 14 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 *
 * $Id: MemberObject.h 194 2009-12-14 11:43:32Z ronquist $
 */

#ifndef MemberObject_H
#define MemberObject_H

#include "MemberFrame.h"
#include "MethodTable.h"
#include "RbComplex.h"

#include <set>
#include <string>

class ArgumentFrame;
class ArgumentRule;
class DAGNode;
class MethodTable;
class VectorString;

typedef std::vector<ArgumentRule*>   MemberRules;                                                                       //!< Member rules type def, for convenience


class MemberObject: public RbComplex {

    public:
        virtual                    ~MemberObject(void) {}                                                               //!< Destructor

        // Basic utility functions you have to override
        virtual MemberObject*       clone(void) const = 0;                                                              //!< Clone object
        virtual const VectorString& getClass(void) const;                                                               //!< Get class vector
        
        // Basic utility functions you may want to override
        virtual MemberObject*       convertTo(const std::string& type, int dim) const;                                  //!< Convert to type and dim (default throws an error)
        virtual bool                isConvertibleTo(const std::string& type, int dim, bool once) const;                 //!< Is convertible to type and dim? (default false)
        virtual void                printValue(std::ostream& o) const;                                                  //!< Print value for user
        virtual std::string         richInfo(void) const;                                                               //!< Complete info

        // Basic utility functions you do not have to override
        MemberObject*               getConstValue(void) const;                                                          //!< Make a constant clone @Fredrik: the name is confusing to me. Maybe something more like getCloneWithConstValues or simply force the caller to call first clone() and then makeValuesConst()? (Sebastian)
        bool                        isConstant(void) const;                                                             //!< Are all members constant? @Fredrik: Better name: areMemberConstant()? (Sebastian)

        // Member variable functions; override getMemberRules to add member variables
        const MemberFrame&          getMembers(void) const { return members; }                                          //!< Get members
        virtual const MemberRules&  getMemberRules(void) const;                                                         //!< Get member rules
        const TypeSpec&             getMemberTypeSpec(const std::string& name) const;                                   //!< Get type spec for a member variable
        const RbObject*             getValue(const std::string& name);                                                  //!< Get member value @Fredrik: Why is there a const getValue() if there is also a non-const getValue()? (Sebastian)
        const RbObject*             getValue(const std::string& name) const;                                            //!< Get member value (const)
        const DAGNode*              getVariable(const std::string& name) const;                                         //!< Get member variable @Fredrik: Why is there a const getVariable() if there is also a non-const getVariable()? (Sebastian)
        DAGNode*                    getVariable(const std::string& name);                                               //!< Get member variable (non-const ptr)
        void                        setValue(const std::string& name, RbObject* val);                                   //!< Set member value
        virtual void                setVariable(const std::string& name, DAGNode* var);                                 //!< Set member variable

        // Member method functions
        DAGNode*                    executeMethod(const std::string& name, const std::vector<Argument>& args);          //!< Direct call of member method
        virtual const MethodTable&  getMethods(void) const;                                                             //!< Get member methods

        // Subscript operator functions
        virtual bool                hasSubscript(void) { return false; }                                                //!< Does object support subscripting?
        virtual DAGNode*            getSubelement(const size_t i);                                                      //!< Return subscript[](index) element
        virtual size_t              getSubelementsSize(void) const { return 0; }                                        //!< Number of subscript elements
        virtual void                setElement(VectorNatural& index, DAGNode* var, bool convert=true);                  //!< Set subelement
        virtual void                setSubelement(size_t index, DAGNode* var, bool convert=true);                       //!< Set subelement

    protected:
									MemberObject(const MemberRules& memberRules);                                       //!< Standard constructor
                                    MemberObject(void){}                                                                //!< Default constructor; no members or methods
 
        // Override this function to map member method calls to internal functions
        virtual DAGNode*            executeOperation(const std::string& name, ArgumentFrame& args);                     //!< Map member methods to internal functions

        // Members is the variable frame that stores member variables
        MemberFrame                 members;                                                                            //!< Member variables

        // Friend classes
        friend class                DistributionFunction;                                                               //!< Give DistributionFunction access
        friend class                MemberFunction;                                                                     //!< Member functino needs access to executeOperation
        friend class                MemberNode;                                                                         //!< Give MemberNode access
};

#endif

