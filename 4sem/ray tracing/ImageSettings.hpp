#ifndef _IMAGE_SETTINGS
#define _IMAGE_SETTINGS

#include "geometry/Double.hpp"
#include "geometry/Geometry.hpp"
#include "objects/GeometricObject.hpp"

#include <vector>

namespace NImageSettings
{
    using NGeometry::Point;
    using NGeometry::Vector;
    
    struct Screen
    {
        ui32 x_size, y_size;
        Point left_bottom_angle;
        Vector x_basis;
        Vector y_basis;
        
        Screen(const Point &left_bottom_angle, const Point &left_upper, const Point &right_bottom, 
               ui32 x_size = 500, ui32 y_size = 500)
        : x_size(x_size), y_size(y_size), left_bottom_angle(left_bottom_angle), 
        x_basis((right_bottom - left_bottom_angle) / x_size), y_basis((left_upper - left_bottom_angle) / y_size)
        {
        }
    };
    
    struct LightSourse
    {
        NDouble::Double light_forse;
        Point position;
    };
    
    struct ImageSettings
    {
        Point eye;
        Screen screen;
        std::vector<NGeometricObjects::IGeometricObject*> objects;
        
        ~ImageSettings()
        {
            for (auto object: objects)
                delete object;
        }
    };
    
    const Point DEFAULT_EYE = {0, -1000000., 0};
    const Screen DEFAULT_SCREEN = Screen(Point{0, -10, -10}, Point{0, -10, 10}, Point{0, 10, -10});
};
#endif
