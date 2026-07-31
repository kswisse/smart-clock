#ifdef SIMULATION

#include "../test_common.h"
#include "../src/utils/json_util.h"

void testApiFlow() {
  testBeginSuite("8. API Flow");

  // Simulate todo creation via REST-like flow:
  // 1. Parse request body
  // 2. Create via service
  // 3. Serialize response

  // Step 1: Build a JSON request body
  StaticJsonDocument<256> reqDoc;
  reqDoc["title"] = "API test todo";
  reqDoc["description"] = "Created via simulated API";
  reqDoc["color"] = "#e91e63";
  String reqBody;
  serializeJson(reqDoc, reqBody);
  testAssert(reqBody.length() > 0, "Request body serialized");

  // Step 2: Parse request (simulating handler logic)
  StaticJsonDocument<256> parseDoc;
  DeserializationError err = deserializeJson(parseDoc, reqBody);
  testAssert(!err, "Request body deserialized without error");

  const char* title = parseDoc["title"] | "";
  const char* desc = parseDoc["description"] | "";
  const char* color = parseDoc["color"] | "";
  testAssert(strcmp(title, "API test todo") == 0, "Parsed title matches");
  testAssert(strcmp(desc, "Created via simulated API") == 0, "Parsed description matches");

  // Step 3: Create via service (same as handler would)
  Todo created = todoService.create(title, desc, color);
  testAssert(created.id > 0, "Todo created via service");
  testAssert(strcmp(created.title, "API test todo") == 0, "Created todo title correct");

  // Step 4: Build JSON response
  StaticJsonDocument<512> resDoc;
  resDoc["success"] = true;
  resDoc["code"] = 200;
  resDoc["message"] = "Todo created";
  resDoc["timestamp"] = millis();
  JsonObject data = resDoc.createNestedObject("data");
  data["id"] = created.id;
  data["title"] = created.title;
  data["completed"] = created.completed;

  String resBody;
  serializeJson(resDoc, resBody);
  testAssert(resBody.length() > 0, "Response body serialized");
  testAssert(resBody.indexOf("success") >= 0, "Response contains 'success'");
  testAssert(resBody.indexOf("API test todo") >= 0, "Response contains todo title");

  // Step 5: Parse response (simulating client-side)
  StaticJsonDocument<512> resParse;
  err = deserializeJson(resParse, resBody);
  testAssert(!err, "Response deserialized without error");
  testAssert(resParse["success"] == true, "Response success is true");
  testAssert((int)resParse["code"] == 200, "Response code is 200");

  // Simulate error response
  String errResp = JsonUtil::errorResponse(404, "Not found");
  testAssert(errResp.indexOf("404") >= 0, "Error response contains 404");
  testAssert(errResp.indexOf("Not found") >= 0, "Error response contains message");

  // Simulate success response
  String okResp = JsonUtil::successResponse("Created", nullptr);
  testAssert(okResp.indexOf("200") >= 0, "Success response contains 200");

  // Status endpoint simulation
  String statusJson = statusService.statusToJson();
  testAssert(statusJson.length() > 0, "Status JSON not empty");
  testAssert(statusJson.indexOf("firmware") >= 0, "Status JSON contains firmware field");

  todoService.remove(created.id);
  testEndSuite();
}

#endif
