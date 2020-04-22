#include "openexr_utils.h"

namespace OpenEXR_utils {

    Mat33f transfromationMatrixRGBToRGB(Imf::Chromaticities &chroma1, Imf::Chromaticities &chroma2){
        if (chroma1.white.x == chroma2.white.x && chroma1.white.y == chroma2.white.y){
            // if the white points are the same
            Mat33f mRGB1ToXYZ = solveRGB2XYZ(chroma1);
            Mat33f mRGB2ToXYZ = solveRGB2XYZ(chroma2);
            Mat33f mRGB1ToRGB2 = mRGB2ToXYZ.inverse() * mRGB1ToXYZ;
            return mRGB1ToRGB2;
        } else {
            // white point are not the same, perform adaptation
            Vec3 whiteXYZ1 (chroma1.white.x / chroma1.white.y, 1, (1-chroma1.white.x-chroma1.white.y) / chroma1.white.y);
            Vec3 whiteXYZ2 (chroma2.white.x / chroma2.white.y, 1, (1-chroma2.white.x-chroma2.white.y) / chroma2.white.y);

            Mat33f mRGB1ToXYZ = solveRGB2XYZ(chroma1);
            std::cout << "source RGB2XYZ: " << std::endl;
            // std::cout << mRGB1ToXYZ << std::endl;
            Mat33f mRGB2ToXYZ = solveRGB2XYZ(chroma2);
            std::cout << "target RGB2XYZ: " << std::endl;
            // std::cout << mRGB2ToXYZ << std::endl;
            // std::cout << mRGB2ToXYZ.inverse() << std::endl;
            Mat33f mXYZToLMS = getXYZ2LMS();

            //perform white point adaptation
            Vec3 whiteLMS1 = mXYZToLMS * whiteXYZ1;
            Vec3 whiteLMS2 = mXYZToLMS * whiteXYZ2;
            Mat33f adaptationMat = Mat33f::Zero();
            adaptationMat(0,0) = whiteLMS2(0)/whiteLMS1(0);
            adaptationMat(1,1) = whiteLMS2(1)/whiteLMS1(1);
            adaptationMat(2,2) = whiteLMS2(2)/whiteLMS1(2);
            std::cout << "adaptation matrix: " << std::endl;
            std::cout << adaptationMat << std::endl;
            Mat33f mRGB1ToRGB2 = mRGB2ToXYZ.inverse() * mXYZToLMS.inverse() * adaptationMat * mXYZToLMS * mRGB1ToXYZ;
            return mRGB1ToRGB2;
        }
    }

    Mat33f solveRGB2XYZ(Imf::Chromaticities &chroma){
        // white point is just (1, 1, 1) and white point for chroma is chroma.white.x/y 1 z/y
        Vec3 whiteXYZ;
        whiteXYZ << chroma.white.x / chroma.white.y, 1, (1-chroma.white.x-chroma.white.y) / chroma.white.y;
        Mat33f matChroma;
        matChroma << chroma.red.x, chroma.green.x, chroma.blue.x,
                     chroma.red.y, chroma.green.y, chroma.blue.y,
                     1-chroma.red.x-chroma.red.y, 1-chroma.green.x-chroma.green.y, 1-chroma.blue.x-chroma.blue.y;

        Vec3 gXYZ = matChroma.inverse() * whiteXYZ;
        Mat33f diagMat = Mat33f::Zero();
        diagMat(0,0) = gXYZ(0);
        diagMat(1,1) = gXYZ(1);
        diagMat(2,2) = gXYZ(2);

        Mat33f mRGB2XYZ = matChroma * diagMat;
        // std::cout << "RGB2XYZ: " << mRGB2XYZ << std::endl;
        return mRGB2XYZ;
    }

    Mat33f getXYZ2LMS(){
        Mat33f bradfordTransformation;
        bradfordTransformation << 0.8951f, 0.2664f, -0.1614f, 
                                  -0.7502f, 1.7135f, 0.0367f, 
                                  0.0389f, -0.0685f, 1.0296f;
        return bradfordTransformation;
    }

    /*
    header include sRGB: chromaticities
    */
    void writeEXR(const char filename[], Imf::Chromaticities &chroma, Imf::Rgba *pixels, const int width, const int height){
        Imf::Header header (width, height);
        Imf::addChromaticities(header, chroma);
        Imf::RgbaOutputFile file (filename, header, Imf::WRITE_RGBA);
        file.setFrameBuffer(pixels, 1, width);
        file.writePixels(height);
    }

    void writeEXR_float(const char filename[], Imf::Chromaticities &chroma, const float *r, const float *g, const float *b, const int width, const int height){
        Imf::Header header(width, height);
        Imf::addChromaticities(header, chroma);
        header.channels().insert("R", Imf::Channel (Imf::FLOAT));
        header.channels().insert("G", Imf::Channel (Imf::FLOAT));
        header.channels().insert("B", Imf::Channel (Imf::FLOAT));

        Imf::OutputFile file (filename, header);
        Imf::FrameBuffer frameBuffer;
        frameBuffer.insert("R", Imf::Slice (Imf::FLOAT, (char *)r, sizeof(*r) * 1, sizeof(*r) * width));
        frameBuffer.insert("G", Imf::Slice (Imf::FLOAT, (char *)g, sizeof(*g) * 1, sizeof(*g) * width));
        frameBuffer.insert("B", Imf::Slice (Imf::FLOAT, (char *)b, sizeof(*b) * 1, sizeof(*b) * width));

        file.setFrameBuffer(frameBuffer);
        file.writePixels(height);
    }

    void writeEXR_half(const char filename[], Imf::Chromaticities &chroma, const unsigned short int *r, const unsigned short int *g, const unsigned short int *b, const int width, const int height){
        Imf::Header header(width, height);
        Imf::addChromaticities(header, chroma);
        header.channels().insert("R", Imf::Channel (Imf::HALF));
        header.channels().insert("G", Imf::Channel (Imf::HALF));
        header.channels().insert("B", Imf::Channel (Imf::HALF));

        Imf::OutputFile file (filename, header);
        Imf::FrameBuffer frameBuffer;
        frameBuffer.insert("R", Imf::Slice (Imf::HALF, (char *)r, sizeof(*r) * 1, sizeof(*r) * width));
        frameBuffer.insert("G", Imf::Slice (Imf::HALF, (char *)g, sizeof(*g) * 1, sizeof(*g) * width));
        frameBuffer.insert("B", Imf::Slice (Imf::HALF, (char *)b, sizeof(*b) * 1, sizeof(*b) * width));

        file.setFrameBuffer(frameBuffer);
        file.writePixels(height);
    }

    void writeTIF(const char filename[], const float *r, const float *g, const float *b, const int width, const int height){
        cv::Mat mat(height, width, CV_8UC3);
        std::vector<int> compression_params;;

        CV_Assert(mat.channels() == 3);
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                cv::Vec3b& bgra = mat.at<cv::Vec3b>(i, j);
                // bgra[0] = cv::saturate_cast<unsigned char>(b[i * mat.cols + j] * 255); // Blue
                // bgra[1] = cv::saturate_cast<unsigned char>(g[i * mat.cols + j] * 255); // Green
                // bgra[2] = cv::saturate_cast<unsigned char>(r[i * mat.cols + j] * 255); // Red
                bgra[0] = (unsigned char)(b[i * mat.cols + j]); // Blue
                bgra[1] = (unsigned char)(g[i * mat.cols + j]); // Green
                bgra[2] = (unsigned char)(r[i * mat.cols + j]); // Red
            }
        }
        try {
            cv::imwrite(filename, mat, compression_params);
        }
        catch (std::runtime_error& ex) {
            fprintf(stderr, "Exception converting image to TIF format: %s\n", ex.what());
        }
        fprintf(stdout, "Saved TIF file with alpha data.\n");
    }
}