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
        Point vertex;
        Vector side_1, side_2;
    public:
        Parallelogram(const Color &_color, const Point &vertex, const Vector &side_1, const Vector &side_2)
        : vertex(vertex), side_1(side_1), side_2(side_2)
        {
            color = _color;
        }
        
        NDouble::Double intersect(const NGeometry::Ray &ray) const
        {
            return NConvexPolygon::intersectConvexPolygon(ray, this);
        }
        
        virtual NGeometry::Vector normal() const
        {
            return side_1 ^ side_2;
        }
        
        size_t size() const
        {
            return 4;
        }
        
        const Point operator [] (int n) const
        {
            if (n == 0)
                return vertex;
            
            if (n == 1)
                return vertex + side_1;
            
            if (n == 2)
                return vertex + side_1 + side_2;
            
            return vertex + side_2;
        }
    };
};

#endif  
