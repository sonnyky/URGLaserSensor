#include "get_data.h"

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

 Tinker::get_data::get_data()
 {
	 cout << "get_data object created." << endl;
	 classifier_obj = NULL;
	 communications_obj = NULL;

	 for (int i = 0; i < max_num_of_touch; i++) {
		 data_point init_point{0, 0};
		 touch_state state{ false, init, init_point, init_point, init_point, 0.0, 0, i };
		 touch_input_states.push_back(state);
	 }
 }

 Tinker::get_data::~get_data()
 {
 }

 float get_distance(data_point a, data_point b) {
	 float pow_2_dist = ((b.y - a.y) * (b.y - a.y)) + ((b.x - a.x) * (b.x - a.x));
	 return pow_2_dist;
 }

 void Tinker::get_data::initialize_with_params(string params,  string _ip, unsigned int _port, unsigned int _process_touch, unsigned int return_device_id) {
	 //float minX, float minY, float maxX, float maxY,  float maxDist,
	 vector<string> paramsList = split(params, ' ');
	 
	 urg_min_x = stof(paramsList[0]);
	 urg_min_y = stof(paramsList[1]);
	 urg_max_x = stof(paramsList[2]);
	 urg_max_y = stof(paramsList[3]);
	 max_distance = stof(paramsList[4]);

	 cout << "urg_min_x : " << urg_min_x << endl;
	 cout << "urg_min_y : " << urg_min_y << endl;
	 cout << "urg_max_x : " << urg_max_x << endl;
	 cout << "urg_max_y : " << urg_max_y << endl;
	 cout << "max_distance : " << max_distance << endl;

	 ip_address = _ip;
	 port = _port;
	 devId = return_device_id;

	 process_touch_events = _process_touch;

	 if (classifier_obj == NULL) {
		 classifier_obj = new classifier();
		 classifier_obj->initialize(urg_min_x, urg_min_y, urg_max_x, urg_max_y, max_distance);
	 }

	 if (communications_obj == NULL) {
		 cout << " ip and port at get_data: " << ip_address << " at port : " << to_string(port)  << endl;
		 communications_obj = new communications(ip_address, port, return_device_id);
	 }
 }

 bool cmp(const data_point& lhs, const data_point& rhs)
 {
	 return lhs.x < rhs.x;
 }

 vector<data_point> Tinker::get_data::condense_to_one_point(vector<vector<data_point>> clustered_points)
 {
	 vector<data_point> result;
	
		
	for (int i = 0; i < 4; i++) {
		data_point condensed;
		if (clustered_points[i].size() > 0) {
			int position_of_smallest_x = std::distance(clustered_points[i].begin(), std::min_element(clustered_points[i].begin(), clustered_points[i].end(), cmp));
			int middle = clustered_points[i].size() / 2;
			condensed.x = clustered_points[i][middle].x;
			condensed.y = clustered_points[i][middle].y;
		}
		else {
			condensed.x = 0.0;
			condensed.y = 0.0;
		}
		result.push_back(condensed);
	}
	 return result;
 }

 void Tinker::get_data::process(const Urg_driver & urg, const vector<long>& data, long time_stamp)
 {
	
	 // TODO : classified points not returned
	 vector<vector<data_point>> list_of_classified_points = classifier_obj->classify(urg, data, time_stamp);

	
	 vector<data_point> condensed = condense_to_one_point(list_of_classified_points);

	 switch (process_touch_events) {
		 case 0:
			 if (condensed.size() > 0) {
				 communications_obj->send_input_legacy(condensed, devId);
			 }
			 break;

		 case 1:

			 if (condensed.size() > 0) {
				 manage_states(condensed);
				 communications_obj->send_touch_events(&touch_input_states);
			 }

			 break;

		 default :
			 if (condensed.size() > 0) {
				 communications_obj->send_input(condensed);
			 }
			 break;
	 }
 }

 void Tinker::get_data::test_send_any()
 {
	 communications_obj->test_send_any();
 }

 void Tinker::get_data::manage_states(vector<data_point> condensed) 
 {
	 int num_of_points_this_frame = condensed.size(); // currently this will always be 4

	 vector<data_point> points_holder;
	 for (int i = 0; i < condensed.size(); i++) {
		 points_holder.push_back(condensed[i]);
	 }
	
	 // Reset processed flag for this frame for all touch states
	 for (int a = 0; a < max_num_of_touch; a++) {
		 touch_input_states[a].processed_this_frame = false;
	 }

	 data_point initial_point{ 0.0, 0.0 };

	 // Step 1: find inputs that are active, meaning began, moved or stationary and evaluate for these first
	 for (int b = 0; b < max_num_of_touch; b++) {
		 if (touch_input_states[b].event != init) {

			 if (touch_input_states[b].event == began || touch_input_states[b].event == moved || touch_input_states[b].event == stationary) {
				 touch_input_states[b].event = ended;
				 for (int l = 0; l < points_holder.size(); l++) {
					 // if the points are exactly zeroes, it means there are none detected
					 if (points_holder[l].x == 0 && points_holder[l].y == 0) {
						 continue;
					 }
					 // get the distance between the new point and the one recorded in the input object
					 float distance = get_distance(points_holder[l], touch_input_states[b].point);
					 //cout << "distance between : " << to_string(points_holder[l].x) << ", " << to_string(points_holder[l].y) << " and " << to_string(touch_input_states[b].point.x) << ", " << to_string(touch_input_states[b].point.y) << endl;
					 //cout << "distance for input: " << to_string(b) << " and condensed point : " << to_string(l) << " is :" <<  to_string(distance) << endl;
					
					 // we found a good candidate for this input
					 if (distance < max_distance_within_one_input) {
						 touch_input_states[b].point = points_holder[l];
						 
						 float distance_from_began_point = get_distance(points_holder[l], touch_input_states[b].began_point);
						 float distance_from_waypoint = get_distance(points_holder[l], touch_input_states[b].waypoint);
						 if (distance_from_waypoint > min_distance_for_stationary)
						 {
							 touch_input_states[b].event = moved;
							 touch_input_states[b].waypoint = points_holder[l];
							 touch_input_states[b].delta_position_magnitude = sqrt(distance_from_began_point);
							 touch_input_states[b].stationary_counter = 75; // we wait 75 frames to be sure user has not moved touch input
						 }
						 else {
							 if (touch_input_states[b].stationary_counter > 0) {
								 touch_input_states[b].event = moved;
								 touch_input_states[b].delta_position_magnitude = sqrt(distance_from_began_point);
								 touch_input_states[b].stationary_counter--;
							 }
							 else {
								 touch_input_states[b].event = stationary;
								 touch_input_states[b].began_point = points_holder[l];
							 }
						 }
						 touch_input_states[b].processed_this_frame = true;

						 points_holder.erase(points_holder.begin() + l);
						 break;
					 }
				 }
			 }
			 else if(touch_input_states[b].event == canceled || touch_input_states[b].event == ended){
				 // if the input is canceled. this should not happen a lot.
				 touch_input_states[b].event = init;
				 touch_input_states[b].point = initial_point;
				 touch_input_states[b].began_point = initial_point;
				 touch_input_states[b].waypoint = initial_point;
				 touch_input_states[b].delta_position_magnitude = 0.0;
				 touch_input_states[b].processed_this_frame = true;
			 }
		 }
	 }
	 
	 // Step 2: next find the ones that are not processed yet and insert the points in the current frame that are not assigned to inputs in the previous step
	 for (int i = 0; i < max_num_of_touch; i++) {
		 // ignore those that are already processed;
		 if (touch_input_states[i].processed_this_frame) continue;

		 // here we compare points for the inputs that are in the init state
		 for (int j = 0; j < points_holder.size(); j++) {
			 if (points_holder[j].x == 0 && points_holder[j].y == 0) {
				 continue;
			 }
			 // check if point j is suitable to assign to input i
			 touch_input_states[i].point = points_holder[j];
			 touch_input_states[i].began_point = points_holder[j];
			 touch_input_states[i].waypoint = points_holder[j];
			 touch_input_states[i].delta_position_magnitude = 0.0;
			 points_holder.erase(points_holder.begin() + j);
			 touch_input_states[i].event = began;
			 break;
		 }
		 touch_input_states[i].processed_this_frame = true;
	 }
	 /*
	 for (int c = 0; c < max_num_of_touch; c++) {
		 cout << "touch phase : " << to_string(c) << " is " << to_string(touch_input_states[c].event) << endl;
	 }
	 */
 }

