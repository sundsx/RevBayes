/**
 * @file
 * This file contains the declaration of the probability density function of some distribution.
 * The distribution is given as a parameter so this file is the wrapper to call the pdf of the distribution.
 * Hence, this function can be used inside deterministic nodes.
 *
 * @brief Declaration of the probability density function.
 *
 * (c) Copyright 2009-
 * @date Last modified: $Date: 2012-06-20 22:57:09 +0200 (Wed, 20 Jun 2012) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2012-08-13, version 1.0
 *
 * $Id: RandomNumberFactory.h 1643 2012-06-20 20:57:09Z hoehna $
 */

#ifndef CummulativeDistributionFunction_H
#define CummulativeDistributionFunction_H

#include "ContinuousFunction.h"
#include "ContinuousDistribution.h"
#include "TypedDagNode.h"

namespace RevBayesCore {
    
    class CummulativeDistributionFunction : public ContinuousFunction {
        
    public:
        CummulativeDistributionFunction(const TypedDagNode<double> *x, ContinuousDistribution *d);
        CummulativeDistributionFunction(const CummulativeDistributionFunction &pdf);
        virtual                            ~CummulativeDistributionFunction(void);
        
        CummulativeDistributionFunction*    clone(void) const;                                                  //!< Create a clon.
        void                                update(void);                                                       //!< Recompute the value
        
    protected:
        void                                swapParameterInternal(const DagNode *oldP, const DagNode *newP);    //!< Implementation of swaping parameters
        
    private:
        const TypedDagNode<double>*         x;
        ContinuousDistribution*             dist;
    };
}


#endif
