This information has been moved to the BuildingOnLinux page

The instructions below are for source distributions from Q3 2008 on.

## Getting Ready ##
  * Required packages:
    * The python headers. This probably means that you need to install the python-dev package (or whatever it's called) for your linux distribution.
    * Blas and lapack. This is also probably just a matter of finding the appropriate package for your linux.
    * flex and bison. These are frequently already installed if you have the various pieces of the development environment installed
    * sqlite3. You also need the shared libraries. This may require that you install a sqlite3-dev package.
    * You need to have numpy (http://www.scipy.org/NumPy) installed. If you do not have python and/or numpy installed in the standard location, you need to edit the Jamroot ($RDBASE/Code/Jamroot) to reflect the location, i.e. replace `<toolset>gcc:<include>/usr/lib/python2.5/site-packages/numpy/core/include` with whatever is appropriate for your python/numpy installation.

## Installing Boost ##
  * download the boost source distribution from [the boost web site](http://www.boost.org)
  * extract the source to a _non-temporary_ location on your machine, you're going to need it to build the RDKit as well as boost. (e.g. `/usr/local/src/boost_1_36_0`)
  * build and install the required boost shared libraries:
    * `cd $BOOST`
    * `bjam  --with-thread --with-python release install`
> If you have any problems with this step, check the boost [installation instructions](http://www.boost.org/more/getting_started/unix-variants.html).

## Installing the boost numeric bindings ##
  * download the most recent version of the boost numeric bindings from [http://mathema.tician.de/dl/software/boost-bindings](http://mathema.tician.de/dl/software/boost-bindings).
  * extract the tar ball to some temporary location.
  * copy the directory `boost-numeric-bindings/boost/numeric/bindings` to `$BOOST/boost/numeric`
  * copy the directory `boost-numeric-bindings/libs/numeric/bindings` to `$BOOST/libs/numeric`


## Building the Code ##
  * follow the Installing Boost instructions above.
  * environment variables:
    * RDBASE: the root directory of the RDKit distribution (e.g. ~/RDKit)
    * BOOSTHOME: should be wherever you extracted the boost source (e.g. /usr/local/src/boost\_1\_34\_1)
    * BOOST\_BUILD\_PATH: should be the same as $BOOSTHOME
    * PYTHON\_ROOT: wherever python is installed (e.g. /usr)
    * PYTHON\_VERSION: the first two digits of the python version (e.g. 2.5, **not** 2.5.1)
    * PYTHONPATH: make sure it includes $RDBASE/Python
    * LD\_LIBRARY\_PATH: make sure it includes $RDBASE/bin and wherever the boost shared libraries were installed
  * Building: cd to $RDBASE/Code and do `bjam` (`bjam address-model=64` on 64bit systems), this builds all libraries, wrappers, and regression tests. This command will generate a lot of text (mostly compiler warnings), but it should not generate any errors. At the end you should **not** see any messages about failed or skipped targets.

## Testing the Build (optional, but recommended) ##
  * cd to $RDBASE/Code and do `python $RDBASE/Python/TestRunner.py test_list.py`
  * cd to $RDBASE/Code/GraphMol and do `python $RDBASE/Python/TestRunner.py test_list.py`
  * create the databases used by the Python tests, requires sqlite3 to be installed:
    * `sqlite3 $RDBASE/Data/RDTests.sqlt < $RDBASE/Python/Dbase/testData/RDTests.sqlite`
    * `sqlite3 $RDBASE/Data/RDData.sqlt < $RDBASE/Python/Dbase/testData/RDData.sqlite`
  * cd to $RDBASE/Python and do: `find . -name 'test_list.py' -exec python $RDBASE/Python/TestRunner.py \{\} >> pytests.out 2>&1 \;`
  * cd to $RDBASE/Projects and do: `python $RDBASE/Python/TestRunner.py test_list.py`