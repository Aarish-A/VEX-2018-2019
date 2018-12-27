#include "main.h"
#include "tracking.hpp"
#include "config.hpp"
#include "auto.hpp"

using namespace pros;
using namespace pilons::tracking;

// Vision Sensor: 640 x 400 Pixels
// Brain: 480 x 272
// Divider: 1.33 x 1.47
#define PXL_X(visionSensorPixel) (visionSensorPixel / 1.47)
#define PXL_Y(visionSensorPixel) 272 - (visionSensorPixel / 1.47)
#define NUMBER_OF_OBJECTS 1

void opcontrol() {
	vision_object_s_t objects[3];
	int buffer[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	// Draws objects to V5 Screen
	lv_obj_t* screen = lv_obj_create(NULL, NULL);
	lv_scr_load(screen);
	lv_obj_t* background = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(background, 435, 275);
	lv_obj_set_style(background, &lv_style_plain_color);
	lv_obj_t* boxLines[NUMBER_OF_OBJECTS];

	for(int i = 0; i < NUMBER_OF_OBJECTS; i++) {
		boxLines[i] = lv_line_create(screen, NULL);
	}

	while(true) {
		int y = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);
		int x = controller.get_analog(E_CONTROLLER_ANALOG_LEFT_X);
		int a = controller.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);
		setDrive(x, y, a);

		visionSensor.read_by_size(0, NUMBER_OF_OBJECTS, objects);

		if (controller.get_digital_new_press(E_CONTROLLER_DIGITAL_L1)) {
			 Vision::print_signature(visionSensor.get_signature(1));
		}

		printf("size: %d\n", objects[0].height);

		for(int i = 0; i < 10; i++) {
			if (i == 0) buffer[0] = objects[0].height;
			else buffer[i] = buffer[i - 1];
		}
		int sum = 0;
		for(int i = 0; i < 10; i++) {
			sum += buffer[i];
		}

		int average = (int)(sum / 10);

		float kP = 0.9;
		int targetSize = 70;
		if (average < targetSize  && controller.get_digital(E_CONTROLLER_DIGITAL_A) && !y && !x && !a) {
			int error = targetSize - average;
			setDrive(0, (int)(error * kP), 0);
		}

		// Draws objects to V5 Screen
		for (int i = 0; i < NUMBER_OF_OBJECTS; i++) {
				short leftX = PXL_X(objects[i].left_coord);
				short topY = PXL_Y(objects[i].top_coord);
				short rightX = PXL_X(objects[i].left_coord + objects[i].width);
				short bottomY = PXL_Y(objects[i].top_coord + objects[i].height);

				lv_point_t linePoints[] = { {leftX, topY}, {rightX, topY}, {rightX, bottomY}, {leftX, bottomY}, {leftX, topY} };
				lv_line_set_points(boxLines[i], linePoints, 5);
		}

		delay(5);
	}
}
