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
        
        //~ bool is_good = 0;
        //~ for (ui32 i = 0; i < polygon.size(); ++i)
        //~ {
            //~ auto next_1 = (i + 1);
            //~ if (next_1 >= polygon.size())
                //~ next_1 -= polygon.size();
            
            //~ auto next_2 = (i + 2);
            //~ if (next_2 >= polygon.size())
                //~ next_2 -= polygon.size();
            
            //~ auto l = (polygon[next_1] - polygon[i]);
            //~ is_good |= ((p - polygon[i]) ^ l) * ((polygon[next_2] - polygon[i]) ^ l) < Double(0);
        //~ }
        
        return s_abs == s;
        //~ return is_good;
    }
    
    double intersectConvexPolygon(const NGeometry::Ray &ray, const ConvexPolygon * polygon)
    {
        Plane plane(polygon->normal(), (*polygon)[0]);
        
        Point intersection = plane.intersect(ray);
        
        if (intersection == NGeometry::INFINITY_POINT || (intersection - ray.start) * ray.direction < Double(0.))
            return -1.;
        
        //~ cerr << "plane: " << std::fixed << plane.a << ' ' << plane.b << ' ' << plane.c << ' ' << plane.d << '\n';
        //~ cerr << "intersection: " << intersection << '\n';
        //~ cerr << (*polygon)[0] << '\n';
        //~ cerr << (*polygon)[1] << '\n';
        //~ cerr << (*polygon)[2] << '\n';
        if (!isPointInConvexPolygon(intersection, *polygon))
            return -1.;
        
        return abs(intersection - ray.start) / abs(ray.direction);
    }
};

#endif
