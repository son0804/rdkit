# $Id$
#
#  Copyright (C) 2007-2008 Greg Landrum
#   @@ All Rights Reserved @@
#
import Chem
from Chem import AllChem
from Chem import Lipinski,Descriptors,Crippen
from Dbase.DbConnection import DbConnect
from Dbase import DbModule
import re

#set up the logger:
import RDLogger as logging
logger = logging.logger()
logger.setLevel(logging.INFO)

def ProcessMol(mol,typeConversions,globalProps,nDone,nameProp='_Name',nameCol='compound_id',
               redraw=False,keepHs=False,
               skipProps=False,addComputedProps=False,
               skipSmiles=False,
               uniqNames=None,namesSeen=None):
  if not mol:
    raise ValueError,'no molecule'
  if keepHs:
    Chem.SanitizeMol(mol)
  try:
    nm = mol.GetProp(nameProp)
  except KeyError:
    nm = None
  if not nm:
    nm = 'Mol_%d'%nDone
  if uniqNames and nm in namesSeen:
    logger.error('duplicate compound id (%s) encountered. second instance skipped.'%nm)
    return None
  namesSeen.add(nm)
  row = [nm]
  if not skipProps:
    if addComputedProps:
      nHD=Lipinski.NumHDonors(mol)
      mol.SetProp('DonorCount',str(nHD))
      nHA=Lipinski.NumHAcceptors(mol)
      mol.SetProp('AcceptorCount',str(nHA))
      nRot=Lipinski.NumRotatableBonds(mol)
      mol.SetProp('RotatableBondCount',str(nRot))
      MW=Descriptors.MolWt(mol)
      mol.SetProp('AMW',str(MW))
      logp=Crippen.MolLogP(mol)
      mol.SetProp('MolLogP',str(logp))

    pns = list(mol.GetPropNames())
    pD={}
    for pi,pn in enumerate(pns):
      if pn.lower()==nameCol.lower(): continue
      pv = mol.GetProp(pn).strip()
      if pv.find('>')<0 and pv.find('<')<0:
        colTyp = globalProps.get(pn,2)
        while colTyp>0:
          try:
            tpi = typeConversions[colTyp][1](pv)
          except:
            colTyp-=1
          else:
            break
        globalProps[pn]=colTyp
        pD[pn]=typeConversions[colTyp][1](pv)
      else:
        pD[pn]=pv
  else:
    pD={}
  if redraw:
    AllChem.Compute2DCoords(m)
  if not skipSmiles:
    row.append(Chem.MolToSmiles(mol,True))
  row.append(DbModule.binaryHolder(mol.ToBinary()))
  row.append(pD)
  return row

def ConvertRows(rows,globalProps,defaultVal,skipSmiles):
  for i,row in enumerate(rows):
    newRow = []
    newRow.append(row[0])
    pD=row[-1]
    for pn in globalProps:
      pv = pD.get(pn,defaultVal)
      newRow.append(pv)
    newRow.append(row[1])
    if not skipSmiles:
      newRow.append(row[2])
    rows[i] = newRow

def LoadDb(suppl,dbName,nameProp='_Name',nameCol='compound_id',silent=False,
           redraw=False,errorsTo=None,keepHs=False,defaultVal='N/A',skipProps=False,
           regName='molecules',skipSmiles=False,maxRowsCached=-1,
           uniqNames=False,addComputedProps=False,lazySupplier=False):
  if not lazySupplier:
    nMols = len(suppl)
  else:
    nMols=-1
  if not silent:
    logger.info("Generating molecular database in file %s"%dbName)
    if not lazySupplier:
      logger.info("  Processing %d molecules"%nMols)
  rows = []
  globalProps = {}
  namesSeen = set()
  nDone = 0
  typeConversions={0:('varchar',str),1:('float',float),2:('int',int)}
  for m in suppl:
    nDone +=1
    if not m:
      if errorsTo:
        if hasattr(suppl,'GetItemText'):
          d = suppl.GetItemText(nDone-1)
          errorsTo.write(d)
        else:
          logger.warning('full error file support not complete')
      continue

    row=ProcessMol(m,typeConversions,globalProps,nDone,nameProp=nameProp,
                   nameCol=nameCol,redraw=redraw,
                   keepHs=keepHs,skipProps=skipProps,
                   addComputedProps=addComputedProps,skipSmiles=skipSmiles,
                   uniqNames=uniqNames,namesSeen=namesSeen)
    if row is None: continue
    rows.append(row)
    if not silent and not nDone%100:
      logger.info('  done %d'%nDone)
    if len(rows)==maxRowsCached:
      break

  nameDef='%s varchar not null'%nameCol
  if uniqNames:
    nameDef += ' unique'
  typs = [nameDef]
  pns = []
  for pn,v in globalProps.iteritems():
    addNm = re.sub(r'[\W]','_',pn)
    typs.append('%s %s'%(addNm,typeConversions[v][0]))
    pns.append(pn.lower())

  if not skipSmiles:
    if 'smiles' not in pns:
      typs.append('smiles varchar')
    else:
      typs.append('cansmiles varchar')
  typs.append('molpkl %s'%(DbModule.binaryTypeName))
  conn = DbConnect(dbName)
  curs = conn.GetCursor()
  try:
    curs.execute('drop table %s'%regName)
  except:
    pass
  curs.execute('create table %s (%s)'%(regName,','.join(typs)))
  qs = ','.join([DbModule.placeHolder for x in typs])


  ConvertRows(rows,globalProps,defaultVal,skipSmiles)
  curs.executemany('insert into %s values (%s)'%(regName,qs),rows)
  conn.Commit()

  rows = []
  while 1:
    nDone +=1
    try:
      m = suppl.next()
    except StopIteration:
      break
    if not m:
      if errorsTo:
        if hasattr(suppl,'GetItemText'):
          d = suppl.GetItemText(nDone-1)
          errorsTo.write(d)
        else:
          logger.warning('full error file support not complete')
      continue
    tmpProps={}
    row=ProcessMol(m,typeConversions,globalProps,nDone,nameProp=nameProp,
                   nameCol=nameCol,redraw=redraw,
                   keepHs=keepHs,skipProps=skipProps,
                   addComputedProps=addComputedProps,skipSmiles=skipSmiles,
                   uniqNames=uniqNames,namesSeen=namesSeen)
    if not row: continue
    rows.append(row)
    if not silent and not nDone%100:
      logger.info('  done %d'%nDone)
    if len(rows)==maxRowsCached:
      ConvertRows(rows,globalProps,defaultVal,skipSmiles)
      curs.executemany('insert into %s values (%s)'%(regName,qs),rows)
      conn.Commit()
      rows = []
  if len(rows):
    ConvertRows(rows,globalProps,defaultVal,skipSmiles)
    curs.executemany('insert into %s values (%s)'%(regName,qs),rows)
    conn.Commit()
