// Copyright (C) 2018-2019 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <gflags/gflags.h>
#include <iostream>

/// @brief Message for help argument
static const char help_message[] = "Print a usage message.";

/// @brief Message for video argument
static const char id_message[] = "Optional. Sends id of laser sensor when there are multiple sensors. Sets to 0 if empty";

static const char params_message[] = "Required. The detection parameters and settings.";

/// @brief Message for model argument
static const char force_crash_message[] = "Optional. If set, forces application crash after x seconds of disconnection. By default this is -1, meaning application never crashes.";

/// @brief Message for assigning Human Pose Estimation inference to device
static const char log_message[] = "Optional. Saves application log to the same folder.";

static const char devid_message[] = "Optional. Decides if app needs to return device ID. Default is false.";

/// @brief Message for performance counter
static const char ip_message[] = "Optional. Sets IP address of client application to send data to.";

/// @brief Message for not showing processed video
static const char port_message[] = "Optional. Sets port of client application to send data to.";

/// @brief Message for raw output
static const char touch_process_message[] = "Optional. Decides if touch events are processed here or by the user application. 0 means processed by user application";

static const char e_message[] = "Required. Ethernet or Serial connection, followed by the sensor IP address.";

/// @brief Defines flag for showing help message <br>
DEFINE_bool(h, false, help_message);

DEFINE_string(params, "0 -300 400 300 1000", params_message);

/// @brief Defines parameter for human pose estimation model file <br>
/// It is a required parameter
DEFINE_string(force_crash, "-1", force_crash_message);

/// @brief Defines parameter for the target device to infer on <br>
/// It is an optional parameter
DEFINE_bool(log, false, log_message);

DEFINE_uint64(devid, 0, log_message);

DEFINE_string(ip, "127.0.0.1", ip_message);

DEFINE_uint64(port, 5001, port_message);

DEFINE_uint64(touch_process, 0, touch_process_message);

DEFINE_string(e, "127.0.0.1", e_message);


/**
* @brief This function shows a help message
*/
static void showUsage() {
	std::cout << std::endl;
	std::cout << "human_pose_estimation_demo [OPTION]" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << std::endl;
	std::cout << "    -h                 " << help_message << std::endl;
	std::cout << "    -params            " << params_message << std::endl;
	std::cout << "    -id                " << id_message << std::endl;
	std::cout << "    -force_crash       " << force_crash_message << std::endl;
	std::cout << "    -log				 " << log_message << std::endl;
	std::cout << "    -ip                " << ip_message << std::endl;
	std::cout << "    -port              " << port_message << std::endl;
	std::cout << "    -touch_process     " << touch_process_message << std::endl;
	std::cout << "    -e                 " << e_message << std::endl;
}
