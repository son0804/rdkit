# Run this with:
#  python setup.py install --install-lib=$RDBASE/Python
from RDBuild import *
import RDConfig
from distutils.core import setup,Extension


destDir = RDConfig.RDCodeDir

libDirs=['.']
libraries=['hc']
setup(name="ML.Cluster.Clustering",version="1.0",
      package_dir={'':destDir},
      ext_modules=[Extension("ML.Cluster.Clustering",["Clustering.cpp"],
                             library_dirs=libDirs,
                             libraries=libraries)])
