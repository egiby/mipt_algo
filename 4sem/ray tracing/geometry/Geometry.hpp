#ifndef _GEOMETRY
#define _GEOMETRY

#include "Double.hpp"

#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cassert>

#include <iostream>
using std::cerr;

typedef uint32_t ui32;

namespace NGeometry
{
    using NDouble::Double;
    
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
        return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
    }
    
    Double abs(const Vector &p)
    {
        //~ return std::hypot(p.x, p.y, p.z);
        return sqrt(p * p);
    }
    
    bool operator == (const Vector &v1, const Vector &v2)
    {
        return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
    }
    
    bool operator != (const Vector &v1, const Vector &v2)
    {
        return !(v1 == v2);
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
            
            //~ cerr.precision(10);
            //~ cerr << std::fixed << operator () (l.start - l.direction * (normal * normal) / (normal * l.direction)) << '\n';
            //~ assert(operator () (l.start - l.direction * (normal * normal) / (normal * l.direction)) == Double(0.));
            
            return l.start - l.direction * (normal * normal) / (normal * l.direction); // it must work
        }
    };
};

#endif
