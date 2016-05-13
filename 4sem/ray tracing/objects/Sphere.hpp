#ifndef _SPHERE
#define _SPHERE

#include "GeometricObject.hpp"
#include "../ImageSettings.hpp"

using NDouble::Double;

class Sphere: public NGeometricObjects::IGeometricObject
{
    NGeometry::Point center;
    NDouble::Double radius;
    NGeometricObjects::Color color;
public:
    
    Sphere(const NGeometry::Point &center, const NDouble::Double &radius, const NGeometricObjects::Color &color)
    : center(center), radius(radius), color(color)
    {
    }
    
    NDouble::Double intersect(const NGeometry::Ray &r) const
    {
        NDouble::Double coefficient = (r.direction * (center - r.start)) / (r.direction * r.direction);
        NGeometry::Point closest = r.start + r.direction * coefficient;
        
        //~ cerr << coefficient << '\n';
        //~ cerr << closest << '\n';
        
        if ((closest - center) * (closest - center) > radius * radius)
            return -1;
        
        //~ cerr << "sphere\n";
        
        if ((closest - center) * (closest - center) == radius * radius)
        {
            return coefficient;
        }
        
        NDouble::Double d_coef = sqrt(radius * radius - (closest - center) * (closest - center)) / abs(r.direction);
        
        //~ cerr << coefficient << '\n';
        
        if (coefficient - d_coef >= 0.)
            return coefficient - d_coef;
        
        if (coefficient + d_coef < 0.)
            return -1;
        
        return coefficient + d_coef;
    }
    
    NGeometricObjects::Color getColor() const
    {
        return color;
    }
    
    NGeometry::Vector getNormal(const NGeometry::Point &p) const
    {
        return p - center;
    }
};

#endif
