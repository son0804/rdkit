// $Id$
//
//  Copyright (C) 2004-2008 Greg Landrum and Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//

#include <iostream>
#include <RDGeneral/Invariant.h>
#include <RDGeneral/RDLog.h>
#include <RDGeneral/utils.h>

#include <GraphMol/RDKitBase.h>
#include <GraphMol/MolPickler.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/FileParsers/FileParsers.h>
#include <GraphMol/FileParsers/MolSupplier.h>

#include <GraphMol/Descriptors/MolDescriptors.h>
#include <GraphMol/Descriptors/Crippen.h>
#include <GraphMol/Descriptors/AtomPairs.h>

#include <DataStructs/BitVects.h>
#include <DataStructs/BitOps.h>
using namespace RDKit;
using namespace RDKit::Descriptors;

void test1(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Crippen parameter acquisition." << std::endl;

  CrippenParamCollection *params=CrippenParamCollection::getParams();
  TEST_ASSERT(params);
  
  CrippenParams p=*(params->begin());
  TEST_ASSERT(p.label=="C1");
  TEST_ASSERT(p.smarts=="[CH4]");

  
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void test2(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Crippen calculation." << std::endl;

  ROMol *mol;
  double logp,mr;

  mol = SmilesToMol("C");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,0.6361));
  TEST_ASSERT(feq(mr,6.7310));
  // check that caching works:
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,0.6361));
  TEST_ASSERT(feq(mr,6.7310));
  CalcCrippenDescriptors(*mol,logp,mr,true,true);
  TEST_ASSERT(feq(logp,0.6361));
  TEST_ASSERT(feq(mr,6.7310));

  // check that things work when we don't add Hs:
  CalcCrippenDescriptors(*mol,logp,mr,false,true);
  TEST_ASSERT(feq(logp,0.1441));
  TEST_ASSERT(feq(mr,2.503));
  delete mol;

  mol = SmilesToMol("C=C");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr,true);
  TEST_ASSERT(feq(logp,0.8022));
  TEST_ASSERT(feq(mr,11.2540));
  delete mol;

  mol = SmilesToMol("C#C");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,0.2494));
  TEST_ASSERT(feq(mr,9.8900));
  delete mol;

  mol = SmilesToMol("CO");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,-0.3915));
  TEST_ASSERT(feq(mr,8.1428));
  delete mol;

  mol = SmilesToMol("C=O");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,-0.1849));
  TEST_ASSERT(feq(mr,7.121));
  delete mol;

  mol = SmilesToMol("C#[O+]");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,0.0059));
  TEST_ASSERT(feq(mr,5.6315));
  delete mol;

  mol = SmilesToMol("C(C)(C)C");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,1.6623));
  TEST_ASSERT(feq(mr,20.512));
  delete mol;

  mol = SmilesToMol("C(C)(C)(C)O");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,logp,mr);
  TEST_ASSERT(feq(logp,0.7772));
  TEST_ASSERT(feq(mr,21.9718));
  delete mol;



  
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testIssue262(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Issue262: problems with Crippen calculation from pickles." << std::endl;

  ROMol *mol,*mol2;
  RWMol *mol3;
  std::string pkl;
  double rlogp,rmr,logp,mr;

  mol = SmilesToMol("c1ncccc1");
  TEST_ASSERT(mol);
  CalcCrippenDescriptors(*mol,rlogp,rmr);
  
  MolPickler::pickleMol(*mol,pkl);

  mol2=new ROMol(pkl);
  TEST_ASSERT(mol2);
  CalcCrippenDescriptors(*mol2,logp,mr);
  TEST_ASSERT(feq(logp,rlogp));
  TEST_ASSERT(feq(mr,rmr));

  mol3=new RWMol();
  TEST_ASSERT(mol3);
  MolPickler::molFromPickle(pkl,mol3);
  
  CalcCrippenDescriptors(*mol3,logp,mr);
  TEST_ASSERT(feq(logp,rlogp));
  TEST_ASSERT(feq(mr,rmr));
  
  delete mol;
  delete mol2;
  delete mol3;

  
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void test3(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test AMW calculation." << std::endl;

  ROMol *mol,*mol2;
  double amw;

  mol = SmilesToMol("C");
  TEST_ASSERT(mol);
  amw = CalcAMW(*mol);
  TEST_ASSERT(feq(amw,16.043,.001));
  amw = CalcAMW(*mol,true);
  TEST_ASSERT(feq(amw,12.011,.001));
  mol2 = MolOps::addHs(*mol);
  amw = CalcAMW(*mol2);
  TEST_ASSERT(feq(amw,16.043,.001));
  amw = CalcAMW(*mol2,true);
  TEST_ASSERT(feq(amw,12.011,.001));
  delete mol;
  delete mol2;

  mol = SmilesToMol("[CH4]");
  TEST_ASSERT(mol);
  amw = CalcAMW(*mol);
  TEST_ASSERT(feq(amw,16.043,.001));
  amw = CalcAMW(*mol,true);
  TEST_ASSERT(feq(amw,12.011,.001));
  delete mol;


  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}


void testAtomCodes(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Atom Codes." << std::endl;

  ROMol *mol;
  boost::uint32_t tgt;
  mol = SmilesToMol("C=C");
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(0))==AtomPairs::getAtomCode(mol->getAtomWithIdx(1)));
  tgt = 1 | (1 | 1<<AtomPairs::numPiBits)<<AtomPairs::numBranchBits;
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(0))==tgt);
  tgt = 1<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(0),1)==tgt);

  delete mol;
  mol = SmilesToMol("C#CO");
  tgt = 1 | 2<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(0))==tgt);
  tgt = 2 | 2<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(1))==tgt);
  tgt = 1 | 0<<AtomPairs::numBranchBits | 3<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(2))==tgt);

  delete mol;
  mol = SmilesToMol("CC(O)C(O)(O)C");
  tgt = 1 | 0<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(1),2)==tgt);
  tgt = 2 | 0<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(3),2)==tgt);
  
  delete mol;
  mol = SmilesToMol("C=CC(=O)O");
  tgt = 0 | 0<<AtomPairs::numBranchBits | 3<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(4),1)==tgt);
  tgt = 3 | 1<<AtomPairs::numBranchBits | 1<<(AtomPairs::numBranchBits+AtomPairs::numPiBits);
  TEST_ASSERT(AtomPairs::getAtomCode(mol->getAtomWithIdx(2))==tgt);


  delete mol;
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testAtomPairs(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Atom Pairs." << std::endl;

  ROMol *mol;
  SparseIntVect<boost::int32_t> *fp;
  boost::uint32_t tgt;
  boost::uint32_t c1,c2,c3;

  mol = SmilesToMol("CCCCC");
  c1=AtomPairs::getAtomCode(mol->getAtomWithIdx(0));
  c2=AtomPairs::getAtomCode(mol->getAtomWithIdx(1));
  c3=AtomPairs::getAtomCode(mol->getAtomWithIdx(2));
  tgt = 1 | (std::min(c1,c2) | std::max(c1,c2)<<AtomPairs::codeSize)<< AtomPairs::numPathBits;
  TEST_ASSERT(AtomPairs::getAtomPairCode(c1,c2,1)==tgt);
  TEST_ASSERT(AtomPairs::getAtomPairCode(c2,c1,1)==tgt);
  tgt = 2 | (std::min(c1,c3) | std::max(c1,c3)<<AtomPairs::codeSize)<< AtomPairs::numPathBits;
  TEST_ASSERT(AtomPairs::getAtomPairCode(c1,c3,2)==tgt);
  TEST_ASSERT(AtomPairs::getAtomPairCode(c3,c1,2)==tgt);

  delete mol;
  mol = SmilesToMol("CCC");
  fp=AtomPairs::getAtomPairFingerprint(*mol);
  TEST_ASSERT(fp->getTotalVal()==3);
  TEST_ASSERT(fp->getNonzeroElements().size()==2);

  c1=AtomPairs::getAtomCode(mol->getAtomWithIdx(0));
  c2=AtomPairs::getAtomCode(mol->getAtomWithIdx(1));
  c3=AtomPairs::getAtomCode(mol->getAtomWithIdx(2));
  TEST_ASSERT(fp->getVal(AtomPairs::getAtomPairCode(c1,c2,1))==2);
  TEST_ASSERT(fp->getVal(AtomPairs::getAtomPairCode(c1,c3,2))==1);
  
  delete mol;
  delete fp;
  mol = SmilesToMol("CC=O.Cl");
  fp=AtomPairs::getAtomPairFingerprint(*mol);
  TEST_ASSERT(fp->getTotalVal()==3);
  TEST_ASSERT(fp->getNonzeroElements().size()==3);

  c1=AtomPairs::getAtomCode(mol->getAtomWithIdx(0));
  c2=AtomPairs::getAtomCode(mol->getAtomWithIdx(1));
  c3=AtomPairs::getAtomCode(mol->getAtomWithIdx(2));
  TEST_ASSERT(fp->getVal(AtomPairs::getAtomPairCode(c1,c2,1))==1);
  TEST_ASSERT(fp->getVal(AtomPairs::getAtomPairCode(c1,c2,1))==1);
  TEST_ASSERT(fp->getVal(AtomPairs::getAtomPairCode(c2,c3,1))==1);
  
  delete mol;
  delete fp;
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testAtomPairs2(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Atom Pairs part 2." << std::endl;

  {
    ROMol *mol;
    SparseIntVect<boost::int32_t> *fp;

    mol = SmilesToMol("CCC");
    fp=AtomPairs::getAtomPairFingerprint(*mol,1,2);
    TEST_ASSERT(fp->getTotalVal()==3);
    TEST_ASSERT(fp->getNonzeroElements().size()==2);
  
    delete fp;
    fp=AtomPairs::getAtomPairFingerprint(*mol,2,2);
    TEST_ASSERT(fp->getTotalVal()==1);
    TEST_ASSERT(fp->getNonzeroElements().size()==1);
  }

  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testHashedAtomPairs(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Hashed Atom Pairs." << std::endl;

  {
    ROMol *mol;
    mol = SmilesToMol("c1ccccc1");
    SparseIntVect<boost::int32_t> *fp1;
    fp1=AtomPairs::getHashedAtomPairFingerprint(*mol);
    SparseIntVect<boost::int32_t> *fp2;
    fp2=AtomPairs::getHashedAtomPairFingerprint(*mol);
    TEST_ASSERT(DiceSimilarity(*fp1,*fp2)==1.0);
    TEST_ASSERT(*fp1==*fp2);

    delete mol;
    delete fp2;
    mol = SmilesToMol("c1ccccn1");
    fp2=AtomPairs::getHashedAtomPairFingerprint(*mol);
    RANGE_CHECK(0.0,DiceSimilarity(*fp1,*fp2),1.0);

    delete mol;
    delete fp1;
    delete fp2;
  }

  {
    ROMol *mol;
    mol = SmilesToMol("c1ccccc1");
    SparseIntVect<boost::int32_t> *fp1;
    fp1=AtomPairs::getHashedAtomPairFingerprint(*mol,2048);
    SparseIntVect<boost::int32_t> *fp2;
    fp2=AtomPairs::getHashedAtomPairFingerprint(*mol,2048,1,3);
    TEST_ASSERT(DiceSimilarity(*fp1,*fp2)==1.0);
    TEST_ASSERT(*fp1==*fp2);

    delete fp2;
    fp2=AtomPairs::getHashedAtomPairFingerprint(*mol,2048,1,2);
    RANGE_CHECK(0.0,DiceSimilarity(*fp1,*fp2),1.0);

    delete mol;
    delete fp1;
    delete fp2;
  }

  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}


void testTorsions(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Topological Torsions." << std::endl;

  ROMol *mol;
  SparseIntVect<boost::int64_t> *fp;
  boost::uint64_t tgt;
  boost::uint64_t  c1,c2,c3,c4;
  std::vector<boost::uint32_t> codes;

  mol = SmilesToMol("CCCC");
  c1=AtomPairs::getAtomCode(mol->getAtomWithIdx(0))-1;
  c2=AtomPairs::getAtomCode(mol->getAtomWithIdx(1))-2;
  c3=AtomPairs::getAtomCode(mol->getAtomWithIdx(2))-2;
  c4=AtomPairs::getAtomCode(mol->getAtomWithIdx(3))-1;
  tgt = c1 | (c2 | (c3 | c4<<AtomPairs::codeSize)<<AtomPairs::codeSize)<<AtomPairs::codeSize;
  codes.clear();
  codes.push_back(static_cast<unsigned int>(c1));
  codes.push_back(static_cast<unsigned int>(c2));
  codes.push_back(static_cast<unsigned int>(c3));
  codes.push_back(static_cast<unsigned int>(c4));
  TEST_ASSERT(AtomPairs::getTopologicalTorsionCode(codes)==tgt);

  fp = AtomPairs::getTopologicalTorsionFingerprint(*mol);
  TEST_ASSERT(fp->getTotalVal()==1);
  TEST_ASSERT(fp->getNonzeroElements().size()==1);


  delete mol;
  delete fp;
  mol = SmilesToMol("CCCCO.Cl");
  fp=AtomPairs::getTopologicalTorsionFingerprint(*mol);
  TEST_ASSERT(fp->getTotalVal()==2);
  TEST_ASSERT(fp->getNonzeroElements().size()==2);

  delete fp;
  fp = AtomPairs::getTopologicalTorsionFingerprint(*mol,3);
  TEST_ASSERT(fp->getTotalVal()==3);
  TEST_ASSERT(fp->getNonzeroElements().size()==3);

  delete mol;
  delete fp;
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testHashedTorsions(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Hashed torsions." << std::endl;

  {
    ROMol *mol;
    mol = SmilesToMol("c1ccccc1");
    SparseIntVect<boost::int64_t> *fp1;
    fp1=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol);
    SparseIntVect<boost::int64_t> *fp2;
    fp2=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol);
    TEST_ASSERT(DiceSimilarity(*fp1,*fp2)==1.0);
    TEST_ASSERT(*fp1==*fp2);

    delete mol;
    delete fp2;
    mol = SmilesToMol("c1ccccn1");
    fp2=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol);
    RANGE_CHECK(0.0,DiceSimilarity(*fp1,*fp2),1.0);

    delete mol;
    delete fp1;
    delete fp2;
  }

  {
    ROMol *mol;
    mol = SmilesToMol("c1ccccc1");
    SparseIntVect<boost::int64_t> *fp1;
    fp1=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol,2048,6);
    SparseIntVect<boost::int64_t> *fp2;
    fp2=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol,2048,6);
    TEST_ASSERT(DiceSimilarity(*fp1,*fp2)==1.0);
    TEST_ASSERT(*fp1==*fp2);

    delete mol;
    delete fp2;
    mol = SmilesToMol("c1ccccn1");
    fp2=AtomPairs::getHashedTopologicalTorsionFingerprint(*mol,2048,6);
    RANGE_CHECK(0.0,DiceSimilarity(*fp1,*fp2),1.0);

    delete mol;
    delete fp1;
    delete fp2;
  }


  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}




void testBulkTorsions(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Bulk Topological Torsions." << std::endl;

  std::string fName= getenv("RDBASE");
  fName += "/Projects/DbCLI/testData/pubchem.200.sdf";
  SDMolSupplier suppl(fName);
  while(!suppl.atEnd()){
    ROMol *mol=suppl.next();
    SparseIntVect<boost::int64_t> *fp;
    fp = AtomPairs::getTopologicalTorsionFingerprint(*mol);
    TEST_ASSERT(fp->getTotalVal()>1);
    delete mol;
    delete fp;
  }
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}


void testLabute(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Labute ASA descriptors." << std::endl;
  ROMol *mol;
  double asa;

  mol = SmilesToMol("CO");
  asa=calcLabuteASA(*mol);
  TEST_ASSERT(feq(asa,13.5335,.0001));
  asa=calcLabuteASA(*mol);
  TEST_ASSERT(feq(asa,13.5335,.0001));
  asa=calcLabuteASA(*mol,true,true);
  TEST_ASSERT(feq(asa,13.5335,.0001));

  delete mol;
  mol = SmilesToMol("OC(=O)c1ccncc1C(=O)O");
  asa=calcLabuteASA(*mol);
  TEST_ASSERT(feq(asa,67.2924,.0001));
  
  delete mol;
  mol = SmilesToMol("C1CCC(c2cccnc2)NC1");
  asa=calcLabuteASA(*mol);
  TEST_ASSERT(feq(asa,73.0198,.0001));
  
  delete mol;
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}

void testRootedAtomPairs(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Rooted Atom Pairs." << std::endl;

  ROMol *mol;
  SparseIntVect<boost::int32_t> *fp1,*fp2;
  std::vector<boost::uint32_t> roots;

  mol = SmilesToMol("OCCCCC");
  fp1=AtomPairs::getAtomPairFingerprint(*mol);
  SparseIntVect<boost::int32_t>::StorageType nz1=fp1->getNonzeroElements();
  TEST_ASSERT(nz1.size()>0);

  roots.push_back(0);
  fp2=AtomPairs::getAtomPairFingerprint(*mol,&roots);
  SparseIntVect<boost::int32_t>::StorageType nz2=fp2->getNonzeroElements();
  TEST_ASSERT(nz2.size()>0);
  TEST_ASSERT(nz2.size()<nz1.size());

  for(SparseIntVect<boost::int32_t>::StorageType::const_iterator bIt=nz2.begin();
      bIt!=nz2.end();++bIt){
    TEST_ASSERT(bIt->second<=fp2->getVal(bIt->first));
  }

  delete mol;
  delete fp1;
  delete fp2;

  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}


void testRootedTorsions(){
  BOOST_LOG(rdErrorLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdErrorLog) << "    Test Rooted Topological Torsions." << std::endl;

  ROMol *mol;
  SparseIntVect<boost::int64_t> *fp1,*fp2;
  std::vector<boost::uint32_t> roots;


  mol = SmilesToMol("OCCCC");
  roots.push_back(0);

  fp1 = AtomPairs::getTopologicalTorsionFingerprint(*mol);
  SparseIntVect<boost::int64_t>::StorageType nz1=fp1->getNonzeroElements();
  TEST_ASSERT(nz1.size()>0);

  fp2 = AtomPairs::getTopologicalTorsionFingerprint(*mol,4,&roots);
  SparseIntVect<boost::int64_t>::StorageType nz2=fp2->getNonzeroElements();
  TEST_ASSERT(nz2.size()>0);
  TEST_ASSERT(nz2.size()<nz1.size());

  for(SparseIntVect<boost::int64_t>::StorageType::const_iterator bIt=nz2.begin();
      bIt!=nz2.end();++bIt){
    TEST_ASSERT(bIt->second<=fp2->getVal(bIt->first));
  }

  delete mol;
  delete fp1;
  delete fp2;
  BOOST_LOG(rdErrorLog) << "  done" << std::endl;
}


//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
int main(){
  RDLog::InitLogs();
#if 1
  test1();
  test2();
  testIssue262();
  test3();
  testAtomCodes();
  testAtomPairs();
  testAtomPairs2();
  testTorsions();
  testBulkTorsions();
  testLabute();
#endif
  testHashedAtomPairs();
  testHashedTorsions();
  testRootedAtomPairs();
  testRootedTorsions();
}
