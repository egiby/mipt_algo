#ifndef _PLANE_GEOMETRIC_OBJECT
#define _PLANE_GEOMETRIC_OBJECT

#include "../GeometricObject.hpp"

namespace NPlaneGeometricObject
{
    class PlaneGeometricObject: public NGeometricObjects::IGeometricObject
    {
    protected:
        NGeometricObjects::Color color;
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
};

#endif
