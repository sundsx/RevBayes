//
//  RlContinuousCharacterData.cpp
//  revbayes
//
//  Created by Nicolas Lartillot on 2014-03-27.
//  Copyright (c) 2014 revbayes team. All rights reserved.
//

#include "RlContinuousCharacterData.h"



#include "ConstantNode.h"
#include "RlContinuousCharacterData.h"
#include "RbUtil.h"
#include "TypeSpec.h"

#include <sstream>

using namespace RevLanguage;

/** Default constructor */
ContinuousCharacterData::ContinuousCharacterData(void) : ModelObject<RevBayesCore::ContinuousCharacterData>() {
    
}

/** Construct from bool */
ContinuousCharacterData::ContinuousCharacterData(const RevBayesCore::ContinuousCharacterData *d) : ModelObject<RevBayesCore::ContinuousCharacterData>( new RevBayesCore::ContinuousCharacterData(*d) ) {
    
}


/** Clone object */
ContinuousCharacterData* ContinuousCharacterData::clone(void) const {
    
	return new ContinuousCharacterData(*this);
}


/** Convert to type. The caller manages the returned object. */
RevObject* ContinuousCharacterData::convertTo(const TypeSpec& type) const {
    
    return RevObject::convertTo(type);
}


/** Get Rev type of object */
const std::string& ContinuousCharacterData::getClassType(void) {
    
    static std::string revType = "ContinuousCharacterData";
    
	return revType;
}

/** Get class type spec describing type of object */
const TypeSpec& ContinuousCharacterData::getClassTypeSpec(void) {
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( RevObject::getClassTypeSpec() ) );
    
	return revTypeSpec;
}

/** Get type spec */
const TypeSpec& ContinuousCharacterData::getTypeSpec( void ) const {
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}



/** Is convertible to type? */
bool ContinuousCharacterData::isConvertibleTo(const TypeSpec& type) const {
    
    return RevObject::isConvertibleTo(type);
}

