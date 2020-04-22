the project is built using cmake library

```bash
mkdir build
cd build
cmake ..
make -j4
./assign1 macbeth.png
```

The file description:
1. main.cpp - main execution file with problem function written
2. openexr_utils.cpp - definition of the chromaticities and color transformation matrix from sRGB to ProPhoto
3. utils.cpp - read and write function for png and tif image file format