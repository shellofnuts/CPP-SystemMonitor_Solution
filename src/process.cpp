#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() const { return _PID; }

float Process::CpuUtilization() {
  long active_time = LinuxParser::UpTime(_PID) / sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime(_PID);
  float cpu_usage = 100 * (active_time / uptime);
  return cpu_usage;
}

string Process::Command() {
  if (_COMMAND.empty()) {
    _COMMAND = LinuxParser::Command(_PID);
  }
  return _COMMAND;
}

string Process::Ram() { return LinuxParser::Ram(_PID); }

string Process::User() {
  if (_USER.empty()) {
    _USER = LinuxParser::User(_PID);
  }
  return _USER;
}

long int Process::UpTime() { return LinuxParser::UpTime(_PID); }

bool Process::operator<(Process const& a) const {
  // Order by CPU utilisation
  return this->_PID < a._PID;
}