/*
Classify and track points to differentiate multiple users
*/
#include "communications.h"

Tinker::communications::communications(string _ip, unsigned int _port, unsigned int return_device_id)
{
	ip_address = _ip;
	port = _port;
	l_return_device_id = return_device_id;
}

Tinker::communications::~communications()
{
}

void Tinker::communications::send_input(vector<data_point> condensed_points)
{
	UdpTransmitSocket transmitSocket(IpEndpointName(ip_address.c_str(), port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream ps(buffer, OUTPUT_BUFFER_SIZE);

	ps << osc::BeginBundleImmediate
		<< osc::BeginMessage("/rangefinder")
		<< condensed_points[0].x << condensed_points[0].y << condensed_points[1].x << condensed_points[1].y << condensed_points[2].x << condensed_points[2].y << condensed_points[3].x << condensed_points[3].y
		<< osc::EndMessage
		<< osc::EndBundle;

	transmitSocket.Send(ps.Data(), ps.Size());
}

void Tinker::communications::send_input_legacy(vector<data_point> condensed_points, int deviceId)
{
	UdpTransmitSocket transmitSocket(IpEndpointName(ip_address.c_str(), port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream ps(buffer, OUTPUT_BUFFER_SIZE);
	char str[200];

	if (l_return_device_id > 0) {
		sprintf(str, "%f, %f, %f, %f, %f, %f, %f, %f, %d", condensed_points[0].x, condensed_points[0].y, condensed_points[1].x, condensed_points[1].y, condensed_points[2].x, condensed_points[2].y, condensed_points[3].x, condensed_points[3].y, deviceId);
	}
	else {
		sprintf(str, "%f, %f, %f, %f, %f, %f, %f, %f", condensed_points[0].x, condensed_points[0].y, condensed_points[1].x, condensed_points[1].y, condensed_points[2].x, condensed_points[2].y, condensed_points[3].x, condensed_points[3].y);
	}

	char *toSendText = str;
	transmitSocket.Send(toSendText, strlen(toSendText) + 1);
}

void Tinker::communications::send_touch_events(vector<touch_state>* current_touch_state)
{
	UdpTransmitSocket transmitSocket(IpEndpointName(ip_address.c_str(), port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream ps(buffer, OUTPUT_BUFFER_SIZE);
	/*
	cout << "input 0 phase : " << to_string((*current_touch_state)[0].event) << " at" << to_string((*current_touch_state)[0].point.x) << ", " << to_string((*current_touch_state)[0].point.y) << endl;
	cout << "input 1 phase : " << to_string((*current_touch_state)[1].event) << " at" << to_string((*current_touch_state)[1].point.x) << ", " << to_string((*current_touch_state)[1].point.y) << endl;
	cout << "input 2 phase : " << to_string((*current_touch_state)[2].event) << " at" << to_string((*current_touch_state)[2].point.x) << ", " << to_string((*current_touch_state)[2].point.y) << endl;
	cout << "input 3 phase : " << to_string((*current_touch_state)[3].event) << " at" << to_string((*current_touch_state)[3].point.x) << ", " << to_string((*current_touch_state)[3].point.y) << endl;
	*/
	ps << osc::BeginBundleImmediate
		<< osc::BeginMessage("/rangefinder")
		<< static_cast<int> ((*current_touch_state)[0].event) << (*current_touch_state)[0].point.x << (*current_touch_state)[0].point.y << (*current_touch_state)[0].delta_position_magnitude
		<< static_cast<int> ((*current_touch_state)[1].event) << (*current_touch_state)[1].point.x << (*current_touch_state)[1].point.y << (*current_touch_state)[1].delta_position_magnitude
		<< static_cast<int> ((*current_touch_state)[2].event) << (*current_touch_state)[2].point.x << (*current_touch_state)[2].point.y << (*current_touch_state)[2].delta_position_magnitude
		<< static_cast<int> ((*current_touch_state)[3].event) << (*current_touch_state)[3].point.x << (*current_touch_state)[3].point.y << (*current_touch_state)[3].delta_position_magnitude
		<< osc::EndMessage
		<< osc::EndBundle;

	transmitSocket.Send(ps.Data(), ps.Size());
	
}

void Tinker::communications::test_send_any()
{
	UdpTransmitSocket transmitSocket(IpEndpointName(ip_address.c_str(), port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream ps(buffer, OUTPUT_BUFFER_SIZE);
	char str[200];
	sprintf(str, "%s", "any test");
	char *toSendText = str;
	transmitSocket.Send(toSendText, strlen(toSendText) + 1);
}
