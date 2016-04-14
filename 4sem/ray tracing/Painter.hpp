#ifndef _PAINTER
#define _PAINTER

#include "GeometricObject.hpp"

#include <png++/png.hpp>

#include <vector>
#include <array>

class IFileParser;

struct Screen
{
    std::array<NGeometry::Point<Double>, 4> angles;
    size_t x_size, y_size;
};

struct ImageSettings
{
    NGeometry::Point<Double> eye;
    Screen screen;// ??
    std::vector<NGeometry::IGeometricObject*> objects;
};

class IFileParser
{
public:
    virtual ImageSettings * parseFile(string filename) = 0;
};

class DefaultParser: public IFileParser
{
public:
    ImageSettings * parseFile(string filename)
    {
    }
};

class Painter
{
    ImageSettings * settings;
public:
    explicit Painter(ImageSettings * settings)
    : settings(settings)
    {
    }
    
    explicit Painter(string filename, IFileParser * parser = makeDefaultParser())
    : settings(parser->parseFile(filename))
    {
    }
    
    png::image<png::rgb_pixel> paint()
    {
    }
};

#endif
