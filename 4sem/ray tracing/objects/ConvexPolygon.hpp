#ifndef _CONVEX_POLYGON
#define _CONVEX_POLYGON

#include "PlaneGeometricObject.hpp"

namespace NConvexPolygon
{
    using NPlaneGeometricObject::PlaneGeometricObject;
    
    using NGeometry::Point;
    using NGeometry::Plane;
    using NDouble::Double;
    
    class ConvexPolygon: public PlaneGeometricObject
    {
    public:
        virtual const Point& operator [] (int n) const = 0;
        virtual size_t size() const = 0;
    };
    
    bool isPointInConvexPolygon(const Point &p, const ConvexPolygon &polygon)
    {
        Double s(0.), s_abs(0.);
        
        for (ui32 i = 0; i < polygon.size(); ++i)
        {
            Double d = abs((polygon[i] - p) ^ (polygon[(i + 1) % polygon.size()] - p));
            s_abs += d;
            
            ui32 next = (i + 1);
            if (next >= polygon.size())
                next -= polygon.size();
            
            if ((((polygon[i] - p) ^ (polygon[next] - p)) * polygon.normal()) > Double(0.))
                s += d;
            else
                s -= d;
        }
        
        return s_abs == s;
    }
    
    double intersectConvexPolygon(const NGeometry::Ray &ray, const ConvexPolygon * polygon)
    {
        Plane plane(polygon->normal(), (*polygon)[0]);
        
        Point intersection = plane.intersect(ray);
        
        if (intersection == NGeometry::INFINITY_POINT || (intersection - ray.start) * ray.direction < Double(0.))
            return -1.;
        
        if (!isPointInConvexPolygon(intersection, *polygon))
            return -1.;
        
        return abs(intersection - ray.start) / abs(ray.direction);
    }
};

#endif
