#pragma once

#include "common.h"

// oscpack
#include <ip/UdpSocket.h>
#include <osc/OscOutboundPacketStream.h>

namespace Tinker {

	class communications {
	public:
		communications(string _ip, unsigned int _port, unsigned int return_device_id);
		~communications();

		void send_input(vector<data_point> condensed_points);

		void send_input_legacy(vector<data_point> condensed_points, int deviceId = 0);

		void send_touch_events(vector<touch_state> * current_touch_state);

		void test_send_any();

	private:
		string ip_address = "127.0.0.1";
		unsigned int port = 5001;

		bool l_return_device_id;
	};
}