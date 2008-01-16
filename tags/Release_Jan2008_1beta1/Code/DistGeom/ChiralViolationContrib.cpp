// $Id$
//
// Created by Santosh Putta, Nov 2006
//

#include "ChiralViolationContrib.h"
#include "EmbedObject.h"
#include <ForceField/ForceField.h>
#include <Geometry/point.h>

namespace DistGeom {
  ChiralViolationContrib::ChiralViolationContrib(ForceFields::ForceField *owner, const ChiralSet* cset,
						 double weight) {
    PRECONDITION(owner,"bad owner");
    PRECONDITION(cset, "bad chiral set")

    RANGE_CHECK(0,cset->d_idx1,owner->positions().size()-1);
    RANGE_CHECK(0,cset->d_idx2,owner->positions().size()-1);
    RANGE_CHECK(0,cset->d_idx3,owner->positions().size()-1);
    RANGE_CHECK(0,cset->d_idx4,owner->positions().size()-1);

    dp_forceField = owner;
    
    d_idx1 = cset->d_idx1;
    d_idx2 = cset->d_idx2;
    d_idx3 = cset->d_idx3;
    d_idx4 = cset->d_idx4;

    d_volLower = cset->getLowerVolumeBound();
    d_volUpper = cset->getUpperVolumeBound();

    d_weight = weight;
  }

  double ChiralViolationContrib::getEnergy(double *pos) const {
    PRECONDITION(dp_forceField,"no owner");
    PRECONDITION(pos,"bad vector");

    unsigned int dim = dp_forceField->dimension();
    
    // even if we are minimizing in higher dimension the chiral volume is 
    // calculated using only the first 3 dimensions
    RDGeom::Point3D v1(pos[d_idx1*dim] - pos[d_idx4*dim], 
		       pos[d_idx1*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx1*dim+2] - pos[d_idx4*dim+2]);
    

    RDGeom::Point3D v2(pos[d_idx2*dim] - pos[d_idx4*dim], 
		       pos[d_idx2*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx2*dim+2] - pos[d_idx4*dim+2]);
    
    RDGeom::Point3D v3(pos[d_idx3*dim] - pos[d_idx4*dim], 
		       pos[d_idx3*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx3*dim+2] - pos[d_idx4*dim+2]);

    RDGeom::Point3D v2xv3 = v2.crossProduct(v3);
    
    double vol = v1.dotProduct(v2xv3);
    
    if (vol < d_volLower) {
      return d_weight*(vol - d_volLower)*(vol - d_volLower);
    } else if (vol > d_volUpper) {
      return d_weight*(vol - d_volUpper)*(vol - d_volUpper);
    } else {
      return 0.0;
    }
  }

  
  void ChiralViolationContrib::getGrad(double *pos, double *grad) const {
    PRECONDITION(dp_forceField,"no owner");
    PRECONDITION(pos,"bad vector");

    unsigned int dim = dp_forceField->dimension();
    
    // even if we are minimizing in higher dimension the chiral volume is 
    // calculated using only the first 3 dimensions
    RDGeom::Point3D v1(pos[d_idx1*dim] - pos[d_idx4*dim], 
		       pos[d_idx1*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx1*dim+2] - pos[d_idx4*dim+2]);
    

    RDGeom::Point3D v2(pos[d_idx2*dim] - pos[d_idx4*dim], 
		       pos[d_idx2*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx2*dim+2] - pos[d_idx4*dim+2]);
    
    RDGeom::Point3D v3(pos[d_idx3*dim] - pos[d_idx4*dim], 
		       pos[d_idx3*dim+1] - pos[d_idx4*dim+1],
		       pos[d_idx3*dim+2] - pos[d_idx4*dim+2]);

    RDGeom::Point3D v2xv3 = v2.crossProduct(v3);
    
    double vol = v1.dotProduct(v2xv3);
    double preFactor;

    if (vol < d_volLower) {
      preFactor = d_weight*(vol - d_volLower);
    } else if (vol > d_volUpper) {
      preFactor = d_weight*(vol - d_volUpper);
    } else {
      return;
    }

    // now comes the hard part - there are a total of 12 variables involved
    // 4 x 3 - four points and 3 dimensions
    // 
    grad[dim*d_idx1] += preFactor*((v2.y)*(v3.z) - (v3.y)*(v2.z));
    grad[dim*d_idx1+1] += preFactor*((v3.x)*(v2.z) - (v2.x)*(v3.z));
    grad[dim*d_idx1+2] += preFactor*((v2.x)*(v3.y) - (v3.x)*(v2.y));
    
    grad[dim*d_idx2] += preFactor*((v3.y)*(v1.z) - (v3.z)*(v1.y));
    grad[dim*d_idx2 + 1] += preFactor*((v3.z)*(v1.x) - (v3.x)*(v1.z));
    grad[dim*d_idx2 + 2] += preFactor*((v3.x)*(v1.y) - (v3.y)*(v1.x));
    
    grad[dim*d_idx3] += preFactor*((v2.z)*(v1.y) - (v2.y)*(v1.z));
    grad[dim*d_idx3 + 1] += preFactor*((v2.x)*(v1.z) - (v2.z)*(v1.x));
    grad[dim*d_idx3 + 2] += preFactor*((v2.y)*(v1.x) - (v2.x)*(v1.y));

    grad[dim*d_idx4] += preFactor*(pos[d_idx1*dim+2]*(pos[d_idx2*dim+1] - pos[d_idx3*dim+1])
				   + pos[d_idx2*dim+2]*(pos[d_idx3*dim+1] - pos[d_idx1*dim+1])
				   + pos[d_idx3*dim+2]*(pos[d_idx1*dim+1] - pos[d_idx2*dim+1]));

    grad[dim*d_idx4+1] += preFactor*(pos[d_idx1*dim]*(pos[d_idx2*dim+2] - pos[d_idx3*dim+2])
				     + pos[d_idx2*dim]*(pos[d_idx3*dim+2] - pos[d_idx1*dim+2])
				     + pos[d_idx3*dim]*(pos[d_idx1*dim+2] - pos[d_idx2*dim+2]));

    grad[dim*d_idx4+2] += preFactor*(pos[d_idx1*dim+1]*(pos[d_idx2*dim] - pos[d_idx3*dim])
				     + pos[d_idx2*dim+1]*(pos[d_idx3*dim] - pos[d_idx1*dim])
				     + pos[d_idx3*dim+1]*(pos[d_idx1*dim] - pos[d_idx2*dim]));
  }
}

