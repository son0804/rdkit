import RDConfig
import os,sys,cPickle
import unittest
import DataStructs as ds

class TestCase(unittest.TestCase):
  def setUp(self) :
    pass

  def test1Discrete(self):
    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.ONEBITVALUE, 30)
    for i  in range(15):
      v1[2*i] = 1;

    self.failUnless(len(v1) == 30)
    self.failUnless(v1.GetTotalVal() == 15)

    for i in range(len(v1)):
      self.failUnless(v1[i] == (i+1)%2)

    self.failUnlessRaises(ValueError, lambda : v1.__setitem__(5, 2))   

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 30)
    for i in range(len(v1)):
      v1[i] = i%4;

    self.failUnless(len(v1) == 30)
    for i in range(len(v1)):
      self.failUnless(v1[i] == i%4)

    self.failUnlessRaises(ValueError, lambda : v1.__setitem__(10, 6))

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.FOURBITVALUE, 30)
    for i in range(len(v1)):
      v1[i] = i%16;

    self.failUnless(len(v1) == 30)
    self.failUnless(v1.GetTotalVal() == 211)
    for i in range(len(v1)):
      self.failUnless(v1[i] == i%16)

    self.failUnlessRaises(ValueError, lambda : v1.__setitem__(10, 16))

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.EIGHTBITVALUE, 32)
    for i in range(len(v1)):
      v1[i] = i%256;

    self.failUnless(len(v1) == 32)
    self.failUnless(v1.GetTotalVal() == 496)
    for i in range(len(v1)):
      self.failUnless(v1[i] == i%256)

    self.failUnlessRaises(ValueError, lambda : v1.__setitem__(10, 256))

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.SIXTEENBITVALUE, 300)
    for i in range(len(v1)):
      v1[i] = i%300;

    self.failUnless(len(v1) == 300)
    self.failUnless(v1.GetTotalVal() == 44850)
    self.failUnlessRaises(ValueError, lambda : v1.__setitem__(10, 65536))
  def test2VectDistances(self):
    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.ONEBITVALUE, 30)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.ONEBITVALUE, 30)
    for i in range(15):
      v1[2*i] = 1
      v2[2*i] = 1
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)
    for i in range(30):
      if (i%3 == 0):
        v2[i] = 1
      else:
        v2[i] = 0
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 15)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 30)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 30)

    for i in range(30):
      v1[i] = i%4
      v2[i] = (i+1)%4

    self.failUnless(ds.ComputeL1Norm(v1, v2) == 44)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.FOURBITVALUE, 16)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.FOURBITVALUE, 16)
    for i in range(16):
      v1[i] = i%16
      v2[i] = i%5
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 90)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.EIGHTBITVALUE, 5)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.EIGHTBITVALUE, 5)
    v1[0] = 34
    v1[1] = 167
    v1[2] = 3
    v1[3] = 56
    v1[4] = 128

    v2[0] = 14
    v2[1] = 67
    v2[2] = 103
    v2[3] = 6
    v2[4] = 228
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 370)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.SIXTEENBITVALUE, 3)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.SIXTEENBITVALUE, 3)
    v1[0] = 2345
    v1[1] = 64578
    v1[2] = 34

    v2[0] = 1345
    v2[1] = 54578
    v2[2] = 10034
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 21000)

  def test3Pickles(self):
    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.ONEBITVALUE, 30)
    for i in range(15):
      v1[2*i] = 1
    v2 = cPickle.loads(cPickle.dumps(v1))
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 30)
    for i in range(30):
      v1[i] = i%4
    v2 = cPickle.loads(cPickle.dumps(v1))
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.FOURBITVALUE, 16)
    for i in range(16):
      v1[i] = i%16
    v2 = cPickle.loads(cPickle.dumps(v1))
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)

    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.EIGHTBITVALUE, 5)
    v1[0] = 34
    v1[1] = 167
    v1[2] = 3
    v1[3] = 56
    v1[4] = 128
    v2 = cPickle.loads(cPickle.dumps(v1))
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)


    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.SIXTEENBITVALUE, 3)
    v1[0] = 2345
    v1[1] = 64578
    v1[2] = 34
    v2 = cPickle.loads(cPickle.dumps(v1))
    self.failUnless(ds.ComputeL1Norm(v1, v2) == 0)

  def test4DiscreteVectOps(self):
    v1 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 8)
    for i in range(4):
      v1[2*i] = 2
    self.failUnless(v1.GetTotalVal()==8)
    v2 = ds.DiscreteValueVect(ds.DiscreteValueType.TWOBITVALUE, 8)
    for i in range(4):
      v2[2*i+1] = 2
      v2[2*i] = 1
    self.failUnless(v2.GetTotalVal()==12)

    v3 = v1|v2
    self.failUnless(len(v3)==len(v2))
    self.failUnless(v3.GetTotalVal()==16)

    v3 = v1&v2
    self.failUnless(len(v3)==len(v2))
    self.failUnless(v3.GetTotalVal()==4)

    v4 = v1+v2
    self.failUnless(len(v4)==len(v2))
    self.failUnless(v4.GetTotalVal()==20)

    v4 = v1-v2
    self.failUnless(v4.GetTotalVal()==4)
    v4 = v2-v1
    self.failUnless(v4.GetTotalVal()==8)

    v4 = v2
    v4 -= v1
    self.failUnless(v4.GetTotalVal()==8)
    v4 -= v4
    self.failUnless(v4.GetTotalVal()==0)
    

if __name__ == '__main__':
    unittest.main()
