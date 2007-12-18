# $Id$
#
# Copyright (C) 2002-2006 greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
""" Atom-based calculation of LogP and MR using Crippen's approach


    Reference:
      S. A. Wildman and G. M. Crippen *JCICS* _39_ 868-873 (1999)


"""
import os
import RDConfig
import Chem
from Chem import rdMolDescriptors
from Numeric import *

_smartsPatterns = {}
_patternOrder = []
# this is the file containing the atom contributions
defaultPatternFileName = os.path.join(RDConfig.RDDataDir,'Crippen.txt')

def _ReadPatts(fileName):
  """ *Internal Use Only*

    parses the pattern list from the data file

  """
  patts = {}
  order = []
  for line in open(fileName,'r').xreadlines():
    if line[0] != '#':
      splitLine = line.split('\t')
      if len(splitLine)>=4 and splitLine[0] != '':
        sma = splitLine[1]
        if sma!='SMARTS':
          sma.replace('"','')
          try:
            p = Chem.MolFromSmarts(sma)
          except:
            pass
          else:
            if p:
              if len(splitLine[0])>1 and splitLine[0][1] not in 'S0123456789':
                cha = splitLine[0][:2]
              else:
                cha = splitLine[0][0]
              logP = float(splitLine[2])
              if splitLine[3] != '':
                mr = float(splitLine[3])
              else:
                mr = 0.0
              if cha not in order:
                order.append(cha)
              l = patts.get(cha,[])
              l.append((sma,p,logP,mr))
              patts[cha] = l
            else:
              print 'Problems parsing smarts: %s'%(sma)
  return order,patts

_GetAtomContribs=rdMolDescriptors._CalcCrippenContribs
def _pyGetAtomContribs(mol,patts=None,order=None,verbose=0,force=0):
  """ *Internal Use Only*

    calculates atomic contributions to the LogP and MR values

    if the argument *force* is not set, we'll use the molecules stored
    _crippenContribs value when possible instead of re-calculating.

  **Note:** Changes here affect the version numbers of MolLogP and MolMR
    as well as the VSA descriptors in Chem.MolSurf

  """
  if not force and hasattr(mol,'_crippenContribs'):
    return mol._crippenContribs

  if patts is None:
    patts = _smartsPatterns
    order = _patternOrder

  nAtoms = mol.GetNumAtoms()
  atomContribs = [(0.,0.)]*nAtoms
  doneAtoms=[0]*nAtoms
  nAtomsFound=0
  done = False
  for cha in order:
    pattVect = patts[cha]
    for sma,patt,logp,mr in pattVect:
      #print 'try:',entry[0]
      for match in mol.GetSubstructMatches(patt,False,False):
        firstIdx = match[0]
        if not doneAtoms[firstIdx]:
          doneAtoms[firstIdx]=1
          atomContribs[firstIdx] = (logp,mr)
          if verbose:
            print '\tAtom %d: %s %4.4f %4.4f'%(match[0],sma,logp,mr)
          nAtomsFound+=1
          if nAtomsFound>=nAtoms:
            done=True
            break
    if done: break
  mol._crippenContribs = atomContribs
  return atomContribs

def _Init():
  global _smartsPatterns,_patternOrder
  if _smartsPatterns == {}:
    _patternOrder,_smartsPatterns = _ReadPatts(defaultPatternFileName)

def pyMolLogP(inMol,patts=None,order=None,verbose=0,addHs=1):
  """ Crippen LogP value

  Definition From JCICS _39_ 868-873 (1999)

  **Arguments**

    - inMol: a molecule

    - patts: (optional)  if provided this should be a dictionary of 4 tuples:

       1)  smarts text

       2)  SmartsPattern object

       3)  logp contribution

       4)  MR contribution
       
    - order: (optional) a list with the order in which the keys of the pattern dictionary
      should be evaluated

    - verbose: (optional) toggles verbose printing

    - addHs: (optional) toggles adding of Hs to the molecule for the calculation.
      If positive, explicit and implicit Hs will be used in the calculation.
      If this negative, then only explicit hydrogens will be taken into account in the
      calculation.  This is available to allow matching of results obtained with the old
      backend code.

  """
  if addHs < 0:
    mol = Chem.AddHs(inMol,1)
  elif addHs > 0:
    mol = Chem.AddHs(inMol,0)
  else:
    mol = inMol
    
  if patts is None:
    global _smartsPatterns,_patternOrder
    if _smartsPatterns == {}:
      _patternOrder,_smartsPatterns = _ReadPatts(defaultPatternFileName)
    patts = _smartsPatterns
    order = _patternOrder
  atomContribs = _pyGetAtomContribs(mol,patts,order,verbose=verbose)
  #print 'AC:',atomContribs
  return sum(atomContribs)[0]
pyMolLogP.version="1.0.0"

def pyMolMR(inMol,patts=None,order=None,verbose=0,addHs=1):
  """ Crippen MR value

  Definition From JCICS _39_ 868-873 (1999)

  **Arguments**

    - inMol: a molecule

    - patts: (optional)  if provided this should be a dictionary of 4 tuples:

       1)  smarts text

       2)  SmartsPattern object

       3)  logp contribution

       4)  MR contribution
       
    - order: (optional) a list with the order in which the keys of the pattern dictionary
      should be evaluated

    - verbose: (optional) toggles verbose printing

    - addHs: (optional) toggles adding of Hs to the molecule for the calculation.
      If positive, explicit and implicit Hs will be used in the calculation.
      If this negative, then only explicit hydrogens will be taken into account in the
      calculation.  This is available to allow matching of results obtained with the old
      backend code.

  """
  if addHs < 0:
    mol = Chem.AddHs(inMol,1)
  elif addHs > 0:
    mol = Chem.AddHs(inMol,0)
  else:
    mol = inMol
    
  if patts is None:
    global _smartsPatterns,_patternOrder
    if _smartsPatterns == {}:
      _patternOrder,_smartsPatterns = _ReadPatts(defaultPatternFileName)
    patts = _smartsPatterns
    order = _patternOrder

  atomContribs = _pyGetAtomContribs(mol,patts,order,verbose=verbose)
  return sum(atomContribs)[1]
pyMolMR.version="1.0.0"

MolLogP=lambda *x,**y:rdMolDescriptors.CalcCrippenDescriptors(*x,**y)[0]
MolLogP.version=rdMolDescriptors.__CalcCrippenDescriptors_version__
MolMR=lambda *x,**y:rdMolDescriptors.CalcCrippenDescriptors(*x,**y)[1]
MolMR.version=rdMolDescriptors.__CalcCrippenDescriptors_version__

if __name__=='__main__':
  import sys

  if len(sys.argv):
    ms = []
    verbose=0
    if '-v' in sys.argv:
      verbose=1
      sys.argv.remove('-v')
    for smi in sys.argv[1:]:
      ms.append((smi,Chem.MolFromSmiles(smi)))

    for smi,m in ms:
      print 'Mol: %s'%(smi)
      logp = MolLogP(m,verbose=verbose)
      print '----'
      mr = MolMR(m,verbose=verbose)
      print 'Res:',logp,mr
      newM = Chem.AddHs(m)
      logp = MolLogP(newM,addHs=0)
      mr = MolMR(newM,addHs=0)
      print '\t',logp,mr
      print '-*-*-*-*-*-*-*-*'
