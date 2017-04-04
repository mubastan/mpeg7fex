# mpeg7fex
MPEG-7 Feature Extraction Library

MPEG-7 Feature Extraction Library : This library is adapted from MPEG-7 XM Reference Software to make it work with Open Source Computer Vision library (OpenCV) data structures (e.g., Mat). It has a very easy-to-use API. Moreover, existing bugs (resulting in wrong descriptor values) in XM software are corrected.

The library was tested with OpenCV 3.1.0 on Linux Mint 18.1.

Supported MPEG-7 Descriptors:
Color Structure Descriptor (CSD),
Scalable Color Descriptor (SCD),
Color Layout Descriptor (CLD),
Dominant Color Descriptor (DCD),
Homogeneous Texture Descriptor (HTD),
Edge Histogram Descriptor (EHD)

See http://www.cs.bilkent.edu.tr/~bilmdg/bilvideo-7 for more details.

Reference:
Muhammet Bastan, Hayati Cam, Ugur Gudukbay and Ozgur Ulusoy, 
"BilVideo-7: An MPEG-7 Compatible Video Indexing and Retrieval System", 
IEEE MultiMedia, vol. 17, no. 3, pp. 62-73, July-September 2010.
