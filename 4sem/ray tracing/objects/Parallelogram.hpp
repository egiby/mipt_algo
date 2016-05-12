#ifndef _PARALLELOGRAM
#define _PARALLELOGRAM

#include "ConvexPolygon.hpp"

namespace NParallelogram
{
    using NGeometry::Point;
    using NGeometry::Vector;
    using NGeometricObjects::Color;
    
    class Parallelogram: public NConvexPolygon::ConvexPolygon
    {
        Vector normal_vector;
        Point vertex[4];
    public:
        Parallelogram(const Color &_color, const Point &_vertex, const Vector &side_1, const Vector &side_2)
        : normal_vector(side_1 ^ side_2)
        {
            color = _color;
            vertex[0] = _vertex;
            vertex[1] = _vertex + side_1;
            vertex[2] = _vertex + side_1 + side_2;
            vertex[3] = _vertex + side_2;
        }
        
        NDouble::Double intersect(const NGeometry::Ray &ray) const
        {
            return NConvexPolygon::intersectConvexPolygon(ray, this);
        }
        
        virtual const NGeometry::Vector& normal() const
        {
            return normal_vector;
        }
        
        size_t size() const
        {
            return 4;
        }
        
        const Point& operator [] (int n) const
        {
            return vertex[n];
        }
    };
};

#endif  
