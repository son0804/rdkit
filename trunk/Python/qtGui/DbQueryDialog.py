#
#  Copyright (C) 2002  Greg Landrum and Rational Discovery LLC
#    All Rights Reserved
#
"""  dialog for setting up db queries

"""    

import RDConfig
from qt import *
from qtGui.DbQueryWidgetImpl import insertQueryWidget
from qtGui.DbDialog import DbDialog

class DbQueryDialog(DbDialog):
  """  dialog for establishing db connections

    This is just a _DbDialog_ containing a _DbQueryWidget_
    
  """    
  def __init__(self,parent=None,initDir=''):
    DbDialog.__init__(self,parent,initDir)
    self.setDbWidget(insertQueryWidget(self))

    
    

