/*!
Main class to handle data from Hokuyo Laser Range Finders
*/

#include "common.h"
#include "communications.h"
#include "get_data.h"
#include "utilities.h"

#include <ctime>

#include "flags.hpp"

using namespace Tinker;

bool ParseAndCheckCommandLine(int argc, char* argv[]) {
	// ---------------------------Parsing and validation of input args--------------------------------------

	gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
	if (FLAGS_h) {
		showUsage();
		return false;
	}

	if (FLAGS_e.empty()) {
		throw std::logic_error("Parameter -e is not set");
	}

	if (FLAGS_params.empty()) {
		throw std::logic_error("Params are not set");
	}

	std::cout << "Parsing input parameters" << std::endl;
	return true;
}


const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[])
{
	for (int i = 0; i < argc; i++) {
		std::cout << "Parameter input : " << argv[i] << endl;
	}

	/* 
	Connection information object must be done before parsing arguments because we are leaving the urg sdk to handle -e.
	If this parameter is handled by gflags, it will be deformed. This is not ideal but the best workaround for now.
	*/
	Connection_information information(argc, argv);

	if (!ParseAndCheckCommandLine(argc, argv)) {
		return EXIT_SUCCESS;
	}

	get_data data_getter =  get_data();
	data_getter.initialize_with_params(FLAGS_params, FLAGS_ip, FLAGS_port, FLAGS_touch_process, FLAGS_devid);	

	Urg_driver urg;
	if (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {
		std::cout << "Urg_driver::open(): "
			<< information.device_or_ip_name() << ": " << urg.what() << endl;
		return 1;
	}

	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);
	long last_time_stamp = 0;

	if (FLAGS_log) {
		if (file_exists(LOGFILE)) {
			cout << "Log file already exists. Deleting old log file..." << endl;
			
			remove(LOGFILE.c_str());
		}
	}

	if (FLAGS_log) Tinker::utilities::log("First connection ready at : " + currentDateTime());
	
	while (1) {

		// Get the time before processing starts
		clock_t begin = clock();

		urg.start_measurement(Urg_driver::Distance, 1, 0);
		vector<long> data;
		long time_stamp = 0;

		// Cannot obtain data suddenly
		if (!urg.get_distance(data, &time_stamp)) {
			std::cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			if (FLAGS_log) Tinker::utilities::log("Connection lost at : " + currentDateTime() + " due to : " + urg.what());
			//urg.start_measurement(Urg_driver::Distance, 1, 0);
			//break;
		}

		if (time_stamp - last_time_stamp < 1000) {
			data_getter.process(urg, data, time_stamp);
		}
		last_time_stamp = time_stamp;

		// Key Check
		const int key = cv::waitKey(10);
		if (key == 'q') {
			break;
		}
		if (key == 't') {
			data_getter.test_send_any();
		}
		// Get time after processing finished and calculate elapsed time
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	}

#if defined(URG_MSC)
	getchar();
#endif
	return EXIT_SUCCESS;
}