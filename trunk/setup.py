#!/usr/bin/env python
#

from distutils.core import setup
from distutils import sysconfig
import os, sys, re, glob, shutil
from setuptools import setup
from setuptools.dist import Distribution

version='2009.Q1-1'
class RDKDist(Distribution):
     def has_ext_modules(self):
         return True

module_ext = sysconfig.get_config_var('SO')

child_packages = [
               "Python.Chem",
               "Python.DataManip",
               "Python.DataStructs",
               "Python.Dbase",
               "Python.DistanceGeometry",
               "Python.ForceField",
               "Python.Geometry",
               "Python.Logger",
               "Python.ML",
               "Python.Numerics",
               "Python.SimDivFilters",
               "Python.VLib",
               "Python.sping",
               "Python.utils",
               ]

sos = [('Python',['Python/rdBase'+module_ext])]
py_packages = ["Python"]+child_packages

for pkg in child_packages:
     for root,dirs,files in os.walk(pkg.replace('.','/')):
          if '.svn' in dirs: dirs.remove('.svn')

          modName=root.replace('/','.')
          if '__init__.py' in files and modName not in py_packages:
               py_packages.append(modName)

          files=[os.path.join(root,file) for file in files if (os.path.splitext(file)[-1]==module_ext or\
                                                                    'test_data' in root)]
          sos.extend([(root,files)])

ext_modules = [
               ]


scripts = []
if sys.platform == "win32":
    for script in scripts:
        filename = os.path.join('scripts', script)
        shutil.copyfile(filename, filename + '.py')
    scripts = [s + '.py' for s in scripts]

data_files = [('Data',glob.glob('Data/*.*'))]
data_files.extend([('',glob.glob('./*.txt'))])
data_files.extend([('bin',glob.glob('bin/*'))])
data_files.extend(sos)


documentation = []


setup(distclass=RDKDist,
      zip_safe=False,
      name='pyRDKit',
      version=version,
      description='RDKit Cheminformatics Library',
      author='Greg Landrum',
      author_email='glandrum@users.sourceforge.net',
      url='http://www.rdkit.org/',
      download_url = 'http://code.google.com/p/rdkit/downloads/list',
      license = 'BSD / GPL',
      classifiers = ['Development Status :: 5 - Production/Stable',
                     'Environment :: Console',
                     'Intended Audience :: Developers',
                     'Programming Language :: Python',
                     'Programming Language :: C++'
                     ],
      packages=py_packages,
      ext_modules=ext_modules,
      package_dir={'pyRDKit':'Python'},
      data_files=data_files,
      )
