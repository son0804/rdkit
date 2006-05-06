// $Id: wrap_Utils.cpp 5054 2006-03-06 22:39:29Z glandrum $
//
//  Copyright (C) 2003-2006 greg Landrum and Rational Discovery LLC
//
//  @@ All Rights Reserved @@
//
#include <DataStructs/BitVects.h>
#include <DataStructs/BitVectUtils.h>
#include <RDBoost/Wrap.h>
namespace python = boost::python;

ExplicitBitVect *createFromBitString(const std::string &bits){
    ExplicitBitVect *res=new ExplicitBitVect(bits.length());
    FromBitString(*res,bits);
    return res;
}

struct Utils_wrapper {
  static void wrap(){
    python::def("ConvertToExplicit", convertToExplicit, 
                python::return_value_policy<python::manage_new_object>());  
    python::def("CreateFromBitString",createFromBitString,
                python::return_value_policy<python::manage_new_object>());  

    python::def("InitFromDaylightString",
		(void (*)(SparseBitVect &,std::string))FromDaylightString);
    python::def("InitFromDaylightString",
		(void (*)(ExplicitBitVect &,std::string))FromDaylightString,
		"Fill a BitVect using an ASCII (Daylight) encoding of fingerprint.\n\
\n\
   **Arguments**\n\
     - bv: either a _SparseBitVect_ or an _ExplicitBitVect_\n\
     - txt: a string with the Daylight encoding (this is the text that\n\
            the Daylight tools put in the FP field of a TDT)\n\
\n");
  }
};

void wrap_Utils() {
  Utils_wrapper::wrap();
}
  
