/*
 * HXTP Micro SDK - Multi-Capability Device Example
 * 
 * Demonstrates a complex device with multiple capabilities
 * and state reporting to the cloud.
 */

#include <Arduino.h>
#include <HXTP.h>

// ---- WiFi Configuration ----
static const char* WIFI_SSID     = "YOUR_WIFI_SSID";
static const char* WIFI_PASS     = "YOUR_WIFI_PASSWORD";

// ---- HxTP Provisioning Payload ----
static const char* API_BASE_URL  = "https://api.hestialabs.com/api/v1";
static const char* DEVICE_ID     = "d123456789abcdef0123456789abcdef";
static const char* TENANT_ID     = "t-987654321";
static const char* DEVICE_SECRET = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f";

hxtp::HXTPClient* hxtpClient = nullptr;

// Internal State
bool systemEnabled = true;

// Capability: Reset System (ID: 10)
HxtpCapabilityResult handleReset(const char*, uint32_t, void*) {
    HxtpCapabilityResult result{};
    Serial.println("[System] Rebooting system in 2 seconds...");
    // Mock sleep/reboot
    result.success = true;
    return result;
}

// Capability: Enable/Disable (ID: 11)
HxtpCapabilityResult handleSetMode(const char* params, uint32_t len, void*) {
    HxtpCapabilityResult result{};
    
    int64_t mode = 0;
    if (!hxtp::json_get_int64(params, len, "enabled", &mode)) {
        result.success = false;
        result.error_code = static_cast<int16_t>(HxtpError::INVALID_PARAMS);
        snprintf(result.error_msg, sizeof(result.error_msg), "Missing 'enabled' param");
        return result;
    }
    
    systemEnabled = (mode != 0);
    Serial.printf("[System] Mode changed to: %s\n", systemEnabled ? "ENABLED" : "DISABLED");
    
    // Publish new state directly back to the cloud after change
    if (hxtpClient && hxtpClient->isConnected()) {
        char statePayload[64];
        snprintf(statePayload, sizeof(statePayload), "{\"enabled\":%s}", systemEnabled ? "true" : "false");
        hxtpClient->publishState(statePayload, strlen(statePayload));
    }
    
    result.success = true;
    return result;
}

void setup() {
    Serial.begin(115200);

    HXTPConfig config;
    config.wifi_ssid        = WIFI_SSID;
    config.wifi_password    = WIFI_PASS;
    config.api_base_url     = API_BASE_URL;
    config.device_id        = DEVICE_ID;
    config.tenant_id        = TENANT_ID;
    config.device_secret    = DEVICE_SECRET;
    config.device_type      = "advanced-hub";
    config.firmware_version = "1.0.0";
    config.verify_server    = false; 

    hxtpClient = new hxtp::HXTPClient(config);
    hxtpClient->begin();

    // Register Multiple Capabilities
    hxtpClient->registerCapability(10, "system_reset", handleReset);
    hxtpClient->registerCapability(11, "set_mode", handleSetMode);

    hxtpClient->connect();
}

void loop() {
    if (hxtpClient) {
        hxtpClient->loop();
    }
}
