//
//  TestCharacterHistory.cpp
//  rb_mlandis
//
//  Created by Michael Landis on 8/6/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#include "TestCharacterHistory.h"

// sorted
#include "BetaSimplexMove.h"
#include "BetaDistribution.h"
#include "BranchHistory.h"
#include "CharacterEvent.h"
#include "CharacterHistoryNodeMonitor.h"
#include "Clade.h"
#include "ConstantRateBirthDeathProcess.h"
#include "ConstantNode.h"
#include "ContinuousStochasticNode.h"
#include "DeterministicNode.h"
#include "DirichletDistribution.h"
#include "ExponentialDistribution.h"
#include "FileMonitor.h"
#include "GammaDistribution.h"
#include "GeographyRateModifier.h"
#include "Mcmc.h"
#include "MetropolisHastingsMove.h"
#include "Model.h"
#include "Monitor.h"
#include "Move.h"
#include "NormalDistribution.h"
#include "NclReader.h"
#include "PhylowoodNhxMonitor.h"
#include "RandomNumberFactory.h"
#include "RandomNumberGenerator.h"
#include "ScaleProposal.h"
#include "ScreenMonitor.h"
#include "SimplexMove.h"
#include "SlidingMove.h"
#include "StochasticNode.h"
#include "TimeAtlas.h"
#include "TimeTree.h"
#include "TimeAtlasDataReader.h"
#include "TestCharacterHistory.h"
#include "UniformDistribution.h"
#include "VectorFunction.h"
#include "VectorScaleMove.h"

// experimental RateMap setup
#include "RateMap_Biogeography.h"
#include "BiogeographyRateMapFunction.h"
#include "BiogeographicTreeHistoryCtmc.h"
#include "FreeBinaryRateMatrixFunction.h"
#include "TreeCharacterHistoryNodeMonitor.h"
#include "TreeCharacterHistoryNhxMonitor.h"
#include "MetropolisHastingsMove.h"
#include "PathRejectionSampleProposal.h"
#include "BiogeographyPathRejectionSampleProposal.h"
#include "NodeRejectionSampleProposal.h"
#include "BiogeographyNodeRejectionSampleProposal.h"
#include "TipRejectionSampleProposal.h"
#include "PathRejectionSampleMove.h"
#include "UniformTimeTreeDistribution.h"
#include "BranchLengthTree.h"
#include "UniformBranchLengthTreeDistribution.h"
#include "DistanceDependentDispersalFunction.h"

using namespace RevBayesCore;


TestCharacterHistory::TestCharacterHistory(const std::string &afn,  const std::string &tfn, const std::string &gfn, int gen, const std::string &fp) : filepath(fp), areaFilename( afn ), treeFilename(tfn),  geoFilename(gfn), mcmcGenerations( gen )
{
    ;
}

TestCharacterHistory::TestCharacterHistory(int ac, const char* av[], const std::string &afn, const std::string &tfn, const std::string &gfn, int gen, const std::string &fp) : filepath(fp), areaFilename( afn ), treeFilename(tfn),  geoFilename(gfn), mcmcGenerations( gen ), argc(ac), argv(av)
{
    ;
}

void TestCharacterHistory::tokenizeArgv(void)
{
    for (int i = 0; i < argc; i++)
    {
        argTokens.push_back(argv[i]);
        std::cout << i << " " << argTokens[i] << "\n";
    }
    std::cout << argc << " == " << argTokens.size() << " arguments\n";
    if (argc > 1)
    {
        areaFilename = argTokens[1];
        treeFilename = argTokens[2];
    }

}

TestCharacterHistory::~TestCharacterHistory() {
    // nothing to do
}

bool TestCharacterHistory::run( void ) {
 
    int idx = 0;
    switch(idx)
    {
        case 0: return run_exp();
        case 1: return run_dollo();
        default: break;
    };
    return false;
    
}

bool TestCharacterHistory::run_exp(void) {
    
    
    ////////////
    // settings
    ////////////
    
    mcmcGenerations *= 50 ;
//    mcmcGenerations=35;
    unsigned int burn = (unsigned int)(mcmcGenerations * .2);

    ////////////
    // io
    ////////////
    
    std::vector<unsigned> old_seed = GLOBAL_RNG->getSeed();
    std::vector<unsigned> seed;
    seed.push_back(15); seed.push_back(1);
//    old_seed = seed;
    GLOBAL_RNG->setSeed(seed);
    std::stringstream ss;
    ss << ".s0_" << old_seed[0] << ".s1_" << old_seed[1];

    bool usingAmbiguousCharacters = !true;
    bool simulate = false;
    bool useClock = !true;
    bool forbidExtinction = true;
    bool useCladogenesis = true;
    bool useEpochs = true;
    bool useDistances = !true;
    bool useAdjacency = true;
    bool useAvailable = true;
    filepath="/Users/mlandis/data/bayarea/output/";
    
    // binary characters
    std::string fn = "";
//    fn = "vireya.nex";
    fn = "psychotria.nex";
//    fn = "16tip_4areas.nex";
//    fn = "2tip_4areas.nex";
//    fn = "vireya_gain0_01_loss0_03_dp0.nex";
//    fn = "16tip_100areas_dp0.nex";
    std::string in_fp = "/Users/mlandis/Documents/code/revbayes-code/examples/data/";
    std::vector<AbstractCharacterData*> data = NclReader::getInstance().readMatrices(in_fp + fn);
    std::cout << "Read " << data.size() << " matrices." << std::endl;
    size_t numAreas = data[0]->getNumberOfCharacters();
    
    // tree
    std::vector<TimeTree*> trees = NclReader::getInstance().readTimeTrees( in_fp + fn );
    std::cout << "Read " << trees.size() << " trees." << std::endl;
    std::cout << trees[0]->getNewickRepresentation() << std::endl;
    size_t numNodes = trees[0]->getNumberOfNodes();

    // geo by epochs
    std::string afn="";
//    afn = "vireya.atlas.txt";
    afn = "hawaii_single.atlas.txt";
//    afn = "hawaii_hard.atlas.txt";
    TimeAtlasDataReader tsdr(in_fp + afn,'\t');
    const TimeAtlas* ta = new TimeAtlas(&tsdr);
    
    ////////////
    // model
    ////////////
    
    // clock
    ContinuousStochasticNode* clockRate = new ContinuousStochasticNode("clockRate", new GammaDistribution( new ConstantNode<double>("clockPrior_A", new double(2.0)),
                                                                                                           new ConstantNode<double>("clockPrior_B", new double(2.0))));
    if (!useClock)
        clockRate->setValue(new double(1.0));
    
    // tree
    std::vector<std::string> names = data[0]->getTaxonNames();
    ConstantNode<double>* origin = new ConstantNode<double>( "origin", new double( trees[0]->getRoot().getAge() ) );
//    ConstantNode<double> *div = new ConstantNode<double>("diversification", new double(1.0));
//    ConstantNode<double> *turn = new ConstantNode<double>("turnover", new double(0.0));
//    ConstantNode<double> *rho = new ConstantNode<double>("rho", new double(1.0));
    StochasticNode<TimeTree>* tau = new StochasticNode<TimeTree>("tau", new UniformTimeTreeDistribution(origin, names));
//    StochasticNode<TimeTree> *tau = new StochasticNode<TimeTree>( "tau", new ConstantRateBirthDeathProcess(origin, div, turn, rho, "uniform", "survival", int(names.size()), names, std::vector<Clade>()) );
    tau->setValue( trees[0] );
    
    
    // geo distances
    DeterministicNode<GeographyRateModifier>* ddd = NULL;
    ContinuousStochasticNode* dp = NULL;
    ConstantNode<double> *dp_pr = NULL;
//    if (useDistances)
    {
//        dp_pr = new ConstantNode<double>( "distancePowerPrior", new double(1.0));
//        dp = new ContinuousStochasticNode("distancePower", new NormalDistribution(new ConstantNode<double>("dp_mu",new double(0.0)), dp_pr));
        dp_pr = new ConstantNode<double>( "distancePowerPrior", new double(10.0));
        dp = new ContinuousStochasticNode("distancePower", new ExponentialDistribution(dp_pr));
        dp->setValue(new double(0.00001));
//        dp->setValue(new double(1.0));
        ddd = new DeterministicNode<GeographyRateModifier>("dddFunction", new DistanceDependentDispersalFunction(dp, ta, useAdjacency, useAvailable, useDistances));
    }
    
    // ctmc rates
    ConstantNode<double>* glr_pr = new ConstantNode<double>("glr_pr", new double(10.0));
    std::vector<const TypedDagNode<double> *> glr;
    std::vector<StochasticNode<double>* > glr_stoch;
	std::vector< ContinuousStochasticNode *> glr_nonConst;
	for( size_t i=0; i<2; i++){
        std::ostringstream glr_name;
        glr_name << "r(" << i << ")";
		ContinuousStochasticNode* tmp_glr = new ContinuousStochasticNode( glr_name.str(), new ExponentialDistribution(glr_pr));
//        tmp_glr->setValue(new double(0.1));
		glr.push_back( tmp_glr );
		glr_nonConst.push_back( tmp_glr );
        glr_stoch.push_back(tmp_glr);
	}
    glr_nonConst[0]->setValue(0.1);
    glr_nonConst[1]->setValue(0.1);
    
    DeterministicNode< std::vector< double > >* glr_vector = new DeterministicNode< std::vector< double > >( "glr_vector", new VectorFunction< double >( glr ) );

    // Q-map used to compute likehood under the full model
    BiogeographyRateMapFunction* brmf_likelihood = new BiogeographyRateMapFunction(numAreas, forbidExtinction);
    brmf_likelihood->setGainLossRates(glr_vector);
    brmf_likelihood->setClockRate(clockRate);
    if (useDistances || useAvailable || useAdjacency)
        brmf_likelihood->setGeographyRateModifier(ddd);
    
    DeterministicNode<RateMap> *q_likelihood = new DeterministicNode<RateMap>("Q_like", brmf_likelihood);
    
    // Q-map used to sample path histories
    BiogeographyRateMapFunction* brmf_sample = new BiogeographyRateMapFunction(numAreas, false);
    brmf_sample->setGainLossRates(glr_vector);
    brmf_sample->setClockRate(clockRate);
    if (useDistances || useAvailable || useAdjacency)
        brmf_sample->setGeographyRateModifier(ddd);

    DeterministicNode<RateMap> *q_sample = new DeterministicNode<RateMap>("Q_sample", brmf_sample);
        
    // and the character model
    BiogeographicTreeHistoryCtmc<StandardState, TimeTree> *biogeoCtmc = new BiogeographicTreeHistoryCtmc<StandardState, TimeTree>(tau, 2, numAreas, usingAmbiguousCharacters, forbidExtinction, useCladogenesis);
    biogeoCtmc->setRateMap(q_likelihood);
    if (data.size() == 2 && usingAmbiguousCharacters)
        biogeoCtmc->setTipProbs( data[1] );
    
    StochasticNode< AbstractCharacterData > *charactermodel = new StochasticNode< AbstractCharacterData >("ctmc", biogeoCtmc );
    
    // simulated data
    if (simulate)
        biogeoCtmc->simulate();

    // real data
    else
        charactermodel->clamp( data[0] );
    
    // initialize mapping
    charactermodel->redraw();
    
    std::cout << "lnL = " << charactermodel->getDistribution().computeLnProbability() << "\n";
    
    
//    GLOBAL_RNG->setSeed(old_seed);
//    glr_nonConst[0]->redraw();
//    glr_nonConst[1]->redraw();

    std::cout << "seed " << old_seed[0] << " " << old_seed[1] << "\n";
    
    ////////////
    // moves
    ////////////
    
    std::cout << "Adding moves\n";
    std::vector<Move*> moves;
    TopologyNode* nd = NULL; // &tau->getValue().getNode(60); 
    
    if (useClock)
    {
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(clockRate, 0.25), 1.0, false) );
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(clockRate, 0.1), 2.0, false) );
    }
    
    if (useDistances)
    {
        moves.push_back(new SlidingMove(dp, 0.1, false, 5 ));
        moves.push_back(new SlidingMove(dp, 0.3, false, 2 ));
    }
    
    moves.push_back( new VectorScaleMove(glr_stoch, 0.25, false, 2));
    moves.push_back( new VectorScaleMove(glr_stoch, 0.1, false, 2));
    for( size_t i=0; i<2; i++)
    {
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(glr_nonConst[i], 0.25), 1, false ) );
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(glr_nonConst[i], 0.1), 2, false ) );
    }

    // path proposals
    if (useEpochs)
    {
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new BiogeographyPathRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes * 2));
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new BiogeographyPathRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.3, nd), 0.3, false, numNodes));
    }
    else if (!useEpochs)
    {
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new PathRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes*2));
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new PathRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.3, nd), 0.3, false, numNodes));
    }
    
    // node proposals
    if (useEpochs)
    {
        
        BiogeographyPathRejectionSampleProposal<StandardState,TimeTree>* eprsp = new BiogeographyPathRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.3, nd);
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new BiogeographyNodeRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, eprsp, 0.2, nd), 0.2, false, numNodes*2));
        
    }
    else if (useCladogenesis && !useEpochs)
    {
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new BiogeographyNodeRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.2, nd), 0.2, false, numNodes*2));
    }
    else if (!useCladogenesis && !useEpochs)
    {
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new NodeRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes*2));
        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new NodeRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.3, nd), 0.3, false, numNodes));
    }
    
//
//    // tip proposals
//    if (usingAmbiguousCharacters)
//    {
////        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new TipRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.5, nd), 0.5, false, numNodes/2));
//        moves.push_back(new PathRejectionSampleMove<StandardState, TimeTree>(charactermodel, tau, q_sample, new TipRejectionSampleProposal<StandardState,TimeTree>(charactermodel, tau, q_sample, 0.1, nd), 0.05, false, numNodes/1));
//    }

    

    
    ////////////
    // monitors
    ////////////
    
    std::cout << "Adding monitors\n";
    std::vector<Monitor*> monitors;
    
    std::set<DagNode*> monitoredNodes;
    monitoredNodes.insert(clockRate);
    monitoredNodes.insert( dp );
    monitoredNodes.insert( glr_nonConst[0] );
    monitoredNodes.insert( glr_nonConst[1] );
    
    monitors.push_back(new FileMonitor(monitoredNodes, 10, filepath + "rb3" + ss.str() + ".mcmc.txt", "\t"));
    monitors.push_back(new ScreenMonitor(monitoredNodes, 10, "\t" ) );
    monitors.push_back(new TreeCharacterHistoryNodeMonitor<StandardState,TimeTree>(charactermodel, tau, 100, filepath + "rb" + ss.str() + ".tree_chars.txt", "\t"));
    monitors.push_back(new TreeCharacterHistoryNodeMonitor<StandardState,TimeTree>(charactermodel, tau, 100, filepath + "rb" + ss.str() + ".num_events.txt", "\t", true, true, true, false, true, true, false));
    monitors.push_back(new TreeCharacterHistoryNhxMonitor<StandardState,TimeTree>(charactermodel, tau, ta, 100, mcmcGenerations, burn, filepath + "rb" + ss.str() + ".nhx.txt", "\t"));

    
    //////////
    // model
    //////////
    std::cout << "Instantiating model\n";
    Model myModel = Model(charactermodel);
    
    
    //////////
    // mcmc
    //////////
    std::cout << "Instantiating mcmc\n";
    Mcmc myMcmc = Mcmc( myModel, moves, monitors );
//    myMcmc.setScheduleType("single");
    myMcmc.run(mcmcGenerations);
    myMcmc.printOperatorSummary();
    

    //////////
    // clean-up
    //////////
    std::cout << "Cleaning up objects\n";
    
    // segfault when uncommented? strange.
//    delete dp;
//    delete glr;
//    delete sglr;
    
    for (std::vector<Move*>::iterator it = moves.begin(); it != moves.end(); ++it) {
        const Move *theMove = *it;
        delete theMove;
    }
    for (std::vector<Monitor*>::iterator it = monitors.begin(); it != monitors.end(); ++it) {
        const Monitor *theMonitor = *it;
        delete theMonitor;
    }
    
    std::cout << "Finished CharacterHistory model test." << std::endl;
    
    return true;
}





















////////////////////////
// drichter test module
////////////////////////


bool TestCharacterHistory::run_dollo(void) {
    
    
    ////////////
    // settings
    ////////////
    
    //unsigned int burn = (unsigned int)(mcmcGenerations * .2);
    std::vector<unsigned> old_seed = GLOBAL_RNG->getSeed();
    std::vector<unsigned> seed;
    seed.push_back(9); seed.push_back(2);
    //GLOBAL_RNG->setSeed(seed);
    std::stringstream ss;
    ss << ".s0_" << old_seed[0] << ".s1_" << old_seed[1];
    
    ////////////
    // io
    ////////////
    bool simulate = false;
    bool useClock = !true;
    bool forbidExtinction = !true;
    bool usingAmbiguousCharacters = true;
    bool useCladogenesis = false;
    filepath="/Users/mlandis/data/ngene/";
    
    // binary characters
    std::string fn = "dolloplus.nex";
    std::string in_fp = "/Users/mlandis/data/ngene/";
   
    std::vector<AbstractCharacterData*> data = NclReader::getInstance().readMatrices(in_fp + fn);
    std::cout << "Read " << data.size() << " matrices." << std::endl;
    size_t numAreas = data[0]->getNumberOfCharacters();
    
    // tree
    // std::vector<TimeTree*> trees = NclReader::getInstance().readTimeTrees( in_fp + fn );
    std::vector<BranchLengthTree*>* trees_ptr = NclReader::getInstance().readBranchLengthTrees(in_fp + fn,"nexus");
    std::vector<BranchLengthTree*> trees = *trees_ptr;
    std::cout << "Read " << trees.size() << " trees." << std::endl;
    std::cout << trees[0]->getNewickRepresentation() << std::endl;
    size_t numNodes = trees[0]->getNumberOfNodes();
    
    ////////////
    // model
    ////////////
    
    // clock
    ConstantNode<double> *clockPriorA = new ConstantNode<double>("clockPrior_A", new double(2.0));
    ConstantNode<double> *clockPriorB = new ConstantNode<double>("clockPrior_B", new double(2.0));
    StochasticNode<double> *clockRate = new StochasticNode<double>("clockRate", new GammaDistribution(clockPriorA, clockPriorB) );
    //clockRate->setValue(new double(1.0));
    
    // tree
    std::vector<std::string> names = data[0]->getTaxonNames();
    ConstantNode<double>* maxBrLen = new ConstantNode<double>("maxBrLen", new double(10e6));
    StochasticNode<BranchLengthTree>* tau = new StochasticNode<BranchLengthTree>("tau_tmp", new UniformBranchLengthTreeDistribution(maxBrLen, names));
    tau->setValue( trees[0] );
    
    // gain loss rates
    ConstantNode<double>* gainRatePrior = new ConstantNode<double>("glr_pr", new double(4.0));
    std::vector<const TypedDagNode<double> *> gainLossRates;
	std::vector< ContinuousStochasticNode *> gainLossRates_nonConst;
    std::vector<StochasticNode<double>* > glr_stoch;

	for( size_t i=0; i<2; i++){
        std::ostringstream glr_name;
        glr_name << "r(" << i << ")";
		ContinuousStochasticNode* tmp_glr = new ContinuousStochasticNode( glr_name.str(), new ExponentialDistribution(gainRatePrior, new ConstantNode<double>("offset", new double(0.0) )));
        tmp_glr->setValue(new double(0.1));
		gainLossRates.push_back( tmp_glr );
		gainLossRates_nonConst.push_back( tmp_glr );
        glr_stoch.push_back(tmp_glr);
	}
    DeterministicNode< std::vector< double > >* glr_vector = new DeterministicNode< std::vector< double > >( "glr_vector", new VectorFunction< double >( gainLossRates ) );
    
    // Q-map used to compute likehood under the full model
    BiogeographyRateMapFunction* brmf_likelihood = new BiogeographyRateMapFunction(numAreas,forbidExtinction);
    brmf_likelihood->setGainLossRates(glr_vector);
    brmf_likelihood->setClockRate(clockRate);
    DeterministicNode<RateMap> *q_likelihood = new DeterministicNode<RateMap>("Q_l", brmf_likelihood);
    
    // Q-map used to sample path histories
    BiogeographyRateMapFunction* brmf_sample = new BiogeographyRateMapFunction(numAreas,false);
    brmf_sample->setGainLossRates(glr_vector);
    brmf_sample->setClockRate(clockRate);
    DeterministicNode<RateMap> *q_sample = new DeterministicNode<RateMap>("Q_s", brmf_sample);
    
    // and the character model
    BiogeographicTreeHistoryCtmc<StandardState, BranchLengthTree> *biogeoCtmc = new BiogeographicTreeHistoryCtmc<StandardState, BranchLengthTree>(tau, 2, numAreas, usingAmbiguousCharacters, forbidExtinction, useCladogenesis);
    biogeoCtmc->setRateMap(q_likelihood);
    
    if (data.size() == 2 && usingAmbiguousCharacters)
        biogeoCtmc->setTipProbs( data[1] );

    StochasticNode< AbstractCharacterData > *charactermodel = new StochasticNode< AbstractCharacterData >("ctmc", biogeoCtmc );
    
    GLOBAL_RNG->setSeed(old_seed);
//    gainLossRates_nonConst[0]->redraw();
//    gainLossRates_nonConst[1]->redraw();

    
    // simulated data
    if (simulate)
        biogeoCtmc->simulate();
    
    // real data
    else
        charactermodel->clamp( data[0] );
    
    // initialize mapping
    charactermodel->redraw();
    
    std::cout << "lnL = " << charactermodel->getDistribution().computeLnProbability() << "\n";
    //    std::cout << GLOBAL_RNG->getSeed()[0] << "\n";
    //    std::cout << GLOBAL_RNG->getSeed()[1] << "\n";
    
    ////////////
    // moves
    ////////////
    
    std::cout << "Adding moves\n";
    std::vector<Move*> moves;
    if (useClock)
    {
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(clockRate, 0.5), 5, true ) );
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(clockRate, 0.1), 5, true ) );
    }
    
    
    moves.push_back( new VectorScaleMove(glr_stoch, 0.25, false, 2));
    moves.push_back( new VectorScaleMove(glr_stoch, 0.1, false, 2));
    for( size_t i=0; i<2; i++)
    {
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(gainLossRates_nonConst[i], 0.5), 2, false ) );
        moves.push_back( new MetropolisHastingsMove( new ScaleProposal(gainLossRates_nonConst[i], 0.1), 2, false ) );
    }
    
    
    TopologyNode* nd = NULL;

    moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new PathRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.5, nd), 0.5, false, numNodes));
    moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new PathRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes*2));
    
    
    moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new NodeRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.5, nd), 0.5, false, numNodes));
    moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new NodeRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes*2));
    
    if (usingAmbiguousCharacters)
    {
        moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new TipRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.5, nd), 0.5, false, numNodes));
        moves.push_back(new PathRejectionSampleMove<StandardState, BranchLengthTree>(charactermodel, tau, q_sample, new TipRejectionSampleProposal<StandardState,BranchLengthTree>(charactermodel, tau, q_sample, 0.1, nd), 0.1, false, numNodes*2));
    }
    
    
    ////////////
    // monitors
    ////////////
    
    std::cout << "Adding monitors\n";
    std::vector<Monitor*> monitors;
    
    std::set<DagNode*> monitoredNodes;
    if (useClock)
        monitoredNodes.insert(clockRate);

    monitoredNodes.insert( glr_vector );
    
    monitors.push_back(new FileMonitor(monitoredNodes, 100, filepath + "rb_dollo" + ss.str() + ".mcmc.txt", "\t"));
    monitors.push_back(new ScreenMonitor(monitoredNodes, 100, "\t" ) );
    monitors.push_back(new TreeCharacterHistoryNodeMonitor<StandardState,BranchLengthTree>(charactermodel, tau, 100, filepath + "rb_dollo" + ss.str() + ".tree_chars.txt", "\t"));
    monitors.push_back(new TreeCharacterHistoryNodeMonitor<StandardState,BranchLengthTree>(charactermodel, tau, 100, filepath + "rb_dollo" + ss.str() + ".num_events.txt", "\t", true, true, true, false, true, true, false));

    
    
    //////////
    // model
    //////////
    std::cout << "Instantiating model\n";
    Model myModel = Model(charactermodel);
    
    
    //////////
    // mcmc
    //////////
    std::cout << "Instantiating mcmc\n";
    Mcmc myMcmc = Mcmc( myModel, moves, monitors );
    myMcmc.run(mcmcGenerations*20);
    myMcmc.printOperatorSummary();
    
    
    //////////
    // clean-up
    //////////
    std::cout << "Cleaning up objects\n";
    
    // segfault when uncommented? strange.
    //    delete dp;
    //    delete glr;
    //    delete sglr;
    
    for (std::vector<Move*>::iterator it = moves.begin(); it != moves.end(); ++it) {
        const Move *theMove = *it;
        delete theMove;
    }
    for (std::vector<Monitor*>::iterator it = monitors.begin(); it != monitors.end(); ++it) {
        const Monitor *theMonitor = *it;
        delete theMonitor;
    }
    
    std::cout << "Finished CharacterHistory model test." << std::endl;
    
    return true;
}