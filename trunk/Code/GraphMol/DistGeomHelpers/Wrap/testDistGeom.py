import Chem
from Chem import rdDistGeom,ChemicalForceFields
import RDConfig
import unittest
import os
import cPickle as pickle
from Numeric import *
import math

def feq(v1, v2, tol=1.e-4) :
    return abs(v1-v2) < tol

def lstEq(l1, l2, tol=1.0e-4) :
    ln = len(l1)
    if (ln != len(l2) ) :
        return 0

    for i in range(ln) :
        if (l1[i] - l2[i]) > tol :
            return 0
    return 1

def compareWithOld(smilesFile, sdFile) :
    smiSup = Chem.SmilesMolSupplier(smilesFile, ",", 0, -1)
    sdsup = Chem.SDMolSupplier(sdFile)
    im = 0
    for mol in smiSup :
        cid = rdDistGeom.EmbedMolecule(mol, 10,1)
        omol = sdsup[im]
        assert cid == 0
        conf = mol.GetConformer(0)
        oconf = omol.GetConformer()
        nat = mol.GetNumAtoms()
        for i in range(nat) :
            #atm = mol.GetAtomWithIdx(i)
            #oatm = omol.GetAtomWithIdx(i)
            pos = conf.GetAtomPosition(i)
            opos = oconf.GetAtomPosition(i)
            if not lstEq(pos, opos):
                return 0
        im += 1
    return 1

def compareMatrices(bm1, bm2, map, tol=1.0e-5) :
    N = shape(bm1)[0]
    for i in range(1,N):
        for j in range(i):
            l, m = map[i], map[j]
            if (l < m) :
                l, m = m,l
            if (abs(bm1[l,m] - bm2[i,j]) > tol):
                return 0
                
            if (abs(bm1[m,l] - bm2[j,i]) > tol):
                return 0
                
    return 1

def compareOrder(smi1, smi2, tol=1.0e-5) :
    m1 = Chem.MolFromSmiles(smi1)
    m2 = Chem.MolFromSmiles(smi2)
    bm1 = rdDistGeom.GetMoleculeBoundsMatrix(m1)
    bm2 = rdDistGeom.GetMoleculeBoundsMatrix(m2)
    map = m1.GetSubstructMatch(m2)
    return compareMatrices(bm1, bm2,map, tol)

def computeDist(lst1, lst2):
    res = 0.0
    for i, val in enumerate(lst1):
        res += (val - lst2[i])*(val - lst2[i])
    res = math.sqrt(res)
    return res

class TestCase(unittest.TestCase) :
    def setUp(self):
        pass

    def _test0Cdk2(self):
        fileN = os.path.join(RDConfig.RDBaseDir,'Code','GraphMol','DistGeomHelpers',
                                            'test_data','cis_trans_cases.csv')
        
        ofile = os.path.join(RDConfig.RDBaseDir,'Code','GraphMol','DistGeomHelpers',
                                            'test_data','embedDistOpti.sdf')
        self.failUnless(compareWithOld(fileN, ofile))

    def test1Small(self):
        #writer = Chem.SDWriter("test.sdf")
        # single double and tripple atoms cases should not fail
        mol = Chem.MolFromSmiles('O')
        rdDistGeom.EmbedMolecule(mol,10,1)
        conf = mol.GetConformer()
        self.failUnless(lstEq(conf.GetAtomPosition(0), [0.0, 0.0, 0.0]))
        #writer.write(mol)
        
        mol = Chem.MolFromSmiles('CO')
        rdDistGeom.EmbedMolecule(mol, 10,1)
        conf = mol.GetConformer()
        self.failUnless(lstEq(conf.GetAtomPosition(0), [0.71308, 0.0, 0.0]))
        self.failUnless(lstEq(conf.GetAtomPosition(1), [-0.71308, 0.0, 0.0]))
        #writer.write(mol)

        mol = Chem.MolFromSmiles('CCC')
        rdDistGeom.EmbedMolecule(mol,10,1)
        conf = mol.GetConformer()
        self.failUnless(lstEq(conf.GetAtomPosition(0), [-1.21676, -0.2989, 0.0]))
        self.failUnless(lstEq(conf.GetAtomPosition(1), [-0.00604, 0.59337, 0.0]))
        self.failUnless(lstEq(conf.GetAtomPosition(2), [1.22281, -0.29446, 0.0]))
        #writer.write(mol)
        
        mol = Chem.MolFromSmiles('O=C=O')
        rdDistGeom.EmbedMolecule(mol,10,1)
        conf = mol.GetConformer()
        
        #writer.write(mol)
        self.failUnless(lstEq(conf.GetAtomPosition(0), [-1.2591, -0.06189, 0.0]))
        self.failUnless(lstEq(conf.GetAtomPosition(1), [-0.00408, 0.12317, 0.0]))
        self.failUnless(lstEq(conf.GetAtomPosition(2), [1.26318, -0.061288, 0.0]))
        
        mol = Chem.MolFromSmiles('C=C=C=C')
        rdDistGeom.EmbedMolecule(mol,10,1)
        conf = mol.GetConformer()
        
        #writer.write(mol)

        d1 = computeDist(conf.GetAtomPosition(0), conf.GetAtomPosition(1))
        self.failUnless(feq(d1, 1.31, 0.01))
        d2 = computeDist(conf.GetAtomPosition(0), conf.GetAtomPosition(2))
        self.failUnless(feq(d2, 2.59, 0.05))
        d3 = computeDist(conf.GetAtomPosition(0), conf.GetAtomPosition(3))
        self.failUnless(feq(d3, 3.84, 0.1))
        d4 = computeDist(conf.GetAtomPosition(1), conf.GetAtomPosition(2))
        self.failUnless(feq(d4, 1.29, 0.01))
        d5 = computeDist(conf.GetAtomPosition(1), conf.GetAtomPosition(3))
        self.failUnless(feq(d5, 2.54, 0.05))
        d6 = computeDist(conf.GetAtomPosition(2), conf.GetAtomPosition(3))
        self.failUnless(feq(d6, 1.31, 0.01))

    def test2Utils(self):
        mol = Chem.MolFromSmiles('CC')
        bm = rdDistGeom.GetMoleculeBoundsMatrix(mol)
        self.failUnless(bm[1,0]>0)
        self.failUnless(bm[0,1]>0)
        self.failUnless(bm[0,1]>=bm[1,0])
        self.failUnless(bm[1,0]<1.510)
        self.failUnless(bm[0,1]>1.510)

    def test3MultiConf(self):
        mol = Chem.MolFromSmiles("CC(C)(C)c(cc12)n[n]2C(=O)/C=C(N1)/COC")
        cids = rdDistGeom.EmbedMultipleConfs(mol,10,30,100)
        energies = [90.05,77.35,91.45,81.82,81.60,75.65,
		    86.50,80.55,73.73,70.57]
        nenergies = []
        for cid in cids:
            ff = ChemicalForceFields.UFFGetMoleculeForceField(mol, 10.0, cid)
            ee = ff.CalcEnergy()
            nenergies.append(ee)
	#print ','.join(['%.2f'%x for x in nenergies])
        self.failUnless(lstEq(energies, nenergies,tol=1e-2))
            
    def test4OrderDepndence(self) :
        self.failUnless(compareOrder("CC(C)(C)C(=O)NC(C1)CC(N2C)CCC12",
                                     "CN1C2CCC1CC(NC(=O)C(C)(C)C)C2"))
        #issue 230
        self.failUnless(compareOrder("C#CC(C)(C)N(CN1)C\N=C/1SC",
                                     "CSC1=NCN(C(C)(C)C#C)CN1"))
        #issue 232
        self.failUnless(compareOrder("CC(C)(C)C(=O)NC(C1)CC(N2C)CCC12",
                                     "CN1C2CCC1CC(NC(=O)C(C)(C)C)C2"))
        
    def test5Issue285(self):
        m = Chem.MolFromSmiles('CNC=O')
        cs = rdDistGeom.EmbedMultipleConfs(m,10)
        for i,ci in enumerate(cs):
            for j in range(i+1,len(cs)):
                cj = cs[j]
                self.failUnless(Chem.MolToMolBlock(m,confId=ci)!=Chem.MolToMolBlock(m,confId=cj))

    def test6RmsPruning(self):
        smiles = ['CC(C)CC(NC(C1[N+]CCC1)=O)C([O-])=O',
                  'CC(NC(CO)C(O)c1ccc([N+]([O-])=O)cc1)=O',
                  'CC([N+])C(NC(C)C(N1C(C=O)CCC1)=O)=O',
                  'CC(NC1C(O)C=C(C([O-])=O)OC1C(O)C(O)CO)=O',
                  'CCCC=C(NC(C1CC1(C)C)=O)C([O-])=O',
                  'OCC(O)C(O)C(Cn1c2c(cc(C)c(C)c2)nc-2c(=O)[nH]c(=O)nc12)O']


        nconfs = []
        expected = [5, 8, 8, 5, 5, 4]
        for smi in smiles:
            mol = Chem.MolFromSmiles(smi)
            cids = rdDistGeom.EmbedMultipleConfs(mol, 50, 30,
                                                 100, pruneRmsThresh=1.5)
            nconfs.append(len(cids))
        self.failUnless(nconfs == expected)
        
        
if __name__ == '__main__':
  unittest.main()
