#include "MakeShape.hpp"
namespace renderer{
    Shape* CreateShape::CreateRectangle(int x, int y, int w, int h, int r, int g, int b, int a){
        Shape* shape = new Shape();
        shape->shape = {x, y, w, h};
        shape->color = {r,g,b,a};
        return shape;
    }
}