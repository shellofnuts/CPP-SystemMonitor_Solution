#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  // Calculate system memory usage by MemTotal - MemFree.
  float memtotal{0.0}, memfree{0.0};
  string key, value, line;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal") { memtotal = std::stof(value); }
      else if (key == "MemFree") { memfree = std::stof(value); }
    }
    return (memtotal - memfree) / memtotal;
  }
  
  return 0.0;
  }

long LinuxParser::UpTime() { 
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()){
    // Going to grab the first value in /proc/uptime. No idea what the second value is.
    stream >> uptime;
  }
  return uptime; 
  }

long LinuxParser::Jiffies() { 
  // Return all jiffies in /proc/stat cpu line.
  long total_jiffies{0}, tmp;
  std::string line, cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> tmp){
      total_jiffies += tmp;
    }
  }
  return total_jiffies; 
 }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() { 
  // Return active jiffies in /proc/stat cpu line. (First three jiffies)
  long active_jiffies{0}, tmp;
  std::string line, cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 3; i++){
      linestream >> tmp;
      active_jiffies += tmp;
    }
  }
  return active_jiffies; 
 }

long LinuxParser::IdleJiffies() { 
  // Return idle jiffies in /proc/stat cpu line. (idle and iowait)
  long idle_jiffies{0}, tmp, idle, iowait;
  std::string line, cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 3; i++){
      linestream >> tmp;
    }
    linestream >> idle >> iowait;
    idle_jiffies += idle + iowait;
  }
  return idle_jiffies; 
 }

vector<string> LinuxParser::CpuUtilization() {
  // NOTE: I'm assuming that we're converting the jiffies to str.
  // to avoid weird long -> float conversions. 
  vector<string> agg_jiffies {};
  std::string line, cpu, tmp;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> tmp){
      agg_jiffies.push_back(tmp);
    }
  }
  return agg_jiffies;
  }

int LinuxParser::TotalProcesses() { 
  int num_proc{0};
  std::string line, buffer;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> buffer;
      if (buffer == "processes"){
        linestream >> buffer;
        num_proc = std::stoi(buffer);
        return num_proc;
      }
    }
  }
  return num_proc;
 }

int LinuxParser::RunningProcesses() { 
  int active_proc{0};
  std::string line, buffer;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> buffer;
      if (buffer == "procs_running"){
        linestream >> buffer;
        active_proc = std::stoi(buffer);
        return active_proc;
      }
    }
  }
  return active_proc;
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
