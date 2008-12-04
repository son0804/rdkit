// $Id: test.cpp 1043 2008-07-25 12:00:41Z landrgr1 $
//
//  Copyright (c) 2008, Novartis Institutes for BioMedical Research Inc.
//  All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met: 
//
//     * Redistributions of source code must retain the above copyright 
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following 
//       disclaimer in the documentation and/or other materials provided 
//       with the distribution.
//     * Neither the name of Novartis Institutes for BioMedical Research Inc. 
//       nor the names of its contributors may be used to endorse or promote 
//       products derived from this software without specific prior
//       written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  Created by Greg Landrum September, 2006
//
#include <iostream>
#include <GraphMol/RDKitBase.h>
#include <GraphMol/SLNParse/SLNParse.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Substruct/SubstructMatch.h>

#include <RDGeneral/RDLog.h>
using namespace std;

void test1(){
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test1 " << std::endl;

  sln = "CH4";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  

  delete mol;
  sln = "CH3CH3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==2);
  TEST_ASSERT(mol->getRingInfo()->numRings()==0);

  delete mol;
  sln = "C[1]H2CH2CH2@1";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==3);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
    
  delete mol;
  sln = "C[1:foo]H2CH2CH2@1";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==3);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);

  delete mol;
  sln = "C[foo;bar=1;baz=bletch]H4";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getRingInfo()->numRings()==0);

  
  delete mol;
  sln = "CH3CH(CH3)OCH3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==5);
  TEST_ASSERT(mol->getRingInfo()->numRings()==0);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getDegree()==1);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getDegree()==3);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getDegree()==1);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getDegree()==2);

  
  delete mol;
  sln = "CH3CH(CH3)OCH3.Cl";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==0);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getDegree()==1);
  TEST_ASSERT(mol->getAtomWithIdx(1)->getDegree()==3);
  TEST_ASSERT(mol->getAtomWithIdx(2)->getDegree()==1);
  TEST_ASSERT(mol->getAtomWithIdx(3)->getDegree()==2);
  TEST_ASSERT(mol->getAtomWithIdx(4)->getDegree()==1);
  TEST_ASSERT(mol->getAtomWithIdx(5)->getDegree()==0);
  
  
  delete mol;
  sln = "HC(F)(Cl)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test2(){
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test2: bond orders " << std::endl;

  sln = "CH3CH3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==2);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::SINGLE);

  delete mol;
  sln = "CH2-CH2";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==2);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::SINGLE);

  delete mol;
  sln = "CH2=CH2";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==2);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::DOUBLE);

  delete mol;
  sln = "CH#CH";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==2);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::TRIPLE);
  delete mol;

  sln = "C[1]H-CH2-CH2-CH2-CH2-CH=@1";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::SINGLE);
  TEST_ASSERT(mol->getBondBetweenAtoms(1,2)->getBondType()==RDKit::Bond::SINGLE);
  TEST_ASSERT(mol->getBondBetweenAtoms(2,3)->getBondType()==RDKit::Bond::SINGLE);
  TEST_ASSERT(mol->getBondBetweenAtoms(3,4)->getBondType()==RDKit::Bond::SINGLE);
  TEST_ASSERT(mol->getBondBetweenAtoms(4,5)->getBondType()==RDKit::Bond::SINGLE);
  TEST_ASSERT(mol->getBondBetweenAtoms(5,0)->getBondType()==RDKit::Bond::DOUBLE);

  delete mol;
  sln = "C[1]H:CH:CH:CH:CH:CH:@1";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
  TEST_ASSERT(mol->getBondBetweenAtoms(0,1)->getBondType()==RDKit::Bond::AROMATIC);
  TEST_ASSERT(mol->getBondBetweenAtoms(1,2)->getBondType()==RDKit::Bond::AROMATIC);
  TEST_ASSERT(mol->getBondBetweenAtoms(2,3)->getBondType()==RDKit::Bond::AROMATIC);
  TEST_ASSERT(mol->getBondBetweenAtoms(3,4)->getBondType()==RDKit::Bond::AROMATIC);
  TEST_ASSERT(mol->getBondBetweenAtoms(4,5)->getBondType()==RDKit::Bond::AROMATIC);
  TEST_ASSERT(mol->getBondBetweenAtoms(5,0)->getBondType()==RDKit::Bond::AROMATIC);
  
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test3(){
  std::string pval;
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test3: atom properties " << std::endl;

  sln = "C[-]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-1);

  delete mol;
  sln = "C[charge=-]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-1);
  
  delete mol;
  sln = "C[charge=-1]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-1);
  
  delete mol;
  sln = "C[CHARGE=-1]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-1);
  
  delete mol;
  sln = "C[chARgE=-1]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-1);
  

  delete mol;
  sln = "C[1:-2]H2";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==-2);
  
  delete mol;
  sln = "C[1:+]H5";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==1);
  
  delete mol;
  sln = "C[1:+2]H6";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getFormalCharge()==2);
  
  delete mol;
  sln = "C[1:foo;bar=baz]H4";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->hasProp("foo"));
  mol->getAtomWithIdx(0)->getProp("foo",pval);
  TEST_ASSERT(pval=="");
  TEST_ASSERT(mol->getAtomWithIdx(0)->hasProp("bar"));
  mol->getAtomWithIdx(0)->getProp("bar",pval);
  TEST_ASSERT(pval=="baz");
  TEST_ASSERT(!mol->getAtomWithIdx(0)->hasProp("baz"));
  

  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test4(){
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test4: hydrogen handling " << std::endl;

  sln = "CH4";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNumExplicitHs()==4);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNumImplicitHs()==0);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNoImplicit());

  delete mol;
  sln = "C[-]H3";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==1);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNumExplicitHs()==3);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNumImplicitHs()==0);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getNoImplicit());


  
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test5(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test5: basic queries " << std::endl;

  sln = "C[charge=+1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(patt->getAtomWithIdx(0)->getNumExplicitHs()==0);
  TEST_ASSERT(patt->getAtomWithIdx(0)->getNoImplicit());
  TEST_ASSERT(patt->getAtomWithIdx(0)->getNumImplicitHs()==0);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));
            
  delete mol;
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);

  delete patt;
  sln = "AnyAny";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "Any-Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "Any=Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);

  delete patt;
  sln = "Any#Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);

  delete patt;
  sln = "Any:Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);

  delete patt;
  sln = "Any~Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);

  delete patt;
  sln = "C[charge=+1|charge=+2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C[CH2+](C)[CH+2]";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "C[charge=+1;HC=2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C[CH2+](CC)[NH+]";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV[0][0].second==1);

  delete patt;
  sln = "Any[charge=+1;!HC=2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C[CH2+](CC)[NH+]";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV[0][0].second==4);

  delete patt;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test6(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test6: ring queries " << std::endl;

  sln = "C[rbc=2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(!patt->getAtomWithIdx(0)->hasProp("rbc"));

  smi = "C1CC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);

  delete mol;
  smi = "C1C2C1C2";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete mol;
  smi = "CCC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));

  delete patt;
  sln = "C[rbc=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  
  delete mol;
  smi = "C1CC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));

  delete mol;
  smi = "CCC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));

  delete patt;
  sln = "C[rbc=3]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  
  delete mol;
  smi = "C1CC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));

  delete mol;
  smi = "C1C2C1C2";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  // test case sensitivity, just because we should:
  delete patt;
  sln = "C[rBC=2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(!patt->getAtomWithIdx(0)->hasProp("rbc"));

  delete mol;
  smi = "C1CC1C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);


  delete patt;
  sln = "C[r]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);

  delete mol;
  smi = "C1CC1C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);

  delete patt;
  sln = "C[1:rbc=f]CCC@1";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==4);
  
  delete mol;
  smi = "C1CCC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);

  delete mol;
  smi = "C1CC2C1CC2";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "C[1:rbc=f]C[rbc=f]CC@1";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==4);
  
  delete mol;
  smi = "C1CCC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV));

  delete mol;
  smi = "C1CC2C1CC2";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "C[1:rbc=f]C[rbc=f]C[rbc=f]C@1";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==4);
  
  delete mol;
  smi = "C1CCC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);

  delete mol;
  smi = "C1CC2C1CC2";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(!RDKit::SubstructMatch(*mol,*patt,mV));

 
  delete patt;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void test7(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test7: qualified queries " << std::endl;

  sln = "C[charge=+1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  
  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
            
  delete patt;
  sln = "C[charge<1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);

  delete mol;
  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);
            
  delete patt;
  sln = "C[charge<=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);
            
  delete patt;
  sln = "C[charge>=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  
  delete patt;
  sln = "C[charge>1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  
  delete patt;
  sln = "C[charge!=0]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  
  delete patt;
  sln = "C[charge!=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);

  smi = "C[CH2+](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);
            
  delete mol;
  smi = "C[CH](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
            
  delete mol;
  smi = "C[CH3+2](C)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
  
  delete patt;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


void test8(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test8: more complex atom properties " << std::endl;

  sln = "Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==4);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[2][0].second==2);
  TEST_ASSERT(mV[3][0].second==3);

  delete patt;
  sln = "CO[F]C";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==3);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==2);
  

  delete patt;
  sln = "Any[TBO=4]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  // CHECK: should TBO really match both Cs here or only atom 3 (i.e. should it be degree or valence)? 
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1][0].second==3);

  delete patt;
  sln = "Any[TAC=2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0][0].second==2);

  delete patt;
  sln = "Any[TAC>2]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1][0].second==3);
  
  delete patt;
  sln = "Any[HC=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0][0].second==0);
  
  delete patt;
  sln = "Any[HC=0]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==1);
  TEST_ASSERT(mV[1][0].second==2);
  
  delete patt;
  sln = "Any[HC=F]H3";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0][0].second==3);
    
  delete patt;
  sln = "Any[HAC>1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1][0].second==2);
  
  delete patt;
  sln = "Any[HAC=1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==1);
  TEST_ASSERT(mV[1][0].second==3);
  
  delete patt;
  sln = "Any[HAC=F]~Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0][0].second==1);
  TEST_ASSERT(mV[1][0].second==3);
  
  delete patt;
  sln = "AnyAny[TAC=F]Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==3);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0][1].second==2);


  delete mol;
  smi = "CCC=C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);

  delete patt;
  sln = "CC";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);

  delete patt;
  sln = "CC[F]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);

  delete patt;
  sln = "CC[F]H3";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);

  delete patt;
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test9(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test9: recursive SLNs " << std::endl;

  sln = "Any[is=Cl,Br,I]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);

  smi = "C(=O)Cl";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==2);

  delete mol;
  smi = "C(=O)Br";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==2);

  delete mol;
  smi = "C(=O)I";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==2);

  delete mol;
  smi = "C(=O)C";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);

  delete patt;
  sln = "C[is=C=O]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  
  delete mol;
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==0);


  delete patt;
  sln = "Any[is=C,O;charge=-1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==0);
  delete mol;
  smi = "C(=O)[O-]";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==2);

  delete patt;
  // make sure we aren't case sensitive:
  sln = "Any[Is=C,O;charge=-1]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "[C-](=O)O";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==0);

  delete patt;
  // check handling of 'not':
  sln = "Any[not=N,O]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C(=O)OCN";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV.size()==2);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1].size()==1);
  TEST_ASSERT(mV[1][0].second==3);

  delete patt;
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test10(){
  std::string pval;
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test10: nested recursive SLNs " << std::endl;

  // do recursions in the 'is':
  sln = "Any[is=C[is=Any=O],O]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==3);
  TEST_ASSERT(mV.size()==3);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[1].size()==1);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[2].size()==1);
  TEST_ASSERT(mV[2][0].second==2);
    
  // do recursions in the 'not':
  delete patt;
  sln = "Any[not=C[is=Any=O],O]";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==1);
  delete mol;
  smi = "C(=O)OC";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==1);
  TEST_ASSERT(mV[0][0].second==3);

  delete patt;
  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test11(){
  std::string pval,cip;
  RDKit::RWMol *mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test11: chiral SLNs " << std::endl;

  sln = "CH(Cl)(F)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  TEST_ASSERT(mol->getAtomWithIdx(0)->getChiralTag()==RDKit::Atom::CHI_UNSPECIFIED);

  delete mol;
  sln = "C[s=N]H(Cl)(F)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  smi = MolToSmiles(*mol,true);
  //TEST_ASSERT(smi=="F[C@@H](Cl)Br");

  delete mol;
  sln = "ClC[s=i]H(F)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  //mol->debugMol(std::cerr);
#if 0
  RDKit::MolOps::assignAtomChiralCodes(*mol);
  TEST_ASSERT(mol->getAtomWithIdx(1)->hasProp("_CIPCode"));
  mol->getAtomWithIdx(1)->getProp("_CIPCode",cip);
  TEST_ASSERT(cip=="R");
#endif
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  smi = MolToSmiles(*mol,true);
  //TEST_ASSERT(smi=="F[C@@H](Cl)Br");

  delete mol;
  sln = "FC[s=N]H(Cl)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  smi = MolToSmiles(*mol,true);
  //TEST_ASSERT(smi=="F[C@@H](Cl)Br");

  delete mol;
  sln = "FC[s=N]H(Br)Cl";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  smi = MolToSmiles(*mol,true);
  //TEST_ASSERT(smi=="F[C@H](Cl)Br");


  delete mol;
  sln = "HC[s=N](Cl)(F)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  smi = MolToSmiles(*mol,true);
  //TEST_ASSERT(smi=="F[C@@H](Cl)Br");



  delete mol;
  sln = "C[s=i]H(Cl)(F)Br";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==4);
  smi = MolToSmiles(*mol,true);
  std::cerr << sln << " -> " << MolToSmiles(*mol,true) << " " << mol->getNumAtoms() << std::endl;
  //TEST_ASSERT(smi=="F[C@H](Cl)Br");




  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test12(){
  RDKit::RWMol *patt,*mol;
  std::vector<RDKit::MatchVectType> mV;
  std::string sln,smi;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test12: bond queries and properties " << std::endl;

  sln = "Any-Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  
  smi = "C=CN";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV.size()==1);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==1);
  TEST_ASSERT(mV[0][1].second==2);

  delete patt;
  sln = "Any~Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==1);
  TEST_ASSERT(mV[1].size()==2);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[1][1].second==2);

  delete patt;
  sln = "Any-=Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==1);
  TEST_ASSERT(mV[1].size()==2);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[1][1].second==2);

  delete patt;
  sln = "Any=-Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==1);
  TEST_ASSERT(mV[1].size()==2);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[1][1].second==2);

  delete patt;
  sln = "Any-:Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==1);
  TEST_ASSERT(mV[0][1].second==2);

  sln = "Any-[type=2]Any";
  delete patt;
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==1);
  delete patt;

  sln = "Any-[type=2|type=1]Any";
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  TEST_ASSERT(mV[0][1].second==1);
  TEST_ASSERT(mV[1].size()==2);
  TEST_ASSERT(mV[1][0].second==1);
  TEST_ASSERT(mV[1][1].second==2);

  sln = "O~[r]C";
  delete patt;
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);

  delete mol;
  smi = "O=CC1COC1";
  mol=RDKit::SmilesToMol(smi);
  TEST_ASSERT(mol);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==2);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==4);
  TEST_ASSERT(mV[1][0].second==4);

  sln = "O~[!r]C";
  delete patt;
  patt=RDKit::SLNQueryToMol(sln);
  TEST_ASSERT(patt);
  TEST_ASSERT(patt->getNumAtoms()==2);
  TEST_ASSERT(RDKit::SubstructMatch(*mol,*patt,mV)==1);
  TEST_ASSERT(mV[0].size()==2);
  TEST_ASSERT(mV[0][0].second==0);
  
  delete mol;
  delete patt;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test13(){
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test13: ring closure details " << std::endl;

  sln = "C[1]H2CH2CH2CH2CH2CH2@1";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
  TEST_ASSERT(mol->getRingInfo()->atomRings()[0].size()==6);

  delete mol;
  sln = "C[1]H2CH2(CH2CH2CH2CH2@1)";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
  TEST_ASSERT(mol->getRingInfo()->atomRings()[0].size()==6);

  delete mol;
  sln = "CH2(C[1]H2)CH2(CH2CH2CH2@1)";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==6);
  TEST_ASSERT(mol->getRingInfo()->numRings()==1);
  TEST_ASSERT(mol->getRingInfo()->atomRings()[0].size()==6);

  delete mol;
  sln = "C[1]H2CH2CH2CH2C[2]HCH@1CH2CH2@2";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(mol);
  TEST_ASSERT(mol->getNumAtoms()==8);
  TEST_ASSERT(mol->getRingInfo()->numRings()==2);
  TEST_ASSERT(mol->getRingInfo()->atomRings()[0].size()==6);
  TEST_ASSERT(mol->getRingInfo()->atomRings()[1].size()==4);

  delete mol;
  sln = "CH2(CH2@1)CH2(CH2CH2C[1]H2)";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(!mol);

  delete mol;
  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}

void test14(){
  RDKit::RWMol *mol;
  std::string sln;

  BOOST_LOG(rdInfoLog) << "-------------------------------------" << std::endl;
  BOOST_LOG(rdInfoLog) << "Test14: error catching " << std::endl;

  sln = "CH2(C@1H2)CH2(CH2CH2C[1]H2)";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(!mol);

  sln = "CH2(CH2[1])CH2(CH2CH2CH2@1)";
  mol=RDKit::SLNToMol(sln);
  TEST_ASSERT(!mol);

  BOOST_LOG(rdInfoLog) << "\tdone" << std::endl;
}


int
main(int argc, char *argv[])
{
  RDLog::InitLogs();

  // FIX: need a test for handling Hs in the SLN itself. This should be done for
  // both normal and query SLNs and must be done after the SLN parser handles
  // that case (errr, duh)
#if 1  
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  //test11();
  test12();
#endif
  test13();
  test14();
}
