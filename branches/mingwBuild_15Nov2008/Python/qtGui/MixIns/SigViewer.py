#
#  Copyright (C) 2003  Greg Landrum and Rational Discovery LLC
#    All Rights Reserved
#
""" #DOC

"""
import RDConfig
from qt import *
from qtGui import GuiTextViewer
from qtGui.GuiLib.SignatureViewDialogImpl import SignatureViewDialog
import os

REQUIRED_MIXINS = ['PiddleCanvas']
MODULES_ALTERED = ['qtGui.GuiBase']
METHODS_DEFINED = {
  '__LaunchSigViewer':'qtGui.GuiBase.GuiBase.svLaunchSigViewer',
  }

def __LaunchSigViewer(self):
  if self._svViewWin is None:
    dlg = SignatureViewDialog(parent=self)
    self._svViewWin = dlg
  else:
    dlg = self._svViewWin
    
  dlg.setDrawTarget(self._pcCanvasWin)
  dlg.show()
  
def LocalInit(self):
  self._svViewWin = None
  self._svMolViewWin = None

  self._svSigMenu = QPopupMenu(self)
  self.menuBar().insertItem(self.trUtf8('&Signatures'),self._svSigMenu)
  self._svSigMenu.insertItem(self.trUtf8('&Viewer'),self.svLaunchSigViewer)


                                
                                
