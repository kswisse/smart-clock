#ifdef SIMULATION

#include "ArduinoJson.h"
#include "../arduino_stubs.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>

// ── JsonVariant helpers ────────────────────────────────────────
void JsonVariant::ensureObject() {
  if (_type != OBJECT || !_object) {
    _type = OBJECT;
    _object = std::make_shared<std::map<std::string, JsonVariant>>();
  }
}

void JsonVariant::ensureArray() {
  if (_type != ARRAY || !_array) {
    _type = ARRAY;
    _array = std::make_shared<std::vector<JsonVariant>>();
  }
}

JsonVariant& JsonVariant::operator[](const char* key) {
  if (!key) return *this;
  ensureObject();
  return (*_object)[key];
}

JsonVariant JsonVariant::operator[](const char* key) const {
  if (!key || _type != OBJECT || !_object) return JsonVariant();
  auto it = _object->find(key);
  if (it != _object->end()) return it->second;
  return JsonVariant();
}

JsonVariant& JsonVariant::operator[](int idx) {
  ensureArray();
  if (idx >= (int)_array->size()) _array->resize(idx + 1);
  return (*_array)[idx];
}

JsonVariant JsonVariant::operator[](int idx) const {
  if (_type != ARRAY || !_array || idx < 0 || idx >= (int)_array->size()) return JsonVariant();
  return (*_array)[idx];
}

bool JsonVariant::containsKey(const char* key) const {
  if (!key || _type != OBJECT || !_object) return false;
  return _object->find(key) != _object->end();
}

bool JsonVariant::operator==(const JsonVariant& o) const {
  if (_type != o._type) return false;
  switch (_type) {
    case INT: return _intVal == o._intVal;
    case BOOL: return _boolVal == o._boolVal;
    case FLOAT: return _floatVal == o._floatVal;
    case STRING: return _strVal == o._strVal;
    default: return false;
  }
}

JsonVariant& JsonVariant::operator=(const JsonVariant& other) {
  if (this != &other) {
    _type = other._type;
    _intVal = other._intVal;
    _boolVal = other._boolVal;
    _floatVal = other._floatVal;
    _strVal = other._strVal;
    _object = other._object;
    _array = other._array;
  }
  return *this;
}

// ── Escaping for JSON strings ──────────────────────────────────
static std::string jsonEscape(const std::string& s) {
  std::string out;
  out.reserve(s.size() + 8);
  for (char c : s) {
    switch (c) {
      case '"':  out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if ((unsigned char)c < 0x20) {
          char buf[8];
          snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
          out += buf;
        } else {
          out += c;
        }
    }
  }
  return out;
}

std::string JsonVariant::toJsonString() const {
  switch (_type) {
    case INT: return std::to_string(_intVal);
    case BOOL: return _boolVal ? "true" : "false";
    case FLOAT: {
      char buf[32];
      snprintf(buf, sizeof(buf), "%g", (double)_floatVal);
      return buf;
    }
    case STRING: return "\"" + jsonEscape(_strVal) + "\"";
    case OBJECT: {
      if (!_object) return "{}";
      std::string out = "{";
      bool first = true;
      for (auto& kv : *_object) {
        if (!first) out += ",";
        out += "\"" + jsonEscape(kv.first) + "\":" + kv.second.toJsonString();
        first = false;
      }
      return out + "}";
    }
    case ARRAY: {
      if (!_array) return "[]";
      std::string out = "[";
      bool first = true;
      for (auto& v : *_array) {
        if (!first) out += ",";
        out += v.toJsonString();
        first = false;
      }
      return out + "]";
    }
    default: return "null";
  }
}

// ── String conversion (forward-declared) ───────────────────────
JsonVariant::JsonVariant(const String& v) : _type(STRING), _intVal(0), _boolVal(false),
  _floatVal(0.0f), _strVal(v.c_str()) {}
JsonVariant& JsonVariant::operator=(const String& v) { _type = STRING; _strVal = v.c_str(); return *this; }
String JsonVariant::operator|(const String& d) const { return _type == STRING ? String(_strVal.c_str()) : d; }

// ── operator bool ──────────────────────────────────────────────
JsonVariant::operator bool() const {
  switch (_type) {
    case BOOL: return _boolVal;
    case INT: return _intVal != 0;
    case FLOAT: return _floatVal != 0.0f;
    case STRING: return !_strVal.empty();
    case OBJECT: return _object && !_object->empty();
    case ARRAY: return _array && !_array->empty();
    default: return false;
  }
}

// ── JsonArray methods ──────────────────────────────────────────
size_t JsonArray::size() const { return (_type == ARRAY && _array) ? _array->size() : 0; }
JsonVariant& JsonArray::add(const char* v) { ensureArray(); _array->emplace_back(v); return _array->back(); }
JsonVariant& JsonArray::add(int v) { ensureArray(); _array->emplace_back(v); return _array->back(); }
JsonVariant& JsonArray::add(bool v) { ensureArray(); _array->emplace_back(v); return _array->back(); }
JsonVariant& JsonArray::add(const JsonVariant& v) { ensureArray(); _array->push_back(v); return _array->back(); }
JsonVariant JsonArray::operator[](int idx) const {
  if (_type != ARRAY || !_array || idx < 0 || idx >= (int)_array->size()) return JsonVariant();
  return (*_array)[idx];
}
JsonVariant& JsonArray::operator[](int idx) { ensureArray(); if (idx >= (int)_array->size()) _array->resize(idx+1); return (*_array)[idx]; }
JsonArray::iterator JsonArray::begin() { ensureArray(); return iterator(_array->begin()); }
JsonArray::iterator JsonArray::end() { ensureArray(); return iterator(_array->end()); }
JsonVariant JsonArray::iterator::operator*() { return *_it; }
JsonObject JsonArray::createNestedObject() {
  ensureArray();
  _array->emplace_back();
  _array->back().ensureObject();
  JsonObject result;
  result._type = JsonVariant::OBJECT;
  result._object = _array->back()._object;
  return result;
}

// ── JsonObject methods ─────────────────────────────────────────
JsonVariant JsonObject::createNestedObject(const char* key) {
  JsonVariant& v = (*this)[key];
  v.ensureObject();
  JsonVariant result;
  result._type = OBJECT;
  result._object = v._object;
  return result;
}
JsonArray JsonObject::createNestedArray(const char* key) {
  JsonVariant& v = (*this)[key];
  v.ensureArray();
  JsonArray result;
  result._type = ARRAY;
  result._array = v._array;
  return result;
}

// ── StaticJsonDocumentBase ─────────────────────────────────────
JsonObject StaticJsonDocumentBase::createNestedObject(const char* key) {
  JsonVariant& v = (*_root)[key];
  v.ensureObject();
  JsonObject result;
  result._type = JsonVariant::OBJECT;
  result._object = v._object;
  return result;
}
JsonArray StaticJsonDocumentBase::createNestedArray(const char* key) {
  JsonVariant& v = (*_root)[key];
  v.ensureArray();
  JsonArray result;
  result._type = JsonVariant::ARRAY;
  result._array = v._array;
  return result;
}

// ── JSON Parser ────────────────────────────────────────────────
static const char* skipWhitespace(const char* p) {
  while (*p && (unsigned char)*p <= ' ') p++;
  return p;
}

static bool parseValue(const char*& p, JsonVariant& v);
static bool parseObject(const char*& p, JsonVariant& v);
static bool parseArray(const char*& p, JsonVariant& v);
static bool parseString(const char*& p, std::string& out);

static bool parseString(const char*& p, std::string& out) {
  if (*p != '"') return false;
  p++;
  out.clear();
  while (*p && *p != '"') {
    if (*p == '\\') {
      p++;
      switch (*p) {
        case '"':  out += '"'; break;
        case '\\': out += '\\'; break;
        case '/':  out += '/'; break;
        case 'b':  out += '\b'; break;
        case 'f':  out += '\f'; break;
        case 'n':  out += '\n'; break;
        case 'r':  out += '\r'; break;
        case 't':  out += '\t'; break;
        case 'u': {
          char hex[5] = {p[1],p[2],p[3],p[4],0};
          unsigned int cp = (unsigned int)strtoul(hex, nullptr, 16);
          if (cp < 0x80) out += (char)cp;
          else if (cp < 0x800) { out += (char)(0xC0|(cp>>6)); out += (char)(0x80|(cp&0x3F)); }
          else { out += (char)(0xE0|(cp>>12)); out += (char)(0x80|((cp>>6)&0x3F)); out += (char)(0x80|(cp&0x3F)); }
          p += 4;
          break;
        }
        default: out += *p; break;
      }
    } else {
      out += *p;
    }
    p++;
  }
  if (*p != '"') return false;
  p++;
  return true;
}

static bool parseNumber(const char*& p, JsonVariant& v) {
  const char* start = p;
  bool isFloat = false;
  if (*p == '-') p++;
  while (*p >= '0' && *p <= '9') p++;
  if (*p == '.') { isFloat = true; p++; while (*p >= '0' && *p <= '9') p++; }
  if (*p == 'e' || *p == 'E') { isFloat = true; p++; if (*p == '+' || *p == '-') p++; while (*p >= '0' && *p <= '9') p++; }
  std::string numStr(start, p);
  if (isFloat) { v = (float)atof(numStr.c_str()); }
  else { v = atoi(numStr.c_str()); }
  return true;
}

static bool parseValue(const char*& p, JsonVariant& v) {
  p = skipWhitespace(p);
  if (*p == '"') { std::string s; if (!parseString(p, s)) return false; v = s.c_str(); return true; }
  if (*p == '{') { v.ensureObject(); return parseObject(p, v); }
  if (*p == '[') { v.ensureArray(); return parseArray(p, v); }
  if (*p == 't') { v = true; p += 4; return true; }
  if (*p == 'f') { v = false; p += 5; return true; }
  if (*p == 'n') { v = JsonVariant(); p += 4; return true; }
  if ((*p >= '0' && *p <= '9') || *p == '-') return parseNumber(p, v);
  return false;
}

static bool parseObject(const char*& p, JsonVariant& v) {
  if (*p != '{') return false;
  p++;
  p = skipWhitespace(p);
  v.ensureObject();
  if (*p == '}') { p++; return true; }
  while (true) {
    p = skipWhitespace(p);
    std::string key;
    if (!parseString(p, key)) return false;
    p = skipWhitespace(p);
    if (*p != ':') return false;
    p++;
    JsonVariant val;
    if (!parseValue(p, val)) return false;
    v.asObject()[key] = val;
    p = skipWhitespace(p);
    if (*p == ',') { p++; continue; }
    if (*p == '}') { p++; return true; }
    return false;
  }
}

static bool parseArray(const char*& p, JsonVariant& v) {
  if (*p != '[') return false;
  p++;
  p = skipWhitespace(p);
  v.ensureArray();
  if (*p == ']') { p++; return true; }
  while (true) {
    JsonVariant val;
    if (!parseValue(p, val)) return false;
    v.asArray().push_back(val);
    p = skipWhitespace(p);
    if (*p == ',') { p++; continue; }
    if (*p == ']') { p++; return true; }
    return false;
  }
}

// ── Public API ─────────────────────────────────────────────────
bool deserializeJson(StaticJsonDocumentBase& doc, const char* json) {
  if (!json) return true;
  const char* p = skipWhitespace(json);
  if (*p == '{') {
    return !parseObject(p, doc.root());
  }
  if (*p == '[') {
    doc.root().ensureArray();
    return !parseArray(p, doc.root());
  }
  return true;
}

bool deserializeJson(StaticJsonDocumentBase& doc, const String& json) {
  return deserializeJson(doc, json.c_str());
}

bool serializeJson(const StaticJsonDocumentBase& doc, String& output) {
  output = doc.root().toJsonString().c_str();
  return true;
}

bool serializeJson(const StaticJsonDocumentBase& doc, char* buffer, size_t bufferSize) {
  std::string s = doc.root().toJsonString();
  strncpy(buffer, s.c_str(), bufferSize - 1);
  buffer[bufferSize - 1] = '\0';
  return true;
}

#endif // SIMULATION
