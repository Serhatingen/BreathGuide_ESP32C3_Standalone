#pragma once

// WiFi AP + captive-portal web server.
void touchPortalActivity();  // reset inactivity timer after any HTTP request
void beginPortal();          // open the AP and start the web server
void stopPortal();           // tear down AP and web server, return to idle
void handlePortalLoop();     // must be called every loop() iteration while portal is active
