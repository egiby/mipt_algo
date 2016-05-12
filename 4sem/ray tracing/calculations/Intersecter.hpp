#ifndef _INTERSECTER
#define _INTERSECTER

#include "../geometry/Double.hpp"
#include "../geometry/Geometry.hpp"

#include "../objects/GeometricObject.hpp"

struct Intersection
{
    NGeometry::Point point;
    NGeometricObjects::IGeometricObject * object;
};

Intersection intersectAll(const NGeometry::Ray &ray, const NImageSettings::ImageSettings * settings)
{
    //~ cerr << "intersectAll\n";
    NDouble::Double min_coefficient = NGeometry::INFINITY_POINT.x;
    NGeometricObjects::IGeometricObject * first_object = nullptr;
    
    for (auto object: settings->objects)
    {
        NDouble::Double coefficient = object->intersect(ray);
        
        if (coefficient < NDouble::Double(0.))
            continue;
        
        if (coefficient < min_coefficient)
        {
            first_object = object;
            min_coefficient = coefficient;
        }
    }
    if (first_object == nullptr)
        return {NGeometry::INFINITY_POINT, first_object};
    
    return {ray.start + min_coefficient * ray.direction, first_object};
}

#endif
