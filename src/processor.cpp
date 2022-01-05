#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

using namespace LinuxParser;
using std::vector, std::string, std::stof;

float Processor::Utilization() {
  // Returns CPU utilisation since cached values.
  vector<string> jiffies{CpuUtilization()};
  float total, active, idle, delta_total, delta_idle;
  idle = stof(jiffies[CPUStates::kIdle_]) + stof(jiffies[CPUStates::kIOwait_]);
  active = stof(jiffies[CPUStates::kUser_]) + stof(jiffies[CPUStates::kNice_]) +
          stof(jiffies[CPUStates::kSystem_]) + stof(jiffies[CPUStates::kIRQ_]) +
          stof(jiffies[CPUStates::kSoftIRQ_]) +
          stof(jiffies[CPUStates::kSteal_]);
    
    total = active + idle;
    delta_total = total - prevTotal;
    delta_idle = idle - prevIdle;
    
    // Update cached jiffies.
    prevIdle = idle;
    prevTotal = total;

    return (delta_total - delta_idle) / delta_total;
}