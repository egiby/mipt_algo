#ifndef _PLANE_GEOMETRIC_OBJECT
#define _PLANE_GEOMETRIC_OBJECT

#include "GeometricObject.hpp"

#include <cassert>

namespace NPlaneGeometricObject
{
    class PlaneGeometricObject: public NGeometricObjects::IGeometricObject
    {
    protected:
        NGeometricObjects::Color color;
    public:
        virtual NGeometry::Vector normal() const = 0;
        
        NGeometry::Vector getNormal(const NGeometry::Point &p) const
        {
            assert(p != NGeometry::INFINITY_POINT);
            return normal();
        }
        
        NGeometricObjects::Color getColor() const
        {
            return color;
        }
    };
};

#endif
