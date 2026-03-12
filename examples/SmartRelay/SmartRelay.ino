/*
 * HXTP Micro SDK - Smart Relay Example
 * 
 * Demonstrates registering a capability handler to control
 * a physical GPIO pin based on cloud commands.
 */

#include <Arduino.h>
#include <HXTP.h>

// ---- Hardware Config ----
static const int RELAY_PIN       = 2; // e.g. GPIO2 on ESP32

// ---- WiFi Configuration ----
static const char* WIFI_SSID     = "YOUR_WIFI_SSID";
static const char* WIFI_PASS     = "YOUR_WIFI_PASSWORD";

// ---- HxTP Provisioning Payload ----
static const char* API_BASE_URL  = "https://api.hestialabs.com/api/v1";
static const char* DEVICE_ID     = "d123456789abcdef0123456789abcdef";
static const char* TENANT_ID     = "t-987654321";
static const char* DEVICE_SECRET = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";

hxtp::HXTPClient* hxtpClient = nullptr;

// ---- Capability Handler ----
HxtpCapabilityResult handleToggleRelay(const char* params, uint32_t len, void*) {
    HxtpCapabilityResult result{};
    
    int64_t state = 0;
    // Extract the "state" parameter from the JSON command payload
    if (!hxtp::json_get_int64(params, len, "state", &state)) {
        result.success = false;
        result.error_code = static_cast<int16_t>(HxtpError::INVALID_PARAMS);
        snprintf(result.error_msg, sizeof(result.error_msg), "Missing 'state' param");
        return result;
    }
    
    // Apply state to GPIO
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
    Serial.printf("[Relay] State changed to: %s\n", state ? "ON" : "OFF");
    
    result.success = true;
    return result;
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    HXTPConfig config;
    config.wifi_ssid        = WIFI_SSID;
    config.wifi_password    = WIFI_PASS;
    config.api_base_url     = API_BASE_URL;
    config.device_id        = DEVICE_ID;
    config.tenant_id        = TENANT_ID;
    config.device_secret    = DEVICE_SECRET;
    config.device_type      = "smart-relay";
    config.firmware_version = "1.0.0";
    config.verify_server    = false; 

    hxtpClient = new hxtp::HXTPClient(config);
    hxtpClient->begin();

    // Register Capability: ID 1, Action "toggle"
    hxtpClient->registerCapability(1, "toggle", handleToggleRelay);

    hxtpClient->connect();
}

void loop() {
    if (hxtpClient) {
        hxtpClient->loop();
    }
}
