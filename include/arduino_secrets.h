// Fill in  your WiFi networks SSID and password
#define SECRET_SSID "-" // Set WIFI ID (Using 2.4GHz, 5GHz does not work)
#define SECRET_PASS "-" // Set WIFI passcode

// Fill in the hostname of your AWS IoT broker
#define SECRET_BROKER "---"

// Fill in the boards public certificate
const char SECRET_CERTIFICATE[] = R"(
-----BEGIN CERTIFICATE-----
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
***REMOVED***
-----END CERTIFICATE-----
)";
