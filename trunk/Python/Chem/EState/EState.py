# $Id: EState.py 5007 2006-02-22 15:14:41Z glandrum $
#
# Copyright (C) 2002-2006 greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
""" Basic EState definitions

"""
from Numeric import *
import Chem

def GetPrincipleQuantumNumber(atNum):
  if atNum<=2: return 1
  elif atNum <= 10: return 2
  elif atNum <= 18: return 3
  elif atNum <= 36: return 4
  elif atNum <= 54: return 5
  elif atNum <= 86: return 6
  else: return 7
  

def EStateIndices(mol,force=1):
  """ returns a tuple of EState indices for the molecule

    Reference: Hall, Mohney and Kier. JCICS _31_ 76-81 (1991)

  """
  if not force and hasattr(mol,'_eStateIndices'):
    return mol._eStateIndices
    
  tbl = Chem.GetPeriodicTable()
  nAtoms = mol.GetNumAtoms()
  Is = zeros(nAtoms,Float)
  for i in range(nAtoms):
    at = mol.GetAtomWithIdx(i)
    atNum = at.GetAtomicNum()
    d = at.GetDegree()
    if d>0:
      h = at.GetTotalNumHs()
      dv = tbl.GetNOuterElecs(atNum)-h
      N = GetPrincipleQuantumNumber(atNum)
      Is[i] = (4./(N*N) * dv + 1)/d
  dists = Chem.GetDistanceMatrix(mol,useBO=0,useAtomWts=0)
  dists += 1
  accum = zeros(nAtoms,Float)
  for i in range(nAtoms):
    for j in range(i+1,nAtoms):
      p = dists[i,j]
      if p < 1e6:
        tmp = (Is[i]-Is[j])/(p*p)
        accum[i] += tmp
        accum[j] -= tmp

  res = accum+Is
  mol._eStateIndices=res
  return res
EStateIndices.version='1.0.0'

if __name__ =='__main__':
  smis = ['CCCC','CCCCC','CCCCCC','CC(N)C(=O)O','CC(N)C(=O)[O-].[Na+]']
  for smi in smis:
    m = Chem.MolFromSmi(smi)
    print smi
    inds = EStateIndices(m)
    print '\t',inds

