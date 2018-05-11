#include "MyForm.h"
#include "readpng.h"
using namespace System;
using namespace System::Windows::Forms;


char* INPUT_FILE = "lineFig2.png";

[STAThread]//leave this as is
int main(array<String^>^ args) {
	readpng_version_info();
	read_png_file(INPUT_FILE);
	analyze_the_picture();
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	testWindowsForms::MyForm form;
	Application::Run(%form);

	
	

}