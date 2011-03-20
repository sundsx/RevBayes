/**
 * @file
 * This file contains the implementation of RealPos, which is
 * used to hold strictly positive scalar values.
 *
 * @brief Implementation of RealPos
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


#include "RealPos.h"
#include "RbException.h"
#include "RbNames.h"
#include "VectorString.h"

#include <sstream>



/** Default constructor */
RealPos::RealPos(void) : Real(1.0) {

}

/** Construct from double */
RealPos::RealPos(const double v) : Real(v) {

    if (v <= 0.0)
        throw RbException("Nonpositive value for " + RealPos_name);
}

/** Construct from int */
RealPos::RealPos(const int v) : Real(v) {

    if (v <= 0)
        throw RbException("Nonpositive value for " + RealPos_name);
}

/** Clone object */
RbObject* RealPos::clone(void) const {

	return  (RbObject*)(new RealPos(*this));
}


/** Get class vector describing type of object */
const VectorString& RealPos::getClass() const {

    static VectorString rbClass = VectorString(RealPos_name) + Real::getClass();
    return rbClass;
}


/** Get complete info about object */
std::string RealPos::richInfo(void) const {

	std::ostringstream o;
    o << "+Real(";
    printValue(o);
	o << ")";

    return o.str();
}


/** Set value */
void RealPos::setValue(double x) {

    if (x <= 0.0)
        throw RbException("Nonpositive value for " + RealPos_name);

    Real::setValue(x);
}

