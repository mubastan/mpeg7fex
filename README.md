# MPEG-7 Feature Extraction Library

This library was adapted from MPEG-7 XM Reference Software to make it work with Open Source Computer Vision library (OpenCV) data structures (e.g., Mat). It has a very easy-to-use API. The original feature extraction code is from XM Reference Software, which was developed by MPEG-7 team long time ago (the code is unfortunately quite messy). Some bugs, resulting in wrong descriptor values, were corrected. There might still be some bugs, so, please feel free to contribute.

The library was tested with OpenCV 3.1.0 on Linux Mint 18.1. The [test](test) directory contains sample code demonstrating the usage of the library. See [HowToCompile.txt](HowToCompile.txt) for instructions on how to compile the library and test code using CodeBlocks IDE.

Supported MPEG-7 Descriptors:
1. Color Structure Descriptor (CSD)
2. Scalable Color Descriptor (SCD)
3. Color Layout Descriptor (CLD)
4. Dominant Color Descriptor (DCD)
5. Group of Frame/Picture Descriptor (GoF/GoP)
6. Homogeneous Texture Descriptor (HTD)
7. Edge Histogram Descriptor (EHD)
8. Contour Shape Descriptor (CShD)
9. Region Shape Descriptor (RSD)
10. Face Recognition Descriptor (FRD)

See http://www.cs.bilkent.edu.tr/~bilmdg/bilvideo-7 for more details.

### Reference:
Muhammet Bastan, Hayati Cam, Ugur Gudukbay and Ozgur Ulusoy, 
"BilVideo-7: An MPEG-7 Compatible Video Indexing and Retrieval System", 
IEEE MultiMedia, vol. 17, no. 3, pp. 62-73, 2010.
