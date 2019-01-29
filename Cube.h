#pragma once

// This class can be used to represent a cube object.
// The object has dimensions in x, y and z direction.
// Dimensions are in meter
class Cube
{
public:
    Cube(double iX, double iY, double    iZ)
        : _X(iX)
        , _Y(iY)
        , _Z(iZ)
    {
    }
    ~Cube() = default;

    double GetX() const {return _X;}
    double GetY() const {return _Y;}
    double GetZ() const {return _Z;}
private:
    double _X;
    double _Y;
    double _Z;
};

