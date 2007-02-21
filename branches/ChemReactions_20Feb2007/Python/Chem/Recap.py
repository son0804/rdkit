#  $Id$
#
#  Copyright (c) 2007, Novartis Institutes for BioMedical Research Inc.
#  All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: 
#
#     * Redistributions of source code must retain the above copyright 
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following 
#       disclaimer in the documentation and/or other materials provided 
#       with the distribution.
#     * Neither the name of Novartis Institutues for BioMedical Research Inc. 
#       nor the names of its contributors may be used to endorse or promote 
#       products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
""" Implementation of the RECAP algorithm from Lewell et al. JCICS *38* 511-522 (1998)
"""
import Chem
from Chem import rdChemReactions as Reactions

reactionDefs = (
  "[C:1](=!@[O:2])!@[N:3]>>[X][C:1]=[O:2].[X][N:3]", # amide

  "[C:1](=!@[O:2])!@[O:3]>>[X][C:1]=[O:2].[O:3][X]", # ester

  "[N:1;D2,D3]!@C(!@=O)!@[N:2,D2,D3]>>[X][N:1].[N:2][X]", # urea

  "[N;!D1](-[*:1])-!@[*:2]>>[X][*:1].[*:2][X]", # amines
  #"[N;!D1](!@[*:1])!@[*:2]>>[X][*:1].[*:2][X]", # amines

  "[#6:1]-!@O-!@[#6:2]>>[#6:1][X].[X][#6:2]", # ether

  "[C:1]=!@[C:2]>>[C:1][X].[X][C:2]", # olefin

  "[n:1]-!@[C:2]>>[n:1][X].[C:2][X]", # aromatic nitrogen - aliphatic carbon

  "O=C-@[N:1]-!@[C:2]>>[N:1][X].[C:2][X]", # lactam nitrogen - aliphatic carbon

  "[c:1]-!@[c:2]>>[c:1][X].[X][c:2]", # aromatic carbon - aromatic carbon

  "[n:1]-!@[c:2]>>[n:1][X].[X][c:2]", # aromatic nitrogen - aromatic carbon *NOTE* this is not part of the standard recap set.

  "[N:1;D2,D3]-!@[S:2](=[O:3])=[O:4]>>[N:1][X].[X][S:2](=[O:3])=[O:4]", # sulphonamide
  )

reactions = tuple([Reactions.ReactionFromSmarts(x) for x in reactionDefs])


class RecapHierarchyNode(object):
  mol=None
  children=None
  parents=None
  smiles = None
  def __init__(self,mol):
    self.mol=mol
    self.children = {}
    self.parents = {}

  def _gacRecurse(self,res):
    for smi,child in self.children.iteritems():
      res[smi] = child
      child._gacRecurse(res)
    
  def GetAllChildren(self):
    res = {}
    for smi,child in self.children.iteritems():
      res[smi] = child
      child._gacRecurse(res)
    return res

  def remove(self):
    for parent in self.parents.values():
      #parent = parent()
      if parent:
	ks = parent.children.keys()[:]
	for k in ks:
	  if parent.children[k] is self:
	    del parent.children[k]

  def getUltimateParents(self):
    if not self.parents:
      res = [self]
    else:
      res = []
      for p in self.parents.values():
        for uP in p.getUltimateParents():
          if uP not in res:
            res.append(uP)
    return res
  
  def __del__(self):
    self.remove()
    self.children={}
    self.parent={}
    self.mol=None


def RecapDecompose(mol,allNodes=None):
  mSmi = Chem.MolToSmiles(mol,1)

  if allNodes is None:
    allNodes={}
  if allNodes.has_key(mSmi):
    return allNodes[mSmi]

  res = RecapHierarchyNode(mol)
  res.smiles =mSmi
  activePool={mSmi:res}
  allNodes[mSmi]=res
  for rxnIdx,reaction in enumerate(reactions):
    localRes = {}
    #print '>',rxnIdx,len(activePool.keys())
    missed = {}
    while activePool:
      nSmi = activePool.keys()[0]
      node = activePool.pop(nSmi)
      ps = reaction.RunReactants((node.mol,))
      if not ps:
        #print '  !',nSmi
	localRes[nSmi]=node
      else:
        rxnApplied=False
        #print '  .',nSmi
	for prodSeq in ps:
	  seqOk=True
	  # we want to disqualify small fragments, so sort the product sequence by size
	  # and then look for "forbidden" fragments
	  prodSeq = [(prod.GetNumAtoms(onlyHeavy=True),prod) for prod in prodSeq]
	  prodSeq.sort()
	  for nats,prod in prodSeq:
	    pSmi = Chem.MolToSmiles(prod,1)
	    if pSmi.replace('[Du]','') in ('','C','CC','CCC'):
	      seqOk=False
	      break
	    prod.pSmi = pSmi
	  if seqOk:
            rxnApplied=True
  	    for nats,prod in prodSeq:
	      pSmi = prod.pSmi
	      pNode = allNodes.get(pSmi,RecapHierarchyNode(prod))
              pNode.smiles=pSmi
              pNode.parents[nSmi]=node
	      node.children[pSmi]=pNode
	      activePool[pSmi] = pNode
	      allNodes[pSmi]=pNode
        if not rxnApplied:
          localRes[nSmi]=node
    activePool=localRes
  return res

      
      
