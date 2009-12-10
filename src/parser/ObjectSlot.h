/**
 * @file
 * This file contains the declaration of ObjectSlot, which is
 * a slot for a variable. It contains a specification of the
 * declared (required) type and dimensions of the object, in
 * addition to the current value.
 *
 * @brief Declaration of ObjectSlot
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The RevBayes development core team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-20, version 1.0
 *
 * $Id$
 */

#ifndef ObjectSlot_H
#define ObjectSlot_H

#include "RbInternal.h"

#include <ostream>
#include <string>

class RbObject;
class RbObjectWrapper;
class StringVector;

class ObjectSlot {

    public:
            ObjectSlot(const std::string valType, int elemDim);     //!< Constructor with NULL init
            ObjectSlot(RbObjectWrapper* initVariable);              //!< Constructor with variable init
            ObjectSlot(RbObject* initValue);                        //!< Constructor with value init
            ObjectSlot(const ObjectSlot& x);                        //!< Copy constructor 
            ~ObjectSlot() { delete variable; }                      //!< Destructor should delete variable

        // Assignment operator
        ObjectSlot&             operator=(const ObjectSlot& x);             //!< Assignment operator

        // Basic utility functions
        ObjectSlot*             clone() const { return new ObjectSlot(*this); } //!< Clone object
        std::string             toString() const;                           //!< Complete object info

        // Regular functions
        int                     getDim() const { return dim; }              //!< Get number of dimensions
        const std::string&      getAtomicType() const { return atomicType; }//!< Get atomic type
        std::string             getTypeDescr() const;                       //!< Get atomic type + "[][]..."
        const RbObject*         getValue() const;                           //!< Get value (const pointer)
        const RbObjectWrapper*  getVariable() const { return variable; }    //!< Get variable (const pointer)
        const RbObject*         getValElement(const IntVector& index);      //!< Get value element
        const RbObjectWrapper*  getVarElement(const IntVector& index);      //!< Get variable element
        void                    setValue(RbObjec* value);                   //!< Set value
        void                    setVariable(RbObjectWrapper* var);          //!< Set variable
        void                    setValElement(const IntVector& index, RbObject* val);       //!< Set value element
        void                    setVarElement(const IntVector& index, RbObjectWrapper* var);//!< Set variable elem

    protected:
        bool                    isMatchingType(const StringVector& class) const;            //!< Test type

    private:
	    std::string         atomicType;     //!< Declared atomic type of the variable
        int                 dim;            //!< Declared dimension of the variable
        RbObjectWrapper*    variable;       //!< Current variable held by the slot
};

#endif
