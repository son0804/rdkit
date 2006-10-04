## Automatically adapted for numpy.oldnumeric Sep 23, 2006 by alter_code1.py

# $Id$
#
#  Copyright (C) 2001-2004  greg Landrum and Rational Discovery LLC
#  All Rights Reserved
#
""" 

"""

from Numeric import *
import numpy.oldnumeric.random_array as RandomArray
from ML.DecTree import QuantTree, ID3
from ML.InfoTheory import entropy
from ML.Data import Quantize

def FindBest(resCodes,examples,nBoundsPerVar,nPossibleRes,
             nPossibleVals,attrs,**kwargs):
  bestGain =-1e6
  best = -1
  bestBounds = []

  if not len(examples):
    return best,bestGain,bestBounds

  nToTake = kwargs.get('randomDescriptors',0)
  if nToTake > 0:
    nAttrs = len(attrs)
    if nToTake < nAttrs:
      ids = RandomArray.permutation(nAttrs)
      tmp = [attrs[x] for x in ids[:nToTake]]
      #print '\tavail:',tmp
      attrs = tmp

  for var in attrs:
    nBounds = nBoundsPerVar[var]
    if nBounds > 0:
      #vTable = map(lambda x,z=var:x[z],examples)
      try:
        vTable = [x[var] for x in examples]
      except IndexError:
        print 'index error retrieving variable: %d'%var
        raise
      qBounds,gainHere = Quantize.FindVarMultQuantBounds(vTable,nBounds,
                                                         resCodes,nPossibleRes)
      #print '\tvar:',var,qBounds,gainHere
    elif nBounds==0:
      vTable = ID3.GenVarTable(examples,nPossibleVals,[var])[0]
      gainHere = entropy.InfoGain(vTable)
      qBounds = []
    else:
      gainHere = -1e6
      qBounds = []
    if gainHere > bestGain:
      bestGain = gainHere
      bestBounds = qBounds
      best = var
  if best == -1:
    print 'best unaltered'
    print '\tattrs:',attrs
    print '\tnBounds:',take(nBoundsPerVar,attrs)
    print '\texamples:'
    for example in examples:
      print '\t\t',example

  return best,bestGain,bestBounds


def BuildQuantTree(examples,target,attrs,nPossibleVals,nBoundsPerVar,
                   depth=0,maxDepth=-1,**kwargs):
  """ 
    **Arguments**
    
      - examples: a list of lists (nInstances x nVariables+1) of variable
        values + instance values

      - target: an int

      - attrs: a list of ints indicating which variables can be used in the tree

      - nPossibleVals: a list containing the number of possible values of
                   every variable.

      - nBoundsPerVar: the number of bounds to include for each variable

      - depth: (optional) the current depth in the tree

      - maxDepth: (optional) the maximum depth to which the tree
                   will be grown
    **Returns**
    
     a QuantTree.QuantTreeNode with the decision tree

    **NOTE:** This code cannot bootstrap (start from nothing...)
          use _QuantTreeBoot_ (below) for that.
  """
  tree=QuantTree.QuantTreeNode(None,'node')
  tree.SetData(-666)
  nPossibleRes = nPossibleVals[-1]
  
  # counts of each result code:
  resCodes = [int(x[-1]) for x in examples]
  counts = [0]*nPossibleRes
  for res in resCodes:
    counts[res] += 1
  nzCounts = nonzero(counts)

  if len(nzCounts) == 1:
    # bottomed out because there is only one result code left
    #  with any counts (i.e. there's only one type of example
    #  left... this is GOOD!).
    res = nzCounts[0]
    tree.SetLabel(res)
    tree.SetName(str(res))
    tree.SetTerminal(1)
  elif len(attrs) == 0 or (maxDepth>=0 and depth>maxDepth):
    # Bottomed out: no variables left or max depth hit
    #  We don't really know what to do here, so
    #  use the heuristic of picking the most prevalent
    #  result
    v =  argmax(counts)
    tree.SetLabel(v)
    tree.SetName('%d?'%v)
    tree.SetTerminal(1)
  else:
    # find the variable which gives us the largest information gain
    best,bestGain,bestBounds = FindBest(resCodes,examples,nBoundsPerVar,
                                        nPossibleRes,nPossibleVals,attrs,
                                        **kwargs)

    # remove that variable from the lists of possible variables
    nextAttrs = attrs[:]
    if not kwargs.get('recycleVars',0):
      nextAttrs.remove(best)

    # set some info at this node
    tree.SetName('Var: %d'%(best))
    tree.SetLabel(best)
    tree.SetQuantBounds(bestBounds)
    tree.SetTerminal(0)
    
    # loop over possible values of the new variable and
    #  build a subtree for each one
    indices = range(len(examples))
    if len(bestBounds) > 0:
      for bound in bestBounds:
        nextExamples = []
        for index in indices[:]:
          ex = examples[index]
          if ex[best] < bound:
            nextExamples.append(ex)
            indices.remove(index)

        if len(nextExamples) == 0:
          # this particular value of the variable has no examples,
          #  so there's not much sense in recursing.
          #  This can (and does) happen.
          v =  argmax(counts)
          tree.AddChild('%d'%v,label=v,data=0.0,isTerminal=1)
        else:
          # recurse
          tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                           nextAttrs,nPossibleVals,
                                           nBoundsPerVar,
                                           depth=depth+1,maxDepth=maxDepth,
                                           **kwargs))
      # add the last points remaining
      nextExamples = []
      for index in indices:
        nextExamples.append(examples[index])
      if len(nextExamples) == 0:
        v =  argmax(counts)
        tree.AddChild('%d'%v,label=v,data=0.0,isTerminal=1)
      else:
        tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                         nextAttrs,nPossibleVals,
                                         nBoundsPerVar,
                                         depth=depth+1,maxDepth=maxDepth,
                                         **kwargs))
    else:
      for val in xrange(nPossibleVals[best]):
        nextExamples = []
        for example in examples:
          if example[best] == val:
            nextExamples.append(example)
        if len(nextExamples) == 0:
          v =  argmax(counts)
          tree.AddChild('%d'%v,label=v,data=0.0,isTerminal=1)
        else:
          tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                           nextAttrs,nPossibleVals,
                                           nBoundsPerVar,
                                           depth=depth+1,maxDepth=maxDepth,
                                           **kwargs))
  return tree

def QuantTreeBoot(examples,attrs,nPossibleVals,nBoundsPerVar,initialVar=None,
                  maxDepth=-1,**kwargs):
  """ Bootstrapping code for the QuantTree

    If _initialVar_ is not set, the algorithm will automatically
     choose the first variable in the tree (the standard greedy
     approach).  Otherwise, _initialVar_ will be used as the first
     split.
     
  """
  attrs = attrs[:]
  for i in range(len(nBoundsPerVar)):
    if nBoundsPerVar[i]==-1 and i in attrs:
      attrs.remove(i)
  
  tree=QuantTree.QuantTreeNode(None,'node')
  nPossibleRes = nPossibleVals[-1]
  tree._nResultCodes = nPossibleRes

  resCodes = [int(x[-1]) for x in examples]
  counts = [0]*nPossibleRes
  for res in resCodes:
    counts[res] += 1
  if initialVar is None:
    best,gainHere,qBounds = FindBest(resCodes,examples,nBoundsPerVar,
                                     nPossibleRes,nPossibleVals,attrs,
                                     **kwargs)
  else:
    best = initialVar
    if nBoundsPerVar[best] > 0:
      vTable = map(lambda x,z=best:x[z],examples)
      qBounds,gainHere = Quantize.FindVarMultQuantBounds(vTable,nBoundsPerVar[best],
                                                         resCodes,nPossibleRes)
    elif nBoundsPerVar[best] == 0:
      vTable = ID3.GenVarTable(examples,nPossibleVals,[best])[0]
      gainHere = entropy.InfoGain(vTable)
      qBounds = []
    else:
      gainHere = -1e6
      qBounds = []

  tree.SetName('Var: %d'%(best))
  tree.SetData(gainHere)
  tree.SetLabel(best)
  tree.SetTerminal(0)
  tree.SetQuantBounds(qBounds)
  nextAttrs = attrs[:]
  if not kwargs.get('recycleVars',0):
    nextAttrs.remove(best)

  indices = range(len(examples))
  if len(qBounds) > 0:
    for bound in qBounds:
      nextExamples = []
      for index in indices[:]:
        ex = examples[index]
        if ex[best] < bound:
          nextExamples.append(ex)
          indices.remove(index)

      if len(nextExamples):
        tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                         nextAttrs,nPossibleVals,
                                         nBoundsPerVar,
                                         depth=1,maxDepth=maxDepth,
                                         **kwargs))
      else:
        v =  argmax(counts)
        tree.AddChild('%d??'%(v),label=v,data=0.0,isTerminal=1)
    # add the last points remaining
    nextExamples = []
    for index in indices:
      nextExamples.append(examples[index])
    if len(nextExamples) != 0:
      tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                       nextAttrs,nPossibleVals,
                                       nBoundsPerVar,
                                       depth=1,maxDepth=maxDepth,
                                       **kwargs))
    else:
      v =  argmax(counts)
      tree.AddChild('%d??'%(v),label=v,data=0.0,isTerminal=1)
  else:
    for val in xrange(nPossibleVals[best]):
      nextExamples = []
      for example in examples:
        if example[best] == val:
          nextExamples.append(example)
      if len(nextExamples) != 0:    
        tree.AddChildNode(BuildQuantTree(nextExamples,best,
                                         nextAttrs,nPossibleVals,
                                         nBoundsPerVar,
                                         depth=1,maxDepth=maxDepth,
                                         **kwargs))
      else:
        v =  argmax(counts)
        tree.AddChild('%d??'%(v),label=v,data=0.0,isTerminal=1)
  return tree

  
def TestTree():
  """ testing code for named trees

  """
  examples1 = [['p1',0,1,0,0],
              ['p2',0,0,0,1],
              ['p3',0,0,1,2],
              ['p4',0,1,1,2],
              ['p5',1,0,0,2],
              ['p6',1,0,1,2],
              ['p7',1,1,0,2],
              ['p8',1,1,1,0]
              ]
  attrs = range(1,len(examples1[0])-1)
  nPossibleVals = [0,2,2,2,3]
  t1 = ID3.ID3Boot(examples1,attrs,nPossibleVals,maxDepth=1)
  t1.Print()


def TestQuantTree():
  """ testing code for named trees

  """
  examples1 = [['p1',0,1,0.1,0],
              ['p2',0,0,0.1,1],
              ['p3',0,0,1.1,2],
              ['p4',0,1,1.1,2],
              ['p5',1,0,0.1,2],
              ['p6',1,0,1.1,2],
              ['p7',1,1,0.1,2],
              ['p8',1,1,1.1,0]
              ]
  attrs = range(1,len(examples1[0])-1)
  nPossibleVals = [0,2,2,0,3]
  boundsPerVar=[0,0,0,1,0]
  
  print 'base'
  t1 = QuantTreeBoot(examples1,attrs,nPossibleVals,boundsPerVar)
  t1.Pickle('regress/QuantTree1.pkl')
  t1.Print()

  print 'depth limit'
  t1 = QuantTreeBoot(examples1,attrs,nPossibleVals,boundsPerVar,maxDepth=1)
  t1.Pickle('regress/QuantTree1.pkl')
  t1.Print()

def TestQuantTree2():
  """ testing code for named trees

  """
  examples1 = [['p1',0.1,1,0.1,0],
              ['p2',0.1,0,0.1,1],
              ['p3',0.1,0,1.1,2],
              ['p4',0.1,1,1.1,2],
              ['p5',1.1,0,0.1,2],
              ['p6',1.1,0,1.1,2],
              ['p7',1.1,1,0.1,2],
              ['p8',1.1,1,1.1,0]
              ]
  attrs = range(1,len(examples1[0])-1)
  nPossibleVals = [0,0,2,0,3]
  boundsPerVar=[0,1,0,1,0]
  
  t1 = QuantTreeBoot(examples1,attrs,nPossibleVals,boundsPerVar)
  t1.Print()
  t1.Pickle('regress/QuantTree2.pkl')

  for example in examples1:
    print example,t1.ClassifyExample(example)

if __name__ == "__main__":
  TestTree()
  TestQuantTree()
  #TestQuantTree2()
  
