#pragma once

#include "common.h"

namespace Tinker {
	class classifier {
	public:
		classifier();
		~classifier();

		void initialize(float minX, float minY, float maxX, float maxY, float maxDist);
		
		vector<vector<data_point>> classify(const Urg_driver & urg, const vector<long>& data, long time_stamp);

	private:
		int max_num_of_inputs;
		int step_difference;

		float urg_min_x, urg_min_y, urg_max_x, urg_max_y;
		float max_distance;
		unsigned int grouping_flag;
		
	};
}