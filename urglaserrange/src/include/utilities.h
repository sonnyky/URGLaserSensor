#pragma once

#include "common.h"
#include <iostream>
#include <fstream>

namespace Tinker {
	extern string LOGFILE;
	class utilities {

	public:
		utilities();
		~utilities();
		
		
		static bool recovered_connection(Urg_driver urg);
		static bool file_exists(string file);

		static void log(string contents, string file = LOGFILE);
		
	};
}