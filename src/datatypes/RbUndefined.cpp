#include "RbException.h"
#include "RbNames.h"
#include "RbUndefined.h"
#include "StringVector.h"



const StringVector RbUndefined::rbClass = StringVector(RbNames::Undefined::name) + RbObject::rbClass;

RbUndefined::RbUndefined(void) {

}

RbUndefined::RbUndefined(const RbUndefined& v) {

}

bool RbUndefined::equals(const RbObject* obj) const {
	return  obj->isType(RbNames::Undefined::name);
}

void RbUndefined::printValue(std::ostream& o) const {

	o << RbNames::Undefined::name << std::endl;
}

std::string RbUndefined::toString(void) const {

	return RbNames::Undefined::name;
}

RbObject& RbUndefined::operator=(const RbObject& o) {

	if (equals(&o)) {
		return (*this);
	}
	else {
		throw RbException("Cannot assign " + o.getClass()[0] + " to " + RbNames::Undefined::name);
	}
}

RbUndefined& RbUndefined::operator=(const RbUndefined& o) {
	return (*this);
}
