/**
 * @file
 * This file contains the implementation of Matrix,
 * a container type used to hold value matrices.
 *
 * @brief Implementation of Matrix
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2010-08-20, version 1.0
 * @extends RbComplex
 *
 * $Id$
 */

#include "Matrix.h"
#include "RbException.h"
#include "RbNames.h"
#include "TypeSpec.h"
#include "VectorInteger.h"
#include "VectorString.h"

#include <sstream>



/** Default constructor for matrix */
Matrix::Matrix(const std::string& elemType)
    : ValueContainer(TypeSpec(elemType, 2)) {
}


/** Get class vector describing type of object */
const VectorString& Matrix::getClass(void) const {

    static VectorString rbClass = VectorString(Matrix_name) + ValueContainer::getClass();
    return rbClass;
}


/** Generic transpose function for all matrices */
void Matrix::transpose(void) {

}
