// $Id$
//
//  Copyright (C) 2003-2008 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include "FileParsers.h"
#include "MolFileStereochem.h"
#include <RDGeneral/Invariant.h>
#include <GraphMol/RDKitQueries.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/format.hpp>
#include <RDGeneral/FileParseException.h>
#include <RDGeneral/BadFileException.h>


namespace RDKit{
  
  //*************************************
  //
  // Every effort has been made to adhere to MDL's standard
  // for mol files
  //  
  //*************************************

  const std::string GetMolFileChargeInfo(const RWMol &mol){
    std::stringstream res;
    std::stringstream chgss;
    std::stringstream radss;
    unsigned int nChgs=0;
    unsigned int nRads=0;
    for(ROMol::ConstAtomIterator atomIt=mol.beginAtoms();
        atomIt!=mol.endAtoms();++atomIt){
      const Atom *atom=*atomIt;
      if(atom->getFormalCharge()!=0){
        ++nChgs;
        chgss << boost::format(" % 3d % 3d") % (atom->getIdx()+1) % atom->getFormalCharge();
        if(nChgs==8){
          res << boost::format("M  CHG% 3d")%nChgs << chgss.str()<<std::endl;
          chgss.str("");
          nChgs=0;
        }
      }
      unsigned int nRadEs=atom->getNumRadicalElectrons();
      if(nRadEs!=0){
        ++nRads;
        switch(nRadEs){
        case 1:
          nRadEs=2;
          break;
        case 2:
          nRadEs=3; // we use triplets, not singlets:
          break;
        default:
          BOOST_LOG(rdWarningLog)<<" unsupported radical count: "<<nRadEs<<" set to 3."<<std::endl;
          nRadEs=3;
        }
        radss << boost::format(" % 3d % 3d") % (atom->getIdx()+1) % nRadEs;
        if(nRads==8){
          res << boost::format("M  RAD% 3d")%nRads << radss.str()<<std::endl;
          radss.str("");
          nRads=0;
        }
      }
    }
    if(nChgs){
      res << boost::format("M  CHG% 3d")%nChgs << chgss.str()<<std::endl;
    }
    if(nRads){
      res << boost::format("M  RAD% 3d")%nRads << radss.str()<<std::endl;
    }
    return res.str();
  }
  const std::string GetMolFileQueryInfo(const RWMol &mol){
    return "";
  }

  const std::string AtomGetMolFileSymbol(const Atom *atom){
    PRECONDITION(atom,"");

    std::string res;
    if(atom->getAtomicNum()){
      res=atom->getSymbol();
    } else {
      if(!atom->hasProp("dummyLabel")){
      	res = "R";
      } else {
	      std::string symb;
        atom->getProp("dummyLabel",symb);
        if(symb=="*") res="R";
        else if(symb=="X") res="R";
        else if(symb=="Xa") res="R1";
        else if(symb=="Xb") res="R2";
      	else if(symb=="Xc") res="R3";
      	else if(symb=="Xd") res="R4";
      	else if(symb=="Xf") res="R5";
      	else if(symb=="Xg") res="R6";
      	else if(symb=="Xh") res="R7";
      	else if(symb=="Xi") res="R8";
      	else if(symb=="Xj") res="R9";
      	else res=symb;
      }
    }
    // pad the end with spaces
    while(res.size()<3) res += " ";
    return res;
  }
  const std::string GetMolFileAtomLine(const Atom *atom, const Conformer *conf=0){
    PRECONDITION(atom,"");
    std::string res;
    int massDiff,chg,stereoCare,hCount,totValence,rxnComponentType;
    int rxnComponentNumber,atomMapNumber,inversionFlag,exactChangeFlag;
    massDiff=0;
    chg=0;
    stereoCare=0;
    hCount=0;
    totValence=0;
    rxnComponentType=0;
    rxnComponentNumber=0;
    atomMapNumber=0;
    inversionFlag=0;
    exactChangeFlag=0;

    double atomMassDiff=atom->getMass()-PeriodicTable::getTable()->getAtomicWeight(atom->getAtomicNum());
    massDiff = static_cast<int>(atomMassDiff+.1);
    
    double x, y, z;
    x = y = z = 0.0;
    if (conf) {
      const RDGeom::Point3D pos = conf->getAtomPos(atom->getIdx());
      x = pos.x; y = pos.y; z = pos.z;
    } 
    std::string symbol = AtomGetMolFileSymbol(atom);
    std::stringstream ss;
    ss << boost::format("% 10.4f% 10.4f% 10.4f %3s% 2d% 3d  0% 3d% 3d% 3d  0% 3d% 3d% 3d% 3d% 3d") % x % y % z % symbol.c_str() %
      massDiff%chg%hCount%stereoCare%totValence%rxnComponentType%
      rxnComponentNumber%atomMapNumber%inversionFlag%exactChangeFlag;
    res += ss.str();
    return res;
  };
  
  const std::string BondGetMolFileSymbol(const Bond *bond){
    PRECONDITION(bond,"");
    // FIX: should eventually recognize queries
    std::string res;
    switch(bond->getBondType()){
    case Bond::SINGLE:
      if(bond->getIsAromatic()){
        res="  4";
      } else {
        res="  1";
      }
      break;
    case Bond::DOUBLE: 
      if(bond->getIsAromatic()){
        res="  4";
      } else {
        res="  2";
      }
      break;
    case Bond::TRIPLE: res="  3";break;
    case Bond::AROMATIC: res="  4";break;
    default: res="  0";break;
    }
    return res;
    //return res.c_str();
  }

  // only valid for single bonds
  int BondGetDirCode(const Bond::BondDir dir){
    int res=0;
    switch(dir){
    case Bond::NONE: res=0;break;
    case Bond::BEGINWEDGE: res=1;break;
    case Bond::BEGINDASH: res=6;break;
    case Bond::UNKNOWN: res=4;break;
    default:
      break;
    }
    return res;
  }

  const std::string GetMolFileBondLine(const Bond *bond, const INT_MAP_INT &wedgeBonds,
                                 const Conformer *conf){
    PRECONDITION(bond,"");
    std::string symbol = BondGetMolFileSymbol(bond);
    int dirCode=0;
    
    Bond::BondDir dir=Bond::NONE;
    bool reverse = false;
    if(bond->getBondType()==Bond::SINGLE){
      // single bond stereo chemistry
      dir = DetermineBondWedgeState(bond, wedgeBonds, conf);
      dirCode = BondGetDirCode(dir);
      // if this bond needs to be wedged it is possible that this
      // wedging was determined by a chiral atom at the end of the
      // bond (instead of at the beginning). In this case we need to
      // reverse the begin and end atoms for the bond when we write
      // the mol file
      if ((dirCode == 1) || (dirCode == 6)) {
        INT_MAP_INT_CI wbi = wedgeBonds.find(bond->getIdx());
        if (static_cast<unsigned int>(wbi->second) != bond->getBeginAtomIdx()) {
          reverse = true;
        }
      }
    } else if (bond->getBondType()==Bond::DOUBLE) {
      // double bond stereo chemistry - 
      // we will worry about it only if it is  "any"
      Bond::BondStereo stType = bond->getStereo();
      if (stType == Bond::STEREOANY) { //"A") {
        dirCode = 3; // can be either cis/trans
      }
    }

    std::stringstream ss;
    if (reverse) {
      // switch the begin and end atoms on the bond line
      ss << std::setw(3) << bond->getEndAtomIdx()+1;
      ss << std::setw(3) << bond->getBeginAtomIdx()+1;
    } else {
      ss << std::setw(3) << bond->getBeginAtomIdx()+1;
      ss << std::setw(3) << bond->getEndAtomIdx()+1;
    }
    ss << symbol;
    ss << " " << std::setw(2) << dirCode;
    return ss.str();
  }
    
  //------------------------------------------------
  //
  //  gets a mol block as a string
  //
  //------------------------------------------------
  std::string MolToMolBlock(const ROMol &mol,bool includeStereo, int confId){
    // NOTE: kekulize the molecule before writing it out
    // because of the way mol files handle aromaticity
    ROMol tromol(mol);
    RWMol &trwmol = static_cast<RWMol &>(tromol);
    MolOps::Kekulize(trwmol);

#if 0
    if(includeStereo){
      // assign "any" status to any stereo bonds that are not 
      // marked with "E" or "Z" code - these bonds need to be explictly written
      // out to the mol file
      MolOps::findPotentialStereoBonds(trwmol);
      // now assign stereo code if any have been specified by the directions on
      // single bonds
      MolOps::assignBondStereoCodes(trwmol);
    }
#endif
    const RWMol &tmol = const_cast<RWMol &>(trwmol);

    std::string res;

    int nAtoms,nBonds,nLists,chiralFlag,nsText,nRxnComponents;
    int nReactants,nProducts,nIntermediates;
    nAtoms = tmol.getNumAtoms();
    nBonds = tmol.getNumBonds();
    nLists = 0;
    chiralFlag = 0;
    nsText=0;
    nRxnComponents=0;
    nReactants=0;
    nProducts=0;
    nIntermediates=0;

    const Conformer *conf;
    if(confId<0 && tmol.getNumConformers()==0){
      conf=0;
    } else {
      conf = &(tmol.getConformer(confId));
    }

    if(tmol.hasProp("_Name")){
      std::string name;
      tmol.getProp("_Name",name);
      res += name;
    }
    res += "\n";

    // info
    if(tmol.hasProp("MolFileInfo")){
      std::string info;
      tmol.getProp("MolFileInfo",info);
      res += info;
    } else {
      std::stringstream ss;
      ss<<"  "<<std::setw(8)<<"RDKit";
      ss<<std::setw(10)<<"";
      if(conf){
        if(conf->is3D()){
          ss<<"3D";
        } else {
          ss<<"2D";
        }
      }
      res += ss.str();
    }
    res += "\n";
    // comments
    if(tmol.hasProp("MolFileComments")){
      std::string info;
      tmol.getProp("MolFileComments",info);
      res += info;
    }
    res += "\n";

    std::stringstream ss;
    ss<<std::setw(3)<<nAtoms;
    ss<<std::setw(3)<<nBonds;
    ss<<std::setw(3)<<nLists;
    ss<<std::setw(3)<<0;
    ss<<std::setw(3)<<chiralFlag;
    ss<<std::setw(3)<<nsText;
    ss<<std::setw(3)<<nRxnComponents;
    ss<<std::setw(3)<<nReactants;
    ss<<std::setw(3)<<nProducts;
    ss<<std::setw(3)<<nIntermediates;
    ss<<"999 V2000\n";
    res += ss.str();
    ROMol::ConstAtomIterator atomIt;
    for(atomIt=tmol.beginAtoms();atomIt!=tmol.endAtoms();atomIt++){
      res += GetMolFileAtomLine(*atomIt, conf);
      res += "\n";
    }

    INT_MAP_INT wedgeBonds = pickBondsToWedge(tmol);
    ROMol::ConstBondIterator bondIt;
    for(bondIt=tmol.beginBonds();bondIt!=tmol.endBonds();bondIt++){
      res += GetMolFileBondLine(*bondIt, wedgeBonds, conf);
      res += "\n";
    }

    res += GetMolFileChargeInfo(tmol);
    res += GetMolFileQueryInfo(tmol);
    
    
    // FIX: aliases, atom lists, etc.
    res += "M  END\n";
    return res;
  }
  
  //------------------------------------------------
  //
  //  Dump a molecule to a file
  //
  //------------------------------------------------
  void MolToMolFile(const ROMol &mol,std::string fName,bool includeStereo, int confId){
    std::ofstream *outStream = new std::ofstream(fName.c_str());
    if (!outStream || !(*outStream) || outStream->bad() ) {
      std::ostringstream errout;
      errout << "Bad output file " << fName;
      throw BadFileException(errout.str());
    }
    std::string outString = MolToMolBlock(mol,includeStereo, confId);
    *outStream  << outString;
    delete outStream;
  }    
}

