#pragma once
#include "Coordinates.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

class RecognizedObject
{
public:
	RecognizedObject() : topVertex(0, 0), bottomVertex(0, 0), LMSVertex(0, 0), RMSVertex(0, 0)
	{}
	~RecognizedObject();
	void setInfo(std::string assump, Coordinates top, Coordinates bottom, Coordinates LMS, Coordinates RMS);
	std::string getAssumption();
	std::string assumption;
	Coordinates topVertex;
	Coordinates bottomVertex;
	Coordinates LMSVertex;
	Coordinates RMSVertex;

	
	

};
