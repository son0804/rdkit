//
//  Copyright (C) 2004-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#ifndef _RD_UFFATOMTYPER_H__
#define _RD_UFFATOMTYPER_H__

#include <vector>
#include <string>

namespace ForceFields {
  namespace UFF {
    class AtomicParams;
  }
}
namespace RDKit {
  class ROMol;
  class Atom;

  namespace UFF {
    typedef std::vector<const ForceFields::UFF::AtomicParams *> AtomicParamVect;

    AtomicParamVect getAtomTypes(const ROMol &mol,const std::string &paramData="");

    namespace Tools {
      // these functions are primarily exposed so they can be tested.
      void addAtomChargeFlags(const Atom *atom,std::string &atomKey,
			      bool tolerateChargeMismatch=true);
      std::string getAtomLabel(const Atom *atom);
    }

  }
}


#endif
