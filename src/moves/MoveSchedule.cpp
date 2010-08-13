/**
 * @file
 * This file contains the implementation of MoveSchedule, which is a
 * complex type holding an update weight and a set of moves for either
 * a stochastic DAG node or a set of stochastic DAG nodes retrieved
 * through a complex object like a tree.
 *
 * @brief Declaration of MoveSchedule
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes core development team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-09-08, version 1.0
 *
 * $Id$
 */

#include "Move.h"
#include "MoveSchedule.h"
#include "RbException.h"
#include "RbNames.h"
#include "StringVector.h"
#include "VariableNode.h"

#include <algorithm>


/** Constructor */
MoveSchedule::MoveSchedule(VariableNode* node, double weight)
    : RbComplex() {

    theNode = node;
    nodeUpdateWeight = weight;
}


/** Copy constructor: copy moves */
MoveSchedule::MoveSchedule(const MoveSchedule& x) {

    theNode          = x.theNode;
    nodeUpdateWeight = x.nodeUpdateWeight;

    for (std::vector<Move*>::const_iterator i=x.schedule.begin(); i!=x.schedule.end(); i++)
        schedule.push_back((Move*)((*i)->clone()));
}


/** Destructor: delete moves */
MoveSchedule::~MoveSchedule(void) {

    for (std::vector<Move*>::iterator i=schedule.begin(); i!=schedule.end(); i++)
        delete (*i);
}


/** Assignment operator */
MoveSchedule& MoveSchedule::operator=(const MoveSchedule& x) {

    if (this != &x) {
        theNode          = x.theNode;
        nodeUpdateWeight = x.nodeUpdateWeight;

        for (std::vector<Move*>::iterator i=schedule.begin(); i!=schedule.end(); i++)
            delete (*i);

        schedule.clear();

        for (std::vector<Move*>::const_iterator i=x.schedule.begin(); i!=x.schedule.end(); i++)
            schedule.push_back((Move*)((*i)->clone()));
    }

    return (*this);
}


/** Add move */
void MoveSchedule::addMove(Move* move) {

    schedule.push_back(move);
}


/** Clone object */
RbObject* MoveSchedule::clone(void) const {

    return (RbObject*)(new MoveSchedule(*this));
}


/** Get class vector describing type of object */
const StringVector& MoveSchedule::getClass(void) const { 

    static StringVector rbClass = StringVector(MoveSchedule_name) + RbComplex::getClass();
	return rbClass;
}


/** Pointer-based equals comparison */
bool MoveSchedule::equals(const RbObject* obj) const {

    const MoveSchedule* p = dynamic_cast<const MoveSchedule*>(obj);
    if (p == NULL)
        return false;

    bool result = true;
    result = result && theNode == p->theNode;
    result = result && nodeUpdateWeight == p->nodeUpdateWeight;
    std::vector<Move*>::const_iterator i, j;
    for (i=schedule.begin(), j=p->schedule.begin(); i!=schedule.end(); i++, j++)
        result = result && (*i)->equals(*j);

    return result;
}


/** Erase move */
void MoveSchedule::eraseMove(const Move* move) {

    std::vector<Move*>::iterator i = find(schedule.begin(), schedule.end(), move);
    if (i == schedule.end())
        throw RbException("Move does not exist in move schedule");

    schedule.erase(i);
}


/** Print value for user */
void MoveSchedule::printValue(std::ostream& o) const {

    o << "Move schedule with " << schedule.size() << " moves; update weight = " << nodeUpdateWeight << std::endl;
}


/** Set update weight */
void MoveSchedule::setUpdateWeight(double weight) {

    if (weight < 0.0)
        throw RbException("Invalid assignment: negative update weight");

    nodeUpdateWeight = weight;
}


/** Complete info about object */
std::string MoveSchedule::toString(void) const {

    std::ostringstream o;
    o << "MoveSchedule:" << std::endl;
    o << "theNode          = " << theNode->getValue() << std::endl;
    o << "schedule         = Vector with " << schedule.size() << " moves" << std::endl;
    o << "nodeUpdateWeight = " << nodeUpdateWeight;

    return o.str();
}

