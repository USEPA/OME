// OMESimRunner.cpp : main project file.

#include "SimWindow.h"
#include "SimWrapper.h"

using namespace OMESimRunner;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	DBG_LOGFILE("OMESimRunnerLog.txt");
	//default manager
	SimWrapper^ mngr=gcnew SimWrapper();
	//load manager data here...

	// Create the main window and run it
	Application::Run(gcnew SimWindow(mngr));
	
	return 0;
}
