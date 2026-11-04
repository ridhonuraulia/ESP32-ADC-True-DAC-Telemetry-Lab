// web_server_app.h
#ifndef WEB_SERVER_APP_H
#define WEB_SERVER_APP_H

#include "config.h"
#include <WebServer.h>

void initWebServer(SystemState& sharedState);
void handleWebRequests();

#endif