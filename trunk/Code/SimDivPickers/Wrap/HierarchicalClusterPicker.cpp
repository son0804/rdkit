// $Id: HierarchicalClusterPicker.cpp 4959 2006-02-17 23:53:31Z glandrum $
//
//  Copyright (C) 2003-2006  Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#define NO_IMPORT_ARRAY

#define PY_ARRAY_UNIQUE_SYMBOL rdpicker_array_API
#include <boost/python.hpp>

#include "Numeric/arrayobject.h"
#include <RDBoost/Wrap.h>

#include <SimDivPickers/DistPicker.h>
#include <SimDivPickers/HierarchicalClusterPicker.h>

namespace python = boost::python;
namespace RDPickers {

  // REVIEW: the poolSize can be pulled from the numeric array
  RDKit::INT_VECT HierarchicalPicks(HierarchicalClusterPicker *picker,
                                   python::numeric::array &distMat,
                                   int poolSize, 
                                   int pickSize) {
    // REVIEW: check pickSize < poolSize, otherwise throw_value_error()
    PyArrayObject *copy;
    // it's painful to have to copy the input matrix, but the
    // picker itself will step on the distance matrix, so use
    // CopyFromObject here instead of ContiguousFromObject
    copy = (PyArrayObject *)PyArray_CopyFromObject(distMat.ptr(), 
						   PyArray_DOUBLE, 1,1);
    double *dMat = (double *)copy->data;
    RDKit::INT_VECT res=picker->pick(dMat, poolSize, pickSize);
    Py_DECREF(copy);
    return res;
  }

  // REVIEW: the poolSize can be pulled from the numeric array
  RDKit::VECT_INT_VECT HierarchicalClusters(HierarchicalClusterPicker *picker,
                                   python::numeric::array &distMat,
                                   int poolSize, 
                                   int pickSize) {
    // REVIEW: check pickSize < poolSize, otherwise throw_value_error()
    PyArrayObject *copy;
    // it's painful to have to copy the input matrix, but the
    // picker itself will step on the distance matrix, so use
    // CopyFromObject here instead of ContiguousFromObject
    copy = (PyArrayObject *)PyArray_CopyFromObject(distMat.ptr(), 
						   PyArray_DOUBLE, 1,1);
    double *dMat = (double *)copy->data;

    RDKit::VECT_INT_VECT res=picker->cluster(dMat, poolSize, pickSize);
    Py_DECREF(copy);
    return res;
  }
  
  struct HierarchCP_wrap {
    static void wrap() {
      std::string docString = "A class for diversity picking of items using Hierarchical Clustering\n";
      python::class_<HierarchicalClusterPicker>("HierarchicalClusterPicker",
                                                docString.c_str(),
                                                python::init<HierarchicalClusterPicker::ClusterMethod>
                                                (python::args("clusterMethod")))
        .def("Pick", HierarchicalPicks,
             "Pick a diverse subset of items from a pool of items using a Hierarchical Clustering\n"
             "\n"
             "ARGUMENTS: \n\n"
             "  - distMat: 1D distance matrix (only the lower triangle elements)\n"
             "  - poolSize: number of items in the pool\n"
             "  - pickSize: number of items to pick from the pool\n")
        .def("Cluster", HierarchicalClusters,
             "Return a list of clusters of item from the pool using hierachical clustering\n"
             "\n"
             "ARGUMENTS: \n\n"
             "  - distMat: 1D distance matrix (only the lower triangle elements)\n"
             "  - poolSize: number of items in the pool\n"
             "  - pickSize: number of items to pick from the pool\n")
        ;

      python::enum_<HierarchicalClusterPicker::ClusterMethod>("ClusterMethod")
        .value("WARD", HierarchicalClusterPicker::WARD)
        .value("SLINK", HierarchicalClusterPicker::SLINK)
        .value("CLINK", HierarchicalClusterPicker::CLINK)
        .value("UPGMA", HierarchicalClusterPicker::UPGMA)
        .value("MCQUITTY", HierarchicalClusterPicker::MCQUITTY)
        .value("GOWER", HierarchicalClusterPicker::GOWER)
        .value("CENTROID", HierarchicalClusterPicker::CENTROID)
        ; 
    };
  };
}

void wrap_HierarchCP() {
  RDPickers::HierarchCP_wrap::wrap();
}
