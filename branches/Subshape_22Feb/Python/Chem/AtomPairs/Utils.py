# $Id$
#
#  Copyright (C) 2004-2006 Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
import Chem
import math

_atomNumberTypes = [
  5,  # B
  6,  # C
  7,  # N
  8,  # O
  9,  # F
  14, # Si
  15, # P
  16, # S
  17, # Cl
  33, # As
  34, # Se
  35, # Br
  51, # Sb
  52, # Te
  53, # I
  -1, # Everything else
]
numTypeBits=int(math.ceil(math.log(len(_atomNumberTypes))/math.log(2)))
_maxNumPi = 3 
numPiBits = 2
_maxNumBranches = 7
numBranchBits = 3
codeSize=numTypeBits+numPiBits+numBranchBits

def GetAtomCode(atom,branchSubtract=0):
  """

  **Arguments**:

    - atom: the atom to be considered

    - branchSubtract: (optional) a constant to be subtracted off the
      number of neighbors before it's integrated into the code.  This
      is used by the topological torsions code.
      

  >>> m = Chem.MolFromSmiles('C=C')
  >>> shift1 = numBranchBits
  >>> shift2 = numBranchBits + numPiBits
  >>> GetAtomCode(m.GetAtomWithIdx(0),1) == (1 << shift1) + (1 << shift2)
  1
  >>> m = Chem.MolFromSmiles('C#CO')
  >>> GetAtomCode(m.GetAtomWithIdx(0),1) == (2 << shift1) + (1 << shift2)
  1
  >>> GetAtomCode(m.GetAtomWithIdx(1),2) == (2 << shift1) + (1 << shift2)
  1
  >>> GetAtomCode(m.GetAtomWithIdx(2),1) == 0 + (3 << shift2)
  1
  
  >>> m = Chem.MolFromSmiles('CC(O)C(O)(O)C')
  >>> GetAtomCode(m.GetAtomWithIdx(1),2) == 1 + (0 << shift1) + (1 << shift2)
  1
  >>> GetAtomCode(m.GetAtomWithIdx(3),2) == 2 + (0 << shift1) + (1 << shift2)
  1


  """
  nBranches = atom.GetDegree() - branchSubtract
  if nBranches<0: nBranches=0
  code = nBranches % _maxNumBranches

  nPi = NumPiElectrons(atom) % _maxNumPi
  code |= (nPi<<numBranchBits)

  try:
    typeIdx = _atomNumberTypes.index(atom.GetAtomicNum())
  except ValueError:
    typeIdx = len(_atomNumberTypes)-1
  code |= typeIdx << (numBranchBits+numPiBits)

  return code

def NumPiElectrons(atom):
  """ Returns the number of electrons an atom is using for pi bonding

  >>> m = Chem.MolFromSmiles('C=C')
  >>> NumPiElectrons(m.GetAtomWithIdx(0))
  1

  >>> m = Chem.MolFromSmiles('C#CC')
  >>> NumPiElectrons(m.GetAtomWithIdx(0))
  2
  >>> NumPiElectrons(m.GetAtomWithIdx(1))
  2

  >>> m = Chem.MolFromSmiles('O=C=CC')
  >>> NumPiElectrons(m.GetAtomWithIdx(0))
  1
  >>> NumPiElectrons(m.GetAtomWithIdx(1))
  2
  >>> NumPiElectrons(m.GetAtomWithIdx(2))
  1
  >>> NumPiElectrons(m.GetAtomWithIdx(3))
  0

  FIX: this behaves oddly in these cases:
  >>> m = Chem.MolFromSmiles('S(=O)(=O)')
  >>> NumPiElectrons(m.GetAtomWithIdx(0))
  2

  >>> m = Chem.MolFromSmiles('S(=O)(=O)(O)O')
  >>> NumPiElectrons(m.GetAtomWithIdx(0))
  0

  In the second case, the S atom is tagged as sp3 hybridized.

  """
  
  res = 0
  if atom.GetIsAromatic():
    res = 1
  elif atom.GetHybridization() != Chem.HybridizationType.SP3:
    # the number of pi electrons is just the number of
    # unsaturations (valence - degree):
    res = atom.GetExplicitValence() - atom.GetDegree()
  return res


def BitsInCommon(v1,v2):
  """ Returns the number of bits in common between two vectors

  **Arguments**:

    - two vectors (sequences of bit ids)

  **Returns**: an integer

  **Notes**

    - the vectors must be sorted

    - duplicate bit IDs are counted more than once

  >>> BitsInCommon( (1,2,3,4,10), (2,4,6) )
  2

  Here's how duplicates are handled:
  >>> BitsInCommon( (1,2,2,3,4), (2,2,4,5,6) )
  3
   
  """
  res = 0
  v2Pos = 0
  nV2 = len(v2)
  for val in v1:
    while v2Pos<nV2 and v2[v2Pos]<val:
      v2Pos+=1
    if v2Pos >= nV2:
      break
    if v2[v2Pos]==val:
      res += 1
      v2Pos += 1
  return res


def DiceSimilarity(v1,v2,bounds=None):
  """ Implements the DICE similarity metric.
   This is the recommended metric in both the Topological torsions
   and Atom pairs papers.

  **Arguments**:

    - two vectors (sequences of bit ids)

  **Returns**: a float.

  **Notes**

    - the vectors must be sorted

    
  >>> DiceSimilarity( (1,2,3), (1,2,3) )
  1.0
  >>> DiceSimilarity( (1,2,3), (5,6) )
  0.0
  >>> DiceSimilarity( (1,2,3,4), (1,3,5,7) )
  0.5
  >>> DiceSimilarity( (1,2,3,4,5,6), (1,3) )
  0.5

  Note that duplicate bit IDs count multiple times:
  >>> DiceSimilarity( (1,1,3,4,5,6), (1,1) )
  0.5

  but only if they are duplicated in both vectors:
  >>> DiceSimilarity( (1,1,3,4,5,6), (1,) )==2./7
  True

  """
  denom = 1.0*(len(v1)+len(v2))
  if not denom:
    res = 0.0
  else:
    if bounds and (min(len(v1),len(v2))/denom) < bounds:
      numer = 0.0
    else:
      numer = 2.0*BitsInCommon(v1,v2)
    res = numer/denom

  return res


def Dot(v1,v2):
  """ Returns the Dot product between two vectors:

  **Arguments**:

    - two vectors (sequences of bit ids)

  **Returns**: an integer

  **Notes**

    - the vectors must be sorted

    - duplicate bit IDs are counted more than once

  >>> Dot( (1,2,3,4,10), (2,4,6) )
  2

  Here's how duplicates are handled:
  >>> Dot( (1,2,2,3,4), (2,2,4,5,6) )
  5
  >>> Dot( (1,2,2,3,4), (2,4,5,6) )
  2
  >>> Dot( (1,2,2,3,4), (5,6) )
  0
  >>> Dot( (), (5,6) )
  0
  
  """
  res = 0
  nV1 = len(v1)
  nV2 = len(v2)
  i = 0
  j = 0
  while i<nV1:
    v1Val = v1[i]
    v1Count = 1
    i+=1
    while i<nV1 and v1[i]==v1Val:
      v1Count += 1
      i+=1
    while j<nV2 and v2[j]<v1Val:
      j+=1
    if j < nV2 and v2[j]==v1Val:
      v2Val = v2[j]
      v2Count = 1
      j+=1
      while j<nV2 and v2[j]==v1Val:
        v2Count+=1
        j+=1
      commonCount=min(v1Count,v2Count)
      res += commonCount*commonCount
    elif j>=nV2:
      break
  return res

def CosineSimilarity(v1,v2):
  """ Implements the Cosine similarity metric.
   This is the recommended metric in the LaSSI paper

  **Arguments**:

    - two vectors (sequences of bit ids)

  **Returns**: a float.

  **Notes**

    - the vectors must be sorted

  >>> print '%.3f'%CosineSimilarity( (1,2,3,4,10), (2,4,6) )
  0.516
  >>> print '%.3f'%CosineSimilarity( (1,2,2,3,4), (2,2,4,5,6) )
  0.714
  >>> print '%.3f'%CosineSimilarity( (1,2,2,3,4), (1,2,2,3,4) )
  1.000
  >>> print '%.3f'%CosineSimilarity( (1,2,2,3,4), (5,6,7) )
  0.000
  >>> print '%.3f'%CosineSimilarity( (1,2,2,3,4), () )
  0.000

  """
  d1 = Dot(v1,v1)
  d2 = Dot(v2,v2)
  denom = math.sqrt(d1*d2)
  if not denom:
    res = 0.0
  else:
    numer = Dot(v1,v2)
    res = numer/denom
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
