#include <complex>
#include <fstream>
#include "drst_image.hpp"

Position invert(Position pos) {
    return {1.0 - pos.x, 1.0 - pos.y};
}

Position logRemap(Position pos) {
    using namespace std::complex_literals;
    std::complex<double> z(pos.x - 0.5, pos.y - 0.5);
    z = std::log(z);
    auto c = 0.915625 * std::exp(-0.41625i);
    auto z0 = -1.1 - 3.8i;
    z = (z - z0) / c + z0;
    z = std::exp(z);
    return {z.real() + 0.5, z.imag() + 0.5};
}

int main() {
    std::ifstream input("cats.drst", std::ios::binary);
    DRSTImage img(input);

    img.saveAsPPM("original.ppm", 2048);
    img.saveAsPPM("inverted.ppm", 2048, invert);
    img.saveAsPPM("twisted.ppm", 2048, logRemap);
}