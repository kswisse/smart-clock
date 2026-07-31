#ifndef LITTLEFS_H_MOCK
#define LITTLEFS_H_MOCK
#ifdef SIMULATION

#include "Arduino.h"
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

// Forward declare — impl in mock_littlefs.cpp
namespace littlefs_impl {
  bool mount(bool format_on_fail);
  bool exists(const char* path);
  bool readFile(const char* path, std::string& output);
  bool writeFile(const char* path, const char* data, size_t len);
  bool removeFile(const char* path);
  size_t fileSize(const char* path);
  size_t totalBytes();
  size_t usedBytes();
  std::vector<std::string> listDir(const char* dir);
  bool format();
}

// Arduino-compatible File mock
class File {
public:
  File() : _valid(false) {}
  File(const std::string& path, bool writable)
    : _path(path), _valid(true), _writable(writable), _pos(0) {
    if (!writable) {
      _valid = littlefs_impl::readFile(path.c_str(), _content);
    }
  }
  ~File() { close(); }

  operator bool() const { return _valid; }
  size_t size() const { return _content.size(); }
  size_t position() const { return _pos; }

  String readString() {
    std::string result = _content.substr(_pos);
    _pos = _content.size();
    return String(result.c_str());
  }

  size_t write(const uint8_t* data, size_t len) {
    if (!_writable || !_valid) return 0;
    _content.append(reinterpret_cast<const char*>(data), len);
    return len;
  }

  void close() {
    if (_valid && _writable && !_content.empty()) {
      littlefs_impl::writeFile(_path.c_str(), _content.c_str(), _content.size());
    }
    _valid = false;
  }

  bool isDirectory() const { return false; }

private:
  std::string _path;
  bool _valid;
  bool _writable;
  size_t _pos = 0;
  std::string _content;
};

// Arduino-compatible Dir mock
class Dir {
public:
  Dir() : _valid(false) {}
  Dir(const std::string& dir) : _dir(dir), _valid(true) {
    _entries = littlefs_impl::listDir(dir.c_str());
    _index = 0;
  }

  bool next() {
    if (_index >= _entries.size()) return false;
    _currentName = _entries[_index];
    _index++;
    return true;
  }

  String fileName() { return String(_currentName.c_str()); }
  size_t fileSize() { return 0; }
  bool isDirectory() { return false; }

private:
  std::string _dir;
  bool _valid;
  std::vector<std::string> _entries;
  size_t _index = 0;
  std::string _currentName;
};

// LittleFS class matching Arduino API
class LittleFSImpl {
public:
  bool begin(bool format_on_fail = false) { return littlefs_impl::mount(format_on_fail); }
  bool begin(bool format_on_fail, const char*, int) { return littlefs_impl::mount(format_on_fail); }
  bool exists(const char* path) { return littlefs_impl::exists(path); }
  size_t totalBytes() { return littlefs_impl::totalBytes(); }
  size_t usedBytes() { return littlefs_impl::usedBytes(); }
  bool remove(const char* path) { return littlefs_impl::removeFile(path); }
  bool format() { return littlefs_impl::format(); }

  File open(const char* path, const char* mode) {
    bool writable = (mode[0] == 'w');
    return File(std::string(path), writable);
  }

  Dir openDir(const char* dir) { return Dir(std::string(dir)); }
};

// Global LittleFS instance — defined in mock_littlefs.cpp
extern LittleFSImpl LittleFS;

#endif // SIMULATION
#endif // LITTLEFS_H_MOCK
