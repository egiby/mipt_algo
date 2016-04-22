#ifndef _PAINTER
#define _PAINTER

#include "GeometricObject.hpp"

#include <png++/png.hpp>

#include <iostream>
#include <vector>
#include <string>

namespace NPainter
{
    using namespace NGeometry;
    using namespace NGeometricObjects;
    using std::cerr;
    
    struct Screen
    {
        Point left_bottom_angle = {0, -10, -10};
        Vector x_basis = {0, 0.01, 0};
        Vector y_basis = {0, 0, 0.01};
        ui32 x_size = 2000, y_size = 2000;
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
    
    class IFileParser
    {
    public:
        virtual ImageSettings * parseFile(std::string filename) = 0;
        virtual ~IFileParser()
        {
        }
    };
    
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
    
    const Point DEFAULT_EYE = {-1., 0., 0.};
    
    class DefaultParser: public IFileParser
    {
    public:
        ImageSettings * parseFile(std::string filename)
        {
            Point eye = DEFAULT_EYE;
            Screen screen;
            
            std::vector<IGeometricObject*> objects;
            
            std::ifstream in(filename);
            
            std::string word;
            while (in >> word)
            {
                if (word == "screen")
                {
                    Point left_upper, right_bottom;
                    in >> screen.left_bottom_angle;
                    in >> left_upper >> right_bottom;
                    in >> screen.x_size >> screen.y_size;
                    
                    screen.x_basis = (-screen.left_bottom_angle + right_bottom) / screen.x_size;
                    screen.y_basis = (-screen.left_bottom_angle + left_upper) / screen.y_size;
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
                    
                    objects.push_back(new Triangle(c, vertex, side_1, side_2));
                }
                
                if (word == "parallelogram")
                {
                    Point vertex;
                    Vector side_1, side_2;
                    in >> vertex >> side_1 >> side_2;
                    
                    Color c;
                    in >> c.red >> c.green >> c.blue;
                    
                    objects.push_back(new Parallelogram(c, vertex, side_1, side_2));
                }
                // to be continued...
            }
            
            return new ImageSettings{eye, screen, objects};
        }
    };
    
    const std::string DEFAULT_OUTPUT_FILE = "rgb.png";
    const std::string DEFAULT_INPUT_FILE = "settings.in";
    const Double MAX_COORDINATE = 1e20;
    
    class Painter
    {
    protected:
        ImageSettings * settings;
        
        Point calcPixelCenter(ui32 x, ui32 y)
        {
            return settings->screen.left_bottom_angle + settings->screen.x_basis * (x + 0.5) + 
                   settings->screen.y_basis * (y + 0.5);
        }
        
        png::rgb_pixel intersectAll(const Ray &ray)
        {
            Double min_coefficient = MAX_COORDINATE;
            NGeometricObjects::IGeometricObject * first_object = 0;
            
            for (auto object: settings->objects)
            {
                Double coefficient = object->intersect(ray);
                
                if (coefficient < Double(0.))
                    continue;
                
                if (coefficient < min_coefficient)
                {
                    first_object = object;
                    min_coefficient = coefficient;
                }
            }
            
            if (first_object)
                return png::rgb_pixel(first_object->getRed(), first_object->getGreen(), first_object->getBlue());
            
            return png::rgb_pixel(0, 0, 0);
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
            png::image<png::rgb_pixel> image(settings->screen.x_size, settings->screen.y_size);
            
            for (ui32 x = 0; x < settings->screen.x_size; ++x)
                for (ui32 y = 0; y < settings->screen.y_size; ++y)
                {
                    Point pixel = calcPixelCenter(x, y);
                    Ray ray(settings->eye, pixel - settings->eye);
                    
                    image[x][y] = intersectAll(ray);
                }
            
            image.write(result_file);
        }
        
        ~Painter()
        {
            delete settings;
        }
    };
};

#endif
