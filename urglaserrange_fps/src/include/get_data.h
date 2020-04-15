#pragma once
#pragma once

#include <iostream>
#include "Urg_driver.h"

using namespace qrk;
using namespace std;

namespace Tinker {
	class get_data
	{
	public:
		get_data();
		~get_data();
		static void acquire(const Urg_driver& urg, const vector<long>& data, long time_stamp);
	};
}