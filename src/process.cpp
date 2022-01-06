#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return _PID; }

float Process::CpuUtilization() {
  float cpu_usage{0.0};
  long active_time = LinuxParser::ActiveJiffies(_PID) / sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime(_PID);
  if (uptime > 0 && active_time > 0) {
    cpu_usage = static_cast<float>(active_time) / static_cast<float>(uptime);
  }
  return cpu_usage;
}

string Process::Command() {
  if (_COMMAND.empty()) {
    _COMMAND = LinuxParser::Command(_PID);
    if (_COMMAND.empty()) {
      has_Null = true;
    }
  }
  return _COMMAND;
}

string Process::Ram() {
  string memory = LinuxParser::Ram(_PID);
  memory.erase(std::remove_if(memory.begin(), memory.end(), isspace), memory.end());  // Use erase-removeif method of removing any whitespace.
  if (memory.empty()) {
    has_Null = true;
  }
  return memory;
}

string Process::User() {
  if (_USER.empty()) {
    _USER = LinuxParser::User(_PID);
    if (_USER.empty()) {
      has_Null = true;
    }
  }
  return _USER;
}

long int Process::UpTime() { return LinuxParser::UpTime(_PID); }

bool Process::operator<(Process &a) {
  // Order by CPU utilisation, in descending order.
  return this->CpuUtilization() > a.CpuUtilization();
}
bool Process::operator==(const Process &a) const {
  return this->_PID == a._PID;
}