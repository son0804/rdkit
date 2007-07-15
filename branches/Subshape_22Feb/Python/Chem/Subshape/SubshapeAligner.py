# $Id$
#
# Copyright (C) 2007 by Greg Landrum 
#  All rights reserved
#
import RDLogger
logger = RDLogger.logger()
import Chem,Geometry
import Numeric
from Numerics import Alignment
from Chem.Subshape import SubshapeObjects

class SubshapeAlignment(object):
  transform=None
  triangleSSD=None
  targetTri=None
  queryTri=None
  alignedConfId=-1
  dirMatch=0.0
  shapeDist=0.0
  
def _getAllTriangles(pts,orderedTraversal=False):
  for i in range(len(pts)):
    if orderedTraversal:
      jStart=i+1
    else:
      jStart=0
    for j in range(jStart,len(pts)):
      if j==i:
        continue
      if orderedTraversal:
        kStart=j+1
      else:
        kStart=0
      for k in range(j+1,len(pts)):
        if k==i or k==j:
          continue
        yield (i,j,k)

class SubshapeDistanceMetric(object):
  TANIMOTO=0
  PROTRUDE=1

# returns the distance between two shapea according to the provided metric
def GetShapeShapeDistance(s1,s2,distMetric):
  if distMetric==SubshapeDistanceMetric.PROTRUDE:
    #print s1.grid.GetOccupancyVect().GetTotalVal(),s2.grid.GetOccupancyVect().GetTotalVal()
    if s1.grid.GetOccupancyVect().GetTotalVal()<s2.grid.GetOccupancyVect().GetTotalVal():
      d = Geometry.ProtrudeDistance(s1.grid,s2.grid)
      #print d
    else:
      d = Geometry.ProtrudeDistance(s2.grid,s1.grid)
  else:
    d = Geometry.TanimotoDistance(s1.grid,s2.grid)
  return d

# clusters a set of alignments and returns the cluster centroid
def ClusterAlignments(mol,alignments,builder,
                      neighborTol=0.1,
                      distMetric=SubshapeAligner.SubshapeDistanceMetric.PROTRUDE,
                      tempConfId=1001):
  from ML.Cluster import Butina
  dists = []
  for i in range(len(alignments)):
    SubshapeAligner.TransformMol(mol,alignments[i].transform,newConfId=tempConfId)
    shapeI=builder.GenerateSubshapeShape(mol,tempConfId,addSkeleton=False)
    for j in range(i):
      SubshapeAligner.TransformMol(mol,alignments[j].transform,newConfId=tempConfId+1)
      shapeJ=builder.GenerateSubshapeShape(mol,tempConfId+1,addSkeleton=False)
      d = SubshapeAligner.GetShapeShapeDistance(shapeI,shapeJ,distMetric)
      dists.append(d)
      mol.RemoveConformer(tempConfId+1)
    mol.RemoveConformer(tempConfId)
  clusts=Butina.ClusterData(dists,len(alignments),neighborTol,isDistData=True)
  res = [alignments[x[0]] for x in clusts]
  return res

def TransformMol(mol,tform,confId=-1,newConfId=100):
  """  Applies the transformation to a molecule and sets it up with
  a single conformer

  """
  newConf = Chem.Conformer()
  newConf.SetId(0)
  refConf = mol.GetConformer(confId)
  for i in range(refConf.GetNumAtoms()):
    pos = list(refConf.GetAtomPosition(i))
    pos.append(1.0)
    newPos = Numeric.matrixmultiply(tform,Numeric.array(pos))
    newConf.SetAtomPosition(i,list(newPos)[:3])
  newConf.SetId(newConfId)
  mol.RemoveConformer(newConfId)
  mol.AddConformer(newConf,assignId=False)
  
class SubshapeAligner(object):
  triangleRMSTol=1.0
  distMetric=SubshapeDistanceMetric.PROTRUDE
  shapeDistTol=0.2
  numFeatThresh=3
  dirThresh=2.6
  edgeTol=6.0
  #coarseGridToleranceMult=1.5
  #medGridToleranceMult=1.25
  coarseGridToleranceMult=1.0
  medGridToleranceMult=1.0
  
  def GetTriangleMatches(self,target,query):
    ssdTol = (self.triangleRMSTol**2)*9
    res = []
    tgtPts = target.skelPts
    queryPts = query.skelPts
    tgtLs = {}
    for i in range(len(tgtPts)):
      for j in range(i+1,len(tgtPts)):
        l2 = (tgtPts[i].location-tgtPts[j].location).LengthSq()
        tgtLs[(i,j)]=l2
    queryLs = {}
    for i in range(len(queryPts)):
      for j in range(i+1,len(queryPts)):
        l2 = (queryPts[i].location-queryPts[j].location).LengthSq()
        queryLs[(i,j)]=l2
    compatEdges={}
    tol2 = self.edgeTol*self.edgeTol
    for tk,tv in tgtLs.iteritems():
      for qk,qv in queryLs.iteritems():
        #print tk,qk,'  -  ',tv,qv,
        if abs(tv-qv)<tol2:
          compatEdges[(tk,qk)]=1
          #print 1
        #else:
        #  print 0
    for tgtTri in _getAllTriangles(tgtPts,orderedTraversal=True):
      tgtLocs=[tgtPts[x].location for x in tgtTri]
      for queryTri in _getAllTriangles(queryPts,orderedTraversal=False):
        if compatEdges.has_key(((tgtTri[0],tgtTri[1]),(queryTri[0],queryTri[1]))) and \
           compatEdges.has_key(((tgtTri[0],tgtTri[2]),(queryTri[0],queryTri[2]))) and \
           compatEdges.has_key(((tgtTri[1],tgtTri[2]),(queryTri[1],queryTri[2]))):
          queryLocs=[queryPts[x].location for x in queryTri]
          ssd,tf = Alignment.GetAlignmentTransform(tgtLocs,queryLocs)
          #print ssd,ssdTol
          if ssd<=ssdTol:
            alg = SubshapeAlignment()
            alg.transform=tf
            alg.triangleSSD=ssd
            alg.targetTri=tgtTri
            alg.queryTri=queryTri
            res.append(alg)
            alg._seqNo=len(res)
              
    return res

  def PruneMatchesUsingFeatures(self,target,query,alignments,pruneStats=None):
    i = 0
    tgtPts = target.skelPts
    queryPts = query.skelPts
    while i<len(alignments):
      nMatched=0
      alg = alignments[i]
      for j in range(3):
        tgtFeats = tgtPts[alg.targetTri[j]].molFeatures
        qFeats = queryPts[alg.queryTri[j]].molFeatures
        for k,kFeat in enumerate(tgtFeats):
          if kFeat in qFeats:
            nMatched+=1
            break
      if nMatched<self.numFeatThresh:
        if pruneStats is not None:
          pruneStats['features']=pruneStats.get('features',0)+1
        del alignments[i]
      else:
        i+=1

  def PruneMatchesUsingDirection(self,target,query,alignments,pruneStats=None):
    i = 0
    tgtPts = target.skelPts
    queryPts = query.skelPts
    while i<len(alignments):
      nMatched=0
      alg = alignments[i]
      dot = 0.0
      sumV=0.0
      for j in range(3):
        tgtPt = tgtPts[alg.targetTri[j]]
        queryPt = queryPts[alg.queryTri[j]]
        #m1,m2,m3=tgtPt.shapeMoments
        #tgtR = m1/(m2+m3)
        #m1,m2,m3=queryPt.shapeMoments
        #queryR = m1/(m2+m3)

        qv = queryPt.shapeDirs[0]
        tv = tgtPt.shapeDirs[0]
        rotV =[0.0]*3
        rotV[0] = alg.transform[0,0]*qv[0]+alg.transform[0,1]*qv[1]+alg.transform[0,2]*qv[2]
        rotV[1] = alg.transform[1,0]*qv[0]+alg.transform[1,1]*qv[1]+alg.transform[1,2]*qv[2]
        rotV[2] = alg.transform[2,0]*qv[0]+alg.transform[2,1]*qv[1]+alg.transform[2,2]*qv[2]
        dot += abs(rotV[0]*tv[0]+rotV[1]*tv[1]+rotV[2]*tv[2])
        if dot>=self.dirThresh:
          # already above the threshold, no need to continue
          break
      if dot<self.dirThresh:
        if pruneStats is not None:
          pruneStats['direction']=pruneStats.get('direction',0)+1
        del alignments[i]
      else:
        alignments[i].dirMatch=dot
        i+=1

  def _addCoarseAndMediumGrids(self,mol,tgt,confId,builder):
    oSpace=builder.gridSpacing
    if mol:
      builder.gridSpacing = oSpace*1.5
      tgt.medGrid = builder.GenerateSubshapeShape(mol,confId,addSkeleton=False)
      builder.gridSpacing = oSpace*2
      tgt.coarseGrid = builder.GenerateSubshapeShape(mol,confId,addSkeleton=False)
      builder.gridSpacing = oSpace
    else:
      tgt.medGrid = builder.SampleSubshape(tgt,oSpace*1.5)
      tgt.coarseGrid = builder.SampleSubshape(tgt,oSpace*2.0)

  def PruneMatchesUsingShape(self,targetMol,target,queryMol,query,builder,
                             alignments,tgtConf=-1,queryConf=-1,
                             pruneStats=None):
    if not hasattr(target,'medGrid'):
      self._addCoarseAndMediumGrids(targetMol,target,tgtConf,builder)

    logger.info("Shape-based Pruning")
    i=0
    nOrig = len(alignments)
    nDone=0
    while i < len(alignments):
      removeIt=False
      tConfId=100+i
      alg = alignments[i]
      TransformMol(queryMol,alg.transform,confId=queryConf,newConfId=tConfId)
      alg.alignedConfId=tConfId
      oSpace=builder.gridSpacing
      builder.gridSpacing=oSpace*2
      coarseGrid=builder.GenerateSubshapeShape(queryMol,tConfId,addSkeleton=False)
      d = GetShapeShapeDistance(coarseGrid,target.coarseGrid,self.distMetric)
      if d>self.shapeDistTol*self.coarseGridToleranceMult:
        removeIt=True
        if pruneStats is not None:
          pruneStats['coarseGrid']=pruneStats.get('coarseGrid',0)+1
      else:
        builder.gridSpacing=oSpace*1.5
        medGrid=builder.GenerateSubshapeShape(queryMol,tConfId,addSkeleton=False)
        d = GetShapeShapeDistance(medGrid,target.medGrid,self.distMetric)
        #print '     ',d
        if d>self.shapeDistTol*self.medGridToleranceMult:
          removeIt=True
          if pruneStats is not None:
            pruneStats['medGrid']=pruneStats.get('medGrid',0)+1
        else:
          builder.gridSpacing=oSpace
          fineGrid=builder.GenerateSubshapeShape(queryMol,tConfId,addSkeleton=False)
          d = GetShapeShapeDistance(fineGrid,target,self.distMetric)
          #print '        ',d
          if d>self.shapeDistTol:
            removeIt=True
            if pruneStats is not None:
              pruneStats['fineGrid']=pruneStats.get('fineGrid',0)+1
          shapeDist=d
      nDone+=1
      if not nDone%100:
        nLeft = len(alignments)
        logger.info('  processed %d of %d. %d alignments remain'%((nDone,
                                                                   nOrig,
                                                                   nLeft)))
      builder.gridSpacing=oSpace
      if removeIt:
        del alignments[i]
      else:
        alignments[i].shapeDist=shapeDist
        i+=1

  def GetSubshapeAlignments(self,targetMol,target,queryMol,query,builder,
                            tgtConf=-1,queryConf=-1,pruneStats=None):
    import time
    if pruneStats is None:
      pruneStats={}
    logger.info("Generating triangle matches")
    t1=time.time()
    res = self.GetTriangleMatches(target,query)
    t2=time.time()
    logger.info("Got %d possible alignments in %.1f seconds"%(len(res),t2-t1))
    pruneStats['gtm_time']=t2-t1
    if builder.featFactory:
      t1 = time.time()
      self.PruneMatchesUsingFeatures(target,query,res,pruneStats=pruneStats)
      t2 = time.time()
      pruneStats['feats_time']=t2-t1
    logger.info("Doing direction pruning")
    t1 = time.time()
    self.PruneMatchesUsingDirection(target,query,res,pruneStats=pruneStats)
    t2 = time.time()
    pruneStats['direction_time']=t2-t1
    logger.info("%d possible alignments remain. (%.1f seconds required)"%(len(res),t2-t1))
    t1 = time.time()
    self.PruneMatchesUsingShape(targetMol,target,queryMol,query,builder,res,
                                tgtConf=tgtConf,queryConf=queryConf,
                                pruneStats=pruneStats)
    t2 = time.time()
    pruneStats['shape_time']=t2-t1
    return res


if __name__=='__main__':
  import cPickle
  tgtMol,tgtShape = cPickle.load(file('target.pkl','rb'))
  queryMol,queryShape = cPickle.load(file('query.pkl','rb'))
  builder = cPickle.load(file('builder.pkl','rb'))
  aligner = SubshapeAligner()
  algs = aligner.GetSubshapeAlignments(tgtMol,tgtShape,queryMol,queryShape,builder)
  print len(algs)

  from Chem.PyMol import MolViewer
  v = MolViewer()
  v.ShowMol(tgtMol,name='Target',showOnly=True)
  v.ShowMol(queryMol,name='Query',showOnly=False)
  SubshapeObjects.DisplaySubshape(v,tgtShape,'target_shape',color=(.8,.2,.2))
  SubshapeObjects.DisplaySubshape(v,queryShape,'query_shape',color=(.2,.2,.8))
  for i,alg in enumerate(algs):
    confId = alg.alignedConfId
    v.ShowMol(queryMol,confId=confId,name='align-%d'%i,showOnly=False)
      
