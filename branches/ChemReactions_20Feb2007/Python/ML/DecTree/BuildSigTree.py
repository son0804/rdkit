# $Id$
#
#  Copyright (C) 2003-2005  Rational Discovery LLC
#    All Rights Reserved
#
""" 

"""

from Numeric import *
import RandomArray
from ML.DecTree import SigTree
from ML import InfoTheory
from ML.FeatureSelect import CMIM
from DataStructs.VectCollection import VectCollection
import copy
import random
def _GenerateRandomEnsemble(nToInclude,nBits):
  """  Generates a random subset of a group of indices

  **Arguments**

    - nToInclude: the size of the desired set

    - nBits: the maximum index to be included in the set

   **Returns**

     a list of indices

  """
  # Before Python 2.3 added the random.sample() function, this was
  # way more complicated:
  res = random.sample(xrange(nBits),nToInclude)
  return res

def BuildSigTree(examples,nPossibleRes,ensemble=None,random=0,
                 metric=InfoTheory.InfoType.BIASENTROPY,
                 biasList=[1],
                 depth=0,maxDepth=-1,
                 useCMIM=0,allowCollections=False,
                 verbose=0,**kwargs):
  """ 
    **Arguments**
    
      - examples: the examples to be classified.  Each example
        should be a sequence at least three entries long, with
        entry 0 being a label, entry 1 a BitVector and entry -1
        an activity value

      - nPossibleRes: the number of result codes possible

      - ensemble: (optional) if this argument is provided, it
        should be a sequence which is used to limit the bits
        which are actually considered as potential descriptors.
        The default is None (use all bits).
        
      - random: (optional) If this argument is nonzero, it
        specifies the number of bits to be randomly selected
        for consideration at this node (i.e. this toggles the
        growth of Random Trees).
        The default is 0 (no random descriptor selection)

      - metric: (optional) This is an _InfoTheory.InfoType_ and
        sets the metric used to rank the bits.
        The default is _InfoTheory.InfoType.BIASENTROPY_

      - biasList: (optional) If provided, this provides a bias
        list for the bit ranker.
        See the _InfoTheory.InfoBitRanker_ docs for an explanation
        of bias.
        The default value is [1], which biases towards actives.

      - maxDepth: (optional) the maximum depth to which the tree
                   will be grown
        The default is -1 (no depth limit).

      - useCMIM: (optional) if this is >0, the CMIM algorithm
          (conditional mutual information maximization) will be
          used to select the descriptors used to build the trees.
          The value of the variable should be set to the number
          of descriptors to be used.  This option and the
          ensemble option are mutually exclusive (CMIM will not be
          used if the ensemble is set), but it happily coexsts
          with the random argument (to only consider random subsets
          of the top N CMIM bits)
        The default is 0 (do not use CMIM)
        
      - depth: (optional) the current depth in the tree
        This is used in the recursion and should not be set
        by the client.

    **Returns**
    
     a SigTree.SigTreeNode with the root of the decision tree

  """
  if verbose: print '  '*depth,'Build'
  tree=SigTree.SigTreeNode(None,'node',level=depth)
  tree.SetData(-666)
  #tree.SetExamples(examples)
  
  # counts of each result code:
  #resCodes = map(lambda x:int(x[-1]),examples)
  resCodes = [int(x[-1]) for x in examples]
  #print 'resCodes:',resCodes
  counts = [0]*nPossibleRes
  for res in resCodes:
    counts[res] += 1
  #print '    '*depth,'counts:',counts

  nzCounts = nonzero(counts)
  if verbose: print '  '*depth,'\tcounts:',counts
  if len(nzCounts) == 1:
    # bottomed out because there is only one result code left
    #  with any counts (i.e. there's only one type of example
    #  left... this is GOOD!).
    res = nzCounts[0]
    tree.SetLabel(res)
    tree.SetName(str(res))
    tree.SetTerminal(1)
  elif maxDepth>=0 and depth>maxDepth:
    # Bottomed out: max depth hit
    #  We don't really know what to do here, so
    #  use the heuristic of picking the most prevalent
    #  result
    v =  argmax(counts)
    tree.SetLabel(v)
    tree.SetName('%d?'%v)
    tree.SetTerminal(1)
  else:
    # find the variable which gives us the best improvement
    #  We do this with an InfoBitRanker:
    fp = examples[0][1]
    nBits = fp.GetNumBits()
    ranker = InfoTheory.InfoBitRanker(nBits,nPossibleRes,metric)
    if biasList: ranker.SetBiasList(biasList)
    if useCMIM > 0 and not ensemble:
      ensemble = CMIM.SelectFeatures(examples,useCMIM,bvCol=1)
    if random:
      if ensemble:
        if len(ensemble)>random:
          picks = _GenerateRandomEnsemble(random,len(ensemble))
          availBits = list(take(ensemble,picks))
        else:
          availBits = range(len(ensemble))
      else:
        availBits = _GenerateRandomEnsemble(random,nBits)
    else:
      availBits=None
    if availBits:
      ranker.SetMaskBits(availBits)
    #print '  2:'*depth,availBits

    useCollections=isinstance(examples[0][1],VectCollection)
    for example in examples:
      #print '  '*depth,example[1].ToBitString(),example[-1]
      if not useCollections:
        ranker.AccumulateVotes(example[1],example[-1])
      else:
        example[1].Reset()
        ranker.AccumulateVotes(example[1].orVect,example[-1])
        
    try:
      bitInfo = ranker.GetTopN(1)[0]
      best = int(bitInfo[0])
      gain = bitInfo[1]
    except:
      import traceback
      traceback.print_exc()
      print 'get top n failed'
      gain = -1.0
    if gain <= 0.0:
      v =  argmax(counts)
      tree.SetLabel(v)
      tree.SetName('?%d?'%v)
      tree.SetTerminal(1)
      return tree
    best = int(bitInfo[0])
    #print '  '*depth,'\tbest:',bitInfo
    if verbose: print '  '*depth,'\tbest:',bitInfo
    # set some info at this node
    tree.SetName('Bit-%d'%(best))
    tree.SetLabel(best)
    #tree.SetExamples(examples)
    tree.SetTerminal(0)
    
    # loop over possible values of the new variable and
    #  build a subtree for each one
    onExamples = []
    offExamples = []
    for example in examples:
      if example[1][best]:
        if allowCollections and useCollections:
          sig = copy.copy(example[1])
          sig.DetachVectsNotMatchingBit(best)
          ex = [example[0],sig]
          if len(example)>2:
            ex.extend(example[2:])
          example = ex
        onExamples.append(example)
      else:
        offExamples.append(example)
    #print '    '*depth,len(offExamples),len(onExamples)
    for ex in (offExamples,onExamples):
      if len(ex) == 0:
        v =  argmax(counts)
        tree.AddChild('%d??'%v,label=v,data=0.0,isTerminal=1)
      else:
        child = BuildSigTree(ex,nPossibleRes,random=random,
                             ensemble=ensemble,
                             metric=metric,biasList=biasList,
                             depth=depth+1,maxDepth=maxDepth,
                             verbose=verbose)
        if child is None:
          v =  argmax(counts)
          tree.AddChild('%d???'%v,label=v,data=0.0,isTerminal=1)
        else:
          tree.AddChildNode(child)
  return tree


def SigTreeBuilder(examples,attrs,nPossibleVals,initialVar=None,ensemble=None,
                   randomDescriptors=0,
                   **kwargs):
  nRes = nPossibleVals[-1]
  return BuildSigTree(examples,nRes,random=randomDescriptors,**kwargs)

def testTrees1():
  import DataStructs
  pts = [ ['p1',[3],0],
          ['p2',[1,3],1],
          ['p3',[2],1],          
          ['p4',[0],1],          
          ['p5',[],0],
          ]
  for pt in pts:
    bv = DataStructs.ExplicitBitVect(4)
    if pt[1]:
      bv.SetBitsFromList(pt[1])
    pt[1] = bv
  t = BuildSigTree(pts,2)
  t.Print()
  
  RandomArray.seed(23,42)
  t2 = BuildSigTree(pts,2,random=4)
  t2.Print()
  # these two happen to come out the same:
  assert t==t2

  RandomArray.seed(23,42)
  # this is just screwing around:
  t3 = BuildSigTree(pts,2,random=4,ensemble=range(4))
  assert t3==t2

  t3 = BuildSigTree(pts,2,useCMIM=3)
  print t3

if __name__ == "__main__":
  testTrees1()
  
