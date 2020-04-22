#pragma once
#include <cmath>
#include <vector>
#include <stdio.h>
#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include "ImfImage.h"
#include "ImfRgba.h"
#include "ImfRgbaFile.h"
#include "ImfOutputFile.h"
#include "ImfNamespace.h"
#include "ImfChromaticities.h"
#include "ImfStandardAttributes.h"

typedef Eigen::Matrix3f Mat33f;
typedef Eigen::Vector3f Vec3;

namespace OpenEXR_utils{

    /*
    sRGB chromaticities:
    ref url: http://www.color.org/chardata/rgb/srgb.xalter
            x       y       z
        r: 0.64   0.33   0.03
        g: 0.30   0.60   0.10
        b: 0.15   0.06   0.79
        w: 0.3127 0.3290 (D65)
    */
    inline Imf::Chromaticities getChromaticities_sRGB(){
        Imf::Chromaticities chroma;
        chroma.red = IMATH_NAMESPACE::V2f(0.6400f, 0.3300f);
        chroma.green = IMATH_NAMESPACE::V2f(0.3000f, 0.6000f);
        chroma.blue = IMATH_NAMESPACE::V2f(0.1500f, 0.0600f);
        chroma.white = IMATH_NAMESPACE::V2f(0.3127f, 0.3290f);
        return chroma;
    }


    /*
    ProPhoto chromaticities:
    ref url: http://www.color.org/chardata/rgb/rommrgb.xalter
            x       y       z
        r: 0.7347   0.2653   0
        g: 0.1596   0.8404   0
        b: 0.0366   0.0001   0.9633
        w: 0.3457 0.3585 (D50)
    */
    inline Imf::Chromaticities getChromaticities_ProPhoto(){
        Imf::Chromaticities chroma;
        chroma.red = IMATH_NAMESPACE::V2f(0.7347f, 0.2653f);
        chroma.green = IMATH_NAMESPACE::V2f(0.1596f, 0.8404f);
        chroma.blue = IMATH_NAMESPACE::V2f(0.0366f, 0.0001f);
        chroma.white = IMATH_NAMESPACE::V2f(0.3457f, 0.3585f);
        return chroma;
    }

    inline Imf::Chromaticities getChromaticities_NTSC(){
        Imf::Chromaticities chroma;
        chroma.red = IMATH_NAMESPACE::V2f(0.63f, 0.34f);
        chroma.green = IMATH_NAMESPACE::V2f(0.31f, 0.595f);
        chroma.blue = IMATH_NAMESPACE::V2f(0.155f, 0.07f);
        chroma.white = IMATH_NAMESPACE::V2f(0.3127f, 0.3290f);
        return chroma;
    }

    inline Imf::Chromaticities getChromaticities_eciRGB(){
        Imf::Chromaticities chroma;
        chroma.red = IMATH_NAMESPACE::V2f(0.67f, 0.33f);
        chroma.green = IMATH_NAMESPACE::V2f(0.21f, 0.71f);
        chroma.blue = IMATH_NAMESPACE::V2f(0.14f, 0.08f);
        chroma.white = IMATH_NAMESPACE::V2f(0.3457f, 0.3585f);
        return chroma;
    }

    /*
    linear sRGB (0..1) to nonlinear sRGB color space
    ref url: http://www.color.org/chardata/rgb/srgb.xalter

    C'=12.92 C  (C <= 0.0031308)
    C'=1.055 C ^(1/2.4) - 0.055   (C > 0.0031308)
    */
    inline float linearTosRGB(float u){
        if (u <= 0.0031308f) {
            return 12.92f * u;
        } else {
            return 1.055f * std::pow(u, 1/2.4) - 0.055f;
        }
    }

    /*
    linear sRGB (0..1) to nonlinear sRGB color space
    nonlinear sRGB (0..255)/255 to linear sRGB color space
    ref url: https://en.wikipedia.org/wiki/SRGB (the reverse transformation)
    C'=C / 12.92
    C'=(C+0.055)/1.055 ^ 2.4
    */
    inline float sRGBToLinear(float u){
        if (u <= 0.04045f) {
            return u / 12.92f;
        } else {
            return std::pow((u + 0.055f)/1.055f, 2.4f);
        }
    }

    /*
    linear sRGB (0..1) to nonlinear sRGB color space
    */
    inline float linearToProPhotoRGB(float u){
        if (u < 0) {return 0;}
        if (u >= 1) return 1;
        float Et = 0.001953125f;
        if (Et <= u < 1) return std::pow(u, 1/1.8);
        if (0 <= u < Et) return 16 * u;
    }
    
    /*
    sanity check: sRGB2XYZ:
        0.4124 0.3576 0.1805
        0.2126 0.7152 0.0722
        0.0193 0.1192 0.9505

    pass
    */
    Mat33f solveRGB2XYZ(Imf::Chromaticities &chroma);

    Mat33f getXYZ2LMS(); 
    
    Mat33f transfromationMatrixRGBToRGB(Imf::Chromaticities &chroma1, Imf::Chromaticities &chroma2);

    void writeEXR(const char filename[], Imf::Chromaticities &chroma,  Imf::Rgba *pixels, const int width, const int height);

    void writeEXR_float(const char filename[], Imf::Chromaticities &chroma, const float *r, const float *g, const float *b, const int width, const int height);

    void writeEXR_half(const char filename[], Imf::Chromaticities &chroma, const unsigned short int *r, const unsigned short int *g, const unsigned short int *b, const int width, const int height);

    void writeTIF(const char filename[], const float *r, const float *g, const float *b, const int width, const int height);
}