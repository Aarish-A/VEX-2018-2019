#include "util.hpp"
#include "config.hpp"

pros::Mutex mutex;

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
const char* log_file_name = "/usd/log.txt";
FILE* log_file = NULL; //fopen("/usd/log.txt", "w+");
int log_close_timer = 0;
void log_init() {
  time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    if (current_time == ((time_t)-1))
    {
        printf("Failure to obtain the current time.\n");
    }

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    if (c_time_string == NULL)
    {
        printf("Failure to convert the current time.\n");
    }
    else
     {
      printf("%d Date: %s \n", pros::millis(), c_time_string);

      /*
      log_file_name = "/usd/";
      strcat(log_file_name, c_time_string);
      */
    }

  log_file = fopen(log_file_name, log_mode);
  log_close_timer = 0;
  printf(">>>> %d log_init(): Start Logging for Program \n", pros::millis());
	if (log_file == NULL) {
		printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
    ctrler.rumble("--------");
    pros::delay(3000);
		return;
	}
  printf(">>>> %d log_init(): Successfully opened SD log file \n", pros::millis());
	fputs("\r\n\r\n--------------------------------------------------\r\n\r\n", log_file);
  fprintf(log_file, ">>>> %d Start Logging for Program \n", pros::millis());

  log_close_timer = pros::millis() + LOG_CLOSE_TIME;
}

void log(log_system system, const char * format, ...) {
  if (system) {
    mutex.take(LOG_MUTEX_TO);
    va_list args;
    va_start(args, format);

    vprintf(format, args); //Print to stdout
    if (log_file == NULL) {
  		//printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
  	}
    else {
      vfprintf(log_file, format, args); //Print to log_file and flush log_file
      //fflush(log_file);

      if (pros::millis() > log_close_timer) {
        log_close_timer = pros::millis() + LOG_CLOSE_TIME;

        fclose(log_file);
        while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);

        /*
        printf(">>> %d Close File\n", pros::millis());
        fprintf(log_file, ">>> %d Close File\r\n", pros::millis());
        */
      }
    }

    va_end (args);
    mutex.give();
  }
}

void log_ln(log_system system, const char * format, ...) {
  if (system) {
    mutex.take(LOG_MUTEX_TO);

    va_list args;
    va_start(args, format);

    vprintf(format, args); //Print to stdout
    printf("\n"); //New line for stdout
    if (log_file == NULL) {
  		//printf("  >>>> %d COULD NOT OPEN SD LOG FILE\n", pros::millis());
  	}
    else {
      vfprintf(log_file, format, args); //Print to log_file and flush log_file
      fprintf(log_file, "\r\n"); //Return and new line
      //fflush(log_file);

      if (pros::millis() > log_close_timer) {
        log_close_timer = pros::millis() + LOG_CLOSE_TIME;

        fclose(log_file);
        while ((log_file = fopen(log_file_name, log_mode)) == NULL) pros::delay(3);

        /*
        printf(">>> %d Close File\n", pros::millis());
        fprintf(log_file, ">>> %d Close File\r\n", pros::millis());
        */
      }
    }

    va_end (args);

    mutex.give();
  }
}
