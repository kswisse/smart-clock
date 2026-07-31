#ifndef ARDUINOJSON_H_MOCK
#define ARDUINOJSON_H_MOCK
#ifdef SIMULATION

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>

class String;
class JsonObject;
class JsonArray;

class JsonVariant {
public:
  enum Type { UNSET=0, INT=1, BOOL=2, FLOAT=3, STRING=4, OBJECT=5, ARRAY=6 };

  JsonVariant() : _type(UNSET), _intVal(0), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(int v) : _type(INT), _intVal(v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(unsigned int v) : _type(INT), _intVal((int)v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(unsigned long v) : _type(INT), _intVal((int)v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(uint16_t v) : _type(INT), _intVal(v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(uint8_t v) : _type(INT), _intVal(v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(long v) : _type(INT), _intVal((int)v), _boolVal(false), _floatVal(0.0f) {}
  JsonVariant(bool v) : _type(BOOL), _intVal(0), _boolVal(v), _floatVal(0.0f) {}
  JsonVariant(float v) : _type(FLOAT), _intVal(0), _boolVal(false), _floatVal(v) {}
  JsonVariant(double v) : _type(FLOAT), _intVal(0), _boolVal(false), _floatVal((float)v) {}
  JsonVariant(const char* v) : _type(STRING), _intVal(0), _boolVal(false), _floatVal(0.0f),
    _strVal(v ? v : "") {}
  JsonVariant(const std::string& v) : _type(STRING), _intVal(0), _boolVal(false), _floatVal(0.0f),
    _strVal(v) {}
  JsonVariant(const String& v);

  operator bool() const;
  operator int() const { return _intVal; }
  operator uint8_t() const { return (uint8_t)_intVal; }
  operator uint16_t() const { return (uint16_t)_intVal; }
  operator uint32_t() const { return (uint32_t)_intVal; }
  operator unsigned long() const { return (unsigned long)_intVal; }
  operator long() const { return (long)_intVal; }
  operator float() const { return _floatVal; }
  operator double() const { return (double)_floatVal; }
  operator const char*() const { return _strVal.c_str(); }

  JsonVariant& operator=(int v) { _type = INT; _intVal = v; return *this; }
  JsonVariant& operator=(unsigned int v) { _type = INT; _intVal = (int)v; return *this; }
  JsonVariant& operator=(unsigned long v) { _type = INT; _intVal = (int)v; return *this; }
  JsonVariant& operator=(long v) { _type = INT; _intVal = (int)v; return *this; }
  JsonVariant& operator=(uint16_t v) { _type = INT; _intVal = v; return *this; }
  JsonVariant& operator=(uint8_t v) { _type = INT; _intVal = v; return *this; }
  JsonVariant& operator=(bool v) { _type = BOOL; _boolVal = v; return *this; }
  JsonVariant& operator=(float v) { _type = FLOAT; _floatVal = v; return *this; }
  JsonVariant& operator=(double v) { _type = FLOAT; _floatVal = (float)v; return *this; }
  JsonVariant& operator=(const char* v) { _type = STRING; _strVal = v ? v : ""; return *this; }
  JsonVariant& operator=(const String& v);
  JsonVariant& operator=(const JsonVariant& other);

  JsonVariant& operator[](const char* key);
  JsonVariant& operator[](int idx);
  JsonVariant operator[](const char* key) const;
  JsonVariant operator[](int idx) const;

  bool containsKey(const char* key) const;

  bool operator|(bool d) const { return _type == BOOL ? _boolVal : d; }
  int operator|(int d) const { return _type == INT ? _intVal : d; }
  long operator|(long d) const { return _type == INT ? (long)_intVal : d; }
  unsigned long operator|(unsigned long d) const { return _type == INT ? (unsigned long)_intVal : d; }
  uint8_t operator|(uint8_t d) const { return _type == INT ? (uint8_t)_intVal : d; }
  uint16_t operator|(uint16_t d) const { return _type == INT ? (uint16_t)_intVal : d; }
  uint32_t operator|(uint32_t d) const { return _type == INT ? (uint32_t)_intVal : d; }
  size_t operator|(size_t d) const { return _type == INT ? (size_t)_intVal : d; }
  float operator|(float d) const { return _type == FLOAT ? _floatVal : d; }
  double operator|(double d) const { return _type == FLOAT ? (double)_floatVal : d; }
  const char* operator|(const char* d) const { return _type == STRING ? _strVal.c_str() : d; }
  String operator|(const String& d) const;

  bool operator==(const JsonVariant& o) const;
  bool operator==(int v) const { return _type == INT && _intVal == v; }
  bool operator==(bool v) const { return _type == BOOL && _boolVal == v; }
  bool operator==(const char* v) const { return _type == STRING && _strVal == (v ? v : ""); }

  template<typename T> T as() const;

  std::string toJsonString() const;

  Type getType() const { return (Type)_type; }

  void ensureObject();
  void ensureArray();

  std::map<std::string, JsonVariant>& asObject() { ensureObject(); return *_object; }
  std::vector<JsonVariant>& asArray() { ensureArray(); return *_array; }

  int _type;
  int _intVal;
  bool _boolVal;
  float _floatVal;
  std::string _strVal;
  std::shared_ptr<std::map<std::string, JsonVariant>> _object;
  std::shared_ptr<std::vector<JsonVariant>> _array;
};

class JsonArray : public JsonVariant {
public:
  using JsonVariant::JsonVariant;

  size_t size() const;
  JsonVariant& add(const char* v);
  JsonVariant& add(int v);
  JsonVariant& add(bool v);
  JsonVariant& add(const JsonVariant& v);
  JsonVariant operator[](int idx) const;
  JsonVariant& operator[](int idx);

  struct iterator {
    std::vector<JsonVariant>::iterator _it;
    iterator(std::vector<JsonVariant>::iterator it) : _it(it) {}
    iterator operator++() { ++_it; return *this; }
    JsonVariant operator*();
    bool operator!=(const iterator& o) const { return _it != o._it; }
  };
  iterator begin();
  iterator end();

  JsonObject createNestedObject();
};

class JsonObject : public JsonVariant {
public:
  using JsonVariant::JsonVariant;
  using JsonVariant::operator[];
  using JsonVariant::containsKey;

  JsonObject() = default;
  JsonObject(const JsonVariant& v) {
    _type = v._type; _intVal = v._intVal; _boolVal = v._boolVal;
    _floatVal = v._floatVal; _strVal = v._strVal;
    _object = v._object; _array = v._array;
  }
  JsonObject(const JsonVariant& v, bool) {
    _type = v._type; _intVal = v._intVal; _boolVal = v._boolVal;
    _floatVal = v._floatVal; _strVal = v._strVal;
    _object = v._object; _array = v._array;
  }

  JsonVariant createNestedObject(const char* key);
  JsonArray createNestedArray(const char* key);
};

class DeserializationError {
public:
  DeserializationError() : _ok(true) {}
  DeserializationError(int code) : _ok(code == 0) {}
  operator bool() const { return !_ok; }
  const char* c_str() const { return _ok ? "" : "parse error"; }
private:
  bool _ok;
};

class StaticJsonDocumentBase {
public:
  StaticJsonDocumentBase() : _root(new JsonVariant()) {}
  virtual ~StaticJsonDocumentBase() {}

  JsonVariant& operator[](const char* key) { return (*_root)[key]; }
  JsonVariant& operator[](int idx) { return (*_root)[idx]; }
  JsonVariant operator[](const char* key) const { return (*_root)[key]; }
  JsonVariant operator[](int idx) const { return (*_root)[idx]; }
  bool containsKey(const char* key) const { return _root->containsKey(key); }

  JsonObject createNestedObject(const char* key);
  JsonArray createNestedArray(const char* key);

  template<typename T> T as() const { return T(); }

  JsonVariant& root() { return *_root; }
  const JsonVariant& root() const { return *_root; }

protected:
  std::shared_ptr<JsonVariant> _root;
};

template<size_t N>
class StaticJsonDocument : public StaticJsonDocumentBase {};

class DynamicJsonDocument : public StaticJsonDocumentBase {
public:
  DynamicJsonDocument(size_t) {}
};

typedef DynamicJsonDocument JsonDocument;

bool deserializeJson(StaticJsonDocumentBase& doc, const char* json);
bool deserializeJson(StaticJsonDocumentBase& doc, const String& json);
bool serializeJson(const StaticJsonDocumentBase& doc, String& output);
bool serializeJson(const StaticJsonDocumentBase& doc, char* buffer, size_t bufferSize);

#define JSON_DOC_SIZE 1024

// as<T> specializations (must be after all class definitions)
template<> inline JsonObject JsonVariant::as<JsonObject>() const {
  JsonObject obj;
  if (_type == OBJECT && _object) {
    obj._type = OBJECT;
    obj._object = _object;
  }
  return obj;
}
template<> inline JsonArray JsonVariant::as<JsonArray>() const {
  JsonArray arr;
  if (_type == ARRAY && _array) {
    arr._type = ARRAY;
    arr._array = _array;
  }
  return arr;
}

#endif // SIMULATION
#endif // ARDUINOJSON_H_MOCK
