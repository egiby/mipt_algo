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
        Vector normal_vector;
        Point vertex[3];
    public:
        Triangle(const Color &_color, const Point &_vertex, const Vector &side_1, const Vector &side_2)
        : normal_vector(side_1 ^ side_2)
        {
            assert(NGeometry::abs(normal_vector) != 0.);
            color = _color;
            
            vertex[0] = _vertex;
            vertex[1] = _vertex + side_1;
            vertex[2] = _vertex + side_2;
        }
        
        NDouble::Double intersect(const NGeometry::Ray &ray) const
        {
            //~ cerr << "intersect\n";
            return intersectConvexPolygon(ray, this);
        }
        
        virtual const NGeometry::Vector& normal() const
        {
            return normal_vector;
        }
        
        virtual size_t size() const
        {
            return 3;
        }
        
        virtual const Point& operator [] (int n) const
        {
            return vertex[n];
        }
    };
};

#endif
