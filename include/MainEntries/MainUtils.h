#pragma once
#include <iostream>

namespace MainUtils
{
	//types
	enum MODE { DEFAULT, QUIET, INTERACTIVE };

	inline void DoExit(const MODE & m, const int & exitCode=0)
	{
		if (m != QUIET)
#if defined(WIN32) || defined(_WIN64)
			system("pause");
#else
			//omit for now since prompt won't disappear
		/*std::cout<<"Press enter to continue..."<<std::endl;
		std::cin.clear();
		std::cin.ignore();
		  std::cout<<"Cleaning up..."<<std::endl;*/
#endif
		exit(exitCode);
	}

	inline void PrintError(const MODE & m, const STLString & err, const bool & exit =false)
	{
		if (m != QUIET)
			DBG_COLOR(std::cout << "***" << err << endl, DBG_RED);
		if (exit)
			DoExit(m, EXIT_FAILURE);
	}
}
