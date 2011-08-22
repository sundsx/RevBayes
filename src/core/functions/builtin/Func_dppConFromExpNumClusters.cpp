/**
 * @file
 * This file contains the implementation of Func_dppConFromExpNumClusters, which is
 * the function used to calculate the approximate, expected value of alpha from a given number
 * of clusters (tables).
 *
 * @brief Implementation of Func_unique
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date$
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @package functions
 * @since Version 1.0, 2009-09-03
 *
 * $Id$
 */

#include "DAGNode.h"
#include "DeterministicNode.h"
#include "Func_dppConFromExpNumClusters.h"
#include "Integer.h"
#include "RbException.h"
#include "RbUtil.h"
#include "RealPos.h"
#include "RbStatisticsHelper.h"
#include "StochasticNode.h"
#include "TypeSpec.h"
#include "ValueRule.h"
#include "VectorInteger.h"
#include "VectorString.h"
#include "VectorReal.h"

#include <cassert>
#include <cmath>

/** Clone object */
Func_dppConFromExpNumClusters* Func_dppConFromExpNumClusters::clone( void ) const {
	
    return new Func_dppConFromExpNumClusters( *this );
}


/** Execute function */
RbLanguageObject* Func_dppConFromExpNumClusters::execute( void ) {
    
	const RealPos* clust = static_cast<const RealPos*>( args[0].getValue() );
	const RealPos* num = static_cast<const RealPos*>( args[1].getValue() );
		
	double p = RbStatistics::Helper::dppConcParamFromNumTables(clust->getValue(), num->getValue());
		
	return ( new RealPos( p ) );
}


/** Get argument rules */
const ArgumentRules& Func_dppConFromExpNumClusters::getArgumentRules( void ) const {
	
    static ArgumentRules argumentRules;
    static bool          rulesSet = false;
	
    if ( !rulesSet ) {
		
        argumentRules.push_back( new ValueRule( "clusters", RealPos_name ) );
        argumentRules.push_back( new ValueRule( "number", RealPos_name ) );
        rulesSet = true;
    }
	
    return argumentRules;
}


/** Get class vector describing type of object */
const VectorString& Func_dppConFromExpNumClusters::getClass( void ) const {
	
    static VectorString rbClass = VectorString( Func_dppConFromExpNumClusters_name ) + RbFunction::getClass();
    return rbClass;
}


/** Get return type */
const TypeSpec Func_dppConFromExpNumClusters::getReturnType( void ) const {
	
    return TypeSpec( RbVoid_name );
}
