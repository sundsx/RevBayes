/**
 * @file
 * This file contains the declaration of RateMatrix, which is
 * class that holds a character matrix in RevBayes.
 *
 * @brief Implementation of RateMatrix
 *
 * (c) Copyright 2009- under GPL version 3
 * @date Last modified: $Date: 2009-12-29 23:23:09 +0100 (Tis, 29 Dec 2009) $
 * @author The RevBayes Development Core Team
 * @license GPL version 3
 * @version 1.0
 * @since 2009-08-27, version 1.0
 * @interface Mcmc
 * @package distributions
 *
 * $Id: Mcmc.h 211 2009-12-29 22:23:09Z ronquist $
 */

#include "Boolean.h"
#include "RateMatrix.h"
#include "MatrixReal.h"
#include "MemberFunction.h"
#include "MemberNode.h"
#include "Natural.h"
#include "RbException.h"
#include "RbMathMatrix.h"
#include "RbNames.h"
#include "RbString.h"
#include "RealPos.h"
#include "ReferenceRule.h"
#include "Simplex.h"
#include "StochasticNode.h"
#include "ValueRule.h"
#include "VariableNode.h"
#include "VectorIndex.h"
#include "VectorNatural.h"
#include "VectorReal.h"
#include "VectorRealPos.h"
#include "VectorString.h"
#include "Workspace.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>



/** Constructor passes member rules and method inits to base class */
RateMatrix::RateMatrix(void) : MemberObject(getMemberRules()) {

    numStates            = 2;
    reversibilityChecked = false;
    isReversible         = false;
    theRateMatrix        = new MatrixReal(numStates, numStates);
    theStationaryFreqs   = new Simplex(numStates);
}


/** Construct rate matrix with n states */
RateMatrix::RateMatrix(size_t n) : MemberObject(getMemberRules()) {

    numStates            = n;
    reversibilityChecked = false;
    isReversible         = false;
    theRateMatrix        = new MatrixReal(numStates, numStates);
    theStationaryFreqs   = new Simplex(numStates);
}


/** Copy constructor */
RateMatrix::RateMatrix(const RateMatrix& m) {

    numStates            = m.numStates;
    reversibilityChecked = m.reversibilityChecked;
    isReversible         = m.isReversible;
    theRateMatrix        = new MatrixReal( *m.theRateMatrix );
    theStationaryFreqs   = new Simplex( *m.theStationaryFreqs );
}


/** Destructor */
RateMatrix::~RateMatrix(void) {
    
    delete theRateMatrix;
    delete theStationaryFreqs;
}


/** Index operator (const) */
const VectorReal& RateMatrix::operator[]( const size_t i ) const {

    if ( i >= numStates )
        throw RbException( "Index to " + RateMatrix_name + "[][] out of bounds" );
    return (*theRateMatrix)[i];
}


/** Index operator */
VectorReal& RateMatrix::operator[]( const size_t i ) {

    if ( i >= numStates )
        throw RbException( "Index to " + RateMatrix_name + "[][] out of bounds" );
    return (*theRateMatrix)[i];
}


/** Calculate the average rate for the rate matrix */
double RateMatrix::averageRate(void) const {

    double ave = 0.0;
    for (size_t i=0; i<numStates; i++)
        ave += -(*theStationaryFreqs)[i] * (*theRateMatrix)[i][i];
    return ave;
}


/** This function calculates the stationary frequencies of the rate matrix. The
    rate matrix, Q, is the infinitesimal generator of the Markov chain. It is an
    n X n matrix whose off-diagonal elements are q_ij >= 0 and whose diagonal elements
    are specified such that each row sums to zero. The rate matrix is finite (has
    a fixed number of states) and we assume that the input matrix is irreducible, as
    is the usual case for substitution models. Because Q is irreducible and finite,
    it has a stationary distribution, pi, which is a row vector of n probabilities.
    The stationary probabilities can be calculated by solving the homogeneous system
    of equations, pi*Q = 0, where 0 is a vector of zeros.
 
    We do the following to calculate the stationary frequencies. 
 
    1. We perform an LU decomposition of the transpose of the matrix Q.
 
       Q' = LU
 
    2. Now we set Ux = z (x will eventually hold the stationary probabilities). 
       Because L is nonsingular, we have z = 0. We proceed to back substitute on
       Ux = z = 0. When u_nn = 0, we can put in any solution for x. Here, we put
       in x_n = 1. We then solve the other values of x through back substitution.
 
    3. The solution obtained in 2 is not a probability vector. We normalize the
       vector such that the sum of the elements is 1.
 
    Note that the only time we need to use this function is when we don't
    know the stationary frequencies of the rate matrix beforehand. For most
    substitution models used in molecular evolution, the stationary frequencies
    are built into the rate matrix itself. These models are time-reversible.
    This function is useful for the non-reversible models.
 
    For more information on the fascinating topic of calculating the stationary
    probabilities of a rate matrix, see:
  
    Stewart, W. J. 1999. Numerical methods for computing stationary distributions of 
       finite irreducible Markov chains. In "Advances in Computational
       Probability", W. Grassmann, ed. Kluwer Academic Publishers. */
void RateMatrix::calculateStationaryFrequencies(void) const {

	// transpose the rate matrix and put into QT
    MatrixReal QT(numStates, numStates);
    for (size_t i=0; i<numStates; i++)
        for (size_t j=0; j<numStates; j++)
            QT[i][j] = (*theRateMatrix)[j][i];

	// compute the LU decomposition of the transposed rate matrix
    MatrixReal L(numStates, numStates);
    MatrixReal U(numStates, numStates);
	RbMath::computeLandU(QT, L, U);
	
	// back substitute into z = 0 to find un-normalized stationary frequencies, starting with x_n = 1.0
    VectorReal pi(numStates, 0.0);
	pi[numStates-1] = 1.0;
	for (int i=(int)numStates-2; i>=0; i--)
		{
		double dotProduct = 0.0;
		for (size_t j=i+1; j<numStates; j++)
			dotProduct += U[i][j] * pi[j];
		pi[i] = (0.0 - dotProduct) / U[i][i];
		}
		
	// normalize the solution vector
	double sum = 0.0;
	for (size_t i=0; i<numStates; i++)
		sum += pi[i];
	for (size_t i=0; i<numStates; i++)
		pi[i] /= sum;
        
    // set the stationary frequencies
    theStationaryFreqs->setValue(pi);
}


/** This function checks that the rate matrix is time reversible. It takes as
    input the rate matrix, a, and the stationary frequencies of the process, f. 
    It checks that f[i] * q[i][j] = f[j] * q[j][i] for all i != j. It does this
    by accumulating the difference | f[i] * q[i][j] - f[j] * q[j][i] | for all
    off-diagonal comparisons. If this difference is less than tolerance,
    it reports that the rate matrix is time-reversible. If the flag isRev
    is set to true, then we do not need to check because then we have determined
    previously that the rate matrix is reversible. */ 
bool RateMatrix::checkTimeReversibity(double tolerance) {
	
	double diff = 0.0;
	for (int i=0; i<numStates; i++)
		for (int j=i+1; j<numStates; j++)
			diff += fabs( (*theStationaryFreqs)[i] * (*theRateMatrix)[i][j] - (*theStationaryFreqs)[j] * (*theRateMatrix)[j][i] );
    reversibilityChecked = true;
	if (diff < tolerance)
        return true;
	return false;
}


/** Clone object */
RateMatrix* RateMatrix::clone(void) const {

    return new RateMatrix(*this);
}


/** Map calls to member methods */
DAGNode* RateMatrix::executeOperation(const std::string& name, ArgumentFrame& args) {

    if (name == "nstates") 
        {
        return ( new Natural((int)numStates) )->wrapIntoVariable();
        }
    else if (name == "stationaryfreqs")
        {
        Simplex* s = theStationaryFreqs->clone();
        return s->wrapIntoVariable();
        }
    else if (name == "averate")
        {
        return ( new RealPos(averageRate()) )->wrapIntoVariable();
        }        
    else if (name == "reversible")
        {
        return ( new Boolean(isReversible) )->wrapIntoVariable();
        }        

    return MemberObject::executeOperation( name, args );
}


/** Get class vector describing type of object */
const VectorString& RateMatrix::getClass(void) const {

    static VectorString rbClass = VectorString(RateMatrix_name) + MemberObject::getClass();
    return rbClass;
}


bool RateMatrix::getIsTimeReversible(void) {

    if (reversibilityChecked == false)
        return checkTimeReversibity(0.000001);
    return isReversible;
}


/** Get member rules */
const MemberRules& RateMatrix::getMemberRules(void) const {

    static MemberRules memberRules;
    static bool        rulesSet = false;

    if (!rulesSet) 
        {
        rulesSet = true;
        }

    return memberRules;
}


/** Get methods */
const MethodTable& RateMatrix::getMethods(void) const {

    static MethodTable   methods;
    static ArgumentRules nstatesArgRules;
    static ArgumentRules stationaryfreqsArgRules;
    static ArgumentRules averateArgRules;
    static ArgumentRules reversibleArgRules;
    static bool          methodsSet = false;

    if ( methodsSet == false ) 
        {
        // this must be here so the parser can distinguish between different instances of a character matrix
        nstatesArgRules.push_back(         new ReferenceRule( "", MemberObject_name ) );
        stationaryfreqsArgRules.push_back( new ReferenceRule( "", MemberObject_name ) );
        
        methods.addFunction("nstates",         new MemberFunction(Natural_name, nstatesArgRules)         );
        methods.addFunction("stationaryfreqs", new MemberFunction(Simplex_name, stationaryfreqsArgRules) );
        methods.addFunction("averate",         new MemberFunction(RealPos_name, averateArgRules)         );
        methods.addFunction("reversible",      new MemberFunction(Boolean_name, reversibleArgRules)      );
        
        // necessary call for proper inheritance
        methods.setParentTable( const_cast<MethodTable*>( &MemberObject::getMethods() ) );
        methodsSet = true;
        }

    return methods;
}


/** Print value for user */
void RateMatrix::printValue(std::ostream& o) const {

    o << "Rate matrix:" << std::endl;
    o << theRateMatrix->richInfo() << std::endl;
    o << "Stationary frequencies: ";
    o << theStationaryFreqs->richInfo() << std::endl;
}


/** Rescale the rates such that the average rate is r */
void RateMatrix::rescaleToAverageRate(const double r) const {

    double curAve = averageRate();
    double scaleFactor = r / curAve;
    for (size_t i=0; i<numStates; i++)
        for (size_t j=0; j<numStates; j++)
            (*theRateMatrix)[i][j] *= scaleFactor;
}


/** Complete info */
std::string RateMatrix::richInfo(void) const {

	std::ostringstream o;
    printValue( o );
    return o.str();
}


void RateMatrix::setDiagonal(void) {

    for (size_t i=0; i<numStates; i++)
        {
        double sum = 0.0;
        for (size_t j=0; j<numStates; j++)
            {
            if (i != j)
                sum += (*theRateMatrix)[i][j];
            }
        (*theRateMatrix)[i][i] = -sum;
        }
}


/** Directly set whether or not the rate matrix is time reversible */
void RateMatrix::setIsTimeReversible(const bool tf) {

    reversibilityChecked = true;
    isReversible = tf;
}


/** Set the stationary frequencies directly. We assume that we know
    what the stationary frequencies are when this function is called. We
    would know the stationary frequencies for most phylogenetic models. */
void RateMatrix::setStationaryFrequencies(std::vector<double>& f) const {

    theStationaryFreqs->setValue(f);
}


/** Wrap value into a variable */
DAGNode* RateMatrix::wrapIntoVariable( void ) {
    
    MemberNode* nde = new MemberNode( this );
    return static_cast<DAGNode*>(nde);
}
