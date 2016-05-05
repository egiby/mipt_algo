#ifndef _TRIANGLE
#define _TRIANGLE

#include "ConvexPolygon.hpp"

namespace NTriangle
{
    using NGeometry::Point;
    using NGeometry::Vector;
    using NGeometricObjects::Color;
    
    class Triangle: public NConvexPolygon::ConvexPolygon
    {
        Point vertex;
        Vector side_1, side_2;
    public:
        Triangle(const Color &_color, const Point &vertex, const Vector &side_1, const Vector &side_2)
        : vertex(vertex), side_1(side_1), side_2(side_2)
        {
            color = _color;
        }
        
        NDouble::Double intersect(const NGeometry::Ray &ray) const
        {
            //~ cerr << "intersect\n";
            return intersectConvexPolygon(ray, this);
        }
        
        virtual NGeometry::Vector normal() const
        {
            assert(NGeometry::abs(side_1 ^ side_2) != 0);
            return side_1 ^ side_2;
        }
        
        virtual size_t size() const
        {
            return 3;
        }
        
        virtual const Point operator [] (int n) const
        {
            if (n == 0)
                return vertex;
            
            if (n == 1)
                return vertex + side_1;
            
            return vertex + side_2;
        }
    };
};

#endif
