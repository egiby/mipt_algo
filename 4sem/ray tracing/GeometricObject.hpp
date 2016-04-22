#ifndef _GEOMETRIC_OBJECT
#define _GEOMETRIC_OBJECT

#include "Double.hpp"

#include <cstdint>
#include <cstdio>
#include <cmath>

#include <iostream>
using std::cerr;

typedef uint32_t ui32;

namespace NGeometry
{
    using namespace NDouble;
    
    struct Point
    {
        Double x, y, z;
        Point operator - () const
        {
            return {-x, -y, -z};
        }
    };
    
    const Point INFINITY_POINT = {1e20, 1e20, 1e20};
    
    typedef Point Vector;
    
    Vector operator + (const Vector &v1, const Vector &v2)
    {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }
    
    Vector operator - (const Vector &v1, const Vector &v2)
    {
        return v1 + (-v2);
    }
    
    Vector operator * (const Vector &v1, const Double &c)
    {
        return {v1.x * c, v1.y * c, v1.z * c};
    }
    
    Vector operator * (const Double &c, const Vector &v1)
    {
        return v1 * c;
    }
    
    Double operator * (const Vector &v1, const Vector &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    
    Vector operator / (const Vector &v1, const Double &c)
    {
        return v1 * (1 / c);
    }
    
    Vector operator ^ (const Vector &v1, const Vector &v2)
    {
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.z};
    }
    
    Double abs(const Vector &p)
    {
        return sqrt(p * p);
    }
    
    bool operator == (const Vector &v1, const Vector &v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }
    
    std::ostream& operator << (std::ostream &out, const Point &p)
    {
        return out << p.x << ' ' << p.y << ' ' << p.z;
    }
    
    std::istream& operator >> (std::istream &in, Point &p)
    {
        in >> p.x >> p.y >> p.z;
        return in;
    }
    
    struct Ray
    {
        Point start;
        Vector direction;
        
        Ray (const Point &start, const Vector &direction)
        : start(start), direction(direction)
        {
        }
    };
    
    typedef Ray Line;
    
    struct Plane
    {
        Double a, b, c, d;
        
        Plane (const Vector &normal, const Point &p)
        : a(normal.x), b(normal.y), c(normal.z), d(-(a * p.x + b * p.y + c * p.z))
        {
        }
        
        Plane (const Point &p1, const Point &p2, const Point &p3)
        {
            Vector normal = (p2 - p1) ^ (p3 - p1);
            Plane(normal, p3);
        }
        
        Double operator () (const Point &p) const
        {
            return a * p.x + b * p.y + c * p.z + d;
        }
        
        Vector getNormal() const
        {
            return Vector{a, b, c};
        }
        
        Point intersect(const Line &l)
        {
            Vector normal = (*this)(l.start) * getNormal() / (getNormal() * getNormal());
            
            if (normal * l.direction == Double(0.))
                return INFINITY_POINT;
            
            return l.start - l.direction * (normal * normal) / (normal * l.direction); // it must work
        }
    };
};

namespace NGeometricObjects
{
    using namespace NGeometry;
    
    class IGeometricObject
    {
    public:
        virtual NDouble::Double intersect(const NGeometry::Ray &r) const = 0;
        virtual ui32 getRed() const = 0;
        virtual ui32 getGreen() const = 0;
        virtual ui32 getBlue() const = 0;
        virtual ~IGeometricObject()
        {
        }
    };
    
    struct Color
    {
        ui32 red, green, blue;
    };
    
    class PlaneGeometricObject: public IGeometricObject
    {
    protected:
        Color color;
    public:
        virtual NGeometry::Vector normal() const = 0;
        
        virtual ui32 getRed() const
        {
            return color.red;
        }
        
        virtual ui32 getGreen() const
        {
            return color.green;
        }
        
        virtual ui32 getBlue() const
        {
            return color.blue;
        }
    };
    
    template<class Polygon>
    bool isPointInConvexPolygon(const Point &p, const Polygon &polygon)
    {
        Double s(0.), s_abs(0.);
        
        for (ui32 i = 0; i < polygon.size(); ++i)
        {
            Double d = abs((polygon[i] - p) ^ (polygon[(i + 1) % polygon.size()] - p));
            s_abs += d;
            
            if ((((polygon[i] - p) ^ (polygon[(i + 1) % polygon.size()] - p)) * polygon.normal()) > Double(0.))
                s += d;
            else
                s -= d;
        }
        
        return s_abs == s;
    }
    
    template<class Polygon>
    double intersectConvexPolygon(const NGeometry::Ray &ray, const Polygon &polygon)
    {
        Plane plane(polygon.normal(), polygon[0]);
            
        Point intersection = plane.intersect(ray);
        
        if (intersection == INFINITY_POINT || (intersection - ray.start) * ray.direction < Double(0.))
            return -1.;
        
        if (!isPointInConvexPolygon(intersection, polygon))
            return -1.;
        
        return abs(intersection - ray.start) / abs(ray.direction);
    }
    
    class Triangle: public PlaneGeometricObject
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
            return intersectConvexPolygon(ray, *this);
        }
        
        virtual NGeometry::Vector normal() const
        {
            return side_1 ^ side_2;
        }
        
        size_t size() const
        {
            return 3;
        }
        
        const Point operator [] (int n) const
        {
            if (n == 0)
                return vertex;
            
            if (n == 1)
                return vertex + side_1;
            
            return vertex + side_2;
        }
    };
    
    class Parallelogram: public PlaneGeometricObject
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
            return intersectConvexPolygon(ray, *this);
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
    
    //~ class Quadrangle: public PlaneGeometricObject
    //~ {
        //~ std::array<Point, 4> points;
    //~ public:
        
    //~ };
};

#endif
