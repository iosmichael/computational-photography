#pragma once
#include <png.h>
#include <tiffio.h>
#include <Eigen/Dense>
#include <vector>
#include "ImfChromaticitiesAttribute.h"

typedef Eigen::Vector3d vec3;

struct png_utils {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep *row_pointers = nullptr;

    void read_png_file(char *filename);
    void write_png_file(char *filename);

};

struct tiff_utils {

    unsigned int width, height;

    void write_tiff_file(const char *filename, Imf::Chromaticities &chroma, unsigned char *image);
};

