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

Pixel DRSTImage::getPixel(int idx, int x, int y) const {
    return blocks[idx*N*N + y*N + x];
}

Pixel DRSTImage::getPixel(double x, double y) const {
    
    for (int depth = 0; depth < 8; ++depth) 
    {
        int tile = findTile(x, y);
        if (x<0.0||x>=1.0||y<0.0||y>=1.0) 
        {
            break;
        }
        if (tile >= 0) 
        {
            int ring = tile/12, pos = tile%12;
            double ts = 1.0 / (4.0 * (1<<ring));
            double ox = 0.5 - 2.0*ts;
            double lx = (x - (ox + COL[pos]*ts)) / ts;
            double ly = (y - (ox + ROW[pos]*ts)) / ts;
            int px = (int)(lx*N); if (px<0) px=0; if (px>=(int)N) px=N-1;
            int py = (int)(ly*N); if (py<0) py=0; if (py>=(int)N) py=N-1;
            return getPixel(tile, px, py);
        }
        
        x = (x-0.5)*16.0 + 0.5;
        y = (y-0.5)*16.0 + 0.5;
    }
    return {0, 0, 0};
}

void DRSTImage::saveAsPPM(const std::string& filename, size_t size) const {
    saveAsPPM(filename, size, [](Position p){ return p; });
}

void DRSTImage::saveAsPPM(const std::string& filename, size_t size, std::function<Position(Position)> phi) const {
    std::ofstream out(filename, std::ios::binary);
    if (!out) throw std::runtime_error("cannot open " + filename);

    out << "P6\n" << size << " " << size << "\n255\n";
    for (size_t row = 0; row < size; ++row)
    {    
        for (size_t col = 0; col < size; ++col) 
        {
            Position mapped = phi({(col+0.5)/size, (row+0.5)/size});
            Pixel p = getPixel(mapped.x, mapped.y);
            char rgb[3] = {(char)p.r, (char)p.g, (char)p.b};
            out.write(rgb, 3);
        }
    }
}
