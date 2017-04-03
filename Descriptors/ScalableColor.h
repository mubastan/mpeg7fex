#ifndef __SCAL_ColorHIST__
#define __SCAL_ColorHIST__

namespace XM
{

	const static int scalableColorQuantValues[256][3] =
	{
		{217,9,255},{-71,9,255},{-27,8,127},{-54,9,255},{-8,7,63},{-14,7,63},{-22,7,63},{-29,8,127},
		{-6,6,31},{-13,7,63},{-11,6,31},{-22,7,63},{-9,7,63},{-14,7,63},{-19,7,63},{-22,7,63},
		{0,4,7},{-1,5,15},{0,3,3},{-2,6,31},{1,5,15},{-5,6,31},{0,5,15},{0,7,63},
		{2,5,15},{-2,6,31},{-2,5,15},{0,7,63},{3,5,15},{-5,6,31},{-1,6,31},{4,7,63},
		{0,3,3},{0,3,3},{0,3,3},{-1,5,15},{0,3,3},{0,3,3},{-1,5,15},{-2,5,15},
		{-1,5,15},{-1,4,7},{-1,5,15},{-3,5,15},{-1,5,15},{-2,5,15},{-4,5,15},{-5,5,15},
		{-1,5,15},{0,3,3},{-2,5,15},{-2,5,15},{-2,5,15},{-3,5,15},{-3,5,15},{0,5,15},
		{0,5,15},{0,5,15},{0,5,15},{2,5,15},{-1,5,15},{0,5,15},{3,6,31},{3,5,15},
		{0,2,1},{0,2,1},{0,3,3},{0,4,7},{0,2,1},{0,2,1},{0,3,3},{-1,4,7},
		{-1,4,7},{-1,4,7},{-2,5,15},{-1,5,15},{-2,5,15},{-2,5,15},{-2,5,15},{-1,5,15},
		{0,3,3},{0,2,1},{0,3,3},{-1,4,7},{0,2,1},{0,3,3},{-1,4,7},{-1,5,15},
		{-2,5,15},{-1,4,7},{-2,5,15},{-1,5,15},{-3,5,15},{-3,5,15},{-2,5,15},{0,5,15},
		{0,3,3},{0,3,3},{0,3,3},{-1,4,7},{0,3,3},{0,3,3},{-2,5,15},{-2,5,15},
		{-2,5,15},{-2,4,7},{-2,5,15},{-1,5,15},{-3,5,15},{-3,5,15},{-1,5,15},{0,5,15},
		{1,4,7},{0,3,3},{0,4,7},{-1,4,7},{0,3,3},{0,4,7},{-1,4,7},{0,4,7},
		{-1,4,7},{-1,3,3},{-1,4,7},{0,4,7},{-1,5,15},{0,5,15},{1,5,15},{-1,5,15},
		{0,2,1},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,2,1},{0,2,1},{0,3,3},{0,4,7},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,2,1},{0,3,3},{-1,4,7},{1,4,7},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{0,3,3},{-1,4,7},{0,4,7},
		{0,3,3},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,2,1},{0,3,3},{0,3,3},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,2,1},{0,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,2,1},{0,3,3},{1,5,15},{0,3,3},{0,3,3},{-1,5,15},{2,5,15},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{0,3,3},{-1,4,7},{1,5,15},
		{0,3,3},{0,2,1},{0,3,3},{0,3,3},{0,2,1},{0,3,3},{0,4,7},{0,4,7},
		{0,3,3},{0,2,1},{0,3,3},{1,4,7},{0,3,3},{0,3,3},{-1,5,15},{1,5,15},
		{0,3,3},{0,2,1},{-1,3,3},{1,5,15},{0,3,3},{-1,4,7},{-1,5,15},{2,5,15},
		{0,3,3},{0,3,3},{0,3,3},{0,4,7},{0,3,3},{-1,3,3},{0,4,7},{1,4,7},
		{1,3,3},{0,2,1},{-1,3,3},{0,3,3},{0,3,3},{0,3,3},{0,3,3},{1,4,7},
		{0,3,3},{0,2,1},{-1,3,3},{0,4,7},{0,3,3},{0,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,2,1},{0,3,3},{0,4,7},{0,3,3},{-1,3,3},{0,4,7},{1,4,7},
		{0,3,3},{0,3,3},{0,3,3},{0,3,3},{0,3,3},{-1,3,3},{0,3,3},{-1,4,7}
	};



class ScalableColorDescriptor
{
public:
	ScalableColorDescriptor();
	virtual ~ScalableColorDescriptor();

	//virtual unsigned long ExportDDL(GenericDSInterfaceABC *aParentDescription);
	//virtual unsigned long ImportDDL(GenericDSInterfaceABC *aDescription);

	unsigned long* GetScalableHistogram(void);

	unsigned long GetNumberOfCoefficients(void);
	unsigned long SetNumberOfCoefficients(unsigned long);

	unsigned long GetCoefficient(unsigned long index);
	unsigned long SetCoefficient(unsigned long index, int value);

	unsigned long SetCoeffValue(unsigned long index, int value);

	unsigned long GetCoeffSign(unsigned long index);
	unsigned long SetCoeffSign(unsigned long index, int value);

	unsigned long GetNumberOfBitplanesDiscarded();
	unsigned long SetNumberOfBitplanesDiscarded(unsigned long);

	// added by mb - 19.07.2008 - set the feature vectors to zero
	void resetDescriptor();

	// distance between 2 SCDs
    float distance(ScalableColorDescriptor* scd);
    float distanceNorm(ScalableColorDescriptor* scd);

	// added by mb - 26.06.2008
	void Print();

public:

	unsigned long* m_ScalableHistogram;  // An array of m_HistoSize	Coefficients representing the frequency of each quantized value in the  source media
	unsigned long* m_Sign;					// An array of signs of m_HistoSize Coefficients
	unsigned long m_NumberOfCoefficients;   // The size of the ScalableHistogram */
	unsigned long m_NumberOfBitplanesDiscarded; // A normalization factor to be applied to the entries of the ScalableHistogram before use
};

};

#endif
