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
    
    //~ Screen screen = Screen(Point{70, 100, 70}, Point{70, 100, -70}, Point{-70, 100, 70}, 500, 500);
    //~ Point eye = {0, 100000, 0};
    Screen screen = Screen(Point{150, 150, 100}, Point{150, 0, 100}, Point{0, 150, 100}, 500, 500);
    Point eye = {0, 0, 100000};
    std::vector<NImageSettings::LightSource> sources;
    
    Point left_bottom_angle = screen.left_bottom_angle;
    Point left_upper = left_bottom_angle + screen.x_basis * screen.x_size;
    Point right_bottom = left_bottom_angle + screen.y_basis * screen.y_size;
    Point right_upper = left_bottom_angle + screen.y_basis * screen.y_size + screen.x_basis * screen.x_size;
    
    sources.push_back({2000, left_bottom_angle});
    sources.push_back({2000, left_upper});
    sources.push_back({2000, right_bottom});
    sources.push_back({2000, right_upper});
    sources.push_back({2000, (left_bottom_angle + right_upper) / 2});
    
    //~ NPainter::Painter painter("models/magnolia.stl", new NSTLParser::STLParser(eye, screen, sources));
    NPainter::Painter painter("models/teapot.stl", new NSTLParser::STLParser(eye, screen, sources));
    //~ NPainter::Painter painter;
    //~ freopen("log", "w", stderr);
    cerr << "painter created\n";
    //~ painter.paint("models/magnolia.png");
    painter.paint("models/teapot.png");
    
    auto end = std::chrono::system_clock::now();
    
    cerr << std::chrono::duration<double>(end - start).count() << "s\n";
    cerr << std::chrono::duration<double, std::ratio<60, 1> >(end - start).count() << "m\n";
    
    return 0;
}
