#ifdef SIMULATION

#include "arduino_stubs.h"
#include "mock_headers/LittleFS.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// Global LittleFS instance
LittleFSImpl LittleFS;

static const char* SIM_FS_ROOT = "sim_fs";
static size_t _totalBytes = 1048576; // 1MB simulated flash

namespace littlefs_impl {

static void ensureDir(const char* path) {
  std::string s(path);
  // Create parent directories as needed
  size_t pos = 0;
  while ((pos = s.find('/', pos + 1)) != std::string::npos) {
    std::string dir = std::string(SIM_FS_ROOT) + s.substr(0, pos);
    fs::create_directories(dir);
  }
}

bool mount(bool format_on_fail) {
  printf("[SIM] LittleFS: mounting at ./%s/\n", SIM_FS_ROOT);
  try {
    if (fs::exists(SIM_FS_ROOT)) {
      if (format_on_fail) {
        printf("[SIM] LittleFS: format requested, clearing filesystem\n");
        fs::remove_all(SIM_FS_ROOT);
      }
    }
    fs::create_directories(SIM_FS_ROOT);
    printf("[SIM] LittleFS: mounted OK\n");
    return true;
  } catch (const std::exception& e) {
    printf("[SIM] LittleFS: mount FAILED: %s\n", e.what());
    return false;
  }
}

bool exists(const char* path) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  return fs::exists(fullPath);
}

bool readFile(const char* path, std::string& output) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  std::ifstream file(fullPath, std::ios::binary);
  if (!file.is_open()) return false;
  std::ostringstream ss;
  ss << file.rdbuf();
  output = ss.str();
  return true;
}

bool writeFile(const char* path, const char* data, size_t len) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  ensureDir(path);
  std::ofstream file(fullPath, std::ios::binary);
  if (!file.is_open()) {
    printf("[SIM] LittleFS: write FAILED: %s\n", path);
    return false;
  }
  file.write(data, len);
  file.close();
  return true;
}

bool removeFile(const char* path) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  try {
    return fs::remove(fullPath);
  } catch (...) {
    return false;
  }
}

size_t fileSize(const char* path) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  try {
    return fs::file_size(fullPath);
  } catch (...) {
    return 0;
  }
}

size_t totalBytes() {
  return _totalBytes;
}

size_t usedBytes() {
  size_t total = 0;
  try {
    for (const auto& entry : fs::recursive_directory_iterator(SIM_FS_ROOT)) {
      if (entry.is_regular_file()) {
        total += entry.file_size();
      }
    }
  } catch (...) {}
  return total;
}

std::vector<std::string> listDir(const char* dir) {
  std::vector<std::string> entries;
  std::string fullPath = std::string(SIM_FS_ROOT) + dir;
  try {
    if (!fs::exists(fullPath)) return entries;
    for (const auto& entry : fs::directory_iterator(fullPath)) {
      std::string name = entry.path().filename().string();
      entries.push_back(name);
    }
  } catch (...) {}
  return entries;
}

bool isDir(const char* path) {
  std::string fullPath = std::string(SIM_FS_ROOT) + path;
  std::error_code ec;
  return fs::is_directory(fullPath, ec);
}

bool format() {
  try {
    fs::remove_all(SIM_FS_ROOT);
    fs::create_directories(SIM_FS_ROOT);
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace littlefs_impl

#endif // SIMULATION
