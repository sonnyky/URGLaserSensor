#pragma once

#include "Urg_driver.h"
#include "Connection_information.h"
#include "math_utilities.h"
#include <iostream>
#include <numeric>

// OPENCV
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace qrk;
using namespace std;

#define OUTPUT_BUFFER_SIZE 1024

struct data_point {
	float x;
	float y;
};

enum touch_events {
	began,
	moved,
	stationary,
	ended,
	canceled,
	init
};

struct touch_state {
	bool processed_this_frame;
	touch_events event;
	data_point point;
	data_point began_point;
	data_point waypoint;
	float delta_position_magnitude;
	int stationary_counter;
	int touch_id;
};

