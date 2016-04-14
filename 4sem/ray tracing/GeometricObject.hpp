#ifndef _GEOMETRIC_OBJECT
#define _GEOMETRIC_OBJECT

#include "Double.hpp"

namespace NGeometry
{
    using namespace NDouble;
    
    template<typename T>
    struct Point
    {
        T x, y, z;
        Point operator - () const
        {
            return {-x, -y, -z};
        }
    };
    
    template<typename T>
    struct Vector: public Point<T>
    {
    };
    
    template<typename T>
    Vector<T> operator + (const Vector<T> &v1, const Vector<T> &v2)
    {
        return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }
    
    template<typename T>
    Vector<T> operator - (const Vector<T> &v1, const Vector<T> &v2)
    {
        return v1 + (-v2);
    }
    
    template<typename T>
    Vector<T> operator * (const Vector<T> &v1, const T &c)
    {
        return {v1.x * c, v1.y * c, v1.z * c};
    }
    
    template<typename T>
    Vector<T> operator * (const T &c, const Vector<T> &v1)
    {
        return v1 * c;
    }
    
    template<typename T>
    T operator * (const Vector<T> &v1, const Vector<T> &v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }
    
    template<typename T>
    Vector<T> operator ^ (const Vector<T> &v1, const Vector<T> &v2)
    {
        return {v1.y * v2.z - v1.z - v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.z};
    }
    
    struct Ray
    {
        Point<Double> start;
        Vector<Double> direction;
    };
};

class IGeometricObject
{
public:
    virtual NGeometry::Point<NDouble::Double> intersect(const NGeometry::Ray &r) const = 0;
};

#endif
