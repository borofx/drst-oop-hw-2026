#include "drst_image.hpp"
#include <fstream>
#include <stdexcept>

static const int TILES = 48;
static const int COL[] = {0,1,2,3, 0,3, 0,3, 0,1,2,3};
static const int ROW[] = {0,0,0,0, 1,1, 2,2, 3,3,3,3};

DRSTImage::DRSTImage(std::istream& in) {
    char magic[4];
    in.read(magic, 4);
    if (magic[0]!='D'||magic[1]!='R'||magic[2]!='S'||magic[3]!='T')
        throw std::runtime_error("Error: not DRST file");

    in.read((char*)&N, 4);    
    blocks = new Pixel[TILES * N * N];

    for (int t = 0; t < TILES; ++t){
        for (unsigned int i = 0; i < N*N; ++i) {
            char b[3];
            in.read(b, 3);
            blocks[t*N*N + i] = {(unsigned char)b[0], (unsigned char)b[1], (unsigned char)b[2]};
        }
    }        
}

DRSTImage::~DRSTImage() {
    delete[] blocks;
}

unsigned int DRSTImage::getSize() const {
    return 4 * N;
}

int DRSTImage::findTile(double x, double y) const {
    for (int i = 0; i < TILES; ++i) 
    {
        int ring = i/12, pos = i%12;
        double ts = 1.0 / (4.0 * (1<<ring));
        double ox = 0.5 - 2.0*ts;
        double tx = ox + COL[pos]*ts, ty = ox + ROW[pos]*ts;
        if (x>=tx && x<tx+ts && y>=ty && y<ty+ts)
            return i;
    }
    return -1;
}
