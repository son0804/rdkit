# $Id: DbUtils.py 5010 2006-02-22 15:23:39Z glandrum $
#
#  Copyright (C) 2000-2006  greg Landrum and Rational Discovery LLC
#
#   @@ All Rights Reserved  @@
#
""" a set of functions for interacting with databases

 When possible, it's probably preferable to use a _DbConnection.DbConnect_ object

"""
import RDConfig
try:
  from Excel import ExcelWrapper
except:
  haveExcel = 0
else:
  haveExcel = 1

from Dbase.DbResultSet import DbResultSet,RandomAccessDbResultSet
def _take(fromL,what):
  return map(lambda x,y=fromL:y[x],what)

from Dbase import DbModule
import sys,types,string
from Dbase import DbInfo

def GetColumns(dBase,table,fieldString,user='sysdba',password='masterkey',
               join=''):
  """ gets a set of data from a table

    **Arguments**

     - dBase: database name

     - table: table name
     
     - fieldString: a string with the names of the fields to be extracted,
        this should be a comma delimited list

     - user and  password:

     - join: a join clause (omit the verb 'join')
       

    **Returns**

     - a list of the data

  """
  cn = DbModule.connect(dBase,user,password)
  c = cn.cursor()
  cmd = 'select %s from %s'%(fieldString,table)
  if join:
    if join.strip().find('join') != 0:
      join = 'join %s'%(join)
    cmd +=' ' + join
  c.execute(cmd)
  return c.fetchall()

def GetData(dBase,table,fieldString='*',whereString='',user='sysdba',password='masterkey',
            removeDups=-1,join='',forceList=0,transform=None,randomAccess=1,extras=None):
  """ a more flexible method to get a set of data from a table

    **Arguments**

     - fields: a string with the names of the fields to be extracted,
          this should be a comma delimited list

     - where: the SQL where clause to be used with the DB query

     - removeDups indicates the column which should be used to screen
        out duplicates.  Only the first appearance of a duplicate will
        be left in the dataset.

    **Returns**

      - a list of the data


    **Notes**

      - EFF: this isn't particularly efficient

  """
  cn = DbModule.connect(dBase,user,password)
  c = cn.cursor()
  cmd = 'select %s from %s'%(fieldString,table)
  if join:
    if join.strip().find('join') != 0:
      join = 'join %s'%(join)
    cmd += ' ' + join
  if whereString:
    if whereString.strip().find('where')!=0:
      whereString = 'where %s'%(whereString)
    cmd += ' ' + whereString

  if forceList:
    try:
      if not extras:
        c.execute(cmd)
      else:
        c.execute(cmd,extras)
    except:
      sys.stderr.write('the command "%s" generated errors:\n'%(cmd))
      import traceback
      traceback.print_exc()
      return None
    if transform is not None:
      raise ValueError,'forceList and transform arguments are not compatible'
    if not randomAccess:
      raise ValueError,'when forceList is set, randomAccess must also be used'
    data = c.fetchall()
    if removeDups>0:
      seen = []
      for entry in data[:]:
        if entry[removeDups] in seen:
          data.remove(entry)
        else:
          seen.append(entry[removeDups])
  else:
    if randomAccess:
      klass = RandomAccessDbResultSet 
    else:
      klass = DbResultSet 

    data = klass(c,cn,cmd,removeDups=removeDups,transform=transform,extras=extras)

  return data


def DatabaseToExcel(dBase,table,fields='*',join='',where='',wrapper=None,
                    user='sysdba',password='masterkey',lowMemory=False):
  """ Pulls the contents of a database and puts them in an Excel worksheet

    **Arguments**
      - dBase: the name of the DB file to be used

      - table: the name of the table to query

      - fields: the fields to select with the SQL query

      - join: the join clause of the SQL query
        (e.g. 'join foo on foo.bar=base.bar')

      - where: the where clause of the SQL query
        (e.g. 'where foo = 2' or 'where bar > 17.6')

      - wrapper: an _Excel.ExcelWrapper.ExcelWrapper_ to be used
        in interacting with Excel
        
      - user: the username for DB access

      - password: the password to be used for DB access

  """
  if not haveExcel:
    return
  if wrapper is None:
    wrapper = ExcelWrapper.ExcelWrapper()
    wrapper.Visible = 1
  if len(where) and where.strip().find('where') != 0:
    where = 'where %s'%(where)
  if len(join) and join.strip().find('join') != 0:
    join = 'join %s'%(join)
    
  sqlCommand = 'select %s from %s %s %s'%(fields,table,join,where)
  cn = DbModule.connect(dBase,user,password)
  c = cn.cursor()
  try:
    c.execute(sqlCommand)
  except:
    print 'problems executing SQL statement %s'%(repr(sqlCommand))
    import sys,traceback
    traceback.print_exc()
    return
  headers = []
  colsToTake = []
  strCols = []
  # the description field of the cursor carries around info about the columns
  #  of the table
  for i in range(len(c.description)):
    item = c.description[i]
    if item[1] not in DbInfo.sqlBinTypes:
      colsToTake.append(i)
      headers.append(item[0])
      if item[1] in DbInfo.sqlTextTypes:
        strCols.append(len(colsToTake)-1)
  wrapper.Workbooks.Add()

  r = wrapper.GetRange(1,1,1,len(headers))
  # add the headers
  r.Value = headers
  # and make them bold
  r.Font.Bold = 1

  # now just insert the data... easy as pie
  results = c.fetchall()
  row = 2
  fullData = []
  for res in results:
    vs = [res[x] for x in colsToTake]
    for col in strCols:
      vs[col] = "'%s"%vs[col]
    if not lowMemory:
      fullData.append(vs)
    else:
      wrapper[row,1:len(headers)]=vs
      row+=1
  if not lowMemory:
    wrapper[row:row+len(fullData),1:len(headers)] = fullData



def DatabaseToText(dBase,table,fields='*',join='',where='',wrapper=None,
                  user='sysdba',password='masterkey',delim=','):
  """ Pulls the contents of a database and makes a deliminted text file from them

    **Arguments**
      - dBase: the name of the DB file to be used

      - table: the name of the table to query

      - fields: the fields to select with the SQL query

      - join: the join clause of the SQL query
        (e.g. 'join foo on foo.bar=base.bar')

      - where: the where clause of the SQL query
        (e.g. 'where foo = 2' or 'where bar > 17.6')

      - wrapper: an _Excel.ExcelWrapper.ExcelWrapper_ to be used
        in interacting with Excel
        
      - user: the username for DB access

      - password: the password to be used for DB access

    **Returns**

      - the CSV data (as text) 

  """
  if len(where) and where.strip().find('where')==-1:
    where = 'where %s'%(where)
  if len(join) and join.strip().find('join') == -1:
    join = 'join %s'%(join)
  sqlCommand = 'select %s from %s %s %s'%(fields,table,join,where)
  cn = DbModule.connect(dBase,user,password)
  c = cn.cursor()
  c.execute(sqlCommand)
  headers = []
  colsToTake = []
  # the description field of the cursor carries around info about the columns
  #  of the table
  for i in range(len(c.description)):
    item = c.description[i]
    if item[1] not in DbInfo.sqlBinTypes:
      colsToTake.append(i)
      headers.append(item[0])

  lines = []
  lines.append(delim.join(headers))

  # grab the data
  results = c.fetchall()
  for res in results:
    d = _take(res,colsToTake)
    lines.append(delim.join(map(str,d)))

  return '\n'.join(lines)


def TypeFinder(data,nRows,nCols,nullMarker=None):
  """ 

    finds the types of the columns in _data_

    if nullMarker is not None, elements of the data table which are
      equal to nullMarker will not count towards setting the type of
      their columns.

  """
  priorities={types.FloatType:3,types.IntType:2,types.StringType:1,-1:-1}
  res = [None]*nCols
  for col in xrange(nCols):
    typeHere = [-1,1]
    for row in xrange(nRows):
      d = data[row][col]
      if d is not None:
        locType = type(d)
        if locType != types.FloatType and locType != types.IntType:
          locType = types.StringType
          try:
            d = str(d)
          except UnicodeError,msg:
            print 'cannot convert text from row %d col %d to a string'%(row+2,col)
            print '\t>%s'%(repr(d))
            raise UnicodeError,msg
        else:
          typeHere[1] = max(typeHere[1],len(str(d)))
        if locType == types.StringType:
          if nullMarker is None or d != nullMarker:
            l = max(len(d),typeHere[1])
            typeHere = [types.StringType,l]
        else:
          try:
            fD = float(int(d))
          except OverflowError:
            locType = types.FloatType
          else:
            if fD == d:
              locType = types.IntType
          if typeHere[0]!=types.StringType and \
             priorities[locType] > priorities[typeHere[0]]:
            typeHere[0] = locType
    res[col] = typeHere
  return res

def DetermineColTypes(wrapper,nullMarker=None):
  """This is kind of crude hack to automagically determine the types
     of columns in the active Excel sheet

   **Arguments**

     - wrapper: the _ExcelWrapper_ to be used in interacting with Excel

     - nullMarker: (optional) if this is not None, elements of the
       data table which are equal to nullMarker will not count towards
       setting the type of their columns.

   **Returns**

     - a list of the types of each column

   **Note**

     - we make the assumption that there are only three possible types: int,
       float and string.

  """
  nCols = wrapper.FindLastCol(1,1)
  nRows = wrapper.FindLastRow(1,1)
  dList = wrapper[2:nRows,1:nCols]
  res = TypeFinder(dList,nRows-1,nCols,nullMarker=nullMarker)
  return res


def _AdjustColHeadings(colHeadings,maxColLabelLen):
  """ *For Internal Use*

    removes illegal characters from column headings
    and truncates those which are too long.
    
  """
  for i in xrange(len(colHeadings)):
    # replace unallowed characters and strip extra white space
    colHeadings[i] = string.strip(colHeadings[i])
    colHeadings[i] = string.replace(colHeadings[i],' ','_')
    colHeadings[i] = string.replace(colHeadings[i],'-','_')

    if len(colHeadings[i]) > maxColLabelLen:
      # interbase (at least) has a limit on the maximum length of a column name
      newHead = string.replace(colHeadings[i],'_','')
      newHead = newHead[:maxColLabelLen]
      print '\tHeading %s too long, changed to %s'%(colHeadings[i],newHead)
      colHeadings[i] = newHead
  return colHeadings

def GetTypeStrings(colHeadings,colTypes,keyCol=None):
  """  returns a list of SQL type strings
  """
  typeStrs=[]
  for i in xrange(len(colTypes)):
    type = colTypes[i]
    if type[0] == types.FloatType:
      typeStrs.append('%s double precision'%colHeadings[i])
    elif type[0] == types.IntType:
      typeStrs.append('%s integer'%colHeadings[i])
    else:
      typeStrs.append('%s varchar(%d)'%(colHeadings[i],type[1]))
    if colHeadings[i] == keyCol:
      typeStrs[-1] = '%s not null primary key'%(typeStrs[-1])
  return typeStrs 

def _insertBlock(conn,sqlStr,block,silent=False):
  try:
    conn.cursor().executemany(sqlStr,block)
  except:
    res = 0
    conn.commit()
    for row in block:
      try:
        conn.cursor().execute(sqlStr,tuple(row))
        res += 1
      except:
        if not silent:
          import traceback
          traceback.print_exc()
          print 'insert failed:',sqlStr
          print '\t',repr(row)
      else:
        conn.commit()
  else:
    res = len(block)
  return res

def _AddDataToDb(dBase,table,user,password,colDefs,colTypes,data,
                 nullMarker=None,blockSize=100):
  """ *For Internal Use*

    (drops and) creates a table and then inserts the values

  """
  cn = DbModule.connect(dBase,user,password)
  c = cn.cursor()
  try:
    c.execute('drop table %s'%(table))
  except:
    print 'cannot drop table %s'%(table)
  try:
    sqlStr = 'create table %s (%s)'%(table,colDefs)
    c.execute(sqlStr)
  except:
    print 'create table failed: ', sqlStr
    print 'here is the exception:'
    import traceback
    traceback.print_exc()
    return
  cn.commit()
  c = None
  
  block = []
  entryTxt = [DbModule.placeHolder]*len(data[0])
  dStr = ','.join(entryTxt)
  sqlStr = 'insert into %s values (%s)'%(table,dStr)
  nDone = 0
  for row in data:
    entries = [None]*len(row)
    for col in xrange(len(row)):
      if row[col] is not None and \
         (nullMarker is None or row[col] != nullMarker):
        if colTypes[col][0] == types.FloatType:
          entries[col] = float(row[col])
        elif colTypes[col][0] == types.IntType:
          entries[col] = int(row[col])
        else:
          entries[col] = str(row[col])
      else:
        entries[col] = None
    block.append(tuple(entries))
    if len(block)>=blockSize:
      nDone += _insertBlock(cn,sqlStr,block)
      if not hasattr(cn,'autocommit') or not cn.autocommit:
        cn.commit()
      block = []
  if len(block):
    nDone += _insertBlock(cn,sqlStr,block)
  if not hasattr(cn,'autocommit') or not cn.autocommit:
    cn.commit()
    


def ExcelToDatabase(dBase,table,wrapper=None,user='sysdba',password='masterkey',
                    maxColLabelLen=31,keyCol=None,nullMarker=None,force=0):
  """convert the active excel worksheet into a database.

   this isn't as smooth or slick as the conversion the other way... sad.
   
    **Arguments**

      - dBase: the name of the DB to use

      - table: the name of the table to create/overwrite

      - wrapper: the _ExcelWrapper_ to use

      - user: the user name to use in connecting to the DB

      - password: the password to use in connecting to the DB

      - maxColLabelLen: the maximum length a column label should be
        allowed to have (truncation otherwise)

      - keyCol: the column to be used as an index for the db
      
    **Notes**

      - if _table_ already exists, it is destroyed before we write
        the new data

  """
  if not haveExcel:
    return
  table.replace('-','_')
  table.replace(' ','_')
  if not force:
    tblNames = [x.strip() for x in DbInfo.GetTableNames(dBase)]
    tmp = table.upper()
    if tmp in tblNames:
      resp = raw_input('Table %s already exists, overwrite it? '%(table))
      if not resp or resp[0] not in ['Y','y']:
        print 'cancelled'
        return

  if wrapper is None:
    wrapper = ExcelWrapper.ExcelWrapper()
  colHeadings = wrapper.GetHeadings()
  _AdjustColHeadings(colHeadings,maxColLabelLen)
  nCols = len(colHeadings)
  nRows = wrapper.FindLastRow(1,1)
  data = wrapper[2:nRows,1:nCols]
  # determine the types of each column
  colTypes = TypeFinder(data,nRows-1,nCols,nullMarker=nullMarker)
  typeStrs = GetTypeStrings(colHeadings,colTypes,keyCol=keyCol)
  colDefs=','.join(typeStrs)

  _AddDataToDb(dBase,table,user,password,colDefs,colTypes,data,nullMarker=nullMarker)
 
def TextFileToDatabase(dBase,table,inF,delim=',',
                       user='sysdba',password='masterkey',
                       maxColLabelLen=31,keyCol=None,nullMarker=None):
  """loads the contents of the text file into a database.

    **Arguments**

      - dBase: the name of the DB to use

      - table: the name of the table to create/overwrite

      - inF: the file like object from which the data should
        be pulled (must support readline())

      - delim: the delimiter used to separate fields  

      - user: the user name to use in connecting to the DB

      - password: the password to use in connecting to the DB
      
      - maxColLabelLen: the maximum length a column label should be
        allowed to have (truncation otherwise)

      - keyCol: the column to be used as an index for the db

    **Notes**

      - if _table_ already exists, it is destroyed before we write
        the new data

      - we assume that the first row of the file contains the column names

  """
  table.replace('-','_')
  table.replace(' ','_')

  colHeadings = inF.readline().split(delim)
  _AdjustColHeadings(colHeadings,maxColLabelLen)
  nCols = len(colHeadings)
  data = []
  inL = inF.readline()
  while inL:
    inL = inL.replace('\r','')
    inL = inL.replace('\n','')
    splitL = inL.split(delim)
    if len(splitL)!=nCols:
      print '>>>',repr(inL)
      assert len(splitL)==nCols,'unequal length'
    tmpVect = []
    for entry in splitL:
      try:
        val = int(entry)
      except:
        try:
          val = float(entry)
        except:
          val = entry
      tmpVect.append(val)
    data.append(tmpVect)   
    inL = inF.readline()
  nRows = len(data)

  # determine the types of each column
  colTypes = TypeFinder(data,nRows,nCols,nullMarker=nullMarker)
  typeStrs = GetTypeStrings(colHeadings,colTypes,keyCol=keyCol)
  colDefs=','.join(typeStrs)

  _AddDataToDb(dBase,table,user,password,colDefs,colTypes,data,
               nullMarker=nullMarker)
 

def DatabaseToDatabase(fromDb,fromTbl,toDb,toTbl,
                       fields='*',join='',where='',
                       user='sysdba',password='masterkey',keyCol=None,nullMarker='None'):
  """

   FIX: at the moment this is a hack
   
  """
  import cStringIO
  io = cStringIO.StringIO()
  io.write(DatabaseToText(fromDb,fromTbl,fields=fields,join=join,where=where,
                          user=user,password=password))
  io.seek(-1)
  TextFileToDatabase(toDb,toTbl,io,user=user,password=password,keyCol=keyCol,
                     nullMarker=nullMarker)


if __name__=='__main__':
  import cStringIO

  io = cStringIO.StringIO()
  io.write('foo,bar,baz\n')
  io.write('1,2,3\n')
  io.write('1.1,4,5\n')
  io.write('4,foo,6\n')
  io.seek(0)
  import RDConfig,os
  dirLoc = os.path.join(RDConfig.RDCodeDir,'Dbase','TEST.GDB')

  TextFileToDatabase(dirLoc,'fromtext',io)
