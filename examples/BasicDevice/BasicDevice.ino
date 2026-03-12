/*
 * HXTP Micro SDK - Basic Device Example
 * 
 * Demonstrates the minimal code required to connect a device
 * to the HxTP Cloud using the provisioning payload.
 * 
 * The SDK automatically handles:
 * - Bootstrapping MQTT broker details
 * - TLS connections and certificate validation
 * - Protocol envelope formatting, signing, and sequence management
 * - Heartbeat management
 */

#include <Arduino.h>
#include <HXTP.h>

// ---- WiFi Configuration ----
static const char* WIFI_SSID     = "YOUR_WIFI_SSID";
static const char* WIFI_PASS     = "YOUR_WIFI_PASSWORD";

// ---- HxTP Provisioning Payload ----
// Copy these values from the Hestia Labs Cloud Console when provisioning a device
static const char* API_BASE_URL  = "https://cloud.hestialabs.in/api/v1";
static const char* DEVICE_ID     = "fc205f63-9f70-48ae-9740-3872bb718511";
static const char* TENANT_ID     = "b6865f98-4627-4869-b399-ae3499b0d30a";
static const char* DEVICE_SECRET = "99031887a84ba0cbe7ceb588b3596392cf07fc5d5fe0c5e22e534a6c354cd2f1";

// ---- Global HXTP Client ----
hxtp::HXTPClient* hxtpClient = nullptr;

// ---- SDK Event Callbacks ----
void onHxtpStateChange(hxtp::HxtpClientState oldState, hxtp::HxtpClientState newState, void*) {
    Serial.print("[HXTP] State changed to: ");
    Serial.println(hxtpClient->stateStr());
}

void onHxtpError(HxtpError err, const char* msg, void*) {
    Serial.printf("[HXTP] ERROR %d: %s\n", static_cast<int>(err), msg ? msg : "Unknown");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- HxTP Basic Device Starting ---");

    // 1. Configure the HXTP Client
    HXTPConfig config;
    config.wifi_ssid        = WIFI_SSID;
    config.wifi_password    = WIFI_PASS;
    
    config.api_base_url     = API_BASE_URL;
    config.device_id        = DEVICE_ID;
    config.tenant_id        = TENANT_ID;
    config.device_secret    = DEVICE_SECRET;
    
    config.device_type      = "basic-device";
    config.firmware_version = "1.0.0";
    
    // In production, you must set verify_server = true and provide a CA cert
    config.verify_server    = false; 

    // 2. Initialize the Client
    hxtpClient = new hxtp::HXTPClient(config);
    hxtpClient->onStateChange(onHxtpStateChange, nullptr);
    hxtpClient->onError(onHxtpError, nullptr);

    HxtpError err = hxtpClient->begin();
    if (err != HxtpError::OK) {
        Serial.printf("Initialization failed: %d\n", static_cast<int>(err));
        while (true) { delay(1000); }
    }

    // 3. Connect to HxTP Cloud
    hxtpClient->connect();
}

void loop() {
    // 4. Run the SDK internal loop continuously
    if (hxtpClient) {
        hxtpClient->loop();
    }
}
