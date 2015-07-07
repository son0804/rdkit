The instructions below are for the source distribution from Jan2008.

## Installing Boost ##
  * download the boost source distribution from [the boost web site](http://www.boost.org)
  * extract the source to a '''non-temporary''' location on your machine, you're going to need it to build the RDKit as well as boost. (e.g. /usr/local/src/boost\_1\_34\_1)
  * read the [installation instructions](http://www.boost.org/more/getting_started/unix-variants.html)
  * [Make a modification to the boost::random source](http://sourceforge.net/mailarchive/forum.php?thread_name=60825b0f0612142119k383e61bah54c85e8ff60fca80%40mail.gmail.com&forum_name=rdkit-devel) _Note: No longer necessary in the subversion versions_
  * Copy the boost::logging stuff into the boost source tree: _Note: No longer necessary in the subversion versions_
    * copy the directory $RDBASE/External/boost\_logging/boost/log to $BOOST/boost
    * copy the directory $RDBASE/External/boost\_logging/libs/log to $BOOST/libs
  * build and install the required boost shared libraries:
    * `cd $BOOST`
    * For the release version: `bjam  --with-thread --with-python --with-log release install`
    * For the subversion version: `bjam  --with-thread --with-python release install`


## New build system (Nov. 2007) ##
  * follow the Installing Boost instructions above.
  * environment variables:
    * RDBASE: the root directory of the RDKit distribution (e.g. ~/RDKit)
    * BOOSTHOME: should be wherever you extracted the boost source (e.g. /usr/local/src/boost\_1\_34\_1)
    * BOOST\_BUILD\_PATH: should be the same as $BOOSTHOME
    * PYTHON\_ROOT: wherever python is installed (e.g. /usr)
    * PYTHON\_VERSION: the first two digits of the python version (e.g. 2.5, '''not''' 2.5.1)
    * PYTHONPATH: make sure it includes $RDBASE/Python
    * LD\_LIBRARY\_PATH: make sure it includes $RDBASE/bin
  * Building: cd to $RDBASE/Code and do "bjam" (this builds all libraries, wrappers, and regression tests)

## Testing the Build (optional, but recommended) ##
  * cd to $RDBASE/Code and do `python $RDBASE/Python/TestRunner.py test_list.py`
  * cd to $RDBASE/Code/GraphMol and do `python $RDBASE/Python/TestRunner.py test_list.py`
  * create the databases used by the Python tests, requires sqlite3 to be installed:
    * `sqlite3 $RDBASE/Data/RDTests.sqlt < $RDBASE/Python/Dbase/testData/RDTests.sqlite`
    * `sqlite3 $RDBASE/Data/RDData.sqlt < $RDBASE/Python/Dbase/testData/RDData.sqlite`
  * cd to $RDBASE/Python and do: `find . -name 'test_list.py' -exec python $RDBASE/Python/TestRunner.py \{\} >> pytests.out 2>&1 \;`
  * cd to $RDBASE/Projects and do: `python $RDBASE/Python/TestRunner.py test_list.py`