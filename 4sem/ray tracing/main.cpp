#include "Painter.hpp"

#include <iostream>

using std::cout;
using std::cerr;

int main()
{
    NPainter::Painter painter("humanoid.stl", new NPainter::STLParser());
    //~ NPainter::Painter painter;
    cerr << "painter created\n";
    painter.paint();
    
    //~ cerr << (NGeometry::Vector{1, 0, 0} ^ NGeometry::Vector{0, 0, 1}) << '\n';
    
    return 0;
}
