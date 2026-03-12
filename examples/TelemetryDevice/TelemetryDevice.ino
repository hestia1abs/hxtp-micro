/*
 * HXTP Micro SDK - Telemetry Device Example
 * 
 * Demonstrates publishing telemetry back to the cloud.
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

unsigned long lastTelemetryMs = 0;
const unsigned long TELEMETRY_INTERVAL = 10000; // 10 seconds

void setup() {
    Serial.begin(115200);

    HXTPConfig config;
    config.wifi_ssid        = WIFI_SSID;
    config.wifi_password    = WIFI_PASS;
    config.api_base_url     = API_BASE_URL;
    config.device_id        = DEVICE_ID;
    config.tenant_id        = TENANT_ID;
    config.device_secret    = DEVICE_SECRET;
    config.device_type      = "sensor-node";
    config.firmware_version = "1.0.0";
    config.verify_server    = false; 

    hxtpClient = new hxtp::HXTPClient(config);
    hxtpClient->begin();
    hxtpClient->connect();
}

void loop() {
    if (hxtpClient) {
        hxtpClient->loop();
        
        // Only publish telemetry when the SDK is fully READY and connected
        if (hxtpClient->isConnected() && (millis() - lastTelemetryMs >= TELEMETRY_INTERVAL)) {
            lastTelemetryMs = millis();
            
            // Read some sensor data
            float temperature = 24.5 + (random(-10, 10) / 10.0);
            float humidity = 45.0 + (random(-50, 50) / 10.0);
            
            char telemetryPayload[128];
            snprintf(telemetryPayload, sizeof(telemetryPayload), 
                "{\"temperature\":%.2f,\"humidity\":%.2f}", 
                temperature, humidity);
                
            Serial.printf("[Sensor] Publishing telemetry: %s\n", telemetryPayload);
            
            // The SDK handles packaging and signing the payload
            HxtpError err = hxtpClient->publishTelemetry(telemetryPayload, strlen(telemetryPayload));
            if (err != HxtpError::OK) {
                Serial.printf("[Sensor] Failed to publish: %d\n", static_cast<int>(err));
            }
        }
    }
}
