//
//  BiogeographyRateMapFunction.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 4/3/14.
//  Copyright (c) 2014 Michael Landis. All rights reserved.
//

#include "BiogeographyRateMapFunction.h"
#include "ConstantNode.h"
#include "RbException.h"

using namespace RevBayesCore;

BiogeographyRateMapFunction::BiogeographyRateMapFunction(size_t nc, bool fe) : TypedFunction<RateMap>( new RateMap_Biogeography( nc, fe ) )
{
    homogeneousGainLossRates            = new ConstantNode<std::vector<double> >("homogeneousGainLossRates", new std::vector<double>(2,0.5));
    heterogeneousGainLossRates          = NULL;
    homogeneousClockRate                = new ConstantNode<double>("clockRate", new double(1.0) );
    heterogeneousClockRates             = NULL;
    geographyRateModifier               = NULL;
    
    branchHeterogeneousClockRates       = false;
    branchHeterogeneousGainLossRates    = false;
    useGeographicDistance               = false;
    
    this->addParameter(homogeneousGainLossRates);
    this->addParameter(homogeneousClockRate);
    
    update();
}


BiogeographyRateMapFunction::BiogeographyRateMapFunction(const BiogeographyRateMapFunction &n) : TypedFunction<RateMap>( n )
{
    homogeneousGainLossRates = n.homogeneousGainLossRates;
    heterogeneousGainLossRates = n.heterogeneousGainLossRates;
    homogeneousClockRate = n.homogeneousClockRate;
    heterogeneousClockRates = n.heterogeneousClockRates;
    geographyRateModifier = n.geographyRateModifier;
    
    branchHeterogeneousClockRates = n.branchHeterogeneousClockRates;
    branchHeterogeneousGainLossRates = n.branchHeterogeneousGainLossRates;
    useGeographicDistance = n.useGeographicDistance;
}


BiogeographyRateMapFunction::~BiogeographyRateMapFunction( void ) {
    // We don't delete the parameters, because they might be used somewhere else too. The model needs to do that!
}



BiogeographyRateMapFunction* BiogeographyRateMapFunction::clone( void ) const {
    return new BiogeographyRateMapFunction( *this );
}


void BiogeographyRateMapFunction::update( void ) {
    
    // touch specialization for granular updates?
    
    // set the gainLossRate
    if (branchHeterogeneousGainLossRates)
    {
        // Disabled for now due to ostream errors...
//        const std::vector<double>& glr = heterogeneousGainLossRates->getValue();
        std::vector<std::vector<double> > glr;
        static_cast< RateMap_Biogeography* >(value)->setHeterogeneousGainLossRates(glr);
    }
    else
    {
        const std::vector<double>& glr = homogeneousGainLossRates->getValue();
        static_cast< RateMap_Biogeography* >(value)->setHomogeneousGainLossRates(glr);
    }

    if (branchHeterogeneousClockRates)
    {
        const std::vector<double>& r = heterogeneousClockRates->getValue();
        static_cast< RateMap_Biogeography* >(value)->setHeterogeneousClockRates(r);
    }
    else
    {
        const double& r = homogeneousClockRate->getValue();
        static_cast< RateMap_Biogeography* >(value)->setHomogeneousClockRate(r);
    }
    
    // set the distancePower
    if (useGeographicDistance)
    {
        const GeographyRateModifier& drm = geographyRateModifier->getValue();
        static_cast< RateMap_Biogeography* >(value)->setGeographyRateModifier(drm);
    }
    
    value->updateMap();
}

void BiogeographyRateMapFunction::setGainLossRates(const TypedDagNode<std::vector<double> > *r)
{
    // remove the old parameter first
    if ( homogeneousGainLossRates != NULL )
    {
        this->removeParameter( homogeneousGainLossRates );
        homogeneousGainLossRates = NULL;
    }
    else
    {
        this->removeParameter( heterogeneousGainLossRates );
        heterogeneousGainLossRates = NULL;
    }
    
    // set the value
    branchHeterogeneousGainLossRates = false;
    homogeneousGainLossRates = r;
    
    // add the parameter
    this->addParameter( homogeneousGainLossRates );
}

void BiogeographyRateMapFunction::setClockRate(const TypedDagNode< double > *r) {
    
    // remove the old parameter first
    if ( homogeneousClockRate != NULL )
    {
        this->removeParameter( homogeneousClockRate );
        homogeneousClockRate = NULL;
    }
    else // heterogeneousClockRate != NULL
    {
        this->removeParameter( heterogeneousClockRates );
        heterogeneousClockRates = NULL;
    }
    
    // set the value
    branchHeterogeneousClockRates = false;
    homogeneousClockRate = r;
    
    // add the parameter
    this->addParameter( homogeneousClockRate );
}

void BiogeographyRateMapFunction::setClockRate(const TypedDagNode< std::vector< double > > *r) {
    
    // remove the old parameter first
    if ( homogeneousClockRate != NULL )
    {
        this->removeParameter( homogeneousClockRate );
        homogeneousClockRate = NULL;
    }
    else // heterogeneousClockRate != NULL
    {
        this->removeParameter( heterogeneousClockRates );
        heterogeneousClockRates = NULL;
    }
    
    // set the value
    branchHeterogeneousClockRates = true;
    heterogeneousClockRates = r;
    
    // add the parameter
    this->addParameter( heterogeneousClockRates );
    
}

void BiogeographyRateMapFunction::setGeographyRateModifier(const TypedDagNode<GeographyRateModifier> *drm) {
    
    // remove the old parameter first
    if ( geographyRateModifier != NULL )
    {
        this->removeParameter( geographyRateModifier );
        geographyRateModifier = NULL;
    }
    
    // set the value
    useGeographicDistance = true;
    geographyRateModifier = drm;
    
    // add the parameter
    this->addParameter( geographyRateModifier );

}

void BiogeographyRateMapFunction::swapParameterInternal(const DagNode *oldP, const DagNode *newP)
{
    if (oldP == homogeneousGainLossRates)
    {
        homogeneousGainLossRates = static_cast<const TypedDagNode<std::vector<double> >* >( newP );
    }
    else if (oldP == heterogeneousGainLossRates)
    {
        heterogeneousGainLossRates = static_cast<const TypedDagNode<std::vector<double> >* >( newP );
    }
    else if (oldP == homogeneousClockRate)
    {
        homogeneousClockRate = static_cast<const TypedDagNode< double >* >( newP );
    }
    else if (oldP == heterogeneousClockRates)
    {
        heterogeneousClockRates = static_cast<const TypedDagNode< std::vector< double > >* >( newP );
    }
    else if (oldP == geographyRateModifier)
    {
        geographyRateModifier = static_cast<const TypedDagNode<GeographyRateModifier>* >( newP );
    }
}

std::ostream& operator<<(std::ostream& o, const std::vector<std::vector<double> >& x)
{
    o << "";
    return o;
}