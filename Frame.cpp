#include <iostream>

#include "Convert.h"
#include "Frame.h"

// Constructor
Frame::Frame( int width, int height, bool imgFlag, bool grayFlag, bool maskFlag)
{
    this->image = 0;
    this->gray = 0;
    this->mask = 0;

	this->allocate( width, height, imgFlag, grayFlag, maskFlag );
}

Frame :: Frame( Mat& src, bool imgFlag, bool grayFlag, bool maskFlag )
{
    this->image = 0;
    this->gray = 0;
    this->mask = 0;

	this->allocate( src.cols, src.rows, imgFlag, grayFlag, maskFlag);
	this->setImage(src);
}

void Frame :: allocate(int width, int height, bool imgFlag, bool grayFlag, bool maskFlag)
{
    /// 3-channel color image
    if(imgFlag)
    {
        if(!image)
            image = initVop( width, height, NONE_TYPE, UCHAR_TYPE, UCHAR_TYPE, UCHAR_TYPE );
        else if( width != image->width || height != image->height )
        {
            freevop(image);
            image = initVop( width, height, NONE_TYPE, UCHAR_TYPE, UCHAR_TYPE, UCHAR_TYPE );
        }

        if(!image )
        {
            std::cerr << "Error: Frame :: allocate() - image - memory allocation failed!" << std::endl;
            return;
        }

    }

	/// grayscale - y_chan
	if(grayFlag)
	{
	    if(!gray)
            gray = initVop( width, height, NONE_TYPE, UCHAR_TYPE, NONE_TYPE, NONE_TYPE );
        else if(width != gray->width || height != gray->height)
        {
            freevop(gray);
            gray = initVop( width, height, NONE_TYPE, UCHAR_TYPE, NONE_TYPE, NONE_TYPE );
        }

        if(!gray )
        {
            std::cerr << "Error: Frame :: allocate() - gray - memory allocation failed!" << std::endl;
            return;
        }
	}

	/// mask for arbitrary shapes - a_chan
    if(maskFlag)
	{
	    if(!mask)
            mask = initVop( width, height, UCHAR_TYPE, NONE_TYPE, NONE_TYPE, NONE_TYPE );
        else if(width != mask->width || height != mask->height)
        {
            freevop(mask);
            mask = initVop( width, height, UCHAR_TYPE, NONE_TYPE, NONE_TYPE, NONE_TYPE );
        }

        if(!mask )
        {
            std::cerr << "Error: Frame :: allocate() - mask - memory allocation failed!" << std::endl;
            return;
        }
	}

	this->width = width;
    this->height = height;
}

void Frame::resize(int width, int height)
{
    if( this->width == width && this->height == height )
        return;

    // first reset all the masks
    this->resetMaskAll();

    if( this->image )
    {
        freevop(this->image);
        this->image = initVop( width, height, NONE_TYPE, UCHAR_TYPE, UCHAR_TYPE, UCHAR_TYPE );
    }

    if( this->gray )
    {
        freevop(this->gray);
        this->gray = initVop( width, height, NONE_TYPE, UCHAR_TYPE, NONE_TYPE, NONE_TYPE );
    }

    if(this->mask)
    {
        freevop(mask);
        mask = initVop( width, height, UCHAR_TYPE, NONE_TYPE, NONE_TYPE, NONE_TYPE );
    }

    this->width = width;
    this->height = height;
}

Frame::~Frame()
{
    if(image)
    {
        image->a_chan = 0;
        freevop(image);
		image = 0;
    }

	if(gray)
	{
        gray->a_chan = 0;
        freevop(gray);
		gray = 0;
	}

	if(mask)
	{
        freevop(mask);
		mask = 0;
	}
}

void Frame :: setImage( Mat& src )
{
    if(this->image && !src.empty())
        Convert::ipl2momvop( src, this->image );
}

void Frame :: setGray( Mat& gray )
{
    if(this->gray && !gray.empty())
        Convert::ipl2momvop( gray, this->gray );
}

//rid: value of the region in mask
// fg: foreground value in the output image - shape
// bg: background value in the output image
void Frame :: setMask( Mat& mask, int rid, int fg, int bg )
{
    if( this->gray && !mask.empty() )
        Convert::setShape( mask, this->mask, rid, bg, fg);
}

void Frame :: setMaskValue( int val )
{

    if(this->mask)
        Convert::setMaskValue( this->mask, val );

	// point to a_chan - initially 0
	if(this->image)
        this->image->a_chan = this->mask->a_chan;

	if(this->gray)
        this->gray->a_chan = this->mask->a_chan;
}


void Frame :: setMaskAll( Mat& mask, int rid, int fg, int bg)
{
    if(!mask.empty())
        this->setMask( mask, rid, fg, bg);

	// point to a_chan - initially 0
	if(this->image)
        this->image->a_chan = this->mask->a_chan;

	if(this->gray)
        this->gray->a_chan = this->mask->a_chan;
}

void Frame :: setMaskImage( Mat& mask, int rid, int fg, int bg)
{
    if(!mask.empty())
        this->setMask( mask, rid, fg, bg);

	// point to a_chan - initially 0
	if(this->image)
        this->image->a_chan = this->mask->a_chan;

}

void Frame :: setMaskGray( Mat& mask, int rid, int fg, int bg)
{
    if(!mask.empty())
        this->setMask( mask, rid, fg, bg);

	// point to a_chan - initially 0
	if(this->gray)
        this->gray->a_chan = this->mask->a_chan;
}

void Frame :: resetMaskAll()
{
	// point to 0 again - as they are initialized
	if(this->image)
        this->image->a_chan = 0;
	if(this->gray)
        this->gray->a_chan = 0;
}

void Frame :: resetMaskImage()
{
	// point to 0 again - as they are initialized
	if(this->image)
        this->image->a_chan = 0;
}

void Frame :: resetMaskGray()
{
	// point to 0 again - as they are initialized
	if(this->gray)
        this->gray->a_chan = 0;
}
