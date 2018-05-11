/*
* RECOGNIZED OBJECT CLASS
*/

#include "Coordinates.h"
#include "RecognizedObject.h"


RecognizedObject::~RecognizedObject()
{
	//destructor
}

void RecognizedObject::setInfo(std::string assump, Coordinates top, Coordinates bottom, Coordinates LMS, Coordinates RMS)
{
	assumption = assump;
	topVertex = top;
	bottomVertex = bottom;
	LMSVertex = LMS;
	RMSVertex = RMS;
}

std::string RecognizedObject::getAssumption()
{
	return assumption;
}