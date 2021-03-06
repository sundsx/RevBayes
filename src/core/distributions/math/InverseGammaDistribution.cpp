//
//  InverseGammaDistribution.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 3/19/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#include "InverseGammaDistribution.h"
#include "DistributionInverseGamma.h"
#include "RandomNumberFactory.h"
#include "RbConstants.h"

using namespace RevBayesCore;

InverseGammaDistribution::InverseGammaDistribution(const TypedDagNode<double> *sh, const TypedDagNode<double> *r) : ContinuousDistribution( new double( 1.0 ) ), shape( sh ), scale( r ) {
    
    *value = RbStatistics::InverseGamma::rv(shape->getValue(), scale->getValue(), *GLOBAL_RNG);
}


InverseGammaDistribution::~InverseGammaDistribution( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}


double InverseGammaDistribution::cdf( void ) const {
    return RbStatistics::InverseGamma::cdf(shape->getValue(), scale->getValue(), *value);
}


InverseGammaDistribution* InverseGammaDistribution::clone( void ) const {
    return new InverseGammaDistribution( *this );
}


double InverseGammaDistribution::computeLnProbability( void ) {
    return RbStatistics::InverseGamma::lnPdf(shape->getValue(), scale->getValue(), *value);
}


double InverseGammaDistribution::getMax( void ) const {
    return RbConstants::Double::inf;
}


double InverseGammaDistribution::getMin( void ) const {
    return 0.0;
}


double InverseGammaDistribution::quantile(double p) const {
    return RbStatistics::InverseGamma::quantile(shape->getValue(), scale->getValue(), p);
}


void InverseGammaDistribution::redrawValue( void ) {
    *value = RbStatistics::InverseGamma::rv(shape->getValue(), scale->getValue(), *GLOBAL_RNG);
}


/** Get the parameters of the distribution */
std::set<const DagNode*> InverseGammaDistribution::getParameters( void ) const
{
    std::set<const DagNode*> parameters;
    
    parameters.insert( shape );
    parameters.insert( scale );
    
    parameters.erase( NULL );
    return parameters;
}


/** Swap a parameter of the distribution */
void InverseGammaDistribution::swapParameter( const DagNode *oldP, const DagNode *newP )
{
    if (oldP == shape)
    {
        shape = static_cast<const TypedDagNode<double>* >( newP );
    }
    if (oldP == scale)
    {
        scale = static_cast<const TypedDagNode<double>* >( newP );
    }
}
