MPEG-7 Low Level Feature Extraction Library - source code
---------------------------------------------------------
by Muhammet Bastan, (c) October 2010-2012, 2017
e-mail: mubastan@gmail.com
---------------------------------------------------------

download page: www.cs.bilkent.edu.tr/~bilmdg/bilvideo-7/Software.html

initial date: 20.02.2010
update: 13.10.2010 (prepare the windows release, static [.lib] & dynamic [.dll] libraries)
update: 04.02.2011 (upgrade to OpenCV 2.2)
update: 12.04.2011 (source code)
update: 12.01.2012 (source code + test code, compiled with OpenCV 2.3.1)

update: 03.04.2017 (source code + test code, compiled with OpenCV 3.1.0, on Linux Mint 18.1)
The rest of this Readme is not updated (yet).

*** the "test" directory contains sample code demonstrating the usage of the library.

Note: the following is old, not updated according to the latest release (12.01.2012)

----------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
Table of Contents:

	Introduction
	License - Liability
	Supported Descriptors
	How to compile your programs
	Using the API
	Similarity Computation (Matching)
----------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
		Introduction
----------------------------------------------------------------------------------------

This library (both static, .lib, and dynamic, .dll versions) is compiled (C++) with MSVC 2010 on Windows XP
and uses OpenCV 2.2 (OpenCV also compiled with MSVC 2010) for image handling (Mat data structure).
It was developed for BilVideo-7 MPEG-7 compatible video indexing and retrieval system:

Muhammet Bastan, Hayati Cam, Ugur Gudukbay and Ozgur Ulusoy, 
"BilVideo-7: An MPEG-7 Compatible Video Indexing and Retrieval System", 
IEEE MultiMedia, vol. 17, no. 3, pp. 62-73, July-September 2010.

Bibtex entry:
@article{BilVideo7-MM2010,
	author = {Muhammet Ba\c{s}tan and Hayati \c{C}am and U\v{g}ur G\"{u}d\"{u}kbay and \"{O}zg\"{u}r Ulusoy},
	title = {{BilVideo-7: An MPEG-7-Compatible Video Indexing and Retrieval System}},
	journal ={IEEE MultiMedia},
	volume = {17},
	number = {3},
	pages = {62--73},
	year = {2009},
	doi = {http://doi.ieeecomputersociety.org/10.1109/MMUL.2009.74},
	publisher = {IEEE Computer Society},
	address = {Los Alamitos, CA, USA},
}

Please cite this paper if you use this library.
Please feel free to contact the author in case of any bugs in the program (which is of course possible),
or valuable suggestions.

---------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------
License - Liability: 
(GPL - You are free to use/distribute the tool including this Readme.txt and all the other files as is)
In no event and under no legal theory,      
whether in tort (including negligence), contract, or otherwise,      
unless required by applicable law (such as deliberate and grossly      
negligent acts) or agreed to in writing, shall any Contributor be      
liable to You for damages, including any direct, indirect, special,      
incidental, or consequential damages of any character arising as a      
result of this License or out of the use or inability to use the      
Work (including but not limited to damages for loss of goodwill,      
work stoppage, computer failure or malfunction, or any and all      
other commercial damages or losses), even if such Contributor      
has been advised of the possibility of such damages.
----------------------------------------------------------------------------------------

The library is adapted from the MPEG-7 XM Reference Software available at 
http://standards.iso.org/ittf/PubliclyAvailableStandards/index.html

As also available in the header files adapted from the XM software, please note that:
"ISO/IEC gives users of MPEG-7 free license to this software module or
modifications thereof for use in hardware or software products claiming
conformance to MPEG-7".

The XM reference software contains some bugs and does not work properly.
Moreover, it is not meant to be used in applications directly, therefore, 
there is no decent interface to use it easily, not mentioning the horrible coding, 
which makes everybody run away at the first glance!
This library is an attempt to make the MPEG-7 feature extraction easier to integrate into new systems..

----------------------------------------------------------------------------------------

Supported descriptors in this library :

 + Color Structure Descriptor (CSD)
 + Scalable Color Descriptor (SCD)
 + GoF/GoP Color Descriptor (GoF/GoP)
 + Color Layout Descriptor (CLD)
 + Dominant Color Descriptor (DCD)
 + Homogeneous Texture Descriptor (HTD)
 + Edge Histogram Descriptor (EHD)
 + Face Recognition Descriptor (FRD)

Please see http://www.chiariglione.org/mpeg/standards/mpeg-7/mpeg-7.htm for a brief description of each.
For more details: 
B. S. Manjunath, P. Salembier, and T. Sikora, Eds., Introduction to MPEG-7: Multimedia Content Description Interface. England: WILEY, 2002.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------
		How to compile your programs
-----------------------------------------------------------------------------------------------

Directory Structure:

MPEG7FexLib
	- include: header files
	- lib: contains static library MPEG7FexLib_MSVC2010.lib, MPEG7FexLib_MSVC2010.dll
	- test: sample codes with (hopefully just enough) comments illustrating the usage of the library (with CodeBlocks project file)


The library is compiled with OpenCV 2.2, which was compiled with MSVC 2010.


include directories:
	MPEG7FexLib/include
	opencv/include	

libraries:
	MPEG7FexLib: ./lib/MPEG7FexLib_MSVC2010.lib	
	OpenCV 2.2 libraries (e.g., core)

-----------------------------------------------------------------------------------------------
		Using the API
-----------------------------------------------------------------------------------------------

The library has a very easy-to-use API.
You only need to include the "Feature.h" in your code and call the static member functions of the "Feature" class.
Have a look at the sample codes in the "test" directory to see how to use the library and what the output (descriptors) looks like.
The codes are commented and are (hopefully) self-explanatory.

-----------------------------------------------------------------------------------------------------------
		Similarity Computation (Matching):
-----------------------------------------------------------------------------------------------------------

For the distance metrics proposed by MPEG-7, see the MPEG-7 book:
B. S. Manjunath, P. Salembier, and T. Sikora, Eds., Introduction to MPEG-7: Multimedia Content Description Interface. England: WILEY, 2002.

or the following papers:

Muhammet Bastan, Hayati Cam, Ugur Gudukbay and Ozgur Ulusoy, 
"BilVideo-7: An MPEG-7 Compatible Video Indexing and Retrieval System", 
IEEE MultiMedia, vol. 17, no. 3, pp. 62-73, July-September 2010.

Muhammet Bastan, Hayati Cam, Ugur Gudukbay, Ozgur Ulusoy, 
An MPEG-7 Compatible Video Retrieval System with Integrated Support for Complex Multimodal Queries,
Bilkent University, Technical Report (BU-CE-0905), 2009.
Available at: http://www.cs.bilkent.edu.tr/tech-reports/2009/BU-CE-0905.pdf

H. Eidenberger, Distance measures for MPEG-7-based retrieval,
in Proceedings of the 5th ACM SIGMM International Workshop on Multimedia Information Retrieval (MIR 03), 
2003, pp. 130-137.

For instance, you may use L1 distance for CSD, SCD..

Good Luck.
