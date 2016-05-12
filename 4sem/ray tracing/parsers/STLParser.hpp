#ifndef _STL_PARSER
#define _STL_PARSER

#include "IFileParser.hpp"

#include <iostream>
#include <fstream>

#include <cassert>

namespace NSTLParser
{
    using NImageSettings::ImageSettings;
    using NImageSettings::Screen;
    
    using NGeometry::Point;
    using NGeometry::Vector;
    
    using NGeometricObjects::IGeometricObject;
    using NGeometricObjects::Color;
    using NImageSettings::LightSource;
    
    class STLParser: public IFileParser
    {
        Point readVertex(std::ifstream &in)
        {
            std::string word; 
            in >> word;
            assert(word == "vertex");
            
            Point p;
            in >> p;
            return p;
        }
        
        std::vector<LightSource> sources;
        Point eye;
        Screen screen;
        
    public:
        STLParser (Point eye = NImageSettings::DEFAULT_EYE,
                   Screen screen = NImageSettings::DEFAULT_SCREEN,
                   std::vector<LightSource> sources = std::vector<LightSource>())
        : sources(sources), eye(eye), screen(screen)
        {
            //~ cerr << "parser created\n";
        }
        
        ImageSettings * parseFile(std::string filename)
        {
            std::vector<IGeometricObject*> objects;
            srand(501);
            
            std::ifstream in(filename);
            
            std::string word;
            while (in >> word)
            {
                if (word == "vertex")
                {
                    Point v1, v2, v3;
                    
                    in >> v1;
                    v2 = readVertex(in);
                    v3 = readVertex(in);
                    
                    //~ cerr << v1 << '\n' << v2 << '\n' << v3 << '\n';
                    //~ cerr << '\n';
                    
                    Color color = {0, 0, 128};
                    
                    objects.push_back(new NTriangle::Triangle(color, v1, v2 - v1, v3 - v1));
                }
            }
            
            if (sources.empty())
            {
                Vector normal = screen.x_basis ^ screen.y_basis;
                
                Point left_bottom_angle = screen.left_bottom_angle;
                Point left_upper = left_bottom_angle + screen.x_basis * screen.x_size;
                Point right_bottom = left_bottom_angle + screen.y_basis * screen.y_size;
                Point right_upper = left_bottom_angle + screen.y_basis * screen.y_size + screen.x_basis * screen.x_size;
                
                //~ LightSource source = {1000, left_bottom_angle - normal * ((screen.x_size + screen.y_size) / 2.0)};
                //~ sources.push_back(source);
                
                sources.push_back({300, left_bottom_angle - normal * ((screen.x_size + screen.y_size) / 2.0)});
                sources.push_back({300, left_upper - normal * ((screen.x_size + screen.y_size) / 2.0)});
                sources.push_back({300, right_bottom - normal * ((screen.x_size + screen.y_size) / 2.0)});
                sources.push_back({300, right_upper - normal * ((screen.x_size + screen.y_size) / 2.0)});
                //~ cerr << source.light_force << '\n' << source.point << '\n';
                //~ cerr << eye << '\n';
            }
            
            return new ImageSettings{eye, screen, objects, sources};
        }
    };
};

#endif
