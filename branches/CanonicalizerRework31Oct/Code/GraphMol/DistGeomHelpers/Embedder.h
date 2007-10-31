//
//  Copyright (C) 2004-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//

#ifndef _RD_EMBEDDER_H_
#define _RD_EMBEDDER_H_

#include <GraphMol/ROMol.h>

namespace RDKit {
  namespace DGeomHelpers {
    
    //! Compute an embedding (in 3D) for the specified molecule using Distance Geometry
    /*!
      The following operations are performed (in order) here:
       -# Build a distance bounds matrix based on the topology, including 1-5
          distances but not VDW scaling
       -# Triangle smooth this bounds matrix
       -# If step 2 fails - repeat step 1, this time without 1-5 bounds and with vdW
          scaling, and repeat step 2
       -# Pick a distance matrix at random using the bounds matrix
       -# Compute initial coordinates from the distance matrix
       -# Repeat steps 3 and 4 until maxIterations is reached or embedding is successful
       -# Adjust initial coordinates by minimizing a Distance Violation error function
      
      \param mol            Molecule of interest
      \param maxIterations  Max. number of times the embedding will be tried if coordinates are 
                            not obtained successfully
      \param seed           provides a seed for the random number generator (so that the same
                            coordinates can be obtained for a molecule on multiple runs)
			    If negative, the RNG will not be seeded.
      \param clearConfs     Clear all existing conformations on the molecule
      \param randNegEig     Picks coordinates at random when a embedding process produces
                            negative eigenvalues
      \param numZeroFail    Fail embedding if we find this many or more zero eigenvalues
                            (within a tolerance)
      \param optimizerForceTol set the tolerance on forces in the distgeom optimizer
                               (this shouldn't normally be altered in client code).

      \param basinThresh    set the basin threshold for the DGeom force field,
                            (this shouldn't normally be altered in client code).

      \return ID of the conformations added to the molecule, -1 if the emdedding failed
    */
    int EmbedMolecule(ROMol &mol, unsigned int maxIterations=30, int seed=-1,
                      bool clearConfs=true,bool randNegEig=true,
                      unsigned int numZeroFail=1,double optimizerForceTol=1e-3,
                      double basinThresh=5.0);

    //*! Embed multiple conformations for a molecule
    /*!
      This is kind of equivalent to calling EmbedMolecule multiple times - just that the bounds
      matrix is computed only once from the topology

      \param mol            Molecule of interest
      \param numConfs       Number of conformations to be generated
      \param maxIterations  Max. number of times the embedding will be tried if coordinates are 
                            not obtained successfully
      \param seed           provides a seed for the random number generator (so that the same
                            coordinates can be obtained for a molecule on multiple runs).
			    If negative, the RNG will not be seeded.
      \param clearConfs     Clear all existing conformations on the molecule
      \param randNegEig     Picks coordinates at random when a embedding process produces
                            negative eigenvalues
      \param numZeroFail    Fail embedding if we find this many or more zero eigenvalues
                            (within a tolerance)
      \param optimizerForceTol set the tolerance on forces in the DGeom optimizer
                               (this shouldn't normally be altered in client code).
      \param basinThresh    set the basin threshold for the DGeom force field,
                            (this shouldn't normally be altered in client code).

      \param pruneRmsThresh Retain only the conformations out of 'numConfs' after embedding that are
                            at least this far apart from each other. RMSD is computed on the heavy atoms.
                            Prunining is greedy; i.e. the first embedded conformation is retained and from
			    then on only those that are atleast pruneRmsThresh away from already 
			    retained conformations are kept. The pruning is done after embedding and 
			    bounds violation minimization. No pruning by default.

      \return an INT_VECT of conformer ids

    */
    INT_VECT EmbedMultipleConfs(ROMol &mol, unsigned int numConfs=10,
                                unsigned int maxIterations=30, 
                                int seed=-1, bool clearConfs=true, 
                                bool randNegEig=true, unsigned int numZeroFail=1,
				double optimizerForceTol=1e-3,double basinThresh=5.0,
                                double pruneRmsThresh=-1.0);
  }
}

#endif
