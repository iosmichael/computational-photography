Original images from Nascimento, S.M.C., Ferreira, F.P., & Foster,
D.H. (2002). Statistics of spatial cone-excitation ratios in natural
scenes. Journal of the Optical Society of America A, 19, 1484-1490.

Data courtesy of David Foster
https://personalpages.manchester.ac.uk/staff/david.foster/Hyperspectral_images_of_natural_scenes_02.html

For each scene, the original MATLAB file has been converted into 31
PNG files with following file names and wavelengths.

reflectance1.png (410 nm), reflectance2.png (420 nm), ..., and
reflectance31.png (710 nm)

The gray levels stored in each PNG file are linear (not sRGB encoded),
where each grayscale image is a scaled version of a hyperspectral
image channel.  In practice, to process this data perform the
following.
1. Read the PNG file to an unsigned 8 bit, single channel image.
2. Convert the unsigned 8 bit, single channel image to a linear color
space 32 bit, single channel image by dividing each unsigned 8 bit
gray level by 255.  Note the PNG file does not contain a nonlinear
sRGB encoded image, so do not perform a nonlinear sRGB color encoding
to a linear color space.
