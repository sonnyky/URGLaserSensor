/*
Classify and track points to differentiate multiple users
*/

#include "classifier.h"

Tinker::classifier::classifier() {
	max_num_of_inputs = 4;
	step_difference = 60;
}

Tinker::classifier::~classifier() {

}

void Tinker::classifier::initialize(float minX, float minY, float maxX, float maxY, float maxDist)
{
	urg_min_x = minX;
	urg_min_y = minY;
	urg_max_x = maxX;
	urg_max_y = maxY;

	max_distance = maxDist;
}

vector<vector<data_point>> Tinker::classifier::classify(const Urg_driver & urg, const vector<long>& data, long time_stamp) {

	// list of clustered points have to be initialized otherwise, exception after step = i
	vector<vector<data_point>> list_of_clustered_points(max_num_of_inputs);

	size_t data_n = data.size();
	int step = 0;
	int current_index = 0;

	for (size_t i = 0; i < data_n; i++) {
		long l = data[i];

		double radian = urg.index2rad(i);
		if (l >= max_distance) {
			continue;
		}
		data_point one_point;

		one_point.x = static_cast<float>(l * cos(radian));
		one_point.y = static_cast<float>(l * sin(radian));

		//account for floor and other obstacles
		if (one_point.x > urg_max_x || one_point.x < urg_min_x || one_point.y > urg_max_y || one_point.y < urg_min_y) {
			continue;
		}		
		//if (step == 0) step = i - 1;

		if (current_index < max_num_of_inputs) {

			if (i - step < step_difference) {
				list_of_clustered_points[current_index].push_back(one_point);
			}
			else {
				step = i;
				if (list_of_clustered_points[current_index].size() > 0) {
					current_index++;
				}
				
			}
		}
		
	}
	
	return list_of_clustered_points;
}