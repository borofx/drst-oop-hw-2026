#pragma once
#include <functional>
#include <iostream>
#include <string>

struct Pixel {
    unsigned char r, g, b;
};

struct Position {
    double x, y;
};

class DRSTImage {
public:
    DRSTImage(std::istream& in);
    unsigned int getSize() const;

    Pixel getPixel(int imageIndex, int x, int y) const;
    Pixel getPixel(double x, double y) const;

    void saveAsPPM(const std::string& filename, size_t size) const;
    void saveAsPPM(const std::string& filename, size_t size, std::function<Position(Position)> phi) const;
    
    ~DRSTImage();
    DRSTImage(const DRSTImage&) = delete;
    DRSTImage& operator=(const DRSTImage&) = delete;

private:
    unsigned int N;
    Pixel* blocks;
    int findTile(double x, double y) const;
};
