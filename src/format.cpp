#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours, minutes;
  minutes = seconds / 60;
  hours = minutes / 60;
  char formatted_time[20];
  sprintf(formatted_time, "%02d:%02d:%02ld", hours, minutes % 60, seconds % 60);
  return string(formatted_time);
}