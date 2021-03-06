/* 
 * File:   RealNodeContainer.cpp
 * Author: nl
 * 
 * Created on 26 juillet 2014, 12:39
 */

#include "RealNodeContainer.h"

#include "RbException.h"
#include "RbOptions.h"
#include "TypedDagNode.h"

#include <cmath>

#include <iomanip>

using namespace RevBayesCore;

// Declarations
std::ostream& operator<<(std::ostream& o, const RealNodeContainer& x);


/* Default constructor */
RealNodeContainer::RealNodeContainer(void) {}


/* constructor based on a timetree and a dimension for the process */
RealNodeContainer::RealNodeContainer(const TimeTree* intree) : std::vector<double>(intree->getNumberOfNodes(), 0.0), tree(intree), clampVector(intree->getNumberOfNodes(),false)  {
    
}


/* Copy constructor */
RealNodeContainer::RealNodeContainer(const RealNodeContainer& p) : std::vector<double>(p), tree(p.getTimeTree()), clampVector(p.clampVector) {
    
}


/* Destructor */
RealNodeContainer::~RealNodeContainer(void) {
}


RealNodeContainer& RealNodeContainer::operator=(const RealNodeContainer &p) {
    
    if (this != &p) {
        std::vector<double>::operator=(p);
        tree = p.getTimeTree();
        clampVector = p.clampVector;
    }
    
    return *this;
}


/* Clone function */
RealNodeContainer* RealNodeContainer::clone(void) const {
    
    return new RealNodeContainer(*this);
}


void RealNodeContainer::executeMethod(const std::string &n, const std::vector<const DagNode *> &args, double &rv) const
{
    
    if ( n == "mean" )
    {
        rv = getMean();
    }
    else if ( n == "tipMean" )
    {
        rv = getMeanOverTips();
    }
    else if ( n == "stdev" )
    {
        rv = getStdev();
    }
    else if ( n == "rootVal" )
    {
        rv = getRootVal();
    }
    else    {
        throw RbException("A RealNodeContainer object does not have a member method called '" + n + "'.");
    }
}

bool RealNodeContainer::isClamped(size_t index) const   {
    return clampVector[index];
}

void RealNodeContainer::clampAt(const ContinuousCharacterData* data, size_t k) {

    recursiveClampAt(getTimeTree()->getRoot(),data,k-1);
}


void RealNodeContainer::recursiveClampAt(const TopologyNode& from, const ContinuousCharacterData* data, size_t l) {
 
    if (from.isTip())   {
        
        // get taxon index
        size_t index = from.getIndex();
        std::string taxon = tree->getTipNames()[index];
        size_t dataindex = data->getIndexOfTaxon(taxon);
        
        if (data->getCharacter(dataindex,l).getMean() != -1000) {
           (*this)[index] = data->getCharacter(dataindex,l).getMean();
            clampVector[index] = true;
        }
        else    {
            std::cerr << "taxon : " << taxon << " is missing for trait " << l+1 << '\n';
        }
    }

    // propagate forward
    size_t numChildren = from.getNumberOfChildren();
    for (size_t i = 0; i < numChildren; ++i) {
        recursiveClampAt(from.getChild(i),data,l);
    }    
}


double RealNodeContainer::getRootVal() const {
    
        return (*this)[getTimeTree()->getRoot().getIndex()];
}

double RealNodeContainer::getMean() const {
    
    int n = 0;
    double e1 = 0;
    double e2 = 0;
    recursiveGetStats(getTimeTree()->getRoot(), e1, e2, n);
    e1 /= n;
    e2 /= n;
    e2 -= e1 * e1;
    return e1;
}

double RealNodeContainer::getMeanOverTips() const {
    
    int n = 0;
    double e1 = 0;
    double e2 = 0;
    recursiveGetStatsOverTips(getTimeTree()->getRoot(), e1, e2, n);
    e1 /= n;
    e2 /= n;
    e2 -= e1 * e1;
    return e1;
}

double RealNodeContainer::getStdev() const {
    
    int n = 0;
    double e1 = 0;
    double e2 = 0;
    recursiveGetStats(getTimeTree()->getRoot(), e1, e2, n);
    e1 /= n;
    e2 /= n;
    e2 -= e1 * e1;
    return sqrt(e2);
}


void RealNodeContainer::recursiveGetStats(const TopologyNode& from, double& e1, double& e2, int& n) const {

    double tmp = (*this)[from.getIndex()];

    n++;
    e1 += tmp;
    e2 += tmp * tmp;
    
    // propagate forward
    size_t numChildren = from.getNumberOfChildren();
    for (size_t i = 0; i < numChildren; ++i) {
        recursiveGetStats(from.getChild(i),e1,e2,n);
    }
    
}


void RealNodeContainer::recursiveGetStatsOverTips(const TopologyNode& from, double& e1, double& e2, int& n) const {

    if(from.isTip())   {
        double tmp = (*this)[from.getIndex()];

        n++;
        e1 += tmp;
        e2 += tmp * tmp;
    }
    // propagate forward
    size_t numChildren = from.getNumberOfChildren();
    for (size_t i = 0; i < numChildren; ++i) {
        recursiveGetStatsOverTips(from.getChild(i),e1,e2,n);
    }
    
}

/*
std::ostream& RevBayesCore::operator<<(std::ostream& os, const RealNodeContainer& x) {

    os << std::fixed;
    os << std::setprecision(4);
        
    x.printBranchContrasts(os);
    os << '\t';
    
    for (size_t i=0; i<x.getDim(); i++)   {
        os << x.getMean(i) << '\t';
    }
    
    for (size_t i=0; i<x.getDim(); i++)   {
        os << x.getStdev(i) << '\t';
    }    
    
    for (size_t i=0; i<x.getDim(); i++)   {
        os << x.getRootVal(i);
        if (i < x.getDim() -1)  {
            os << '\t';
        }
    }    
    
    return os;
}

*/