# $Id$
#
#  Copyright (C) 2006  greg Landrum 
#
#   @@ All Rights Reserved  @@
#
import RDConfig
import unittest,sys,os,math
import Chem
from Chem.FeatMaps import FeatMaps,FeatMapParser
from Geometry import Point3D

def feq(n1,n2,tol=1e-5):
  return abs(n1-n2)<=tol

class TestCase(unittest.TestCase):
  def setUp(self):
    pass
  
  def test1Basics(self):
    txt="""

ScoreMode=Best
DirScoreMode=DotFullRange

BeginParams
  family=Aromatic radius=2.5 width=1.0 profile=Triangle
  family=Acceptor radius=1.5
EndParams

# optional
BeginPoints
  family=Acceptor pos=(1.0, 0.0, 5.0) weight=1.25 dir=(1, 1, 0)
  family=Aromatic pos=(0.0,1.0,0.0) weight=2.0 dir=(0,0,1) dir=(0,0,-1)
  family=Acceptor pos=(1.0,1.0,2.0) weight=1.25
EndPoints

"""
    p = FeatMapParser.FeatMapParser()
    p.SetData(txt)
    fm = p.Parse()
    self.failUnless(fm.scoreMode==FeatMaps.FeatMapScoreMode.Best)
    self.failUnless(fm.dirScoreMode==FeatMaps.FeatDirScoreMode.DotFullRange)
    self.failUnless(fm.GetNumFeatures()==3)


    feats = fm.GetFeatures()
    self.failUnless(feq(feats[0].weight,1.25))
    self.failUnless(feq(feats[1].weight,2.0))
    self.failUnless(feq(feats[2].weight,1.25))
    
    self.failUnless(len(feats[0].featDirs)==1)
    self.failUnless(len(feats[1].featDirs)==2)
    self.failUnless(len(feats[2].featDirs)==0)
    
    fams = [x.GetFamily() for x in feats]
    self.failUnless(fams==['Acceptor','Aromatic','Acceptor'])
    
if __name__ == '__main__':
  unittest.main()

