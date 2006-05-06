// $Id: rdGeometry.cpp 4955 2006-02-17 23:37:53Z glandrum $
//
//  Copyright (C) 2005-2006 Rational Discovery LLC
//
//   @@ All Rights Reserved  @@
//
#include <RDBoost/Wrap.h>
#include <boost/python.hpp>

namespace python = boost::python;
void wrap_point3D();
void wrap_uniformGrid();

BOOST_PYTHON_MODULE(rdGeometry)
{
  python::scope().attr("__doc__") =
    "Module containing geomtery objects like points, grids etc\n";
  python::register_exception_translator<IndexErrorException>(&translate_index_error);
  python::register_exception_translator<ValueErrorException>(&translate_value_error);

  wrap_point3D();
  wrap_uniformGrid();
}
