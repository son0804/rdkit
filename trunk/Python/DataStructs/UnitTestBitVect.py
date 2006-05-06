# $Id: UnitTestBitVect.py 5009 2006-02-22 15:19:49Z glandrum $
#
# Copyright (C) 2001-2006 greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
""" unit testing code for the python BitVects
"""

import unittest
from DataStructs import BitVect

class TestCase(unittest.TestCase):
  def testVectIdx(self):
    """ test indexing into BitVects
    """
    v = BitVect.BitVect(10)
    ok = 1
    try:
      v[0] = 1
      v[2] = 1
    except:
      ok = 0
    assert ok, 'setting bits failed'

    try:
      v[10] = 1
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'setting high bit should have failed'
    
    try:
      v[-1] = 1
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'setting negative bit should have failed'

    assert v[0] == 1, 'bad bit'
    assert v[1] == 0, 'bad bit'
    assert v[2] == 1, 'bad bit'

    try:
      foo = v[10]
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'getting high bit should have failed'
    
    try:
      foo = v[-1]
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'getting negative bit should have failed'

  def testSparseIdx(self):
    """ test indexing into SparseBitVects
    """
    v = BitVect.SparseBitVect(10)
    ok = 1
    try:
      v[0] = 1
      v[2] = 1
    except:
      ok = 0
    assert ok, 'setting bits failed'

    try:
      v[10] = 1
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'setting high bit should have failed'
    
    try:
      v[-1] = 1
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'setting negative bit should have failed'

    assert v[0] == 1, 'bad bit'
    assert v[1] == 0, 'bad bit'
    assert v[2] == 1, 'bad bit'

    try:
      foo = v[10]
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'getting high bit should have failed'
    
    try:
      foo = v[-1]
    except:
      ok = 1
    else:
      ok = 0
    assert ok, 'getting negative bit should have failed'

  def testVectBitGet(self):
    """ test operations to get bits
    """
    v = BitVect.BitVect(10)
    v[0] = 1
    v[2] = 1
    v[6] = 1
    assert len(v)==10,'len(BitVect) failed'
    assert v.NumOnBits()==3,'NumOnBits failed'
    assert v.GetOnBits()==[0,2,6], 'GetOnBits failed'
    assert v.GetOnBits(reverse=1)==[6,2,0], 'GetOnBits(reverse) failed'
    
  def testSparseBitGet(self):
    """ test operations to get sparse bits
    """
    v = BitVect.BitVect(10)
    v[0] = 1
    v[2] = 1
    v[6] = 1
    assert len(v)==10,'len(SparseBitVect) failed'
    assert v.NumOnBits()==3,'NumOnBits failed'
    assert v.GetOnBits()==[0,2,6], 'GetOnBits failed'
    assert v.GetOnBits(reverse=1)==[6,2,0], 'GetOnBits(reverse) failed'
    
  def testVectBitOps(self):
    """ test bit operations on BitVects
    """
    v1 = BitVect.BitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.BitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1&v2 == [0,6],'binary & failed'
    assert v1|v2 == [0,2,3,6],'binary | failed'
    assert v1^v2 == [2,3],'binary ^ failed'

  def testCrossBitOps(self):
    """ test bit operations between BitVects and SparseBitVects
    """
    v1 = BitVect.SparseBitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.BitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1&v2 == [0,6],'binary & failed'
    assert v1|v2 == [0,2,3,6],'binary | failed'
    assert v1^v2 == [2,3],'binary ^ failed'
    
  def testSparseBitOps(self):
    """ test bit operations on SparseBitVects
    """
    v1 = BitVect.SparseBitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.SparseBitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1&v2 == [0,6],'binary & failed'
    assert v1|v2 == [0,2,3,6],'binary | failed'
    assert v1^v2 == [2,3],'binary ^ failed'

  def testVectTanimoto(self):
    v1 = BitVect.BitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.BitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1.TanimotoSimilarity(v2)==0.5,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v1)==0.5,'TanimotoSimilarity failed'
    assert v1.TanimotoSimilarity(v1)==1.0,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v2)==1.0,'TanimotoSimilarity failed'
    
  def testSparseTanimoto(self):
    v1 = BitVect.SparseBitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.SparseBitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1.TanimotoSimilarity(v2)==0.5,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v1)==0.5,'TanimotoSimilarity failed'
    assert v1.TanimotoSimilarity(v1)==1.0,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v2)==1.0,'TanimotoSimilarity failed'
    
  def testCrossTanimoto(self):
    v1 = BitVect.SparseBitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.BitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1
    assert v1.TanimotoSimilarity(v2)==0.5,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v1)==0.5,'TanimotoSimilarity failed'
    assert v1.TanimotoSimilarity(v1)==1.0,'TanimotoSimilarity failed'
    assert v2.TanimotoSimilarity(v2)==1.0,'TanimotoSimilarity failed'
    
  def testVectEuclid(self):
    v1 = BitVect.BitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.BitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1

    assert abs(v1.EuclideanDistance(v2)-.14142)<.0001, 'EuclideanDistance failed'
    assert abs(v2.EuclideanDistance(v1)-.14142)<.0001, 'EuclideanDistance failed'
    assert v1.EuclideanDistance(v1)==0.0, 'EuclideanDistance failed'
    assert v2.EuclideanDistance(v2)==0.0, 'EuclideanDistance failed'

  def testSparseEuclid(self):
    v1 = BitVect.SparseBitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.SparseBitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1

    assert abs(v1.EuclideanDistance(v2)-.14142)<.0001, 'EuclideanDistance failed'
    assert abs(v2.EuclideanDistance(v1)-.14142)<.0001, 'EuclideanDistance failed'
    assert v1.EuclideanDistance(v1)==0.0, 'EuclideanDistance failed'
    assert v2.EuclideanDistance(v2)==0.0, 'EuclideanDistance failed'

  def testCrossEuclid(self):
    v1 = BitVect.BitVect(10)
    v1[0] = 1
    v1[2] = 1
    v1[6] = 1
    v2 = BitVect.SparseBitVect(10)
    v2[0] = 1
    v2[3] = 1
    v2[6] = 1

    assert abs(v1.EuclideanDistance(v2)-.14142)<.0001, 'EuclideanDistance failed'
    assert abs(v2.EuclideanDistance(v1)-.14142)<.0001, 'EuclideanDistance failed'
    assert v1.EuclideanDistance(v1)==0.0, 'EuclideanDistance failed'
    assert v2.EuclideanDistance(v2)==0.0, 'EuclideanDistance failed'


if __name__ == '__main__':
  unittest.main()
