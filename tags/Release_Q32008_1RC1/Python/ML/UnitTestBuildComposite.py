# $Id$
#
#  Copyright (C) 2003-2006  greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
"""unit testing code for the BuildComposite functionality

"""
import RDConfig
import unittest,os
from ML import BuildComposite
from ML import ScreenComposite
from Dbase.DbConnection import DbConnect
import cPickle as pickle

def feq(a,b,tol=1e-4):
  if abs(a-b)>tol: return 0
  else: return 1

class TestCase(unittest.TestCase):
  def setUp(self):
    #print '\n%s: '%self.shortDescription(),
    self.baseDir = os.path.join(RDConfig.RDCodeDir,'ML','test_data')
    self.dbName = RDConfig.RDTestDatabase
      
    self.details = BuildComposite.SetDefaults()
    self.details.dbName = self.dbName
    self.details.dbUser = RDConfig.defaultDBUser
    self.details.dbPassword = RDConfig.defaultDBPassword

  def _init(self,refCompos,copyBounds=0):
    BuildComposite._verbose=0
    conn = DbConnect(self.details.dbName,self.details.tableName)
    cols = [x.upper() for x in conn.GetColumnNames()]
    cDescs = [x.upper() for x in refCompos.GetDescriptorNames()]
    assert cols==cDescs,'bad descriptor names in table: %s != %s'%(cols,cDescs)
    
    self.details.nModels = 10
    self.details.lockRandom = 1
    self.details.randomSeed = refCompos._randomSeed
    self.details.splitFrac = refCompos._splitFrac

    if self.details.splitFrac:
      self.details.splitRun = 1
    else:
      self.details.splitRun = 0

    if not copyBounds:
      self.details.qBounds = [0]*len(cols)
    else:
      self.details.qBounds = refCompos.GetQuantBounds()[0]

  def compare(self,compos,refCompos):
    assert len(compos)==len(refCompos),'%d != %d'%(len(compos),len(refCompos))
    cs = []
    rcs = []
    for i in range(len(compos)):
      cs.append(compos[i])
      rcs.append(refCompos[i])
    def sortHelp(x,y):
      if x[2]==y[2]:
        return cmp(x[1],y[1])
      else:
        return cmp(x[2],y[2])
    cs.sort(sortHelp)
    rcs.sort(sortHelp)
    for i in range(len(compos)):
      tree,count,err = cs[i]
      refTree,refCount,refErr = rcs[i]
      assert count==refCount, str((count,refCount))
      assert feq(err,refErr),'%f != %f'%(err,refErr)
    
  def test1(self):
    """ basics """
    self.details.tableName = 'ferro_quant'
    refComposName = 'ferromag_quant_10.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))

    # first make sure the data are intact
    self._init(refCompos)
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)
    
  def test2(self):
    """ depth limit """
    self.details.tableName = 'ferro_quant'
    refComposName = 'ferromag_quant_10_3.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))

    # first make sure the data are intact
    self._init(refCompos)
    self.details.limitDepth = 3
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)
    
  def test3(self):
    """ depth limit + less greedy """
    self.details.tableName = 'ferro_quant'
    refComposName = 'ferromag_quant_10_3_lessgreedy.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))

    # first make sure the data are intact
    self._init(refCompos)
    self.details.limitDepth = 3
    self.details.lessGreedy = 1
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)
    
  def test4(self):
    """ more trees """
    self.details.tableName = 'ferro_quant'
    refComposName = 'ferromag_quant_50_3.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))

    # first make sure the data are intact
    self._init(refCompos)
    self.details.limitDepth = 3
    self.details.nModels = 50
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)

  def test5(self):
    """ auto bounds """
    self.details.tableName = 'ferro_noquant'
    refComposName = 'ferromag_auto_10_3.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))


    # first make sure the data are intact
    self._init(refCompos,copyBounds=1)
    self.details.limitDepth = 3
    self.details.nModels = 10
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)

  def test6(self):
    """ auto bounds with a real valued activity"""
    self.details.tableName = 'ferro_noquant_realact'
    refComposName = 'ferromag_auto_10_3.pkl'

    refCompos = pickle.load(open(os.path.join(self.baseDir,refComposName),
                                 'rb'))

    # first make sure the data are intact
    self._init(refCompos,copyBounds=1)
    self.details.limitDepth = 3
    self.details.nModels = 10
    self.details.activityBounds=[0.5]
    compos = BuildComposite.RunIt(self.details,saveIt=0)

    self.compare(compos,refCompos)

  def test7(self):
    """ Test composite of naive bayes"""
    self.details.tableName = 'ferro_noquant'
    refComposName = 'ferromag_NaiveBayes.pkl'
    pklFile = open(os.path.join(self.baseDir,refComposName), 'rb')
    refCompos = pickle.load(pklFile)
    self._init(refCompos,copyBounds=1)
    self.details.useTrees = 0
    self.details.useNaiveBayes = 1
    self.details.mEstimateVal = 20.0
    self.details.qBounds = [0] + [2]*6 + [0]
    compos = BuildComposite.RunIt(self.details, saveIt= 0)
    self.compare(compos,refCompos)
        
    
if __name__ == '__main__':
  unittest.main()

