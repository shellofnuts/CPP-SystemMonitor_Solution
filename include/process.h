#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : _PID(pid) {
      // Initiliase the class
      User();
      Command();
      Ram();
  };
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process & a);
  bool operator==(const Process &a) const;
  bool has_Null{false};

 private:
  int _PID;
  std::string _USER{}, _COMMAND{};
  std::string _RAM;
  float _CPU_UTIL;
  long int _UPTIME;
};

#endif