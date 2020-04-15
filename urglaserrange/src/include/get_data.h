#pragma once

#include "common.h"
#include "classifier.h"
#include "communications.h"

using namespace qrk;
using namespace std;

namespace Tinker {
	class get_data
	{
	private:
		float urg_min_x, urg_min_y, urg_max_x, urg_max_y;
		float max_distance;

		// The IP address of the user application to send data to. This is not the sensor IP address.
		string ip_address;

		// The port of the user application to send data to.
		unsigned int port;
		unsigned int devId;

		int process_touch_events;

		int max_num_of_touch = 4;
		float max_distance_within_one_input = 10000.0; // this is the pow^2 of the actual distance
		float min_distance_for_stationary = 900.0;

		vector<touch_state> touch_input_states;
		void manage_states(vector<data_point> condensed);

		// classifier object to do the classification of input points
		classifier* classifier_obj;

		// communications object to send the data to necessary parties
		communications* communications_obj;

		vector<data_point> list_of_condensed_clustered_points;

	public:
		get_data();
		~get_data();

		void initialize_with_params(string params, string _ip, unsigned int _port, unsigned int _process_touch, unsigned int return_device_id);
		vector<data_point> condense_to_one_point(vector<vector<data_point>> clustered_points);
		void process(const Urg_driver& urg, const vector<long>& data, long time_stamp);

		void test_send_any();
	};
}