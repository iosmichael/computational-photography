#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>

#include "ImfImage.h"
#include "ImfRgba.h"
#include "ImfRgbaFile.h"
#include "utils.h"
#include "openexr_utils.h"

typedef struct Image {
    unsigned int width;
    unsigned int height;
    Vec3 *pixels;

    Image(const unsigned int width, const unsigned int height):width(width), height(height){
        std::cout << "allocating memory space for item no. :"<< height * width << std::endl;
        pixels = new Vec3[height * width];
    };
    ~Image(){
        delete[] pixels;
    };
} Image;
/*
problem one: 
    - expecting filename "macbeth.png", 3 channel image with 8 bit unsigned int, non-linear sRGB (chromaticities)
    - convert the non-linear to linear sRGB, 32 bit float per sample
    - save the file as "macbeth_sRGB.exr" 3 channels image
*/
void problemOne(int argc, char **argv);

/*
problem two:
    - calculate the chromaticities mapping from sRGB to ProPhoto (ROMM RGB)
    - expecting filename "macbeth.png", 3 channel image with 8 bit unsigned int, non-linear sRGB (chromaticities)
    - convert the non-linear to linear sRGB, 32 bit float per sample
    - convert the linear sRGB to linear ProPhoto, 32 bit float per sample
    - save one with chromaticities set to ProPhoto "macbeth_ProPhotoRGB.exr"
    - save one with chromaticities set to sRGB "macbeth_ProPhotoRGB_incorrect.exr"
*/
void problemTwo(int argc, char **argv);

/*
problem three:
    - calculate the chromaticities mapping from sRGB to ProPhoto
    - expecting filename "macbeth.png", 3 channel image with 8 bit unsigned int, non-linear sRGB (chromaticities)
    - convert the non-linear to linear sRGB, 32 bit float per sample
    - convert the linear sRGB to linear ProPhoto, 32 bit float per sample
    - coinvert the linear ProPhoto to non-linear ProPhoto RGB, 3 channel image with 16 bit unsigned int per sample
    - save the result in "macbeth_ProPhoto.tif"
*/
void problemThree(int argc, char **argv);

void sanityCheck();

int main(int argc, char ** argv){

    problemOne(argc, argv);

    problemTwo(argc, argv);

    problemThree(argc, argv);

    // sanityCheck();
    
    return 0;
}


void problemOne(int argc, char **argv){
    if (argc <= 1) {
        return;
    }
    char *imgPath = argv[1];
    png_utils pngImage;
    pngImage.read_png_file(imgPath);
    std::cout << "Image Size: (" << pngImage.height << " , " << pngImage.width << ") "<< std::endl;
    const int y = pngImage.width, x = pngImage.height;
    
    /* test code for saving png file
        =====================================
        std::string saveStr = "macbech_png.png";
        char savePath[saveStr.size() + 1];
        strcpy(savePath, saveStr.c_str());
        pngImage.write_png_file(savePath);
    */

    Image img(y, x);
    // row number
    for (unsigned int i = 0; i < x; i++){
        // col number
        png_bytep row = pngImage.row_pointers[i];
        for (unsigned int j = 0; j < y; j++){
            png_bytep pix = &(row[j * 4]);
            img.pixels[i * y + j] << (float) OpenEXR_utils::sRGBToLinear(pix[0]/255.0f), 
                                     (float) OpenEXR_utils::sRGBToLinear(pix[1]/255.0f),
                                     (float) OpenEXR_utils::sRGBToLinear(pix[2]/255.0f); // rgb
            
            /* test code for unchanged non-linear RGB:
            =============================================
                img.pixels[i * y + j] << pix[0]/255.0f, 
                                            pix[1]/255.0f,
                                            pix[2]/255.0f; // rgb
            =============================================
            */
        }
    }

    std::cout << "set chromaticities as sRGB" << std::endl;
    Imf::Chromaticities sRGB_chroma = OpenEXR_utils::getChromaticities_sRGB();
    
    //test if the float type will make it better
    float *r = new float[x * y];
    float *g = new float[x * y];
    float *b = new float[x * y];
    for (unsigned int i = 0; i < x; i++){
        for (unsigned int j = 0; j < y; j++){
            r[i * y + j] = img.pixels[i * y + j](0); // r
            g[i * y + j] = img.pixels[i * y + j](1); // r
            b[i * y + j] = img.pixels[i * y + j](2); // r
        }
    }
    OpenEXR_utils::writeEXR_float("macbeth_sRGB.exr", sRGB_chroma, r, g, b, y, x);
    delete []r,g,b;
}

void problemTwo(int argc, char **argv){
    if (argc <= 1) {
        return;
    }
    char *imgPath = argv[1];
    png_utils pngImage;
    pngImage.read_png_file(imgPath);
    std::cout << "Image Size: (" << pngImage.height << " , " << pngImage.width << ") "<< std::endl;

    const int y = pngImage.width, x = pngImage.height;
    
    Image img(y, x);
    // row number
    for (unsigned int i = 0; i < x; i++){
        // col number
        png_bytep row = pngImage.row_pointers[i];
        for (unsigned int j = 0; j < y; j++){
            png_bytep pix = &(row[j * 4]);
            img.pixels[i * y + j] << (float) OpenEXR_utils::sRGBToLinear(pix[0]/255.0f), 
                                     (float) OpenEXR_utils::sRGBToLinear(pix[1]/255.0f),
                                     (float) OpenEXR_utils::sRGBToLinear(pix[2]/255.0f); // rgb
        }
    }

    std::cout << "set sRGB as source chromaticities" << std::endl;
    Imf::Chromaticities source = OpenEXR_utils::getChromaticities_sRGB();
    std::cout << "set ProPhoto as source chromaticities" << std::endl;
    Imf::Chromaticities target = OpenEXR_utils::getChromaticities_ProPhoto();
    
    Mat33f transformationMatrix = OpenEXR_utils::transfromationMatrixRGBToRGB(source, target);
    std::cout << "chromaticities matrix from sRGB to ProPhoto " << transformationMatrix << std::endl;


    for (unsigned int i = 0; i < x * y; i++){
        img.pixels[i] = transformationMatrix * img.pixels[i];
    }

    //test if the float type will make it better
    float *r = new float[x * y];
    float *g = new float[x * y];
    float *b = new float[x * y];
    for (unsigned int i = 0; i < x; i++){
        for (unsigned int j = 0; j < y; j++){
            r[i * y + j] = img.pixels[i * y + j](0); // r
            g[i * y + j] = img.pixels[i * y + j](1); // r
            b[i * y + j] = img.pixels[i * y + j](2); // r
        }
    }

    std::cout << "Write image with correct chromaticities" << std::endl;
    OpenEXR_utils::writeEXR_float("macbeth_ProPhoto.exr", target, r, g, b, y, x);

    std::cout << "Write image with incorrect chromaticities" << std::endl;
    OpenEXR_utils::writeEXR_float("macbeth_ProPhoto_incorrect.exr", source, r, g, b, y, x);

    delete []r,g,b;
}

void problemThree(int argc, char **argv){
    if (argc <= 1) {
        return;
    }
    char *imgPath = argv[1];
    png_utils pngImage;
    pngImage.read_png_file(imgPath);
    std::cout << "Image Size: (" << pngImage.height << " , " << pngImage.width << ") "<< std::endl;

    const int y = pngImage.width, x = pngImage.height;
    
    Image img(y, x);
    // row number
    for (unsigned int i = 0; i < x; i++){
        // col number
        png_bytep row = pngImage.row_pointers[i];
        for (unsigned int j = 0; j < y; j++){
            png_bytep pix = &(row[j * 4]);
            img.pixels[i * y + j] << (float) OpenEXR_utils::sRGBToLinear(pix[0]/255.0f), 
                                     (float) OpenEXR_utils::sRGBToLinear(pix[1]/255.0f),
                                     (float) OpenEXR_utils::sRGBToLinear(pix[2]/255.0f); // rgb
        }
    }

    std::cout << "set sRGB as source chromaticities" << std::endl;
    Imf::Chromaticities source = OpenEXR_utils::getChromaticities_sRGB();
    std::cout << "set ProPhoto as source chromaticities" << std::endl;
    Imf::Chromaticities target = OpenEXR_utils::getChromaticities_ProPhoto();
    
    Mat33f transformationMatrix = OpenEXR_utils::transfromationMatrixRGBToRGB(source, target);
    std::cout << "chromaticities matrix from sRGB to ProPhoto \n" << transformationMatrix << std::endl;

    for (unsigned int i = 0; i < x * y; i++){
        img.pixels[i] = transformationMatrix * img.pixels[i];
    }
    
    //test if the float type will make it better
    float *r = new float[x * y];
    float *g = new float[x * y];
    float *b = new float[x * y];
    for (unsigned int i = 0; i < x; i++){
        for (unsigned int j = 0; j < y; j++){
            r[i * y + j] = (float) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](0)); // r
            g[i * y + j] = (float) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](1)); // g
            b[i * y + j] = (float) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](2)); // b
        }
    }
    // OpenEXR_utils::writeEXR_float("macbeth_prophoto_tif.exr", target, r, g, b, y, x);
    // std::cout << "Write exr image non-linear ProPhoto with chromaticities" << std::endl;
    OpenEXR_utils::writeTIF("macbeth_ProPhoto.tif", r, g, b, y, x);

    // delete []r,g,b;

    /* convert Image to array of RGBA objects and write to TIFF image format
    ========================================================================
    unsigned short int *writePix = new unsigned short int[x * y * 3];
    for (unsigned int i = 0; i < x; i++){
        for (unsigned int j = 0; j < y; j++){
            writePix[i * y * 3 + 3 * j] = (unsigned short int) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](0)) * 255.0f; // r
            writePix[i * y * 3 + 3 * j + 1] = (unsigned short int) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](1)) * 255.0f; // g
            writePix[i * y * 3 + 3 * j + 2] = (unsigned short int) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](2)) * 255.0f;// b
        }
    }

    std::cout << "finish writing pixels with unsigned short int, " << sizeof(unsigned short int) * 8 << " bits per sample." << std::endl;

    tiff_utils writer;
    writer.width = y;
    writer.height = x;
    writer.write_tiff_file("macbeth_ProPhoto.tif", target, writePix);
    */
    // unsigned char *writePix = new unsigned char[x * y * 3];
    // for (unsigned int i = 0; i < x; i++){
    //     for (unsigned int j = 0; j < y; j++){
    //         writePix[i * y * 3 + 3 * j] = (unsigned char) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](0)) * 255.0f; // r
    //         writePix[i * y * 3 + 3 * j + 1] = (unsigned char) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](1)) * 255.0f; // g
    //         writePix[i * y * 3 + 3 * j + 2] = (unsigned char) OpenEXR_utils::linearToProPhotoRGB(img.pixels[i * y + j](2)) * 255.0f;// b
    //     }
    // }

    // std::cout << "finish writing pixels with unsigned short int, " << sizeof(unsigned char) * 8 << " bits per sample." << std::endl;

    // tiff_utils writer;
    // writer.width = y;
    // writer.height = x;
    // writer.write_tiff_file("macbeth_ProPhoto.tif", target, writePix);
}

void sanityCheck(){
    Imf::Chromaticities source = OpenEXR_utils::getChromaticities_sRGB();
    Imf::Chromaticities target = OpenEXR_utils::getChromaticities_ProPhoto();
    Mat33f transformationMatrix = OpenEXR_utils::transfromationMatrixRGBToRGB(source, target);
    std::cout << "transformation matrix from source chromaticities to target chromaticies: sRGB -> ProPhoto" << std::endl;
    std::cout << transformationMatrix << std::endl;

    Mat33f t1;
    t1 << 0.7976749,  0.1351917, 0.0313534, 0.2880402, 0.7118741, 0.0000857, 0.0000000, 0.0000000, 0.8252100;
    Mat33f t2;
    t2 << 0.4360747, 0.3850649, 0.1430804, 0.2225045, 0.7168786, 0.0606169, 0.0139322, 0.0971045, 0.7141733;
    std::cout << t1.inverse() * t2 << std::endl;

}