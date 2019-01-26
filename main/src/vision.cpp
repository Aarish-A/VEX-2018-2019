#include "vision.hpp"

pros::vision_object_s_t object_buffer_array[10];
int buffer_index = 0;

void vison_init() {

}

void vision_handle() {
  object_buffer_array[buffer_index] = vision_sensor.get_by_size(0);
  if (buffer_index < 9) buffer_index++;
  else buffer_index = 0;
}

bool detecting_signature(short signature) {
  int counter = 0;
  for (int i = 0; i < 10; i++) {
    if (signature == 2 && object_buffer_array[i].signature && object_buffer_array[i].width > 250 && object_buffer_array[i].top_coord < 40) counter++;
    else if (signature == object_buffer_array[i].signature) counter++;
  }
  if (counter > 5) {
    log_ln(LOG_VISION, "%d Detected Signature: %d", pros::millis(), signature);
    // ctrler.print(2, 0, "T:%d, H:%d   ", object_buffer_array[buffer_index].top_coord, object_buffer_array[buffer_index].height);
    return true;
  } else {
    log_ln(LOG_VISION, "%d Did not detect a good signature", pros::millis());
    return false;
  }
}
