#ifndef _PAINTER
#define _PAINTER

#include "geometry/Double.hpp"
#include "geometry/Geometry.hpp"

#include "objects/GeometricObject.hpp"
#include "objects/Triangle.hpp"
#include "objects/Parallelogram.hpp"

#include "parsers/DefaultParser.hpp"

#include "calculations/Intersecter.hpp"

#include <png++/png.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <future>

#include <cstdlib>
#include <cmath>

namespace NPainter
{
    using NGeometry::Point;
    using NGeometry::Vector;
    using NGeometry::Ray;
    
    using NDouble::Double;
    
    using NGeometricObjects::IGeometricObject;
    using NGeometricObjects::Color;
    
    using NImageSettings::ImageSettings;
    
    using NDefaultParser::DefaultParser;
    
    using std::cerr;
    
    const std::string DEFAULT_OUTPUT_FILE = "rgb.png";
    const std::string DEFAULT_INPUT_FILE = "settings.in";
    
    png::rgb_pixel makePixel(const Color &c)
    {
        return png::rgb_pixel(c.red, c.green, c.blue);
    }
    
    class Painter;
    
    void paintPart(ui32 first, ui32 last, png::image<png::rgb_pixel> &image, Painter &painter);
    
    png::rgb_pixel calcColor(const Intersection &result, const ImageSettings * settings)
    {
        if (result.object == nullptr)
            return png::rgb_pixel(0, 0, 0);
        
        Double illuminance = 0.;
        //~ Double illuminance = 1;
        
        for (const auto &source: settings->light_sources)
        {
            Ray light_ray = Ray(source.point, result.point - source.point);
            auto intersection = intersectAll(light_ray, settings);
            
            if (intersection.object != result.object)
                continue;
            
            Vector radius = -(result.point - source.point);
            Vector normal = result.object->getNormal(result.point);
            
            
            illuminance += source.light_force * std::fabs(radius * normal) / 
                           (std::pow(abs(radius), 3) * abs(normal));
        }
        
        //~ if (illuminance == Double(0.1))
            //~ cerr << result.point << '\n';
        
        return makePixel(result.object->getColor() * illuminance);
    }
    
    class Painter
    {
    protected:
        ImageSettings * settings;
        
        Point calcPixelCenter(ui32 x, ui32 y)
        {
            return settings->screen.left_bottom_angle + settings->screen.x_basis * (x + 0.5) + 
                   settings->screen.y_basis * (y + 0.5);
        }
        
    public:
        explicit Painter(ImageSettings * settings)
        : settings(settings)
        {
        }
        
        explicit Painter(std::string filename = DEFAULT_INPUT_FILE, IFileParser * parser = new DefaultParser())
        : settings(parser->parseFile(filename))
        {
            delete parser;
        }
        
        virtual void paint(std::string result_file = DEFAULT_OUTPUT_FILE)
        {
        #ifndef _DEBUG
            auto number_threads = std::thread::hardware_concurrency() + 1;
        #else
            ui32 number_threads = 1; // debug mode
        #endif
            png::image<png::rgb_pixel> image(settings->screen.x_size, settings->screen.y_size);
            
            std::vector<std::thread> t(number_threads);
            
            for (ui32 k = 0; k < number_threads; ++k)
            {
                ui32 first =  k * (settings->screen.x_size / number_threads);
                ui32 last = (k + 1) * (settings->screen.x_size / number_threads);
                if (k + 1 == number_threads)
                    last = settings->screen.x_size;
                
                t[k] = std::thread(paintPart, first, last, std::ref(image), std::ref(*this));
            }
            
            for (ui32 i = 0; i < number_threads; ++i)
                t[i].join();
            
            image.write(result_file);
        }
        
        friend void paintPart(ui32 first, ui32 last, png::image<png::rgb_pixel> &image, Painter &painter);
        
        ~Painter()
        {
            delete settings;
        }
    };
    
    void paintPart(ui32 first, ui32 last, png::image<png::rgb_pixel> &image, Painter &painter)
    {
        //~ cerr << first << ' ' << last << std::endl;
        //~ cerr << painter.settings->screen.y_size << std::endl;
        for (ui32 x = first; x < last; ++x)
            for (ui32 y = 0; y < painter.settings->screen.y_size; ++y)
            {
                Point pixel = painter.calcPixelCenter(x, y);
                Ray ray(painter.settings->eye, pixel - painter.settings->eye);
                
                //~ cerr << x << ' ' << y << std::endl;
                image[y][x] = calcColor(intersectAll(ray, painter.settings), painter.settings);
            }
        //~ cerr << "done\n";
    }
};

#endif
