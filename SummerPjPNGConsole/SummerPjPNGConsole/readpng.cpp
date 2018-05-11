/*--------------------------------------------//
* READ PNG IMAGE FILE						  //
* READS PNG IMAGE							  //	
* LOOKS FOR SEARCHED OBJECT					  //
* CONTAINS ALL INFORMATION ABOUT THE PNG FILE //
* ALL FUN PART IS HERE                        //
*---------------------------------------------//
* CREATED BY EKATERINA NIKONOVA AS A SUMMER   //
* SEMESTER PROJECT FOR PROGRAMMING II COURSE  //
*---------------------------------------------//
*/

//libpng files
#include "include\png.h"
#include "include\zlib.h"
#include "readpng.h"



//use only in console mode:
//char* INPUT_FILE = "lineFig2.png";

//our structures for image, required by libpng
static png_structp png_ptr = NULL;
static png_infop info_ptr = NULL;


//Libpng variables
int width, height;
int number_passes;
png_byte bit_depth;
png_byte color_type;
png_bytep * row_pointers;

//Global variables
std::string assumption;
int rowsLength[1000];

std::vector<RecognizedObject> recObjects;

int leftSnoop = 0;
int rightSnoop = 0;

/*----------------------------------------//
* READ PNG FILE FUNCTION, uses libpng
* Creates the data structures to store image data
* Reads and writes image data into the structures
*/
//initializing the read png function
int read_png_file(char* f)
{
	//first read the header of img file to check if it is a PNG signature or not
	

	uch sig[8]; //type unsigned char; singnature storage

	FILE *fp = fopen(f, "rb");
	if (fp== NULL)
	{
		perror("Can't open a file");
		return 1; //cant open a file
	}
		
	

	//so read the first 8 bytes of the file and check signature
	fread(sig, 1, 8, fp);
	bool ispng = !png_sig_cmp(sig, 0, 8);
	if (!ispng)
		return 1; //if not PNG

	//if PNG then continue
	//create the structures that will hold info about PNG image
	//pointer at the structure that keeps track of the image state
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return 4;//out of memory

	//indicates what its state will be after all of the transformations
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 4;//error creating info struct
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return 4; //error creating end info structure
	}

	
	//handle errors
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		//free any memory that is used by the png_ptr and info_ptr structures
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return 2;//error during initialization of IO
	}
	
	//set up the libpng, prepare for reading PNG
	png_init_io(png_ptr, fp); //takes files stream pointer and stores in the png_ptr
	png_set_sig_bytes(png_ptr, 8); //signature is checked, so don't expect it
	png_read_info(png_ptr, info_ptr); //reads and processes PNG IHDR chunks up to Image DATA,stores it in the info structure


	//uses png_get_IHDR(....) info structure
	//get the image width,height and bit depth
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr,info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	//turn on interrlace handling, REQUIRED by png_read_image()
	number_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		return 1;//error reading image
	}

	//retrieve the image data
	//points to png_bytep row_pointers[height] structure, basically a picture rows
	//since we know the size of image we allocate it this way
	row_pointers = (png_bytep*) malloc(height * sizeof(png_bytep));
	for (int row = 0; row < height; row++)
		row_pointers[row] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	//read image into memory
	png_read_image(png_ptr, row_pointers);
	
	//read rest of the image, usually some additional chunks if there are any
	png_read_end(png_ptr, info_ptr);

	fclose(fp);
}


/*---------------------------------------------//
* WRITE PNG FILE, uses libpng				   //
* Basically creates the new PNG file           //
* Using it to create image                     //
* of the objects that were searched            //
*----------------------------------------------//
*/

int write_png_file(char* file_name)
{
	//create the file
	FILE *fp = fopen(file_name, "wb");//opens in write bytes mode

	if (!fp)
		return 0; //cannot open the file

	//init 
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		return 4;//out of memory

	//indicates what its state will be after all of the transformations
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 4;//error creating info struct
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		return 2; //error during init io
	}
	png_init_io(png_ptr, fp); //takes files stream pointer and stores in the png_ptr

	//write header
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		return 2;//error during writing header
	}

	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	//write bytes
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		return 2;//error during writing bytes
	}
	png_write_image(png_ptr, row_pointers);

	//write ending of the file
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		return 2;//error during writing end of the file
	}
	png_write_end(png_ptr, NULL);

	//clean up the heap
	for (int y = 0; y < height; y++)
		free(row_pointers[y]);
	free(row_pointers);
	

	fclose(fp);

	
}

/*-------CLASS OBJECTFOUND METHODS IMPLEMENTATION-------*/
//constuctor of ObjectFound
ObjectFound::ObjectFound(int fRow, int lRow, int wRow, int sCh, int tCh, int cCh)
{
	firstRow = fRow;
	lastRow = lRow;
	widestRow = wRow;
	squareChance = sCh;
	triangleChance = tCh;
	circleChance = cCh;
}

ObjectFound::~ObjectFound()
{
	//destructor
}

int ObjectFound::getFirstRow()
{
	return firstRow;
}

void ObjectFound::resetChances()
{
	squareChance = 0;
	triangleChance = 0;
	circleChance = 0;
}

int ObjectFound::getFirstRowSize()
{
	return firstRowSize;
}

void ObjectFound::setFirstRowSize(int rS)
{
	firstRowSize = rS;
}

void ObjectFound::setHighestColumn(int h)
{
	highestColumn = h;
}



void ObjectFound::setFirstRow(int fRw)
{
	firstRow = fRw;
}

void ObjectFound::setLastRow(int lRw)
{
	lastRow = lRw;
}


void ObjectFound::setLB(int l)
{
	LB = l;
}

void ObjectFound::setRB(int r)
{
	RB = r;
}

int ObjectFound::getLB()
{
	return LB;
}

int ObjectFound::getRB()
{
	return RB;
}

void ObjectFound::setLastCol(int c)
{
	lastCol = c;
}


bool AwithInB(Coordinates a, Coordinates b)
{
	//if ((a.column <= b.column + 3) && (a.column >= b.column - 5))
	//{
		//now check closure under the row
		if ((a.row <= b.row + 5) && (a.row >= b.row - 1))
		{
			return true;
		}
		else
			return false;
	//}
	
}

/*-----------------------------------------------------------//
* ANALYSIS OF THE CHANCES THAT WAS CALCULATED FOR THE OBJECT
* picking the maximum of the three parameters chances
* triangle - 3 vertices , square - 4 vertices, circle - 4 vertices
* -----------------------------------------------------------//
*/
void ObjectFound::analyze()
{
	
	//so here we going to analyze if it is the triangle or circle xor square
	//between the square and the circle the difference is trivial
	//however between triangle and circle the difference is 3 vs 4 vertices
	//find top vertex
	Coordinates coordTopVertex(firstRow, highestColumn);
	//find bottom vertex
	Coordinates coordBottomVertex(lastRow, lastCol);
	//find side vertices
	Coordinates coordLMSVertex(LMSRow, LB);
	Coordinates coordRMSVertex(RMSRow, RB);
	bool TnR = AwithInB(coordRMSVertex,coordTopVertex);
	bool TnL = AwithInB(coordLMSVertex, coordTopVertex);
	bool BnR = AwithInB(coordBottomVertex, coordRMSVertex);
	bool BnL = AwithInB(coordBottomVertex, coordLMSVertex);
	

	if (TnR || TnL || BnR || BnL) //if any within another then triangle or square
	{
		if (triangleChance > squareChance)
			assumption = "Triangle";
		else
			assumption = "Square";
	}
	else
	{
		//here check if square or circle
		if (squareChance > circleChance)
		{
			assumption = "Square";
		}
		else
		{
			//even tho if for some reason triangle were sent here we catch it
			//widest row of the object were within one/two rows from the bottom or top? then triangle
			if ((widestNumRow <= lastRow && widestNumRow >= lastRow - 3) || (widestNumRow >= firstRow && widestNumRow <= firstRow + 4))
			{
				assumption = "Triangle";
			}
			else
			{
				assumption = "Circle";
			}
			
		}
	}

	RecognizedObject newRecognizedObj;
	newRecognizedObj.setInfo(assumption, coordTopVertex, coordBottomVertex, coordLMSVertex, coordRMSVertex);
	recObjects.push_back(newRecognizedObj);

}


/*RULES OF THE CHANCES OF THE OBJECT TO BE RECOGNIZED AS SOME FIGURE
*
* -10 and less  ABSOLUTELY not, if object once was set as -10 it is definitly not it
* -5   PROBABLY not, object assumed to be not this figure
*  0   NEUTRAL, it is where the program starts by default
*  5   PROBABLY yes, object assumed to be most likely this figure
*  10 and more MOSTLIKELY yes, but not guaranteed to be absolute true that the object is this figure
*/
//changes the object chances to be recognized as some trigonometry figure
void ObjectFound::changeChance(int sqC, int trCh, int cirCh)
{
	if (squareChance != -10)//if it is -10 we dont do anything
	{
		squareChance = squareChance + sqC;
	}

	if (triangleChance != -10)//dont do anything if some rule was broken
	{
		triangleChance = triangleChance + trCh;
	}

	circleChance = circleChance + cirCh;
}



/*------------------------------------------------------------------------//
* ASSUMES WHAT GEOMETRIC FIGURE GIVEN OBJECT IS
* ASSUMPTIONS MADE BASED ON THE SHAPE OF THE OBJECT
* IN PARTICULAR ITS ROWS SHAPE
* RETURNS OBJECTFOUND INSTANCE
*
*--------------------RULES OF ASSUMPTION----------------------------------//
* there are several rules that define geometric figure
* for example all rows of the square should be equal
* and etc...some rules helps to distinguish with absolute certaince,
* that this object is not some certain geometric figure
*-------------------------------------------------------------------------//
*/
ObjectFound makeAnAssumption(int ar[], int depth, ObjectFound object)
{

	int rowOld, rowNew = 0;
	int firstRow = ar[0];
	for (int i = 0; i < (depth - object.getFirstRow()); i++)
	{
		rowOld = ar[i];
		rowNew = ar[i + 1];
		//1st rule if n-th row < n+1-th row then not SQUARE
		if (rowOld < rowNew && rowOld != 0)
		{
		object.changeChance(-10, +5, +2);
		}
		//2nd rule the more equal rows than higher chances of being square
		else if (rowOld == rowNew && rowOld != 0)
		{
			//with each step chances it is a square chances ++
			object.changeChance(+3, +1, +2);
		}
		else if (rowOld > rowNew && rowOld != 0)
		{
			object.changeChance(-10, +2, +2);
		}
		
	}
	return object;
}

bool isNextRowEmpty(int row, int col)
{
	if (row < height - 2)
	{
		png_byte* rP = row_pointers[row + 1];
		png_byte* Pix = &(rP[col * 3]);

		//if not white then figure continues...
		if (Pix[0] != 255 && Pix[1] != 255 && Pix[2] != 255)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}


/*---------------------------------------------------------------//
* DISCOVERY ALGORITHM
* CHECKS THE DEPTH OF THE OBJECT
* THEN SENDS TWO SO CALLED SNOOPS TO DISCOVER
* LEFT AND RIGHT BOUNDARIES OF THE OBJECTS ROW
* THE WAY THE ALGORITHM WORKS YOU CAN FIND IN THE DOCUMENTATION
* RETURNS DEPTH OF THE OBJECT
*----------------------------------------------------------------//
*/
ObjectFound discoverObject(int midCol, int row, int i, ObjectFound obj)
{
	leftSnoop--;
	rightSnoop++;
	leftSnoop = midCol;//both snoops start from the midColumn
	rightSnoop = midCol;
	int lengthRight = 0;
	int lengthLeft = 0;
	obj.nextRowEmpty = false;
	png_byte* rP = row_pointers[row];
	png_byte* Pix = &(rP[midCol * 3]);

	//now need to loop through this column and send discovers to both directions
	while (Pix[0] != 255 && Pix[1] != 255 && Pix[2] != 255 && leftSnoop > 0)
	{
		leftSnoop--;//left discovery
		lengthLeft++;
		Pix = &(rP[leftSnoop * 3]);
	}
	//color border
	Pix[0] = 0;
	Pix[1] = 255;
	Pix[2] = 255;
	Pix = &(rP[midCol * 3]);
	if (obj.getLB() >= leftSnoop)
	{
		obj.setLB(leftSnoop);
		obj.LMSRow = row;
	}
	while (Pix[0] != 255 && Pix[1] != 255 && Pix[2] != 255 && rightSnoop < width - 1)
	{
		rightSnoop++;//right discovery
		lengthRight++;
		Pix = &(rP[rightSnoop * 3]);
	}
	//color border
	Pix[0] = 0;
	Pix[1] = 0;
	Pix[2] = 255;
	if (obj.getRB() <= rightSnoop)
	{
		obj.setRB(rightSnoop);
		obj.RMSRow = row;
	}

	//after both snoops met white pix sum them
	int totalLength = lengthLeft + lengthRight - 1;

	rowsLength[i] = totalLength;

	//check if the widest row
	if (totalLength > obj.widestRow)
	{
		obj.widestRow = totalLength;
		obj.widestNumRow = row;
	}

	//check if the next row empty
	obj.nextRowEmpty = isNextRowEmpty(row, midCol);
	return obj;
}


void discoveryController(int lengthRow, int firstPix, int lastPix, int row, int i, ObjectFound obj)
{
	//so divide length in half to find the mid column
	int midCol = (firstPix + lastPix) / 2;
	obj.nextRowEmpty = false;
	int rowIn = row;
	bool done = false;
	png_byte* rP = row_pointers[row];
	png_byte* Pix = &(rP[midCol * 3]);

	obj.setFirstRowSize(lengthRow);
	obj.setLB(firstPix);
	obj.setRB(lastPix);
	

	while (!obj.nextRowEmpty && row < height -1)
	{
		obj = discoverObject(midCol,row,i,obj);
		i++;
		row++;
		midCol = (leftSnoop + rightSnoop) / 2;
	}

	obj.setLastRow(row);
	obj.setLastCol(leftSnoop);
	obj = makeAnAssumption(rowsLength, row, obj);
	obj.analyze();//make an assumption and make recognizedobject class instance
}





std::vector<RecognizedObject> returnRecObjectsList()
{
	return recObjects;
}

void clearVector()
{
	recObjects.clear();
}


/*-------------------------------------------//
* ANALYZING THE PICTURE
* THE MAIN PART OF THIS FILE
* CALLS MOST OF THE FUNCTIONS FROM THE ABOVE
* READS THE IMAGE DATA 
* RETURNS int != 0 IF ERROR OCCURED
*--------------------------------------------//
*/
int analyze_the_picture()
{
	int firstRow = 0;
	int rowLength = 0;
	bool hittedBorder = false;
	ObjectFound object(0, 0, 0, 0, 0, 0); //init object
	if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGB)
		return 1; //input file must be RGB
	

	for (int row = 0; row < height; row++)
	{
		png_byte* rowP = row_pointers[row];
		for (int column = 0; column < width; column++)
		{
			rowLength = 0;
			//here we muplitly each rowP by 3 since to "shift" to the next pixel it takes 3 shifts
			//so every 3rd value from our row is one pixel, therefore we get exactly one pixel at the time
			png_byte* ptr = &(rowP[column*3]);
			if (((ptr[0] != 255 && ptr[1] != 255 && ptr[2] != 255) || (ptr[0] != 0 && ptr[1] != 255 && ptr[2] != 255)) && !hittedBorder )
			{
				//then not white so record row
				if (firstRow == 0)
				{
					//so first row met
					firstRow = row;
					//change object properties
					object.setFirstRow(firstRow);
					object.setHighestColumn(column);
					object.resetChances();
					object.setLastRow(0);
					object.depth = 0;

				}

				//so if hitted white pixel then continue to store info about this object until empty row
				//save info about the first pixel position of the row
				int firstPixel = column;

				//continue to read while pixel is not white
				while (ptr[0] != 255 && ptr[1] != 255 && ptr[2] != 255 && column < width - 1)
					{
						rowLength++;
						column++; //go to the next pixel
						ptr = &(rowP[column * 3]);
					}
				//if loop stoped then hitted white pixel so just save info about row
				int lastPixel = column - 1;

				discoveryController(rowLength, firstPixel, lastPixel, row, 0, object);
				//discoverObject(rowLength, firstPixel, lastPixel, row,0,0,object);
				firstRow = 0;
			}

			//so if we hitted the in border ignore the object
			if (ptr[0] == 0 && ptr[1] == 255 && ptr[2] == 255)
				hittedBorder = true;

			//if hitted the out border(blue) then continue to read the obj
			if (ptr[0] == 0 && ptr[1] == 0 && ptr[2] == 255)
				hittedBorder = false;

			firstRow = 0;
		}

			//reset the object
			firstRow = 0;
	}
	//so the entire picture was analyzes
	return 0; //no errors occured
}



//console mode only: 
//used for debug
/*
int main(int argc, char *argv[])
{
readpng_version_info();
read_png_file(INPUT_FILE);
analyze_the_picture();
write_png_file(INPUT_FILE);
}
*/


