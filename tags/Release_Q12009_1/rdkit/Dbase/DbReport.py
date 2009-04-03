# $Id$
#
#  Copyright (C) 2003-2006  Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
try:
  from reportlab import platypus
except ImportError:
  import sys
  sys.stderr.write('ReportLab module could not be imported.  Db->PDF functionality not available')
  GetReportlabTable = None
  QuickReport = None
else:
  from rdkit import Chem
  try:
    from pyRDkit.utils import chemdraw
  except ImportError:
    hasCDX=0
  else:
    hasCDX=1
  from rdkit.utils import cactvs  
  from rdkit.Chem import rdDepictor
  from rdkit.Chem.Draw import DrawUtils
  from rdkit.Dbase.DbConnection import DbConnect
  from rdkit.Dbase import DbInfo
  from rdkit.Reports.PDFReport import PDFReport,ReportUtils
  import os,tempfile,sys
  
  def GetReportlabTable(self,*args,**kwargs):
    """ this becomes a method of DbConnect  """
    dbRes = self.GetData(*args,**kwargs)
    rawD = [dbRes.GetColumnNames()]
    colTypes = dbRes.GetColumnTypes()
    binCols = []
    for i in range(len(colTypes)):
      if colTypes[i] in DbInfo.sqlBinTypes or colTypes[i]=='binary':
        binCols.append(i)
    nRows = 0
    for entry in dbRes:
      nRows += 1
      for col in binCols:
        entry = list(entry)
        entry[col] = 'N/A'
      rawD.append(entry)
      #if nRows >10: break
   
    res = platypus.Table(rawD)
    return res

  from reportlab.lib.units import inch
  class CDXImageTransformer(object):
    def __init__(self,smiCol,width=1,verbose=1,tempHandler=None):
      self.smiCol = smiCol
      if tempHandler is None:
        tempHandler = ReportUtils.TempFileHandler()
      self.tempHandler = tempHandler
      self.width = width*inch
      self.verbose=verbose
    def __call__(self,arg):
      res = list(arg)
      if self.verbose:
        print 'Render:',res[0]
      if hasCDX:
        smi = res[self.smiCol]
        tmpName = self.tempHandler.get('.jpg')
        try:
          img = chemdraw.SmilesToPilImage(smi)
          w,h = img.size
          aspect = float(h)/w
          img.save(tmpName)
          img = platypus.Image(tmpName)
          img.drawWidth = self.width
          img.drawHeight = aspect*self.width
          res[self.smiCol] = img
        except:
          import traceback
          traceback.print_exc()
          res[self.smiCol] = 'Failed'
      return res

  class CactvsImageTransformer(object):
    def __init__(self,smiCol,width=1.,verbose=1,tempHandler=None):
      self.smiCol = smiCol
      if tempHandler is None:
        tempHandler = ReportUtils.TempFileHandler()
      self.tempHandler = tempHandler
      self.width = width*inch
      self.verbose=verbose
    def __call__(self,arg):
      res = list(arg)
      if self.verbose:
        sys.stderr.write('Render(%d): %s\n'%(self.smiCol,str(res[0])))
      smi = res[self.smiCol]
      tmpName = self.tempHandler.get('.gif')
      aspect = 1
      width = 300
      height = aspect*width
      ok = cactvs.SmilesToGif(smi,tmpName,(width,height))
      if ok:
        try:
          img = platypus.Image(tmpName)
          img.drawWidth = self.width
          img.drawHeight = aspect*self.width
        except:
          ok = 0
      if ok:
        res[self.smiCol] = img
      else:
        # FIX: maybe include smiles here in a Paragraph?
        res[self.smiCol] = 'Failed'
      return res
        

  from rdkit.sping.ReportLab.pidReportLab import RLCanvas as Canvas
  from rdkit.Chem.Draw.MolDrawing import MolDrawing
  class ReportLabImageTransformer(object):
    def __init__(self,smiCol,width=1.,verbose=1,tempHandler=None):
      self.smiCol = smiCol
      self.width = width*inch
      self.verbose=verbose
    def __call__(self,arg):
      res = list(arg)
      if self.verbose:
        sys.stderr.write('Render(%d): %s\n'%(self.smiCol,str(res[0])))
      smi = res[self.smiCol]
      aspect = 1
      width = self.width
      height = aspect*width
      try:
        mol = Chem.MolFromSmiles(smi)
        Chem.Kekulize(mol)
        canv = Canvas((width,height))
        drawing = MolDrawing()
        drawing.atomLabelMinFontSize=3
        drawing.minLabelPadding=(.5,.5)
        drawing.bondLineWidth=0.5
        if not mol.GetNumConformers():
          rdDepictor.Compute2DCoords(mol)
        drawing.AddMol(mol,canvas=canv)
        ok = True
      except:
        if self.verbose:
          import traceback
          traceback.print_exc()
        ok = False

      if ok:
        res[self.smiCol] = canv.drawing
      else:
        # FIX: maybe include smiles here in a Paragraph?
        res[self.smiCol] = 'Failed'
      return res
        

  class RDImageTransformer(object):
    def __init__(self,smiCol,width=1.,verbose=1,tempHandler=None):
      self.smiCol = smiCol
      if tempHandler is None:
        tempHandler = ReportUtils.TempFileHandler()
      self.tempHandler = tempHandler
      self.width = width*inch
      self.verbose=verbose
    def __call__(self,arg):
      res = list(arg)
      if self.verbose:
        sys.stderr.write('Render(%d): %s\n'%(self.smiCol,str(res[0])))
      smi = res[self.smiCol]
      tmpName = self.tempHandler.get('.jpg')
      aspect = 1
      width = 300
      height = aspect*width
      ok = DrawUtils.SmilesToJpeg(smi,tmpName,size=(width,height))
      if ok:
        try:
          img = platypus.Image(tmpName)
          img.drawWidth = self.width
          img.drawHeight = aspect*self.width
        except:
          ok = 0
      if ok:
        res[self.smiCol] = img
      else:
        # FIX: maybe include smiles here in a Paragraph?
        res[self.smiCol] = 'Failed'
      return res
        
  def QuickReport(conn,fileName,*args,**kwargs):
    from reportlab.lib import colors
    from reportlab.lib.styles import getSampleStyleSheet
    from reportlab.lib.units import inch

    styles = getSampleStyleSheet()
    title = 'Db Report'
    if kwargs.has_key('title'):
      title = kwargs['title']
      del kwargs['title']

    names = [x.upper() for x in conn.GetColumnNames()]
    try:
      smiCol = names.index('SMILES')
    except ValueError:
      try:
        smiCol = names.index('SMI')
      except ValueError:
        smiCol = -1
    if smiCol >-1:
      if hasCDX:
        tform = CDXImageTransformer(smiCol)
      elif 1:
        tform = ReportLabImageTransformer(smiCol)
      else:
        tform = CactvsImageTransformer(smiCol)

    else:
      tform = None
    kwargs['transform'] = tform
    tbl = conn.GetReportlabTable(*args,**kwargs)
    tbl.setStyle(platypus.TableStyle([('GRID',(0,0),(-1,-1),1,colors.black),
                                      ('FONT',(0,0),(-1,-1),'Times-Roman',8),
                                      ]))

    
    if smiCol >-1 and tform:
      tbl._argW[smiCol] = tform.width*1.2
    elements = [tbl]
    reportTemplate = PDFReport()
    reportTemplate.pageHeader = title

    doc = platypus.SimpleDocTemplate(fileName)
    doc.build(elements,onFirstPage=reportTemplate.onPage,
              onLaterPages=reportTemplate.onPage)
    
  DbConnect.GetReportlabTable = GetReportlabTable

if __name__=='__main__':
  import sys
  dbName = sys.argv[1]
  tblName = sys.argv[2]
  fName = 'report.pdf'
  conn = DbConnect(dbName,tblName)
  QuickReport(conn,fName,where="where mol_id in ('1','100','104','107')")
  
