// web_server_app.cpp
#include "web_server_app.h"
#include "web_pages.h"

static WebServer server(80);
static SystemState* pState = nullptr;

static void handleRootRoute() {
    server.send_P(200, "text/html", INDEX_HTML);
}

static void handleDataRoute() {
    if (!pState) return;

    String jsonPayload = "{";
    jsonPayload += "\"adc\":" + String(pState->adcRawValue) + ",";
    jsonPayload += "\"dac\":" + String(pState->dacOutputValue) + ",";
    jsonPayload += "\"btn_hw\":" + String(pState->hardwareBtnState ? 1 : 0) + ",";
    jsonPayload += "\"btn_web\":" + String(pState->webBtnState ? 1 : 0);
    jsonPayload += "}";

    server.send(200, "application/json", jsonPayload);
}

static void handleToggleRoute() {
    if (!pState) return;
    pState->webBtnState = !pState->webBtnState;
    server.send(200, "text/plain", "OK");
}

void initWebServer(SystemState& sharedState) {
    pState = &sharedState;

    server.on("/", HTTP_GET, handleRootRoute);
    server.on("/api/data", HTTP_GET, handleDataRoute);
    server.on("/api/toggle", HTTP_POST, handleToggleRoute);
    server.on("/api/toggle", HTTP_GET, handleToggleRoute); // Fallback for simple GET

    server.begin();
}

void handleWebRequests() {
    server.handleClient();
}