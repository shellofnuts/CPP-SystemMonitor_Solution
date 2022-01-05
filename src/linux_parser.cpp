#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

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

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal") {
        memtotal = std::stof(value);
      } else if (key == "MemFree") {
        memfree = std::stof(value);
      }
    }
    return (memtotal - memfree) / memtotal;
  }

  return 0.0;
}

long LinuxParser::UpTime() {
  long uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    // Going to grab the first value in /proc/uptime. No idea what the second
    // value is.
    stream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  // Return all jiffies in /proc/stat cpu line.
  long total_jiffies{0}, tmp;
  std::string line, cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> tmp) {
      total_jiffies += tmp;
    }
  }
  return total_jiffies;
}

long LinuxParser::ActiveJiffies(int pid) {
  long active_jiffies{0};
  int const utime{13}, stime{14}, cutime{15}, cstime{16};

  std::string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/" +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<std::string> begin(linestream);
    std::istream_iterator<std::string> end;
    std::vector<string> tmp_vec(begin, end);
    
    active_jiffies += atol(tmp_vec[utime].c_str());
    active_jiffies += atol(tmp_vec[stime].c_str());
    active_jiffies += atol(tmp_vec[cutime].c_str());
    active_jiffies += atol(tmp_vec[cstime].c_str());
  }
  return active_jiffies;
}

long LinuxParser::ActiveJiffies() {
  // Return active jiffies in /proc/stat cpu line. (First three jiffies)
  long active_jiffies{0}, tmp;
  std::string line, cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 3; i++) {
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
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 3; i++) {
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
  vector<string> agg_jiffies{};
  std::string line, cpu, tmp;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> tmp) {
      agg_jiffies.push_back(tmp);
    }
  }
  return agg_jiffies;
}

int LinuxParser::TotalProcesses() {
  int num_proc{0};
  std::string line, buffer;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> buffer;
      if (buffer == "processes") {
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
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> buffer;
      if (buffer == "procs_running") {
        linestream >> buffer;
        active_proc = std::stoi(buffer);
        return active_proc;
      }
    }
  }
  return active_proc;
}

string LinuxParser::Command(int pid) {
  std::string command, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/cmdline");
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> command;
  }
  return command;
}

string LinuxParser::Ram(int pid) {
  string memory{}, key, value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/status");

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize") {
        memory = std::to_string(std::atol(value.c_str()) / 1000);
      }
    }
  }

  return memory;
}

string LinuxParser::Uid(int pid) {
  string userid{}, key, value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/status");

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid") {
        userid = value;
      }
    }
  }
  return userid;
}

string LinuxParser::User(int pid) {
  string userid = LinuxParser::Uid(pid);
  string username{};
  if (!userid.empty()) {
    string key, value, line, tmp;
    std::ifstream filestream(kPasswordPath);

    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> value >> tmp >> key;
        if (key == userid) {
          username = value;
        }
      }
    }
  }
  return username;
}


long LinuxParser::UpTime(int pid) {
  long uptime{0};
  string value, line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + "/stat");

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) {
      linestream >> value;
    }
    uptime = LinuxParser::UpTime() - std::atol(value.c_str()) / sysconf(_SC_CLK_TCK);
  }
  
  return uptime;
}
