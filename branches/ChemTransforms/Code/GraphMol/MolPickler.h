//
//  Copyright (C) 2001-2006 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#ifndef _RD_MOLPICKLE_H
#define _RD_MOLPICKLE_H

// Std stuff
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#ifdef WIN32
#include <ios>
#endif

namespace RDKit{
  class ROMol;

  //! used to indicate exceptions whilst pickling (serializing) molecules
  class MolPicklerException : public std::exception {
    public :
      MolPicklerException(const char *msg) : _msg(msg) {};
      MolPicklerException(const std::string msg) : _msg(msg) {};
      const char *message () const { return _msg.c_str(); };
      ~MolPicklerException () throw () {};
    
    private :
      std::string _msg;
  };

  //! handles pickling (serializing) molecules
  class MolPickler{
  public:
    static const int versionMajor,versionMinor,versionPatch; //!< mark the pickle version
    static const int endianId;  //! mark the endian-ness of the pickle

    //! the pickle format is tagged using these tags:
    typedef enum {
      VERSION=0,
      BEGINATOM,
      ATOM_INDEX,
      ATOM_NUMBER,
      ATOM_POS,
      ATOM_CHARGE,
      ATOM_NEXPLICIT,
      ATOM_CHIRALTAG,
      ATOM_MASS,
      ATOM_ISAROMATIC,
      ENDATOM,
      BEGINBOND,
      BOND_INDEX,
      BOND_BEGATOMIDX,
      BOND_ENDATOMIDX,
      BOND_TYPE,
      BOND_DIR,
      ENDBOND,
      BEGINPROPS,
      ENDPROPS,
      BEGINSSSR,
      ENDSSSR,
      ENDMOL,
      BEGINCONFS,
    } Tags;

    //! pickles a molecule and sends the results to stream \c ss
    static void pickleMol(const ROMol *mol,std::ostream &ss);
    static void pickleMol(const ROMol &mol,std::ostream &ss) {MolPickler::pickleMol(&mol,ss);};
    //! pickles a molecule and adds the results to string \c res
    static void pickleMol(const ROMol *mol,std::string &res);
    static void pickleMol(const ROMol &mol,std::string &res) {MolPickler::pickleMol(&mol,res);};

    //! constructs a molecule from a pickle stored in a string
    static void molFromPickle(const std::string &pickle,ROMol *mol);
    static void molFromPickle(const std::string &pickle,ROMol &mol) {MolPickler::molFromPickle(pickle,&mol);};

    //! constructs a molecule from a pickle stored in a stream
    static void molFromPickle(std::istream &ss,ROMol *mol);
    static void molFromPickle(std::istream &ss,ROMol &mol) { MolPickler::molFromPickle(ss,&mol); };
  private:
    //! do the actual work of pickling a molecule
    template <typename T>
    static void _pickle(const ROMol *mol,std::ostream &ss);

    //! do the actual work of pickling an Atom
    template <typename T>
      static void _pickleAtom(std::ostream &ss,const Atom *atom);
    //bool includeAtomCoords);

    //! do the actual work of pickling a Bond
    template <typename T>
    static void _pickleBond(std::ostream &ss,const Bond *bond,
			    std::map<int,int> &atomIdxMap);

    //! do the actual work of pickling an SSSR structure
    template <typename T>
    static void _pickleSSSR(std::ostream &ss,const RingInfo *ringInfo,
			    std::map<int,int> &atomIdxMap);

    //! do the actual work of pickling a Conformer
    template <typename T>
    static void _pickleConformer(std::ostream &ss,const Conformer *conf);

    //! do the actual work of de-pickling a molecule
    template <typename T>
    static void _depickle(std::istream &ss,ROMol *mol, int version);


    //! extract atomic data from a pickle and add the resulting Atom to the molecule
    template <typename T>
      static Atom *_addAtomFromPickle(std::istream &ss,ROMol *mol, RDGeom::Point3D &pos,
				      int version,
                                      bool directMap=false);

    //! extract bond data from a pickle and add the resulting Bond to the molecule
    template <typename T>
    static Bond *_addBondFromPickle(std::istream &ss,ROMol *mol,
				    int version,
				    bool directMap=false);

    //! extract ring info from a pickle and add the resulting RingInfo to the molecule
    template <typename T>
    static void _addRingInfoFromPickle(std::istream &ss,ROMol *mol,
				       int version,
				       bool directMap=false);

    //! extract a conformation from a pickle
    template <typename T> 
      static Conformer *_conformerFromPickle(std::istream &ss,int version);

    //! backwards compatibility
    static void _pickleV1(const ROMol *mol,std::ostream &ss);
    //! backwards compatibility
    static void _depickleV1(std::istream &ss,ROMol *mol);
    //! backwards compatibility
    static void _addAtomFromPickleV1(std::istream &ss,ROMol *mol);
    //! backwards compatibility
    static void _addBondFromPickleV1(std::istream &ss,ROMol *mol);

  };  
  
};


#endif
