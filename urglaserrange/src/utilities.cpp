#include "utilities.h"

namespace Tinker{

	string LOGFILE = "log.txt";

	utilities::utilities()
	{
		
	}

	Tinker::utilities::~utilities()
	{
	}

	void Tinker::utilities::log(string contents, string filePath)
	{
		std::ofstream file;
		file.open(filePath, ios_base::out | ios_base::app);
		file << contents << endl;
		file.close();
	}

	bool Tinker::utilities::file_exists(string file) {
		struct stat buffer;
		return (stat(file.c_str(), &buffer) == 0);
	}
}