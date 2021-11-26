// This file has been generated by Py++.

#include "boost/python.hpp"
#include "wrappable_v3d.h"
#include "RGBA8.pypp.hpp"

namespace bp = boost::python;

void register_RGBA8_class(){

    bp::class_< RGBA8 >( "RGBA8" )    
        .def_readwrite( "a", &RGBA8::a )    
        .def_readwrite( "b", &RGBA8::b )    
        .def_readwrite( "g", &RGBA8::g )    
        .def_readwrite( "r", &RGBA8::r )    
        .def_readwrite( "c", &RGBA8::c )    
        .def_readwrite( "i", &RGBA8::i );

}
