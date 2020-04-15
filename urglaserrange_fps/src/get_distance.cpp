/*!
\based on example get_distance.cpp Obtains distance data
\by author Satofumi KAMIMURA

modified by Sonny Kurniawan, initial draft at 2017/11/01
*/

#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include <iostream>
#include <fstream>
#include <numeric>

// oscpack
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

// OPENCV
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "get_data.h"
#include <ctime>
using namespace qrk;
using namespace std;

#define OUTPUT_BUFFER_SIZE 1024

const char* ip_address = "127.0.0.1";
const char* logpath = ".";
int times_reconnected = 0;
unsigned int port = 5001;
unsigned int device_id = 0;
unsigned int grouping_flag = 0;
unsigned int max_retry_time = 0;

bool show_found_data = false;
bool return_device_id = false;
bool force_abort = false;

// All units in mm
float urg_min_x = 200.0, urg_min_y = -1800.0, urg_max_x = 1800.0, urg_max_y = 2000.0;
float max_distance_preset = 500.0;

cv::Point2f urg_pt[] = {
	cv::Point2f(0, 0),  // 左上
	cv::Point2f(0, 0),  // 左下
	cv::Point2f(0, 0),  // 右下
	cv::Point2f(0, 0) }; // 右上
const cv::Point2f unity_pt_3d[] = {
	// x 左右 , y 上下
	cv::Point2f(-23.0 , -0.3),  // 左上
	cv::Point2f(-23.0 , -12.0),    // 左下
	cv::Point2f(23.0 , -12.0),   // 右下
	cv::Point2f(23.0 , -0.3) };  // 右上

// homography 行列を算出
cv::Mat homography_matrix = cv::getPerspectiveTransform(urg_pt, unity_pt_3d);

namespace
{
	void print_data(const Urg_driver& urg,
		const vector<long>& data, long time_stamp)
	{
#if 0
		// 前方のデータのみを表示
		int front_index = urg.step2index(0);
		cout << data[front_index] << " [mm], ("
			<< time_stamp << " [msec])" << endl;
#else
		// 全てのデータの X-Y の位置を表示
		long min_distance = urg.min_distance();
		long max_distance = urg.max_distance();
		max_distance = max_distance_preset;

		// Variables to hold multi touch position data
		vector<float> avgx(4);
		vector<float> avgy(4);

		// Group of points that hold distance information
		vector<vector<float>> vx(4);
		vector<vector<float>> vy(4);

		int step = 0;
		int prev = 0;
		int current_index = 0;

		//cout << "min:" << min_distance << "[mm] max: " << max_distance << "[mm] time: " << time_stamp << endl;

		size_t data_n = data.size();
#ifdef DEBUG
		cout << "data point=" << data_n << endl;
#endif

		for (size_t i = 0; i < data_n; i++) {
			long l = data[i];

			double radian = urg.index2rad(i);
			if ((l <= min_distance) || (l >= max_distance)) {
				continue;
			}

			long x = static_cast<long>(l * cos(radian));
			long y = static_cast<long>(l * sin(radian));

			//account for floor and other obstacles
			if (x > urg_max_x || x < urg_min_x || y > urg_max_y || y < urg_min_y) {
				continue;
			}

			/* The point is within valid range and we process it to detect hands */
			// 3x1 matrix
			cv::Mat urgPosMat = (cv::Mat_<double>(3, 1) << (double)x, (double)y, 1.0);
			cv::Mat unityPosMat = homography_matrix * urgPosMat;

			float ux = (float)urgPosMat.at<double>(0, 0);
			float uy = (float)urgPosMat.at<double>(1, 0);

			step = i;

			if (current_index < 4) {


				if (step - prev < 20) {
					vx[current_index].push_back(ux);
					vy[current_index].push_back(uy);
				}
				else {
					if (vx[0].size() > 0) {
						current_index++;
					}
				}
			}
			prev = step;

#ifdef DEBUG
			std::cout << "(" << x << ", " << y << ") => (" << ux << ", " << uy << ")" << " rad: " << i << " dist: " << l << " [mm]" << std::endl;
#endif
		}

		// For every group, make sure there are more than 3 points and take their average
		if (grouping_flag == 0) {
			for (int i = 0; i < 4; i++) {
				if (vx[i].size() > 2) {
					int position_of_smallest_x = std::distance(vx[i].begin(), std::min_element(vx[i].begin(), vx[i].end()));

					avgx[i] = vx[i][position_of_smallest_x];
					avgy[i] = vy[i][position_of_smallest_x];
				}
				else {
					avgx[i] = 0;
					avgy[i] = 0;
				}
			}
		}
		else {
			for (int i = 0; i < 4; i++) {
				if (vx[i].size() > 2) {
					float size = vx[i].size();
					float sum = std::accumulate(vx[i].begin(), vx[i].end(), 0);
					avgx[i] = sum / size;
					float sizey = vy[i].size();
					float sumy = std::accumulate(vy[i].begin(), vy[i].end(), 0);
					avgy[i] = sumy / sizey;
				}
				else {
					avgx[i] = 0;
					avgy[i] = 0;
				}
			}
		}
		if (vx[0].size() > 2) {

			UdpTransmitSocket transmitSocket(IpEndpointName(ip_address, port));
			char buffer[OUTPUT_BUFFER_SIZE];
			osc::OutboundPacketStream ps(buffer, OUTPUT_BUFFER_SIZE);
			char str[200];

			if (return_device_id) {
				sprintf(str, "%f, %f, %f, %f, %f, %f, %f, %f, %d", avgx[0], avgy[0], avgx[1], avgy[1], avgx[2], avgy[2], avgx[3], avgy[3], device_id);
			}
			else {
				sprintf(str, "%f, %f, %f, %f, %f, %f, %f, %f", avgx[0], avgy[0], avgx[1], avgy[1], avgx[2], avgy[2], avgx[3], avgy[3]);
			}

			if (show_found_data) {
				cout << "found data : " << to_string(avgx[0]) << ", " << to_string(avgy[0]) << endl;
			}

			char *toSendText = str;
			transmitSocket.Send(toSendText, strlen(toSendText) + 1);
		}
#endif
	}
}


inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
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


int main(int argc, char *argv[])
{
	char * id = getCmdOption(argv, argv + argc, "-id");

	show_found_data = cmdOptionExists(argv, argv + argc, "-ldata");

	bool force_crash_param_exists = cmdOptionExists(argv, argv + argc, "-forcecrash");

	if (force_crash_param_exists) {
		char * seconds_until_force_crash = getCmdOption(argv, argv + argc, "-forcecrash");

		cout << "Force crash param : " << to_string(force_crash_param_exists) << " and delay time until force crash : " << seconds_until_force_crash << endl;
		if (seconds_until_force_crash) {
			max_retry_time = atoi(seconds_until_force_crash);
		}
	}

	vector<string> parameters;
	for (int i = 0; i < argc; i++) {
		std::cout << "Parameter input : " << argv[i] << endl;
		parameters.push_back(argv[i]);
	}

	if (id)
	{
		device_id = atoi(id);
		return_device_id = true;
	}
	
	if (argc < 7) {
		std::cout << "Please input min and max values for x and y" << endl;
		std::cout << "Usage : urglaserrange.exe {min X value} {min Y value} {max X value} {max Y value} {max_distance_preset} {IP address to send to} {Port to send to} {Grouping flag} -e" << endl;
		std::cout << "Options : " << endl;
		std::cout << " e : Connected with ethernet/LAN cable" << endl;
		std::cout << "Are you missing the IP address to send to ?" << endl;
		std::cout << "Are you missing the port ?" << endl;
		return 1;
	}
	else {

		std::cout << "IP address : " << argv[6] << endl;
		std::cout << "Port:  " << argv[7] << endl;

		urg_min_x = atof(argv[1]); urg_min_y = atof(argv[2]); urg_max_x = atof(argv[3]); urg_max_y = atof(argv[4]);
		max_distance_preset = atof(argv[5]);
		urg_pt[0] = cv::Point2f(urg_min_x, urg_min_y);
		urg_pt[1] = cv::Point2f(urg_max_x, urg_min_y);
		urg_pt[2] = cv::Point2f(urg_max_x, urg_max_y);
		urg_pt[3] = cv::Point2f(urg_min_x, urg_max_y);
		ip_address = argv[6];
		port = atoi(argv[7]);
		grouping_flag = atoi(argv[8]);
	
	}
	Connection_information information(argc, argv);

	// 接続
	Urg_driver urg;
	
	// ログの準備をする
	string logcontent = "";
	
	clock_t begin = clock();
	while (!urg.open(information.device_or_ip_name(),
		information.baudrate_or_port_number(),
		information.connection_type())) {

		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

		
	
		cout << "could not connect to sensor. waiting 3 seconds" << endl;
		_sleep(3000);
		times_reconnected++;
	}

	times_reconnected = 0;
	string first_connection_time = currentDateTime();
	string baudrate = to_string(information.baudrate_or_port_number());

	logcontent += "Started connection at : " + first_connection_time + "\n";

	cout << "First connection ready at : " << first_connection_time << endl;
	cout << "baudrate : " << baudrate << endl;

	long last_time_stamp = 0;
	urg.set_scanning_parameter(urg.deg2step(-90), urg.deg2step(+90), 0);

	while (1) {
		vector<long> data;
		long time_stamp = 0;

		urg.start_measurement(Urg_driver::Distance, 1, 0);

		if (!urg.get_distance(data, &time_stamp)) {


			cout << "\n" << currentDateTime() << endl;
			clock_t begin = clock();

			while (!urg.open(information.device_or_ip_name(),
				information.baudrate_or_port_number(),
				information.connection_type())) {
				cout << currentDateTime() << endl;
				clock_t end = clock();
				double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

				if (times_reconnected > 2 && force_abort && elapsed_secs > (double) max_retry_time) {

					
					cout << "Could not make reconnection during runtime. Giving up..." << endl;
					string final_log = "Giving up at : " + currentDateTime() + " and elapsed time since data failure : " + to_string(elapsed_secs) + " after " + to_string(times_reconnected) + " retry attempts.";
					cout << final_log << endl;
					abort();
				}

				cout << "could not connect to sensor. waiting 1 second" << endl;
				_sleep(1000);
				times_reconnected++;
				//cout << "urg.get_distance : " << urg.get_distance(data, &time_stamp) << endl;
				cout << "urg.status : " << urg.status() << endl;
				cout << "urg.what : " << urg.what() << endl;
				cout << currentDateTime() << "\n" <<  endl;
				urg.close();
			}
			times_reconnected = 0;
		}

#ifdef DEBUG
		cout << "curr: " << time_stamp << " last: " << last_time_stamp << " diff: " << time_stamp - last_time_stamp;
#endif

		if (time_stamp - last_time_stamp < 1000) {
			print_data(urg, data, time_stamp);
		}
		last_time_stamp = time_stamp;

		// Key Check
		const int key = cv::waitKey(10);
		if (key == 'q') {
			break;
		}
	}

#if defined(URG_MSC)
	getchar();
#endif
	return 0;
}