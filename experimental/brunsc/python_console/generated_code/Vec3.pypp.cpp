// This file has been generated by Py++.

#include "boost/python.hpp"
#include "wrappable_v3d.h"
#include "vec3_container_traits.hpp"
#include "Vec3.pypp.hpp"

namespace bp = boost::python;

void register_Vec3_class(){

    { //::SimTK::Vec< 3, double, 1 >
        typedef bp::class_< SimTK::Vec< 3, double, 1 > > Vec3_exposer_t;
        Vec3_exposer_t Vec3_exposer = Vec3_exposer_t( "Vec3", "\n Generic Vec\n" );
        bp::scope Vec3_scope( Vec3_exposer );
        bp::scope().attr("NRows") = (int)SimTK::Vec<3, double, 1>::NRows;
        bp::scope().attr("NCols") = (int)SimTK::Vec<3, double, 1>::NCols;
        bp::scope().attr("NPackedElements") = (int)SimTK::Vec<3, double, 1>::NPackedElements;
        bp::scope().attr("NActualElements") = (int)SimTK::Vec<3, double, 1>::NActualElements;
        bp::scope().attr("NActualScalars") = (int)SimTK::Vec<3, double, 1>::NActualScalars;
        bp::scope().attr("RowSpacing") = (int)SimTK::Vec<3, double, 1>::RowSpacing;
        bp::scope().attr("ColSpacing") = (int)SimTK::Vec<3, double, 1>::ColSpacing;
        bp::scope().attr("ImagOffset") = (int)SimTK::Vec<3, double, 1>::ImagOffset;
        bp::scope().attr("RealStrideFactor") = (int)SimTK::Vec<3, double, 1>::RealStrideFactor;
        bp::scope().attr("ArgDepth") = (int)SimTK::Vec<3, double, 1>::ArgDepth;
        bp::scope().attr("IsScalar") = (int)SimTK::Vec<3, double, 1>::IsScalar;
        bp::scope().attr("IsULessScalar") = (int)SimTK::Vec<3, double, 1>::IsULessScalar;
        bp::scope().attr("IsNumber") = (int)SimTK::Vec<3, double, 1>::IsNumber;
        bp::scope().attr("IsStdNumber") = (int)SimTK::Vec<3, double, 1>::IsStdNumber;
        bp::scope().attr("IsPrecision") = (int)SimTK::Vec<3, double, 1>::IsPrecision;
        bp::scope().attr("SignInterpretation") = (int)SimTK::Vec<3, double, 1>::SignInterpretation;
        Vec3_exposer.def(bp::indexing::container_suite<
                            SimTK::Vec<3, double, 1>,
                            bp::indexing::all_methods,
                            list_algorithms< vec_container_traits< SimTK::Vec<3, double, 1> > > >());
    }

}
