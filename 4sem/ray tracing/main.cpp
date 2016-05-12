#include "Painter.hpp"
#include "parsers/STLParser.hpp"

#include <iostream>
#include <chrono>

using std::cout;
using std::cerr;

using NGeometry::Point;
using NGeometry::Vector;
using NImageSettings::Screen;

int main()
{
    //~ cerr << "1\n";
    auto start = std::chrono::system_clock::now();

    
    Screen screen = Screen(Point{100, 80, 80}, Point{100, 0, 80}, Point{100, 80, 0}, 1000, 1000);
    Point eye = {100000, 0, 0};
    std::vector<NImageSettings::LightSource> sources;
    
    Point left_bottom_angle = screen.left_bottom_angle;
    Point left_upper = left_bottom_angle + screen.x_basis * screen.x_size;
    Point right_bottom = left_bottom_angle + screen.y_basis * screen.y_size;
    Point right_upper = left_bottom_angle + screen.y_basis * screen.y_size + screen.x_basis * screen.x_size;
    
    sources.push_back({1000, left_bottom_angle});
    sources.push_back({1000, left_upper});
    sources.push_back({1000, right_bottom});
    sources.push_back({1000, right_upper});
    sources.push_back({1000, (left_bottom_angle + right_upper) / 2});
    
    //~ NPainter::Painter painter("magnolia.stl", new NSTLParser::STLParser(eye, screen, sources));
    NPainter::Painter painter("sphere.stl", new NSTLParser::STLParser(eye, screen, sources));
    //~ NPainter::Painter painter;
    //~ freopen("log", "w", stderr);
    cerr << "painter created\n";
    painter.paint("rgb2.png");
    
    auto end = std::chrono::system_clock::now();
    
    cerr << std::chrono::duration<double>(end - start).count() << "s\n";
    cerr << std::chrono::duration<double, std::ratio<60, 1> >(end - start).count() << "m\n";
    
    return 0;
}
