/**
 * @file
 * This file contains the implementation of Move_mmultinomial,
 * which changes a vector of positive reals.
 *
 * @brief Implementation of Move_mmultinomial
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

#include "ArgumentRule.h"
#include "Distribution.h"
#include "Move_mmultinomial.h"
#include "RealPos.h"
#include "RandomNumberGenerator.h"
#include "RbException.h"
#include "Integer.h"
#include "RbNames.h"
#include "RbStatistics.h"
#include "Simplex.h"
#include "StochasticNode.h"
#include "VectorString.h"
#include "Workspace.h"

#include <cmath>


/** Constructor for parser */
Move_mmultinomial::Move_mmultinomial(void) : SimpleMove(getMemberRules()) {

}

/** Constructor for internal use */
Move_mmultinomial::Move_mmultinomial(StochasticNode* node, double tuning, int nc, double weight, RandomNumberGenerator* rng) : SimpleMove(getMemberRules()) {

    setValue("tuning",   new RealPos(tuning));
    setValue("num_cats", new Integer(nc));
    setValue("weight",   new RealPos(weight));
    setValue("rng",      rng);

    setNodePtr(node);
}

/** Clone object */
Move_mmultinomial* Move_mmultinomial::clone(void) const {

    return new Move_mmultinomial(*this);
}

/** Convert to object of another class. The caller manages the object. */
RbObject* Move_mmultinomial::convertTo(const std::string& type) const {

    return NULL;
}

/** Get class vector describing type of object */
const VectorString& Move_mmultinomial::getClass() const {

    static VectorString rbClass = VectorString(Move_mmultinomial_name) + Move::getClass();
    return rbClass;
}

/** Return member rules */
const MemberRules& Move_mmultinomial::getMemberRules(void) const {

    static MemberRules memberRules;
    static bool        rulesSet = false;

    if (!rulesSet) 
		{
        memberRules.push_back(new ArgumentRule("tuning", RealPos_name));
        memberRules.push_back(new ArgumentRule("num_cats", Integer_name));

        /* Inherit weight and rng from Move, put it at back */
        const MemberRules& inheritedRules = Move::getMemberRules();
        memberRules.insert(memberRules.end(), inheritedRules.begin(), inheritedRules.end()); 

        rulesSet = true;
		}

    return memberRules;
}

/** Is the object convertible to an object of another class type? */
bool Move_mmultinomial::isConvertibleTo(const std::string& type) const {

    return false;
}

/** Perform the move */
double Move_mmultinomial::perform(std::set<StochasticNode*>& affectedNodes) {

    VectorReal*            valPtr  = (VectorReal*)(nodePtr->getValuePtr(affectedNodes));
    RandomNumberGenerator* rng     = (RandomNumberGenerator*)(getValue("rng"));
    double                 alpha0  = ((Real*)(getValue("tuning")))->getValue();
    int                    k       = ((Integer*)(getValue("num_cats")))->getValue();
	int                    n       = valPtr->getValue().size();

	std::vector<double> curVal = valPtr->getValue();
    double sum = 0.0;
    for (int i=0; i<n; i++)
        sum += curVal[i];
     for (int i=0; i<n; i++)
        curVal[i] /= sum;
	std::vector<double> newVal = curVal;

	/* We update the simplex values by proposing new values from a Dirichlet centered
	   on the current values. The i-th parameter of the Dirichlet is the i-th value
	   in the simplex multiplied by a parameter (alpha0, AKA tuning) that controls the
	   variance of the Dirichlet. We implement two cases of this general move. In one
	   case, all of the elements of the simplex are targeted for update (n == k). In the
	   other, more complicated, case a subset of the elements of the simplex are updated
	   (k < n). Here, we construct a smaller simplex with k+1 elements. The first k of the
	   elements are the values from the full simplex that were targeted for update. The last
	   element of the smaller simplex accumulates the probabilities of all of the simplex
	   values in the full simplex that were not targeted for update. We then update the
	   small simplex by centering a Dirichlet on the small simplex. The values for those elements
	   in the full simplex that were not targeted for update are all changed proportionally.
	   This means that we need to calculate the Jacobian for the Hastings ratio in this case. */
	double lnProposalRatio = 0.0;
	if ( k > n )
		{
		// we can't update more elements that there are elements in the simplex
		throw (RbException("Attempting to update too many simplex variables"));
		}
	else if ( k < 1 )
		{
		// we also can't update 0 or a negative number of elements
		throw (RbException("Attempting to update too few simplex variables"));
		}
	else if ( k < n )
		{
		// we update a subset of the elements in the full simplex
		// pick k values at random, producing a map from the index in the full vector (curVal) to
		// the index in the reduced vector (x, alpha, and z)
		std::vector<int> indicesToUpdate;
		std::vector<int> tmpV;
		for (int i=0; i<n; i++)
			tmpV.push_back(i);
		RbStatistics::Helper::randomlySelectFromVectorWithoutReplacement<int>(tmpV, indicesToUpdate, k, rng);
		std::map<int,int> mapper;
		for (size_t i=0; i<indicesToUpdate.size(); i++)
			mapper.insert( std::make_pair(indicesToUpdate[i], i) );
			
		// set up the vectors
		std::vector<double> x(indicesToUpdate.size()+1, 0.0);
		std::vector<double> alphaForward(indicesToUpdate.size()+1, 0.0);
		std::vector<double> alphaReverse(indicesToUpdate.size()+1, 0.0);
		std::vector<double> z(indicesToUpdate.size()+1, 0.0);
		for (int i=0; i<n; i++)
			{
			std::map<int,int>::iterator it = mapper.find(i);
			if (it != mapper.end())
				x[it->second] += curVal[it->first];
			else 
				x[x.size()-1] += curVal[i];
			}
		for (size_t i=0; i<x.size(); i++)
			alphaForward[i] = x[i] * alpha0;
			
		// draw a new value for the reduced vector
		z = RbStatistics::Dirichlet::rv( alphaForward, rng );
		
		// fill in the Dirichlet parameters for the reverse probability calculations
		for (size_t i=0; i<z.size(); i++)
			alphaReverse[i] = z[i] * alpha0;
		
		// fill in the full vector
		double factor = z[z.size()-1] / x[x.size()-1];
		for (int i=0; i<n; i++)
			{
			std::map<int,int>::iterator it = mapper.find(i);
			if (it != mapper.end())
				newVal[i] = z[it->second];
			else 
				newVal[i] = curVal[i] * factor;
			}
			
		// Hastings ratio
		lnProposalRatio  = RbStatistics::Dirichlet::lnPdf(alphaReverse, x) - RbStatistics::Dirichlet::lnPdf(alphaForward, z); // Hastings Ratio
		lnProposalRatio += (n - k) * log(factor); // Jacobian
		}
	else 
		{
		// we update all of the elements in the vector
		// first, we get the parameters of the Dirichlet for the forward move
		std::vector<double> alphaForward(curVal.size());
		for (size_t i=0; i<curVal.size(); i++)
			alphaForward[i] = curVal[i] * alpha0;
			
		// then, we propose new values
		newVal = RbStatistics::Dirichlet::rv( alphaForward, rng );
		
		// and calculate the Dirichlet parameters for the (imagined) reverse move
		std::vector<double> alphaReverse(newVal.size());
		for (size_t i=0; i<curVal.size(); i++)
			alphaReverse[i] = newVal[i] * alpha0;	
		
		// finally, ew calculate the log of the Hastings ratio
		lnProposalRatio = RbStatistics::Dirichlet::lnPdf(alphaReverse, curVal) - RbStatistics::Dirichlet::lnPdf(alphaForward, newVal);
		}
        
    for (int i=0; i<n; i++)
        newVal[i] *= sum;
		
    valPtr->setValue(newVal);
	
    return lnProposalRatio;
}

