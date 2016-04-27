#ifndef _PAINTER
#define _PAINTER

#include "GeometricObject.hpp"
#include "objects/Triangle.hpp"
#include "objects/Parallelogram.hpp"

#include <png++/png.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include <cstdlib>

namespace NPainter
{
    using namespace NGeometry;
    using namespace NGeometricObjects;
    using std::cerr;
    
    struct Screen
    {
        Point left_bottom_angle = {3, 0, 3};
        Vector x_basis = {-0.01, 0, 0};
        Vector y_basis = {0, 0, 0.04};
        ui32 x_size = 500, y_size = 500;
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
    
    const Point DEFAULT_EYE = {0, -1000000., 0};
    
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
            
            return new ImageSettings{eye, screen, objects};
        }
    };
    
    class STLParser: public IFileParser
    {
        Point readVertex(std::ifstream &in)
        {
            std::string word; 
            in >> word;
            //~ cerr << word << '\n';
            assert(word == "vertex");
            
            Point p;
            in >> p;
            return p;
        }
        
    public:
        ImageSettings * parseFile(std::string filename)
        {
            Point eye = DEFAULT_EYE;
            Screen screen;
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
                    
                    Color color = {(ui32)(rand() % 256), (ui32)(rand() % 256), (ui32)(rand() % 256)};
                    
                    objects.push_back(new NTriangle::Triangle(color, v1, v2 - v1, v3 - v1));
                }
            }
            
            return new ImageSettings{eye, screen, objects};
        }
    };
    
    const std::string DEFAULT_OUTPUT_FILE = "rgb.png";
    const std::string DEFAULT_INPUT_FILE = "settings.in";
    const Double MAX_COORDINATE = 1e20;
    
    class Painter;
    
    void paintPart(ui32 first, ui32 last, png::image<png::rgb_pixel> &image, Painter &painter);
    
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
            auto number_threads = std::thread::hardware_concurrency() + 1;
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
        for (ui32 x = first; x < last; ++x)
            for (ui32 y = 0; y < painter.settings->screen.y_size; ++y)
            {
                Point pixel = painter.calcPixelCenter(x, y);
                Ray ray(painter.settings->eye, pixel - painter.settings->eye);
                
                image[x][y] = painter.intersectAll(ray);
            }
    }
};

#endif
