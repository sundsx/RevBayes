//
//  Move_NodeCharacterHistoryRejectionSample.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 7/5/14.
//  Copyright (c) 2014 Michael Landis. All rights reserved.
//



#include "ArgumentRule.h"
#include "ArgumentRules.h"
#include "BiogeographicTreeHistoryCtmc.h"
#include "BiogeographyNodeRejectionSampleProposal.h"
#include "ContinuousStochasticNode.h"
#include "Dist_phyloDACTMC.h"
#include "MetropolisHastingsMove.h"
#include "Move_NodeCharacterHistoryRejectionSample.h"
#include "NodeRejectionSampleProposal.h"
#include "Probability.h"
#include "RateMap_Biogeography.h"
#include "RbException.h"
#include "Real.h"
#include "RealPos.h"
#include "RevObject.h"
#include "RlAbstractCharacterData.h"
#include "RlBoolean.h"
#include "RlRateMap.h"
#include "RlString.h"
#include "RlTimeTree.h"
#include "ScaleProposal.h"
#include "TypedDagNode.h"
#include "TypeSpec.h"


using namespace RevLanguage;

/**
 * Default constructor.
 *
 * The default constructor does nothing except allocating the object.
 */
Move_NodeCharacterHistoryRejectionSample::Move_NodeCharacterHistoryRejectionSample() : Move()
{
    
}


/**
 * The clone function is a convenience function to create proper copies of inherited objected.
 * E.g. a.clone() will create a clone of the correct type even if 'a' is of derived type 'b'.
 *
 * \return A new copy of the move.
 */
Move_NodeCharacterHistoryRejectionSample* RevLanguage::Move_NodeCharacterHistoryRejectionSample::clone(void) const
{
    
	return new Move_NodeCharacterHistoryRejectionSample(*this);
}


/**
 * Create a new internal move object.
 *
 * This function simply dynamically allocates a new internal move object that is
 * associated with the variable (DAG-node). The internal move object is created by calling its
 * constructor and passing the move-parameters (the variable and other parameters) as arguments of the
 * constructor. The move constructor takes care of the proper hook-ups.
 *
 * \return A new internal distribution object.
 */
void Move_NodeCharacterHistoryRejectionSample::constructInternalObject( void )
{
    // we free the memory first
    delete value;
    
    // move/proposal arguments
    double d        = static_cast<const Probability &>( lambda->getRevObject() ).getValue();
    double w        = static_cast<const RealPos &>( weight->getRevObject() ).getValue();
    std::string mt  = static_cast<const RlString &>( type->getRevObject() ).getValue();
    
    // move/proposal parameters
    RevBayesCore::TypedDagNode<RevBayesCore::AbstractCharacterData>* ctmc_tdn   = static_cast<const RevLanguage::AbstractCharacterData&>( ctmc->getRevObject() ).getDagNode();
    RevBayesCore::TypedDagNode<RevBayesCore::RateMap>* qmap_tdn                 = static_cast<const RateMap&>( qmap->getRevObject() ).getDagNode();
    RevBayesCore::TypedDagNode<RevBayesCore::TimeTree>* tree_tdn                = static_cast<const TimeTree&>( tree->getRevObject() ).getDagNode();
    RevBayesCore::StochasticNode<RevBayesCore::AbstractCharacterData>* ctmc_sn  = static_cast<RevBayesCore::StochasticNode<RevBayesCore::AbstractCharacterData>* >(ctmc_tdn);
    RevBayesCore::DeterministicNode<RevBayesCore::RateMap>* qmap_dn             = static_cast<RevBayesCore::DeterministicNode<RevBayesCore::RateMap>* >(qmap_tdn);
    RevBayesCore::StochasticNode<RevBayesCore::TimeTree>* tree_sn               = static_cast<RevBayesCore::StochasticNode<RevBayesCore::TimeTree>* >(tree_tdn);

    // finally create the internal move object
    RevBayesCore::Proposal *p = NULL;
    if (mt == "std")
        ;
    else if (mt == "biogeo")
        p = new RevBayesCore::BiogeographyPathRejectionSampleProposal<RevBayesCore::StandardState, RevBayesCore::TimeTree>(ctmc_sn, tree_sn, qmap_dn, d);
    
    value = new RevBayesCore::MetropolisHastingsMove(p,w,false);
}


/**
 * Get Rev type of object
 *
 * \return The class' name.
 */
const std::string& Move_NodeCharacterHistoryRejectionSample::getClassType(void)
{
    
    static std::string revType = "Move_NodeCharacterHistoryRejectionSample";
    
	return revType;
}


/**
 * Get class type spec describing type of an object from this class (static).
 *
 * \return TypeSpec of this class.
 */
const TypeSpec& Move_NodeCharacterHistoryRejectionSample::getClassTypeSpec(void)
{
    
    static TypeSpec revTypeSpec = TypeSpec( getClassType(), new TypeSpec( Move::getClassTypeSpec() ) );
    
	return revTypeSpec;
}


/**
 * Get the member rules used to create the constructor of this object.
 *
 * The member rules of the scale move are:
 * (1) the variable which must be a positive real.
 * (2) the tuning parameter lambda that defines the size of the proposal (positive real)
 * (3) a flag whether auto-tuning should be used.
 *
 * \return The member rules.
 */
const MemberRules& Move_NodeCharacterHistoryRejectionSample::getMemberRules(void) const
{
    
    static MemberRules nodeChrsMoveMemberRules;
    static bool rulesSet = false;
    
    if ( !rulesSet )
    {
        nodeChrsMoveMemberRules.push_back( new ArgumentRule( "ctmc", false, AbstractDiscreteCharacterData::getClassTypeSpec() ) );
        nodeChrsMoveMemberRules.push_back( new ArgumentRule( "qmap", false, RateMap::getClassTypeSpec() ) );
        nodeChrsMoveMemberRules.push_back( new ArgumentRule( "tree", false, TimeTree::getClassTypeSpec() ) );
        nodeChrsMoveMemberRules.push_back( new ArgumentRule( "lambda", true, Probability::getClassTypeSpec() , new Probability(1.0) ) );
//        nodeChrsMoveMemberRules.push_back( new ArgumentRule( "type", true, RlString::getClassTypeSpec(), new RlString("std") ) );
        std::vector<RlString> options;
        options.push_back( RlString("std") );
        options.push_back( RlString("biogeo") );
        nodeChrsMoveMemberRules.push_back( new OptionRule( "type", new RlString("std"), options ) );
        
        /* Inherit weight from Move, put it after variable */
        const MemberRules& inheritedRules = Move::getMemberRules();
        nodeChrsMoveMemberRules.insert( nodeChrsMoveMemberRules.end(), inheritedRules.begin(), inheritedRules.end() );
        
        rulesSet = true;
    }
    
    return nodeChrsMoveMemberRules;
}


/**
 * Get type-specification on this object (non-static).
 *
 * \return The type spec of this object.
 */
const TypeSpec& Move_NodeCharacterHistoryRejectionSample::getTypeSpec( void ) const
{
    
    static TypeSpec typeSpec = getClassTypeSpec();
    
    return typeSpec;
}


void Move_NodeCharacterHistoryRejectionSample::printValue(std::ostream &o) const {
    
    o << "CharacterHistoryRejectionSample(";
    if (qmap != NULL)
    {
        o << qmap->getName();
    }
    else
    {
        o << "?";
    }
    o << ")";
    
}


/**
 * Set a member variable.
 *
 * Sets a member variable with the given name and store the pointer to the variable.
 * The value of the variable might still change but this function needs to be called again if the pointer to
 * the variable changes. The current values will be used to create the distribution object.
 *
 * \param[in]    name     Name of the member variable.
 * \param[in]    var      Pointer to the variable.
 */

void Move_NodeCharacterHistoryRejectionSample::setConstMemberVariable(const std::string& name, const RevPtr<const Variable> &var)
{
    
    if ( name == "ctmc" )
    {
        ctmc = var;
    }
    else if ( name == "tree" )
    {
        tree = var;
    }
    else if ( name == "qmap" )
    {
        qmap = var;
    }
    else if ( name == "lambda" )
    {
        lambda = var;
    }
    else if ( name == "type" )
    {
        type = var;
    }
    else
    {
        Move::setConstMemberVariable(name, var);
    }
    
}


