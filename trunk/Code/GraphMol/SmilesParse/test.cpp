// $Id$
//
//  Copyright (C) 2003-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include <iostream>
#include <GraphMol/RDKitBase.h>
#include "SmilesParse.h"
#include "SmilesWrite.h"
#include <RDGeneral/RDLog.h>
#include <boost/log/functions.hpp>
using namespace RDKit;
using namespace std;
typedef ROMol Mol;

void testPass(){
  int i = 0;
  ROMol *mol,*mol2;
  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing molecules which should parse." << std::endl;
  string smis[]={
#if 1
    "C1CC2C1CC2",
    "c1cccn(=O)c1",
    "C",
    "CC",
    "C-C",
    "C=C",
    "[CH2+]C[CH+2]",
    "C1CC1",
    "C1CC=1",
    "C=1CC1",
    "C=C-O",
    "C1CC1",
    "C1NC1",
    "C1=CC1",
    "C1CCC1",
    "CC(C)CC",
    "CC(=O)O",
    "C1C(=O)C1",
    "C1C(N)C1",
    "CC(O)C",
    "OC=CCC",
    "CC([O-])O",
    "C1CC2C1CC2",
    "Cl/C=C/Cl",
    "Cl/C=C\\Cl",
    "Cl/C=C/Cl",
    "Cl/C=C\\Cl",
    "C1CC.CC1",
    "C1C(C2CC2).C2CC2C1",
    "[Na+].[Cl-].[NH4+].[Cl-]",
    "C[35Cl]",
    "C%10CC%10",
    "[H][H]",
    "[H+]",
    "C[N+](=O)[O-]",
#endif
    "N1C(=N)SC=C1",
    "[O-][N+](=O)C1=CNC(=N)S1",
    "CN(=O)=O",
    "C1=CC=C[N+]([O-])=C1",
    "C1=CC=CN(=O)=C1",
    // test whitespace tolerance:
    "  C1=CC=CN(=O)=C1",
    "C1=CC=CN(=O)=C1  ",
    "  C1=CC=CN(=O)=C1  ",
    "\tC1=CC=CN(=O)=C1\r\n",
    "EOS"};
  while( smis[i] != "EOS" ){
    string smi = smis[i];
    mol = SmilesToMol(smi);
    CHECK_INVARIANT(mol,smi);
    if (mol) {
      int nAts = mol->getNumAtoms();
      CHECK_INVARIANT(nAts!=0,smi.c_str());
      mol2 = SmilesToMol(MolToSmiles(*mol));
      CHECK_INVARIANT(mol2->getNumAtoms()==nAts,smi.c_str())
      delete mol;
      delete mol2;
    }
    i++;
  }
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testFail(){
  int i = 0;
  Mol *mol;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing molecules which should fail to parse/sanitize." << std::endl;

  // alternate good and bad smiles here to ensure that the parser can resume parsing
  // on good input:
  string smis[]={
    "CC=(CO)C",
    "CC(=CO)C",
    "C1CC",
    "C1CC1",
    "Ccc",
    "CCC",
    "fff", // tests the situation where the parser cannot do anything at all
    "CCC",
    "N(=O)(=O)=O", // bad sanitization failure
    "C1CC1",
    "EOS"};

  // turn off the error log temporarily:
  while( smis[i] != "EOS" ){
    string smi = smis[i];
    boost::logging::disable_logs("rdApp.error");
    try {
      mol = SmilesToMol(smi);
    } catch (MolSanitizeException) {
      mol = (Mol *)0;
    }
    boost::logging::enable_logs("rdApp.error");
    if(!(i%2)) {
      CHECK_INVARIANT(!mol,smi);
    }
    else{
      CHECK_INVARIANT(mol,smi);
    }
    i++;
  }
  BOOST_LOG(rdInfoLog)  << "\tdone" << std::endl;

}

void testDetails(){
  int i = 0;
  Mol *mol;
  Mol::GRAPH_NODE_TYPE a;
  std::string smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing details" << std::endl;
  // implicit/explicit H handling
  smi = "OC([OH])C[O-]";
  mol = SmilesToMol(smi);
  CHECK_INVARIANT(mol,smi);
  CHECK_INVARIANT(mol->getNumAtoms()==5,"");
  a = mol->getAtomWithIdx(0);
  CHECK_INVARIANT(a->getImplicitValence()==1,"");
  CHECK_INVARIANT(a->getExplicitValence()==1,"");
  CHECK_INVARIANT(a->getNoImplicit()==0,"");
  CHECK_INVARIANT(a->getFormalCharge()==0,"");
  a = mol->getAtomWithIdx(2);
  CHECK_INVARIANT(a->getImplicitValence()==0,"");
  CHECK_INVARIANT(a->getExplicitValence()==2,"");
  CHECK_INVARIANT(a->getNoImplicit()==1,"");
  CHECK_INVARIANT(a->getFormalCharge()==0,"");
  a = mol->getAtomWithIdx(4);
  CHECK_INVARIANT(a->getImplicitValence()==0,"");
  CHECK_INVARIANT(a->getExplicitValence()==1,"");
  CHECK_INVARIANT(a->getNoImplicit()==1,"");
  CHECK_INVARIANT(a->getFormalCharge()==-1,"");
    

  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testProblems(){
  int i = 0;
  Mol *mol;
  std::string smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing smiles that were previously problems" << std::endl;

  // ring closure handling with branches/fragments
  VECT_INT_VECT rings;
  smi = "C1(CC1CC1CC1)";
  mol = SmilesToMol(smi);
  CHECK_INVARIANT(mol,smi);
  int ringCount=MolOps::findSSSR(*mol,rings);
  CHECK_INVARIANT(ringCount==2,"");
  CHECK_INVARIANT(rings.size()==2,"");
  CHECK_INVARIANT(rings[0].size()==3,"");
  CHECK_INVARIANT(rings[1].size()==3,"");
  
  // this is truly pathological, but both daylight
  //   and chemdraw parse it properly
  smi = "C1.C1CC1CC1";
  delete mol;
  mol = SmilesToMol(smi);
  CHECK_INVARIANT(mol,smi);
  ringCount=MolOps::findSSSR(*mol,rings);
  CHECK_INVARIANT(ringCount==1,"");
  CHECK_INVARIANT(rings.size()==1,"");
  CHECK_INVARIANT(rings[0].size()==3,"");
  
  // here's another stupid case that we need to handle:
  delete mol;
  smi = "C1CC11CC1";
  mol = SmilesToMol(smi);
  CHECK_INVARIANT(mol,smi);
  ringCount=MolOps::findSSSR(*mol,rings);
  CHECK_INVARIANT(ringCount==2,"");
  CHECK_INVARIANT(rings.size()==2,"");
  CHECK_INVARIANT(rings[0].size()==3,"");
  CHECK_INVARIANT(rings[1].size()==3,"");
  
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testBasicCanon(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing basic SMILES canonicalization" << std::endl;
#if 1
  smi = "C1OCCCC1";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol);

  delete mol;
  smi = "C1COCCC1";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);

  delete mol;
  smi = "O1CCCCC1";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);


  delete mol;
  smi = "OC=CC";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol);
  delete mol;
  smi = "CC=CO";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  smi = "C(C)=CO";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  smi = "C(O)=CC";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);


  // --- These are related to Issue 109
  delete mol;
  smi = "C([H])Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol->getNumAtoms()==2);
  refSmi = MolToSmiles(*mol);
  delete mol;
  smi = "CCl";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);
  delete mol;
#endif
  // -- Issue 131
  smi = "P#[Ga]";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol->getNumAtoms()==2);
  refSmi = MolToSmiles(*mol);
  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "O=[Ba]";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol->getNumAtoms()==2);
  refSmi = MolToSmiles(*mol);
  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(refSmi==smi);
  

  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}
void testLeak(){
  int i = 0;
  Mol *mol;
  std::string smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing a leak" << std::endl;

  smi = "C1CC1";
  for(i=0;i<1000000;i++){
    mol = SmilesToMol(smi,0,1);
    if(!(i%1000)) BOOST_LOG(rdInfoLog) << i << std::endl;
    delete mol;
  }
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;

}


void testStereochem(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing handling of stereochemical smiles" << std::endl;

  smi = "F[C@](Cl)(Br)I";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  smi = "F[C@](Br)(I)Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "F[C@](I)(Cl)Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Cl[C@](Br)(F)I";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Cl[C@](F)(I)Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "I[C@](F)(Br)Cl";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "I[C@](Br)(Cl)F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "F[C@@](Br)(Cl)I";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "F[C@@](Cl)(I)Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Cl[C@@](Br)(I)F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Cl[C@@](F)(Br)I";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "F[C@H](Cl)Br";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  smi = "Br[C@H](F)Cl";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Br[C@]([H])(F)Cl";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  delete mol;
  smi = "Br[C@](F)(Cl)[H]";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(smi==refSmi);

  // -----------------------------------
  // test some double-bond containing molecules:

  //-- cis --
  delete mol;
  smi = "F\\C=C/Br";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "Br\\C=C/F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "Br/C=C\\F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F/C=C\\Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  

  //-- trans --
  delete mol;
  smi = "F\\C=C\\Br";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "Br\\C=C\\F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "Br/C=C/F";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F/C=C/Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  //-- more complex --
  delete mol;
  smi = "F\\C=C(/Cl)\\Br";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F/C=C(\\Cl)/Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F/C=C(\\Cl)Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F/C=C(Cl)/Br";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  


  //-- combine chirality with cis/trans --
  delete mol;
  smi = "F[C@H](Cl)\\C=C(/F)";
  mol = SmilesToMol(smi);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  mol = SmilesToMol(refSmi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);
  
  delete mol;
  smi = "F[C@H](Cl)/C=C(\\F)";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);

  smi = "Cl[C@@H](F)/C=C(\\F)";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);

  smi = "Cl[C@@H](F)\\C=C(/F)";
  mol = SmilesToMol(smi);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);


  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;

}


void testIssue127(){
  int i = 0;
  Mol *mol,*mol2;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 127 (chiral smiles with fused rings)" << std::endl;

  smi = "Cl[C@]12[Si]C(C2)O1";
  mol = SmilesToMol(smi);
  //mol->debugMol(std::cout);
  TEST_ASSERT(mol);

#if 1
  // first roundtrip the non-chiral SMILES:
  refSmi = MolToSmiles(*mol);
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  tempStr=MolToSmiles(*mol2);
  TEST_ASSERT(refSmi==tempStr);
  delete mol2;
#endif

  // now do the true SMILES:
  refSmi = MolToSmiles(*mol,1);
  mol2 = SmilesToMol(refSmi);
  //mol2->debugMol(std::cout);
  TEST_ASSERT(mol2);
  tempStr=MolToSmiles(*mol2,1);
  //std::cout << refSmi << " : " << tempStr << std::endl;
  TEST_ASSERT(refSmi==tempStr);
  delete mol2;
  
  

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void testIssue143(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 143 (removing chiral tags for non-chiral centers)" << std::endl;

  smi = "C[C@](C)(C)C";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="CC(C)(C)C");
  delete mol;

  smi = "CC[C@](C)(C)C=O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="CCC(C=O)(C)C");
  delete mol;

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue151(){
  int i = 0;
  Mol *mol,*mol2;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 151 (Chiral centers in rings with hydrogen on them not handled correctly)" << std::endl;

  smi = "C1S[C@H]1O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CW);

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="O[C@H]1CS1");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  smi = "F[C@@H]1O[C@H](Cl)S1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(4)->getChiralTag()==Atom::CHI_UNSPECIFIED);

  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="F[C@@H]1O[C@H](Cl)S1");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  
  smi = "Cl[C@@H]1S[C@@H](O1)F";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(4)->getChiralTag()==Atom::CHI_UNSPECIFIED);

  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CW);

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="F[C@@H]1O[C@H](Cl)S1");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  smi = "Cl[C@@H]1O[C@H](F)S1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(4)->getChiralTag()==Atom::CHI_UNSPECIFIED);

  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="F[C@H]1O[C@@H](Cl)S1");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue153(){
  int i = 0;
  std::string code;
  Mol *mol,*mol2;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 153 (Incorrect order of ring-closure bonds from SMILES)" << std::endl;

  smi = "C1(O[C@H]12)S2";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  MolOps::assignAtomChiralCodes(*mol);
  TEST_ASSERT(mol->getAtomWithIdx(2)->hasProp("_CIPCode"));
  mol->getAtomWithIdx(2)->getProp("_CIPCode",code);
  TEST_ASSERT(code=="S");
  

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="O1C2S[C@H]12");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  smi = "C1(O[C@H]21)S2";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()!=Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CW);
  MolOps::assignAtomChiralCodes(*mol);
  TEST_ASSERT(mol->getAtomWithIdx(2)->hasProp("_CIPCode"));
  mol->getAtomWithIdx(2)->getProp("_CIPCode",code);
  TEST_ASSERT(code=="R");

  refSmi = MolToSmiles(*mol,true);
  TEST_ASSERT(refSmi=="O1C2S[C@@H]12");
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void testIssue157(){
  int i = 0;
  std::string code;
  Mol *mol,*mol2;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 157 (Symmetric molecules with multiple chiral centers badly canonicalized)" << std::endl;

  smi = "O[C@](C)(Cl)[C@@](O)(Cl)C";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==Atom::CHI_UNSPECIFIED);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CCW);
  TEST_ASSERT(mol->getAtomWithIdx(4)->getChiralTag()==Atom::CHI_TETRAHEDRAL_CW);

  MolOps::assignAtomChiralCodes(*mol);
  TEST_ASSERT(mol->getAtomWithIdx(1)->hasProp("_CIPCode"));
  mol->getAtomWithIdx(1)->getProp("_CIPCode",code);
  TEST_ASSERT(code=="R");
  TEST_ASSERT(mol->getAtomWithIdx(1)->hasProp("_CIPCode"));
  mol->getAtomWithIdx(4)->getProp("_CIPCode",code);
  TEST_ASSERT(code=="S");
  
  refSmi = MolToSmiles(*mol,true);
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  smi = "Cl[C@@](C)1CC[C@@](C)(C1)Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  refSmi = MolToSmiles(*mol,true);
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  smi = "[H][C@@]12C[C@@](C=C1)(C3C2C(NC3=O)=O)[H]";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  refSmi = MolToSmiles(*mol,true);
  mol2 = SmilesToMol(refSmi);
  TEST_ASSERT(mol2);
  smi = MolToSmiles(*mol2,true);
  TEST_ASSERT(refSmi==smi);
  delete mol;
  delete mol2;

  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void testIssue159(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 159 (cis/trans wrong in some branched systems)" << std::endl;

  smi = "C/C=C/O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);
  refSmi = MolToSmiles(*mol,1);

  delete mol;
  smi = "C(\\C)=C/O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);

  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);


  delete mol;
  smi = "C(=C/O)\\C";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);

  
  delete mol;
  smi = "C(\\C/C=C/Cl)=C/O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(4)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);

  
  delete mol;
  smi = "O=C\\C=C/F";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(0)->getBondType()==Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOZ);

  delete mol;
  smi = "C(/C=O)=C/F";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(0)->getBondType()==Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOZ);

  delete mol;
  smi = "C(=C/F)/C=O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(3)->getStereo() == Bond::STEREONONE);

  delete mol;
  smi = "C(=O)\\C=C/Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);

  delete mol;
  smi = "CC(=O)\\C=C/Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(3)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREONONE);

  delete mol;
  smi = "C(=O)\\N=C\\Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREONONE);

  delete mol;
  smi = "CC(=O)\\N=C\\Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(3)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREONONE);

  delete mol;
  smi = "C(/Br)(=C/Cl)Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOZ);

  delete mol;
  smi = "C(=C/Cl)(/Br)Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOZ);

  delete mol;
  smi = "Cl\\C=C(\\Br)";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);

  delete mol;
  smi = "Cl\\C(=C\\Br)";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);

  // ---------
  // These next few molecules test propagation of bond flips:
  // ---------
  delete mol;
  smi = "Cl/C=C(/C=C/C)\\C=C\\Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(6)->getStereo() == Bond::STEREOE);


  delete mol;
  smi = "C(/C=C/C)(\\C=C\\Br)=C\\Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(0)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(3)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(6)->getStereo() == Bond::STEREOZ);

  delete mol;
  smi = "Br/C=C/C(/C=C/C)=C\\Cl";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(3)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(6)->getStereo() == Bond::STEREOZ);
  
  delete mol;
  smi = "Cl/C=C(/C=C/C=C\\F)\\C=C\\Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(4)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(8)->getStereo() == Bond::STEREOE);
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}



void testIssue175(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 175 (cis/trans wrong on ring closures)" << std::endl;

  smi = "Cl\\C=C1.F/1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);

  delete mol;
  smi = "Cl\\C=C1CN/1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(1)->getStereo() == Bond::STEREOE);

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue176(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi,tempStr;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 176 (problems with 'mol BOND ring_number')" << std::endl;

  smi = "C1CC1C1CC1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumBonds()==7);

  delete mol;
  smi = "C1CC1C1CC-1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumBonds()==7);

  delete mol;
  smi = "C1CC1C1CC=1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumBonds()==7);

  delete mol;
  smi = "C1CC1C=1CC1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumBonds()==7);

  delete mol;

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue180(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 180: Z/E problems" << std::endl;

  smi = "Cl/C(=N\\O)/C(=N\\O)Br";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(5)->getStereo() == Bond::STEREOE);
  refSmi = MolToSmiles(*mol,1);
  
  delete mol;
  smi="Cl/C(/C(Br)=N\\O)=N\\O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(5)->getStereo() == Bond::STEREOZ);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue184(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 184: Cis/Trans incorrect on ring-closure bonds" << std::endl;

  smi = "C1NC(Cl)C(=N\\O)/C1=N\\O";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  //mol->debugMol(std::cout);
  TEST_ASSERT(mol->getBondWithIdx(5)->getBondType() == Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(5)->getStereo() == Bond::STEREOZ);
  TEST_ASSERT(mol->getBondWithIdx(7)->getBondType() == Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(7)->getStereo() == Bond::STEREOZ);
  refSmi = MolToSmiles(*mol,1);
  delete mol;
  mol = SmilesToMol(refSmi);
  TEST_ASSERT(mol);
  smi = MolToSmiles(*mol,1);
  TEST_ASSERT(refSmi==smi);

  for(RWMol::BondIterator bondIt=mol->beginBonds();
      bondIt!=mol->endBonds();
      bondIt++){
    if((*bondIt)->getBondType()==Bond::DOUBLE){
      TEST_ASSERT((*bondIt)->getStereo()==Bond::STEREOZ);
    }
  }
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue185(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 185: Cis/Trans incorrect on writing branches" << std::endl;

  smi ="CC(=N\\O)/C=P/N";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(2)->getBondType() == Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(2)->getStereo() == Bond::STEREOE);
  TEST_ASSERT(mol->getBondWithIdx(4)->getBondType() == Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(4)->getStereo() == Bond::STEREOE);
  refSmi = MolToSmiles(*mol,1);
  delete mol;
  mol = SmilesToMol(refSmi);
  TEST_ASSERT(mol);

  for(RWMol::BondIterator bondIt=mol->beginBonds();
      bondIt!=mol->endBonds();
      bondIt++){
    if((*bondIt)->getBondType()==Bond::DOUBLE){
      TEST_ASSERT((*bondIt)->getStereo()==Bond::STEREOE);
    }
  }
  smi = MolToSmiles(*mol,1);
  //std::cout << "ref: " << refSmi << " -> " << smi << std::endl;
  TEST_ASSERT(refSmi==smi);
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue191(){
  int i = 0;
  Mol *mol;
  std::string smi,refSmi;
  int numE=0;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 191: Bad bond directions in a branch" << std::endl;

  smi ="C2=NNC(N=C2)=N\\N=C\\c1ccccc1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getBondWithIdx(7)->getBondType() == Bond::DOUBLE);
  TEST_ASSERT(mol->getBondWithIdx(7)->getStereo() == Bond::STEREOE);
  refSmi = MolToSmiles(*mol,1);
  delete mol;
  mol = SmilesToMol(refSmi);
  TEST_ASSERT(mol);
  //std::cout << "ref: " << refSmi << std::endl;
  //mol->debugMol(std::cout);
  numE = 0;
  for(RWMol::BondIterator bondIt=mol->beginBonds();
      bondIt!=mol->endBonds();
      bondIt++){
    if((*bondIt)->getBondType()==Bond::DOUBLE){
      TEST_ASSERT((*bondIt)->getStereo()!=Bond::STEREOZ);
      if((*bondIt)->getStereo()==Bond::STEREOE){
	numE++;
      }
    }
  }
  TEST_ASSERT(numE==1);
  smi = MolToSmiles(*mol,1);
  //std::cout << "ref: " << refSmi << " -> " << smi << std::endl;
  TEST_ASSERT(refSmi==smi);
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testIssue256(){
  int i = 0;
  Mol *mol;
  Bond *bond;
  std::string smi;
  int numE=0;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 256: SMILES yields incorrect structure" << std::endl;

  smi ="C1CC[C+]1=1CCC1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  bond=mol->getBondBetweenAtoms(3,0);
  TEST_ASSERT(bond)
  TEST_ASSERT(bond->getBondType()==Bond::SINGLE);
  bond=mol->getBondBetweenAtoms(3,6);
  TEST_ASSERT(bond)
  TEST_ASSERT(bond->getBondType()==Bond::DOUBLE);
  delete mol;

  smi ="C1CC[C+]=11CCC1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  bond=mol->getBondBetweenAtoms(3,0);
  TEST_ASSERT(bond)
  TEST_ASSERT(bond->getBondType()==Bond::DOUBLE);
  bond=mol->getBondBetweenAtoms(3,6);
  TEST_ASSERT(bond)
  TEST_ASSERT(bond->getBondType()==Bond::SINGLE);
  delete mol;
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void testIssue266(){
  int i = 0;
  RWMol *mol;
  std::string smi;
  int numE=0;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing Issue 266: kekulized SMILES output" << std::endl;

  smi ="c1ccccc1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(smi=="c1ccccc1");

  MolOps::Kekulize(*mol);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(smi=="C1=CC=CC=C1");
  delete mol;

  smi ="c1ccccc1c1ccccc1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(smi=="c1ccc(-c2ccccc2)cc1");

  MolOps::Kekulize(*mol);
  smi = MolToSmiles(*mol);
  TEST_ASSERT(smi=="C1=CC=C(C2=CC=CC=C2)C=C1");
  delete mol;
  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testRootedAt(){
  RWMol *mol;
  std::string smi;
  int numE=0;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing rootedAtAtom functionality" << std::endl;

  smi ="CN(C)C";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  smi = MolToSmiles(*mol,false,false,-1);
  TEST_ASSERT(smi=="CN(C)C");
  smi = MolToSmiles(*mol,false,false,1);
  TEST_ASSERT(smi=="N(C)(C)C");
  smi = MolToSmiles(*mol,false,false,2);
  TEST_ASSERT(smi=="CN(C)C");

  
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void testBug1670149(){
  RWMol *mol;
  std::string smi;
  int numE=0;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Testing SF.net bug 1670149" << std::endl;

  smi ="C1[NH2+]CCC1";
  mol = SmilesToMol(smi);
  TEST_ASSERT(mol);
  smi = MolToSmiles(*mol,false,false,-1);
  TEST_ASSERT(smi=="C1CC[NH2+]C1");

  mol->getAtomWithIdx(1)->setNumExplicitHs(0);
  mol->getAtomWithIdx(1)->setNoImplicit(false);
  mol->getAtomWithIdx(1)->updatePropertyCache();
  TEST_ASSERT(mol->getAtomWithIdx(1)->getNumImplicitHs()==2);
  smi = MolToSmiles(*mol,false,false,-1);
  TEST_ASSERT(smi=="C1CC[NH2+]C1");


  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}
 
int
main(int argc, char *argv[])
{
  RDLog::InitLogs();
#if 1
  testPass();
  testFail();
  testDetails();
  testProblems();
  //testLeak();
  testBasicCanon();
  testStereochem();
  testIssue127();
  testIssue143();
  testIssue151();
  testIssue153();
  testIssue157();
  testIssue175();
  testIssue176();
  testIssue180();
  testIssue159();
  testIssue185();
  testIssue184();
  testIssue191();
  testIssue256();
  testIssue266();
#endif
  testRootedAt();
  testBug1670149();
}
