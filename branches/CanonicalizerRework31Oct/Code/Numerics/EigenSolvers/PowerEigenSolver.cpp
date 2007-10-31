// $Id$
//
//  Copyright (C) 2004-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include "PowerEigenSolver.h"
#include <Numerics/Vector.h>
#include <Numerics/Matrix.h>
#include <Numerics/SymmMatrix.h>
#include <RDGeneral/Invariant.h>
#include <time.h>

#define MAX_ITERATIONS 1000
#define TOLERANCE 0.001
#define HUGE_EIGVAL 1.0e10
#define TINY_EIGVAL 1.0e-10

namespace RDNumeric {
  namespace EigenSolvers {
    bool powerEigenSolver(unsigned int numEig, DoubleSymmMatrix &mat,
                          DoubleMatrix &eigenVectors, DoubleVector &eigenValues, int seed) {
      // first check all the sizes
      unsigned int N = mat.numRows();
      unsigned int evRows, evCols;
      evRows = eigenVectors.numRows();
      evCols = eigenVectors.numCols();
      CHECK_INVARIANT(evCols >= N, "");
      CHECK_INVARIANT(evRows >= numEig, "");
      CHECK_INVARIANT(eigenValues.size() >= numEig, "");
      CHECK_INVARIANT(numEig <= N, "");
      
      unsigned int ei;
      double eigVal, prevVal, maxEval;
      bool converged=false;
      unsigned int i, j, id, iter, evalId;
      
      DoubleVector v(N), z(N);
      if(seed<=0) seed = clock();
      for (ei = 0; ei < numEig; ei++) {
        eigVal = -HUGE_EIGVAL;
	seed += ei;
	v.setToRandom(seed);

        converged = false;
        for (iter = 0; iter < MAX_ITERATIONS; iter++) {
          // z = mat*v
          multiply(mat, v, z);
          prevVal = eigVal;
          maxEval = -1.0;
          evalId = z.largestAbsValId();
          eigVal = z.getVal(evalId);
          
          if (fabs(eigVal) < TINY_EIGVAL) {
            break;
          }
          
          // compute the next estimate for the eigen vector
          v.assign(z);
          v /= eigVal;
	  if (fabs(eigVal - prevVal) < TOLERANCE) {
	    converged = true;
            break;
          }
        }
        if (!converged) {
          break;
        }
        v.normalize();
        
        // save this is a eigen vector and value
        // directly access the data instead of setVal so that we save time
        double *eigVecData = eigenVectors.getData();
        id = ei*evCols;
        double *vdata = v.getData();
        for (i = 0; i < N; i++) {
          eigVecData[id + i] = vdata[i];
        }
        eigenValues.getData()[ei] = eigVal;
        
        // now remove this eigen vector space out of the matrix
        double *matData = mat.getData();
        for (i = 0; i < N; i++) {
          id = i*(i+1)/2;
          for (j = 0; j < i+1; j++) {
            matData[id+j] -= (eigVal*vdata[i]*vdata[j]);
          }
        }
      }
      return converged;
    }
  }
}
