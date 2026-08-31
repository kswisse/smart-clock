#ifndef REQUEST_BODY_H
#define REQUEST_BODY_H

#include <Arduino.h>
#include <AsyncWebServer.h>
#include "api_response.h"

inline const char* collect_request_body(AsyncWebServerRequest* request,
                                        uint8_t* data, size_t len,
                                        size_t index, size_t total,
                                        size_t max_length) {
  if (total == 0 || total > max_length) {
    if (index == 0) ApiResponse::badRequest(request, "Request body too large");
    return nullptr;
  }

  if (index == 0) {
    request->_tempObject = malloc(total + 1);
    if (!request->_tempObject) {
      ApiResponse::serverError(request, "Not enough memory");
      return nullptr;
    }
  }

  char* body = static_cast<char*>(request->_tempObject);
  if (!body || index + len > total) return nullptr;
  memcpy(body + index, data, len);
  if (index + len != total) return nullptr;

  body[total] = '\0';
  return body;
}

#endif // REQUEST_BODY_H
