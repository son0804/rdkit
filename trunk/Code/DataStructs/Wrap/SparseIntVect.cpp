// $Id$
//
//  Copyright (C) 2007 Greg Landrum
//
//  @@ All Rights Reserved @@
//
#include <boost/python.hpp>

#include <RDGeneral/types.h>
#include <RDGeneral/Invariant.h>
#include <RDBoost/PySequenceHolder.h>
#include <DataStructs/SparseIntVect.h>

using namespace RDKit;

template <typename IndexType>
struct siv_pickle_suite : python::pickle_suite
{
  static python::tuple
  getinitargs(const SparseIntVect<IndexType>& self)
  {
    return python::make_tuple(self.toString());
  };
};

namespace {
  template <typename IndexType>
  void pyUpdateFromSequence(SparseIntVect<IndexType> &vect,
			    python::object &seq){
    PySequenceHolder<IndexType> seqL(seq);
    for(unsigned int i=0;i<seqL.size();++i){
      IndexType idx=seqL[i];
      vect.setVal(idx,vect[idx]+1);
    }
  }
}

std::string sparseIntVectDoc="A container class for storing integer\n\
values within a particular range.\n\
\n\
The length of the vector is set at construction time.\n\
\n\
As you would expect, _SparseIntVects_ support a set of binary operations\n\
so you can do things like:\n\
  Arithmetic:\n\
  siv1 += siv2\n\
  siv3 = siv1 + siv2\n\
  siv1 -= siv3\n\
  siv3 = siv1 - siv2\n\
  \"Fuzzy\" binary operations:\n\
  siv3 = siv1 & siv2  the result contains the smallest value in each entry\n\
  siv3 = siv1 | siv2  the result contains the largest value in each entry\n\
\n\
Elements can be set and read using indexing (i.e. bv[i] = 4 or val=bv[i])\n\
\n";

struct sparseIntVec_wrapper {
  template <typename IndexType>
  static void wrapOne(const char *className){
    python::class_<SparseIntVect<IndexType> >(className, 
					      sparseIntVectDoc.c_str(),
					      python::init<IndexType>("Constructor"))
      .def(python::init<std::string>())
      .def("__len__", &SparseIntVect<IndexType>::getLength,
	   "Get the number of entries in the vector")
      .def("__setitem__", &SparseIntVect<IndexType>::setVal,
	   "Set the value at a specified location")
      .def("__getitem__", &SparseIntVect<IndexType>::getVal,
	   "Get the value at a specified location")
      .def(python::self & python::self)
      .def(python::self | python::self)
      .def(python::self - python::self)
      .def(python::self -= python::self)
      .def(python::self + python::self)
      .def(python::self += python::self)
      .def(python::self == python::self)
      .def(python::self != python::self)
      .def("GetTotalVal", &SparseIntVect<IndexType>::getTotalVal,
	   (python::args("useAbs")=false),
	   "Get the sum of the values in the vector, basically L1 norm")
      .def("ToBinary", &SparseIntVect<IndexType>::toString,
	   "returns a binary (pickle) representation of the vector")
      .def("UpdateFromSequence",
	   &pyUpdateFromSequence<IndexType>,
	   "update the vector based on the values in the list or tuple")
      .def_pickle(siv_pickle_suite<IndexType>())
      ;

  }

  static void wrap() {
    wrapOne<int>("IntSparseIntVect");
    wrapOne<long long>("LongSparseIntVect");
  }
};

void wrap_sparseIntVect() {
  sparseIntVec_wrapper::wrap();
}

