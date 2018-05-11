/*--------------------------------------//
* MAIN WINDOW FILE						//
* ACCOUNTS FOR ALL WINDOWS PROCESSES    //
* DEALS WITH WINDOWS CONTEXT			//
*---------------------------------------------//
* CREATED BY EKATERINA NIKONOVA AS A SUMMER   //
* SEMESTER PROJECT FOR PROGRAMMING II COURSE  //
*---------------------------------------------//
*/


#include <Windows.h>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <tchar.h>
#include <atlbase.h>
#include <atlconv.h>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif // _UNICODE

#include <gdiplus.h>
#include "readpng.h"
using namespace Gdiplus;//using it to display the png images
// Global variables

char* INPUT_FILE = "512x512.png";
char* OUTPUT_FILE = "outputImage.png";
wchar_t textReceived[20];
std::vector<RecognizedObject> recObjs;
LPTSTR szText = new TCHAR[100];
std::string wrd;

// The main window class name.
static TCHAR szWindowClass[] = _T("summerprojectclass");
static TCHAR szWindowClass2[] = _T("summerprojectclass");
bool win2Open = false;
bool win1Open = false;
bool winWasRegistered = false;
enum winToOpenT {none,win2};
winToOpenT winToOpen = none;
HWND TextField;

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Summer Project, Geometric Shape Recognition");



HINSTANCE hInst;



// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc1(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc2(HWND, UINT, WPARAM, LPARAM);

void createWnd2(WNDCLASSEX& wc, HWND& hwnd, HINSTANCE hInst, int nCmdShow);


void highlightObj(RecognizedObject obj, HDC hdc)
{
	Graphics gph(hdc);
	Pen pen(Color(255, 255, 0, 0),5.0F);
	if (obj.assumption == "Square")
	{
		Point p1(obj.topVertex.column, obj.topVertex.row);
		Point p2(obj.RMSVertex.column, obj.topVertex.row);
		Point p3(obj.RMSVertex.column, obj.bottomVertex.row);
		Point p4(obj.LMSVertex.column, obj.bottomVertex.row);
		gph.DrawLine(&pen, p1, p2);//top line
		gph.DrawLine(&pen, p2, p3);//right line
		gph.DrawLine(&pen, p3, p4);//bottom line
		gph.DrawLine(&pen, p4, p1);//left line
	}
	else if (obj.assumption == "Triangle")
	{
		//two possibilities either it is ^ or V
		//so if upside down V
		if (obj.LMSVertex.row < (obj.bottomVertex.row -5))
		{
			Point p1(obj.bottomVertex.column, obj.bottomVertex.row);
			Point p2(obj.RMSVertex.column, obj.RMSVertex.row);
			Point p3(obj.LMSVertex.column, obj.LMSVertex.row);
			gph.DrawLine(&pen, p1, p2);
			gph.DrawLine(&pen, p2, p3);
			gph.DrawLine(&pen, p3, p1);
		}
		else //if ^ or > or <
		{
			Point p1(obj.topVertex.column, obj.topVertex.row);
			Point p2(obj.RMSVertex.column, obj.RMSVertex.row);
			Point p3(obj.LMSVertex.column, obj.LMSVertex.row);
			gph.DrawLine(&pen, p1, p2);
			gph.DrawLine(&pen, p2, p3);
			gph.DrawLine(&pen, p3, p1);
		}
		
	}
	else if (obj.assumption == "Circle")
	{
		Point p1(obj.LMSVertex.column, obj.topVertex.row);
		Point p2(obj.RMSVertex.column, obj.topVertex.row);
		Point p3(obj.RMSVertex.column, obj.bottomVertex.row);
		Point p4(obj.LMSVertex.column, obj.bottomVertex.row);
		gph.DrawLine(&pen, p1, p2);//top line
		gph.DrawLine(&pen, p2, p3);//right line
		gph.DrawLine(&pen, p3, p4);//bottom line
		gph.DrawLine(&pen, p4, p1);//left line
	}
	
}


void lookObj(std::string wrd,HDC hdc)
{
	RecognizedObject obj;
	for (int i = 0; i < recObjs.size(); i++)
	{
		obj = recObjs[i];
		if (obj.getAssumption() == wrd)
		{
			//need to take vertices and paint the lines between them 
			highlightObj(obj,hdc);
		}

	}
}



int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	//init GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);//start GDI+

	WNDCLASSEX wcex;
	WNDCLASSEX wcex2;
	HWND hWnd2;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc1;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Summer Project"),
			NULL);

		return 1;
	}

	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowEx(
		0,
		szWindowClass, // the name of the application
		szTitle,       //the text that appears in the title bar
		WS_MINIMIZEBOX | WS_SYSMENU, //the type of window to create, user cannot resize the window
		CW_USEDEFAULT, CW_USEDEFAULT,  //initial position (x, y)
		800, 600,  //initial size (width, length)
		HWND_DESKTOP, //the parent of this window
		NULL,   //this application does not have a menu bar
		hInstance, //the first parameter from WinMain
		NULL   //not used in this application
		);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Summer Project"),
			NULL);

		return 1;
	}

	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(hWnd, nCmdShow);
	bool end = false;
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	while (!end)
	{
		if (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (winToOpen != none)
		{
			switch (winToOpen)
			{
			case win2:
				if (!win2Open)
					createWnd2(wcex2, hWnd2, hInst, nCmdShow);
				break;
			}
			winToOpen = none;
		}
		if (!win2Open && !win1Open)
			end = true;	
	}
	MessageBox(NULL, "Bye!", "Summer Project", MB_ICONINFORMATION);
	// shut down - only once per process
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return (int)msg.wParam;
}

void createWnd2(WNDCLASSEX& wc, HWND& hwnd, HINSTANCE hInst, int nCmdShow)
	{
		if (!winWasRegistered)
		{
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = WndProc2;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInst;
			wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLICATION));
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = "win2";
			wc.hIconSm = NULL;

			if (!RegisterClassEx(&wc))
			{
				MessageBox(NULL,
					_T("Call to RegisterClassEx failed!"),
					_T("Win32 Summer Project"),
					MB_ICONERROR);
			}
			else
				winWasRegistered = true;
		}
		hwnd = CreateWindowEx(
			0,
			wc.lpszClassName, // the name of the application
			"Found objects",       //the text that appears in the title bar
			WS_OVERLAPPEDWINDOW, //the type of window to create, user cannot resize the window
			CW_USEDEFAULT, CW_USEDEFAULT,  //initial position (x, y)
			800, 600,  //initial size (width, length)
			NULL, //the parent of this window
			NULL,   //this application does not have a menu bar
			hInst, //the first parameter from WinMain
			NULL   //not used in this application
			);

		if (!hwnd)
		{
			MessageBox(NULL,
				_T("Call to CreateWindow failed!"),
				_T("Summer Project"),
				NULL);
		}

		// hWnd: the value returned from CreateWindow
		// nCmdShow: the fourth parameter from WinMain
		ShowWindow(hwnd, nCmdShow);
	}


LRESULT CALLBACK WndProc1(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	//handlers
	
	Rect rect(20, 20, 400, 400);
	Image image(L"512x512.png", true);
	Image image1(L"1024x1024.png", true);
	Image image2(L"64x64L.png", true);
	Image image3(L"40Objects.png", true);
	Image image4(L"varObj.png", true);
	Image image5(L"hardcore.png", true);
	Image image6(L"TriangleRotation.png", true);
	Image image7(L"handraw.png", true);
	int an=0;
	int an2 = 0;
	
	Graphics grpx(GetDC(hWnd));

	//handle the messages
	switch (message)
	{
	case WM_CREATE:
	{
		read_png_file(INPUT_FILE);//start reading the image
		analyze_the_picture();//analyze the png image
		recObjs = returnRecObjectsList();
		write_png_file(OUTPUT_FILE);
		win1Open = true;
		CreateWindow("BUTTON", "1024x1024", WS_VISIBLE | WS_CHILD | WS_BORDER,
			500, 255, 100, 20, hWnd, (HMENU)1, NULL, NULL);

		CreateWindow("BUTTON", "512x512", WS_VISIBLE | WS_CHILD | WS_BORDER,
			500, 220, 100, 20, hWnd, (HMENU)2, NULL, NULL);

		CreateWindow("BUTTON", "64x64 Column", WS_VISIBLE | WS_CHILD | WS_BORDER,
			500, 185, 100, 20, hWnd, (HMENU)3, NULL, NULL);

		CreateWindow("BUTTON", "40 Objects", WS_VISIBLE | WS_CHILD | WS_BORDER,
			500, 150, 100, 20, hWnd, (HMENU)4, NULL, NULL);

		CreateWindow("BUTTON", "Variation of Shapes", WS_VISIBLE | WS_CHILD | WS_BORDER,
			640, 150, 130, 20, hWnd, (HMENU)5, NULL, NULL);

		CreateWindow("BUTTON", "Hardcore", WS_VISIBLE | WS_CHILD | WS_BORDER,
			640, 185, 130, 20, hWnd, (HMENU)6, NULL, NULL);

		CreateWindow("BUTTON", "Rotations", WS_VISIBLE | WS_CHILD | WS_BORDER,
			640, 220, 130, 20, hWnd, (HMENU)7, NULL, NULL);

		CreateWindow("BUTTON", "Hand Made", WS_VISIBLE | WS_CHILD | WS_BORDER,
			640, 255, 130, 20, hWnd, (HMENU)8, NULL, NULL);

		TextField = CreateWindow("EDIT", "Square", WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			470, 360, 200, 20, hWnd, NULL, NULL, NULL);

		CreateWindow("STATIC", "Type searched object (Square, Triangle, Circle) below: ", WS_CHILD | WS_VISIBLE,
			450, 320, 330, 40, hWnd, NULL, NULL, NULL);

		CreateWindow("STATIC", "Please choose the image: ", WS_CHILD | WS_VISIBLE,
			450, 100, 330, 40, hWnd, NULL, NULL, NULL);

		CreateWindow("STATIC", "Welcome to the Geometric Shape Recognition Program!", WS_CHILD | WS_VISIBLE,
			450, 20, 330, 40, hWnd, NULL, NULL, NULL);

		CreateWindow("BUTTON", "FIND", WS_VISIBLE | WS_CHILD | WS_BORDER,
			600, 390, 100, 20, hWnd, (HMENU)9, GetModuleHandle(NULL), NULL);
		break;
	}
		
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			GetWindowText(TextField, szText, 100);
			//wrd = CW2A(szText);
			wrd = szText;
			//output the changes into the image
			winToOpen = win2;
			
			break;  
		}

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
			//1024
		case 1:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image1, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "1024x1024.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);

			
			break;

			//512 original
		case 2:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "512x512.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;
		
			//64x64
		case 3:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image2, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "64x64L.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

			//huge number testing
		case 4:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image3, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "40Objects.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

		case 5:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image4, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "varObj.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

		case 6:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image5, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "hardcore.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

		case 7:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image6, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "TriangleRotation.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

		case 8:
			hdc = BeginPaint(hWnd, &ps);
			//redraw the original image
			grpx.DrawImage(&image7, rect);
			EndPaint(hWnd, &ps);
			INPUT_FILE = "handraw.png";
			clearVector();
			an2=read_png_file(INPUT_FILE);//start reading the image
			an=analyze_the_picture();//analyze the png image
			recObjs = returnRecObjectsList();
			write_png_file(OUTPUT_FILE);
			break;

			//find btn
		case 9:
			
			GetWindowText(TextField, szText, 100);
			//wrd = CW2A(szText);
			wrd = szText;
			
			//output the changes into the image
			winToOpen = win2;

			break;
		}
		if (an != 0)
		{
			::MessageBeep(NULL);
			::MessageBox(hWnd, "Ups! Make sure the image is in RGB format!", "Error", NULL);
		}
		if (an2 != 0)
		{
			::MessageBeep(NULL);
			::MessageBox(hWnd, "Program cannot read the image! Make sure it is reachable and in PNG format!", "Error", NULL);
		}
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);

		//draw the original image
		grpx.DrawImage(&image, rect);
		EndPaint(hWnd, &ps);
		break;
	}
		

	case WM_DESTROY:
		win1Open = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	
	return 0;
}

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Image imageOut(L"outputImage.png",true);
	
	Rect rect(0, 0, imageOut.GetWidth(), imageOut.GetHeight());
	HDC hdc;
	PAINTSTRUCT ps;

	Graphics grpx(GetDC(hWnd));
	//handle the messages
	switch (message)
	{
	case WM_CREATE:
	{
		win2Open = true;
		break;
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		//draw the original image
		grpx.DrawImage(&imageOut, rect);
		lookObj(wrd,hdc);
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
		win2Open = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}


	return 0;
}