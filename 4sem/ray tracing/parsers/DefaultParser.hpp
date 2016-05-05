#ifndef _DEFAULT_PARSER
#define _DEFAULT_PARSER

#include "IFileParser.hpp"

#include <iostream>
#include <fstream>

/*
 * language:
 * <point> = <double> <double> <double>
 * <color> = <uint8_t> <uint8_t> <uint8_t>
 * <size> = <size_t> <size_t>
 * <eye> = "eye" <point>
 * <screen> = "screen" [
 * (left bottom angle)<point>
 * (left upper angle)<point>
 * (right bottom angle)<point>
 * (size)<size>]
 * <triangle> = "triangle" [
 * (vertex)<point>
 * (side_1)<vector>
 * (side_2)<vector>
 * (rgb)<color>]
 * <parallelogram> = "parallelogram" [
 * (vertex)<point>
 * (side_1)<vector>
 * (side_2)<vector>
 * (rgb)<color>]
 */

namespace NDefaultParser
{
    using NImageSettings::ImageSettings;
    using NImageSettings::Screen;
    using NImageSettings::LightSource;
    
    using NGeometry::Point;
    using NGeometry::Vector;
    
    using NGeometricObjects::IGeometricObject;
    using NGeometricObjects::Color;
    
    class DefaultParser: public IFileParser
    {
    public:
        ImageSettings * parseFile(std::string filename)
        {
            Point eye = NImageSettings::DEFAULT_EYE;
            Screen screen = NImageSettings::DEFAULT_SCREEN;
            std::vector<IGeometricObject*> objects;
            std::vector<LightSource> sources;
            
            std::ifstream in(filename);
            
            std::string word;
            while (in >> word)
            {
                if (word == "screen")
                {
                    Point left_bottom, left_upper, right_bottom;
                    ui32 x_size, y_size;
                    in >> left_bottom >> left_upper >> right_bottom;
                    in >> x_size >> y_size;
                    
                    screen = Screen(left_bottom, left_upper, right_bottom, x_size, y_size);
                }
                
                if (word == "eye")
                {
                    in >> eye;
                }
                
                if (word == "triangle")
                {
                    Point vertex;
                    Vector side_1, side_2;
                    in >> vertex >> side_1 >> side_2;
                    
                    Color c;
                    in >> c.red >> c.green >> c.blue;
                    
                    objects.push_back(new NTriangle::Triangle(c, vertex, side_1, side_2));
                }
                
                if (word == "parallelogram")
                {
                    Point vertex;
                    Vector side_1, side_2;
                    in >> vertex >> side_1 >> side_2;
                    
                    Color c;
                    in >> c.red >> c.green >> c.blue;
                    
                    objects.push_back(new NParallelogram::Parallelogram(c, vertex, side_1, side_2));
                }
                // to be continued...
            }
            
            if (sources.empty())
            {
                Vector normal = screen.x_basis ^ screen.y_basis;
                Point center = screen.x_basis * (screen.x_size / 2.0) + screen.y_basis * (screen.y_size / 2.0);
                
                cerr << normal << '\n';
                
                LightSource source = {1000, center - normal * ((screen.x_size + screen.y_size)) * 10};
                sources.push_back(source);
                cerr << source.light_force << '\n' << source.point << '\n';
                //~ cerr << eye << '\n';
            }
            
            return new ImageSettings{eye, screen, objects, sources};
        }
    };
};

#endif
