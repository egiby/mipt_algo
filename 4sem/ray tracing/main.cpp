#include "Painter.hpp"
#include "parsers/STLParser.hpp"

#include <iostream>

using std::cout;
using std::cerr;

int main()
{
    NPainter::Painter painter("magnolia.stl", new NSTLParser::STLParser());
    //~ NPainter::Painter painter;
    cerr << "painter created\n";
    painter.paint("rgb.png");
    
    return 0;
}
