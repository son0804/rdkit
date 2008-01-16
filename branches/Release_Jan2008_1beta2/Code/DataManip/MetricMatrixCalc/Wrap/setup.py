# Run this with:
#  python setup.py install --install-lib=$RDBASE/Python
from RDBuild import *
from distutils.core import setup,Extension


libDirs +=[
  rdBitDir,
  ]

libraries += ["DataStructs", "RDGeneral", "RDBoost"]
setup(name="DataManip.Metric.rdMetricMatrixCalc", version="2.0",
      ext_modules=[Extension("DataManip.Metric.rdMetricMatrixCalc",
                             ["rdMetricMatrixCalc.cpp"],
                             library_dirs=libDirs,
                             libraries=libraries,
                             extra_link_args=linkArgs,
                             extra_compile_args=compileArgs)])
