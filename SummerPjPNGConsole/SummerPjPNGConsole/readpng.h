
#include "RecognizedObject.h"
#include "Coordinates.h"
#include "ObjectFound.h"
//standard files
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include <iostream>
#include <vector>

typedef unsigned char   uch;
typedef unsigned short  ush;
typedef unsigned long   ulg;


/* prototypes for public functions in readpng.c */

int read_png_file(char* f);

int analyze_the_picture();
void clearVector();

std::vector<RecognizedObject> returnRecObjectsList();

int write_png_file(char* f);

