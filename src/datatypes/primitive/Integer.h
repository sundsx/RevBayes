/**
 * @file
 * This file contains the declaration of Integer, which is
 * a RevBayes wrapper around a regular int.
 *
 * @brief Declaration of Integer
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-11-20, version 1.0
 * @extends RbObject
 *
 * $Id$
 */

#ifndef Integer_H
#define Integer_H

#include "RbObject.h"

#include <ostream>
#include <string>

class VectorString;

class Integer : public RbObject {

    public:
                                    Integer(void);                                  //!< Default constructor
                                    Integer(const int v);                           //!< Constructor from int
                                    Integer(const bool v);                          //!< Constructor from bool

        // Basic utility functions
        RbObject*                   clone(void) const;                               //!< Clone object
        bool                        equals(const RbObject* obj) const;               //!< Equals comparison
        const VectorString&         getClass(void) const;                            //!< Get class vector
        void                        printValue(std::ostream& o) const;               //!< Print value (for user)
        std::string                 richInfo(void) const;                            //!< Complete info about object

        // Type conversion
        RbObject*                   convertTo(const std::string& type, int dim=0) const;                        //!< Convert to type and dim
        bool                        isConvertibleTo(const std::string& type, int dim=0, bool once=false) const; //!< Is convertible to type and dim?
                                    operator int(void) const { return value; }       //!< Type conversion to int

        // Getters and setters
        void                        setValue(int x) { value = x; }                   //!< Set value
        int                         getValue(void) const { return value; }           //!< Get value

	protected:

    private:
        int                         value;                                           //!< Value member
};

#endif

