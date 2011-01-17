#include "rice/Class.hpp"
#include "rice/String.hpp"
#include "rice/Constructor.hpp"
#include "rice/Enum.hpp"

#include <base/geometry/spline.h>

using namespace Rice;
using base::geometry::SplineBase;

static std::vector<double> array_to_double_vector(Array array)
{
    std::vector<double> result;
    result.reserve(array.size());
    for (unsigned int i = 0; i < array.size(); ++i)
        result.push_back(from_ruby<double>(array[i]));
    return result;
}

static Array double_vector_to_array(std::vector<double> const& v)
{
    return Array(v.begin(), v.end());
}

class RubySpline : public SplineBase
{
public:
    RubySpline(int dimension, double geometric_resolution, int order)
        : SplineBase(dimension, geometric_resolution, order) {}


    void do_interpolate(Array coordinates, Array values)
    {
        interpolate(array_to_double_vector(coordinates), array_to_double_vector(values));
    }

    Array do_coordinates()
    {
        return double_vector_to_array(getCoordinates());
    }

    Array do_knots()
    {
        return double_vector_to_array(getKnots());
    }

    void do_reset(Array coordinates, Array knots, int kind)
    {
        SplineBase::reset(array_to_double_vector(coordinates), array_to_double_vector(knots), kind);
    }

    Array do_getPoint(double param, bool with_tangent)
    {
        std::vector<double> result;
        if (with_tangent)
        {
            result.resize(getDimension() * 2);
            getPointAndTangent(&result[0], param);
        }
        else
        {
            result.resize(getDimension());
            getPoint(&result[0], param);
        }

        return double_vector_to_array(result);
    }

    double do_findOneClosestPoint(Array _ref_point, double guess, double geores)
    {
        std::vector<double> ref_point = array_to_double_vector(_ref_point);
        return findOneClosestPoint(&ref_point[0], guess, geores);
    }

    Array do_findClosestPoints(Array _ref_point, double guess, double geores)
    {
        std::vector<double> ref_point = array_to_double_vector(_ref_point);
        std::vector<double> result_points;
        std::vector< std::pair<double, double> > result_curves;
        findClosestPoints(&ref_point[0], result_points, result_curves, geores);

        Array ruby_points = double_vector_to_array(result_points);
        Array ruby_curves;
        for (unsigned int i = 0; i < result_curves.size(); ++i)
        {
            Array pair;
            pair[0] = result_curves[i].first;
            pair[1] = result_curves[i].second;
            ruby_curves.push(pair);
        }

        Array result;
        result.push(ruby_points);
        result.push(ruby_curves);
        return result;
    }

    void initialize_copy(RubySpline const& other)
    {
        *this = other;
    }
};



void Init_spline_ext(Rice::Module& base_m)
{
    typedef std::vector<double>(RubySpline::*SimplifySelector)(double);
    Data_Type<SplineBase> rb_SplineBase = define_class_under<SplineBase>(base_m, "SplineBase")
        .define_constructor(Constructor<SplineBase,int,double,int>(),
                (Arg("dimension"), Arg("geometric_resolution") = 0.1, Arg("order") = 3))
        .define_method("geometric_resolution=", &SplineBase::setGeometricResolution)
        .define_method("geometric_resolution", &SplineBase::getGeometricResolution)
        .define_method("order", &SplineBase::getCurveOrder)
        .define_method("empty?", &SplineBase::isEmpty)
        .define_method("singleton?", &SplineBase::isSingleton)
        .define_method("reverse", &SplineBase::reverse)
        .define_method("dimension", &SplineBase::getDimension)
        .define_method("point_count", &SplineBase::getPointCount)
        .define_method("curve_length", &SplineBase::getCurveLength)
        .define_method("curvature_max", &SplineBase::getCurvatureMax)
        .define_method("start_param", &SplineBase::getStartParam)
        .define_method("end_param", &SplineBase::getEndParam)
        .define_method("unit_parameter", &SplineBase::getUnitParameter)
        .define_method("curvature_at", &SplineBase::getCurvature)
        .define_method("variation_of_curvature_at", &SplineBase::getVariationOfCurvature)
        .define_method("simplify", static_cast<SimplifySelector>(&SplineBase::simplify))
        .define_method("clear", &SplineBase::clear)
        .define_method("sisl_curve_type", &SplineBase::getSISLCurveType)
        .define_method("coordinate_stride", &SplineBase::getCoordinatesStride)
        .define_method("join", &SplineBase::join, (Arg("curve"), Arg("tolerance") = 0))
        .define_method("append", &SplineBase::append);

    Data_Type<RubySpline> rb_Spline = define_class_under<RubySpline, SplineBase>(base_m, "Spline")
        .define_constructor(Constructor<RubySpline,int,double,int>(),
                (Arg("dimension"), Arg("geometric_resolution") = 0.1, Arg("order") = 3))
        .define_method("initialize_copy", &RubySpline::initialize_copy)
        .define_method("do_interpolate", &RubySpline::do_interpolate)
        .define_method("coordinates", &RubySpline::do_coordinates)
        .define_method("knots", &RubySpline::do_knots)
        .define_method("reset", &RubySpline::do_reset, (Arg("coordinates"), Arg("knots"), Arg("kind") = -1))
        .define_method("get", &RubySpline::do_getPoint, (Arg("parameter"), Arg("with_tangent") = false));
}

