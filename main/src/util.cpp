#include "util.hpp"

int set_dz(int val, int dz) {
  dz = abs(dz);
  return (abs(val) < dz ? 0 : val);
}

int set_scaled_dz(int val, int dz) {
  dz = abs(dz);
  val = set_dz(val, dz);
  if (val != 0) {
    val = (127/(127-dz)) * (abs(val)-dz) * sgn(val);
  }

  return val;
}

/* Vectors */

vector vector::operator+(vector other) {
  return {x + other.x, y + other.y};
}

vector vector::operator-(vector other) {
  return *this + -other;
}

vector vector::operator+() {
  return *this;
}

vector vector::operator-() {
  return {-x, -y};
}

double vector::phase() {
  return atan2(x, y);
}

double vector::magnitude() {
  return sqrt(x * x + y * y);
}

vector rotate(vector v, double offset) {
  double m = v.magnitude();
  double a = v.phase() + offset;
  return {m * sin(a), m * cos(a)};
}

/* Logging */
FILE* log_file = NULL; //fopen("/usd/log.txt", "w+");
void log_init() {
  log_file = fopen("/usd/log.txt", "a");
  printf(" >>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
		return;
	}
  printf(" >>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, " >>>> %d Start Logging for Program \n", pros::millis());
}

void log(const char * format, ...)
{
  va_list args;
  va_start(args, format);

  //vfprintf(stdout, format, args); //Print to stdout
  if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
	}
  else {
    vfprintf(log_file, format, args); //Print to log_file and flush log_file
    fflush(log_file);
  }

  va_end (args);
}
