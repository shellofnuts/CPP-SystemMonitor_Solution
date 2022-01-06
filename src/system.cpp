#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

System::System() {
  OS_ = LinuxParser::OperatingSystem();
  kernel_ = LinuxParser::Kernel();
}

Processor& System::Cpu() {
  cpu_ = Processor();
  return cpu_;
}

vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  for (int i : pids) {
    // Only add processes that have all values.
    Process tmp_proc(i);
    if (std::count(processes_.begin(), processes_.end(), tmp_proc) == 0 && !tmp_proc.has_Null) {
      processes_.emplace_back(tmp_proc);
    }
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

std::string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return OS_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }
