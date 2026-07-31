#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <Arduino.h>

class StringUtil {
public:
  static String trim(const String& str);
  static bool startsWith(const String& str, const String& prefix);
  static bool endsWith(const String& str, const String& suffix);
  static String toLowerCase(const String& str);
  static String toUpperCase(const String& str);
  static String repeat(const String& str, int count);
  static String padLeft(const String& str, size_t len, char pad = ' ');
  static String padRight(const String& str, size_t len, char pad = ' ');
  static int indexOf(const String& str, const String& search, int start = 0);
  static String replace(const String& str, const String& from, const String& to);
  static String substring(const String& str, int from, int to = -1);
  static size_t length(const char* str);
  static bool isEmpty(const String& str);
  static bool equals(const String& a, const String& b);
};

#endif // STRING_UTIL_H
