#include "utils.h"
#include <memory>
#include <iostream>

void png_utils::read_png_file(char *filename) {
    FILE *fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) return;

    png_infop info = png_create_info_struct(png);
    if(!info) return;

    if(setjmp(png_jmpbuf(png))) return;

    png_init_io(png, fp);

    png_read_info(png, info);

    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(bit_depth == 16)
        png_set_strip_16(png);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    if (row_pointers) return;

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);
}

void png_utils::write_png_file(char *filename) {
  int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
}


void tiff_utils::write_tiff_file(const char *filename, Imf::Chromaticities &chroma, unsigned char *image){

    TIFF *out = TIFFOpen(filename, "w");
    int sampleperpixel = 3; // no alpha channel
    // unsigned int *image = new unsigned int[width * height * sampleperpixel];

    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8); // 8 bit unsigned int per sample
    TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);    // set the origin of the image.
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, 1);

    // TIFFSetField(out, TIFFTAG_PRIMARYCHROMATICITIES, chroma.red.x, chroma.red.y, chroma.green.x, chroma.green.y, chroma.blue.x, chroma.blue.y);
    std::cout << "setting file header" << std::endl;
    tsize_t linebytes = sampleperpixel * width;

    unsigned char *buf = nullptr;

    if (TIFFScanlineSize(out) == linebytes){
        buf = (unsigned char *)_TIFFmalloc(linebytes);
    } else {
        std::cout << "line mismatched with linebtyes" << std::endl;
        buf = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(out));
    }
    //Now writing image to the file one strip at a time
    unsigned char* pdst = new unsigned char[(width * 3)];
    unsigned int src_index, dst_index;
    // // now go line by line to write out the image data
    for (int row = 0; row < height; row++ ){

        // initialize the scan line to zero
        memset(pdst,0,(size_t)(width * 3));

        // moving the data from the dib to a row structure that
        // can be used by the tiff library
        for (int col = 0; col < width; col++){
            src_index = (height - row - 1) * width * sizeof(unsigned char)
                                        + col * sizeof(unsigned char);
            dst_index = col * 3;
            pdst[dst_index++] = image[src_index+2];
            pdst[dst_index++] = image[src_index+1];
            pdst[dst_index] = image[src_index];
        }

        // now actually write the row data
        TIFFWriteScanline(out, pdst, row, 0);
    }

    // for (uint32 row = 0; row < height; row++)
    // {
    //     memcpy(buf, &image[(height-row-1) * linebytes], linebytes);
    //     if (TIFFWriteScanline(out, buf, row, 0) < 0)
    //     break;
    // }
    (void) TIFFClose(out);
    if (buf) _TIFFfree(buf);
}