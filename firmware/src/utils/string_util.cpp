#include "string_util.h"

String StringUtil::trim(const String& str) {
  int start = 0;
  int end = str.length() - 1;
  while (start <= end && isspace(str.charAt(start))) start++;
  while (end >= start && isspace(str.charAt(end))) end--;
  return str.substring(start, end + 1);
}

bool StringUtil::startsWith(const String& str, const String& prefix) {
  return str.startsWith(prefix);
}

bool StringUtil::endsWith(const String& str, const String& suffix) {
  return str.endsWith(suffix);
}

String StringUtil::toLowerCase(const String& str) {
  String result = str;
  result.toLowerCase();
  return result;
}

String StringUtil::toUpperCase(const String& str) {
  String result = str;
  result.toUpperCase();
  return result;
}

String StringUtil::repeat(const String& str, int count) {
  String result;
  for (int i = 0; i < count; i++) result += str;
  return result;
}

String StringUtil::padLeft(const String& str, size_t len, char pad) {
  if (str.length() >= len) return str;
  String padding;
  for (size_t i = 0; i < len - str.length(); i++) padding += pad;
  return padding + str;
}

String StringUtil::padRight(const String& str, size_t len, char pad) {
  if (str.length() >= len) return str;
  String padding;
  for (size_t i = 0; i < len - str.length(); i++) padding += pad;
  return str + padding;
}

int StringUtil::indexOf(const String& str, const String& search, int start) {
  return str.indexOf(search, start);
}

String StringUtil::replace(const String& str, const String& from, const String& to) {
  String result = str;
  result.replace(from, to);
  return result;
}

String StringUtil::substring(const String& str, int from, int to) {
  if (to < 0) return str.substring(from);
  return str.substring(from, to);
}

size_t StringUtil::length(const char* str) {
  return str ? strlen(str) : 0;
}

bool StringUtil::isEmpty(const String& str) {
  return str.length() == 0;
}

bool StringUtil::equals(const String& a, const String& b) {
  return a.equals(b);
}
