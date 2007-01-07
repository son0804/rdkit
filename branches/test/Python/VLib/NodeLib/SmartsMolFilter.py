#  $Id$
#
#  Copyright (C) 2003 Rational Discovery LLC
#     All Rights Reserved
#
import RDConfig
import sys,os,types
import Chem
from VLib.Filter import FilterNode

class SmartsFilter(FilterNode):
  """ filter out molecules matching one or more SMARTS patterns

  There is a count associated with each pattern. Molecules are
  allowed to match the pattern up to this number of times.
  
  Assumptions:

    - inputs are molecules


  Sample Usage:
    >>> smis = ['C1CCC1','C1CCC1C=O','CCCC','CCC=O','CC(=O)C','CCN','NCCN','NCC=O']
    >>> mols = [Chem.MolFromSmiles(x) for x in smis]
    >>> from VLib.Supply import SupplyNode
    >>> suppl = SupplyNode(contents=mols)
    >>> ms = [x for x in suppl]
    >>> len(ms)
    8

    We can pass in SMARTS strings:
    >>> smas = ['C=O','CN']
    >>> counts = [1,2]
    >>> filt = SmartsFilter(patterns=smas,counts=counts)
    >>> filt.AddParent(suppl)
    >>> ms = [x for x in filt]
    >>> len(ms)
    5

    Alternatively, we can pass in molecule objects:
    >>> mols =[Chem.MolFromSmarts(x) for x in smas]
    >>> counts = [1,2]
    >>> filt.Destroy()
    >>> filt = SmartsFilter(patterns=mols,counts=counts)
    >>> filt.AddParent(suppl)
    >>> ms = [x for x in filt]
    >>> len(ms)
    5

    Negation does what you'd expect:
    >>> filt.SetNegate(1)
    >>> ms = [x for x in filt]
    >>> len(ms)
    3


  """
  def __init__(self,patterns=[],counts=[],**kwargs):
    FilterNode.__init__(self,func=self.filter,**kwargs)
    self._initPatterns(patterns,counts)

  def _initPatterns(self,patterns,counts):
    nPatts = len(patterns)
    if len(counts) and len(counts)!=nPatts:
      raise ValueError,'if counts is specified, it must match patterns in length'
    if not len(counts):
      counts = [1]*nPatts
    targets = [None]*nPatts
    for i in range(nPatts):
      p = patterns[i]
      c = counts[i]
      if type(p) in types.StringTypes:
        m = Chem.MolFromSmarts(p)
        if not m:
          raise ValueError,'bad smarts: %s'%(p)
        p = m
      targets[i] = p,c
    self._patterns = tuple(targets)  
      
  def filter(self,cmpd):
    neg = self.Negate()
    res = 0
    #sys.stderr.write('\tFILTER: %s\n'%(Chem.MolToSmiles(cmpd)))
    for patt,count in self._patterns:
      ms = cmpd.GetSubstructMatches(patt)
      nMatches = len(ms)
      if nMatches >= count:
        # this query is an or, so we short circuit true:
        res = 1
        break
    return res
      
#------------------------------------
#
#  doctest boilerplate
#
def _test():
  import doctest,sys
  return doctest.testmod(sys.modules["__main__"])


if __name__ == '__main__':
  import sys
  failed,tried = _test()
  sys.exit(failed)

  
