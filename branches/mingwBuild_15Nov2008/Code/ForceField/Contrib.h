//
//  Copyright (C) 2004-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#ifndef __RD_FFCONTRIB_H__
#define __RD_FFCONTRIB_H__

namespace ForceFields {
  class ForceField;

  //! abstract base class for contributions to ForceFields
  class ForceFieldContrib {
  public:
    ForceFieldContrib() : dp_forceField(0) {};
    virtual ~ForceFieldContrib() {};

    //! returns our contribution to the energy of a position
    virtual double getEnergy(double *pos) const = 0;

    //! calculates our contribution to the gradients of a position
    virtual void getGrad(double *pos,double *grad) const = 0;
    
  protected:
    ForceField *dp_forceField;  //!< our owning ForceField
  };

}

#endif
