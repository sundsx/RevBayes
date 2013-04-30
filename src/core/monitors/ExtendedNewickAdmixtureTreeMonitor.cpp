//
//  ExtendedNewickAdmixtureTreeMonitor.cpp
//  revbayes_mlandis
//
//  Created by Michael Landis on 1/28/13.
//  Copyright (c) 2013 Michael Landis. All rights reserved.
//

#include "ExtendedNewickAdmixtureTreeMonitor.h"
#include "DagNode.h"
#include "Model.h"
#include "Monitor.h"
#include "RbException.h"

using namespace RevBayesCore;

/* Constructor */
ExtendedNewickAdmixtureTreeMonitor::ExtendedNewickAdmixtureTreeMonitor(TypedDagNode<AdmixtureTree> *t,  TypedDagNode< std::vector< double > >* br, int g, const std::string &fname, const std::string &del, bool pp, bool l, bool pr) : Monitor(g,t), outStream(), tree( t ), branchRates(br), filename( fname ), separator( del ), posterior( pp ), likelihood( l ), prior( pr ) {
    
    nodes.insert(branchRates);
    
}

ExtendedNewickAdmixtureTreeMonitor::ExtendedNewickAdmixtureTreeMonitor(const ExtendedNewickAdmixtureTreeMonitor &m) : Monitor( m ), outStream( ), tree( m.tree ), branchRates( m.branchRates), nodeVariables( m.nodeVariables ) {
    
    filename    = m.filename;
    separator   = m.separator;
    prior       = m.prior;
    posterior   = m.posterior;
    likelihood  = m.likelihood;
}


/* Clone the object */
ExtendedNewickAdmixtureTreeMonitor* ExtendedNewickAdmixtureTreeMonitor::clone(void) const {
    
    return new ExtendedNewickAdmixtureTreeMonitor(*this);
}


void ExtendedNewickAdmixtureTreeMonitor::closeStream() {
    outStream.close();
}


std::string ExtendedNewickAdmixtureTreeMonitor::buildExtendedNewick( void ) {
    std::string newick = buildExtendedNewick( &tree->getValue().getRoot() );
    return newick;
}




/* Build newick string */
std::string ExtendedNewickAdmixtureTreeMonitor::buildExtendedNewick( AdmixtureNode* n ) {
    // create the newick string
    std::stringstream o;
    
    // extended data is only found on admixture nodes
    std::string additionalInfo = "";
    
    // loop over admixture nodes per branch
    std::stringstream admixturestream;
    
    double br = 1.0;
    if (!n->isRoot())
    {
        
        br = branchRates->getValue()[n->getIndex()];
        //std::cout << "hmm\n";
        // [{s=&srcPtr,d=&dstPtr,t=double,w=double},{...},...,{...}]
        
        admixturestream << "[";
        AdmixtureNode* p = &n->getParent();
        while (p->getNumberOfChildren() == 1)
        {
    //        std::cout << "ok\n";

            admixturestream << "{s=";
            if (&p->getAdmixtureParent() == NULL)
                admixturestream << p;
            else
                admixturestream << &p->getAdmixtureParent();
            
            admixturestream << ",d=";
            if (&p->getAdmixtureChild() == NULL)
                admixturestream << p;
            else
                admixturestream << &p->getAdmixtureChild();
            
            admixturestream << ",t=" << p->getAge();
            admixturestream << ",w=" << p->getWeight();
            admixturestream << "}";
            
            p = &p->getParent();
            
            if (p->getNumberOfChildren() == 1)
                admixturestream << ",";

        }
        admixturestream << "]";
        additionalInfo = admixturestream.str();
    }
//    std::cout << br << "\n";
    //std::cout << additionalInfo << "\n";
    
    // test whether this is a internal or external node
    if (n->isTip()) {
        //std::cout << "tipnode\t" << additionalInfo << "\n";
        // this is a tip so we just return the name of the node
        o << n->getName() << additionalInfo << ":" << n->getBranchLength() * br;
    }
    else {
        //std::cout << "intnode\t" << additionalInfo << "\n";
        o << "(";
        for (size_t i=0; i<(n->getNumberOfChildren()-1); i++) {
            o << buildExtendedNewick( &n->getTopologyChild(i) ) << ",";
        }
        o << buildExtendedNewick( &n->getTopologyChild(n->getNumberOfChildren()-1) ) << ")" << additionalInfo << ":" << n->getBranchLength() * br;
    }
    
    return o.str();
}


/** Monitor value at generation gen */
void ExtendedNewickAdmixtureTreeMonitor::monitor(long gen) {
    
    // get the printing frequency
    int samplingFrequency = printgen;
    
    if (gen % samplingFrequency == 0) {
        // print the iteration number first
        outStream << gen;
        
        if ( posterior ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                pp += (*it)->getLnProbability();
            }
            outStream << pp;
        }
        
        if ( likelihood ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                if ( (*it)->isClamped() ) {
                    pp += (*it)->getLnProbability();
                }
            }
            outStream << pp;
        }
        
        if ( prior ) {
            // add a separator before every new element
            outStream << separator;
            
            const std::vector<DagNode*> &n = model->getDagNodes();
            double pp = 0.0;
            for (std::vector<DagNode*>::const_iterator it = n.begin(); it != n.end(); ++it) {
                if ( !(*it)->isClamped() ) {
                    pp += (*it)->getLnProbability();
                }
            }
            outStream << pp;
        }
        
        // add a separator before the tree
        outStream << separator;
        
        std::string en = buildExtendedNewick();
        std::cout << "\n" << en << "\n\n";
        
        outStream << en;
        
        outStream << std::endl;
        
    }
}


/** open the file stream for printing */
void ExtendedNewickAdmixtureTreeMonitor::openStream(void) {
    
    // open the stream to the file
    outStream.open( filename.c_str(), std::fstream::out );
    
}

/** Print header for monitored values */
void ExtendedNewickAdmixtureTreeMonitor::printHeader() {
    
    // print one column for the iteration number
    outStream << "Iteration";
    
    if ( posterior ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Posterior";
    }
    
    if ( likelihood ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Likelihood";
    }
    
    if ( prior ) {
        // add a separator before every new element
        outStream << separator;
        outStream << "Prior";
    }
    
    // add a separator tree
    outStream << separator << "Tree";
    
    // end line of stream
    outStream << std::endl;
}


void ExtendedNewickAdmixtureTreeMonitor::swapNode(DagNode *oldN, DagNode *newN) {
    
    if ( oldN == tree ) {
        tree = static_cast< TypedDagNode< AdmixtureTree > *>( newN );
    }
    else if (oldN == branchRates)
    {
        branchRates = static_cast<TypedDagNode< std::vector<double> >* >(newN);
    }
    
    else {
        // error catching
        if ( nodeVariables.find(oldN) == nodeVariables.end() ) {
            throw RbException("Cannot replace DAG node in this monitor because the monitor doesn't hold this DAG node.");
        }
        
        nodeVariables.erase( oldN );
        nodeVariables.insert( newN );
    }
    
    // delegate to base class
    Monitor::swapNode(oldN, newN);
}