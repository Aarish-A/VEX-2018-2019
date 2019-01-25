#include "vision.hpp"

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
    if (signature == object_buffer_array[i].signature) counter++;
  }
  if (counter > 5) {
    log_ln(LOG_VISION, "%d Detected Signature: %d", pros::millis(), signature);
    return true;
  } else {
    log_ln(LOG_VISION, "%d Did not detect a good signature", pros::millis());
    return false;
  }
}
