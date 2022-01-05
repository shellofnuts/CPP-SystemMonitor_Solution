#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  Processor& Cpu();
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();
  std::string OperatingSystem();

 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_{};
  std::string OS_{};
};

#endif