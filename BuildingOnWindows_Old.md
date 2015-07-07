The instructions below are for source distributions from May2008 on.

## Installing Boost ##
  * download the boost source distribution from [the boost web site](http://www.boost.org)
  * extract the source to a **non-temporary** location on your machine, you're going to need it to build the RDKit as well as boost. (e.g. c:/boost/boost\_1\_34\_1)
  * Read section 5.2 of the  [Boost.Build documentation for windows](http://www.boost.org/more/getting_started/windows.html#prepare-to-use-a-boost-library-binary)

## Building the C++ Components ##
  * follow the Installing Boost instructions above.
  * environment variables:
    * RDBASE: the root directory of the RDKit distribution (e.g. c:/glandrum/RDKit)
    * BOOSTHOME: should be wherever you extracted the boost source (e.g. c:/boost/boost\_1\_34\_1)
    * BOOST\_BUILD\_PATH: should be the same as $BOOSTHOME
    * PYTHON\_ROOT: wherever python is installed (e.g. /usr)
    * PYTHON\_VERSION: the first two digits of the python version (e.g. 2.5, **not** 2.5.1)
    * PYTHONPATH: make sure it includes $RDBASE/Python

**NOTE:** PATH needs to include $RDBASE/bin. If you're using cygwin, be sure to use the /c form of the path (not c:/). So if $RDBASE is c:/glandrum/RDKit, then /c/glandrum/RDKit/bin should be in the PATH.

  * Building: cd to $RDBASE/Code and do `bjam` (this builds all libraries, wrappers, and regression tests)

## Testing the Build (optional, but recommended) ##
  * cd to $RDBASE/Code and do `python $RDBASE/Python/TestRunner.py test_list.py`
  * cd to $RDBASE/Code/GraphMol and do `python $RDBASE/Python/TestRunner.py test_list.py`