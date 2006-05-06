# $Id: UnitTestScreenComposite.py 5030 2006-03-02 18:46:51Z glandrum $
#
#  Copyright (C) 2003-2006  greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
"""unit testing code for the ScreenComposite functionality

"""
import RDConfig
import unittest,os
from ML import BuildComposite
from ML import ScreenComposite
import cPickle as pickle

def feq(a,b,tol=1e-4):
  if abs(a-b)>tol: return 0
  else: return 1
  
class TestCase(unittest.TestCase):
  def setUp(self):
    #print '\n%s: '%self.shortDescription(),
    self.baseDir = os.path.join(RDConfig.RDCodeDir,'ML','test_data')
    if not RDConfig.usePgSQL:
      self.dbName = os.path.join(self.baseDir,'test.gdb')
    else:
      self.dbName = "::RDTests"
    self.details = ScreenComposite.SetDefaults()
    self.details.dbName = self.dbName
    self.details.dbUser = RDConfig.defaultDBUser
    self.details.dbPassword = RDConfig.defaultDBPassword

  def test1(self):
    """ basics """
    self.details.tableName = 'ferro_quant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_quant_10.pkl'),
                              'rb'))
    tgt = 7
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==93
    assert misCount==2
    assert nSkipped==0
    assert feq(avgGood,.9753)
    assert feq(avgBad,.8000)
    assert tbl[0,0] == 54
    assert tbl[1,1] == 39
    assert tbl[0,1] == 2
    assert tbl[1,0] == 0
    
  def test2(self):
    """ include holdout data only """
    self.details.tableName = 'ferro_quant'
    self.details.doHoldout=1
    self.details.doTraining=0
    
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_quant_10.pkl'),
                              'rb'))
    tgt = 7
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==28
    assert misCount==1
    assert nSkipped==0
    assert feq(avgGood,.9607)
    assert feq(avgBad,.9000)
    assert tbl[0,0] == 16
    assert tbl[1,1] == 12
    assert tbl[0,1] == 1
    assert tbl[1,0] == 0
    
  def test3(self):
    """ include training data only """
    self.details.tableName = 'ferro_quant'
    self.details.doHoldout=0
    self.details.doTraining=1

    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_quant_10.pkl'),
                              'rb'))
    tgt = 7
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==65
    assert misCount==1
    assert nSkipped==0
    assert feq(avgGood,.9815)
    assert feq(avgBad,.7000)
    assert tbl[0,0] == 38
    assert tbl[1,1] == 27
    assert tbl[0,1] == 1
    assert tbl[1,0] == 0
    
  def test4(self):
    """ include thresholding """
    self.details.tableName = 'ferro_quant'
    self.details.threshold = 0.80
    self.details.doHoldout=0
    self.details.doTraining=0

    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_quant_10.pkl'),
                              'rb'))
    tgt = 7
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==86,str(nGood)
    assert misCount==1
    assert nSkipped==8
    assert feq(avgGood,.9930)
    assert feq(avgBad,.9000)
    assert feq(avgSkip,.7500)
    assert tbl[0,0] == 50
    assert tbl[1,1] == 36
    assert tbl[0,1] == 1
    assert tbl[1,0] == 0
    
  def test5(self):
    """ basics """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_auto_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==93
    assert misCount==10
    assert nSkipped==0
    assert feq(avgGood,.9505)
    assert feq(avgBad,.7600)
    assert tbl[0,0] == 55
    assert tbl[1,1] == 38
    assert tbl[0,1] == 0
    assert tbl[1,0] == 10
    
  def test6(self):
    """ multiple models """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_auto_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    composites = [compos,compos]
    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = \
                                                       ScreenComposite.ScreenFromDetails(composites,self.details)
    assert feq(nGood[0],93)
    assert feq(misCount[0],10)
    assert feq(nSkipped[0],0)
    assert feq(avgGood[0],.9505)
    assert feq(avgBad[0],.7600)
    assert feq(nGood[1],0)
    assert feq(misCount[1],0)
    assert feq(nSkipped[1],0)
    assert feq(avgGood[1],0)
    assert feq(avgBad[1],0)
    assert feq(tbl[0,0],55)
    assert feq(tbl[1,1],38)
    assert feq(tbl[0,1],0)
    assert feq(tbl[1,0],10)
    
  def test7(self):
    """ shuffle """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_shuffle_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.shuffleActivities=1
    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==62
    assert misCount==41
    assert nSkipped==0
    assert feq(avgGood,.7839)
    assert feq(avgBad,.7049)
    assert tbl[0,0] == 36
    assert tbl[1,1] == 26
    assert tbl[0,1] == 19
    assert tbl[1,0] == 22
    
  def test8(self):
    """ shuffle with segmentation """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_shuffle_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.shuffleActivities=1
    self.details.doHoldout=1
    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==15
    assert misCount==16
    assert nSkipped==0
    assert feq(avgGood,.6867)
    assert feq(avgBad,.7750)
    assert tbl[0,0] == 13
    assert tbl[1,1] == 2
    assert tbl[0,1] == 6
    assert tbl[1,0] == 10
    
  def test9(self):
    """ shuffle with segmentation2 """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_shuffle_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.shuffleActivities=1
    self.details.doTraining=1
    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==47
    assert misCount==25
    assert nSkipped==0
    assert feq(avgGood,.8149)
    assert feq(avgBad,.6600)
    assert tbl[0,0] == 23
    assert tbl[1,1] == 24
    assert tbl[0,1] == 13
    assert tbl[1,0] == 12
    
  def test10(self):
    """ filtering """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_filt_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.filterVal=1
    self.details.filterFrac=.33

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood==90
    assert misCount==13
    assert nSkipped==0
    assert feq(avgGood,.9578)
    assert feq(avgBad,.8538)
    assert tbl[0,0] == 54
    assert tbl[1,1] == 36
    assert tbl[0,1] == 1
    assert tbl[1,0] == 12
    
  def _test11(self):
    """ filtering with segmentation """
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_filt_10_3.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.doHoldout=1
    self.details.filterVal=1
    self.details.filterFrac=.33

    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    print nGood,misCount,nSkipped,avgGood,avgBad,avgSkip
    assert nGood==46,nGood
    assert misCount==6
    assert nSkipped==0
    assert feq(avgGood,.9500)
    assert feq(avgBad,.8333)
    assert tbl[0,0] == 37
    assert tbl[1,1] == 9
    assert tbl[0,1] == 1
    assert tbl[1,0] == 5

  def test12(self):
    """ test the naive bayes composite"""
    self.details.tableName = 'ferro_noquant'
    compos = pickle.load(open(os.path.join(self.baseDir,'ferromag_NaiveBayes.pkl'),
                              'rb'))
    tgt = 10
    assert len(compos)==tgt,'bad composite loaded: %d != %d'%(len(compos),tgt)
    self.details.doHoldout=1
    nGood,misCount,nSkipped,avgGood,avgBad,avgSkip,tbl = ScreenComposite.ScreenFromDetails(compos,self.details)
    assert nGood == 26
    assert misCount == 5
    assert nSkipped == 0
    assert feq(avgGood, 0.9538)
    assert feq(avgBad, 0.86)
    assert tbl[0,0] == 9
    assert tbl[0,1] == 4
    assert tbl[1,0] == 1
    assert tbl[1,1] == 17
    

if __name__ == '__main__':
  unittest.main()

