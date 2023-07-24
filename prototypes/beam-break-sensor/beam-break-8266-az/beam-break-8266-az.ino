/*
 * This file has been adapted from the Azure_IoT_Hub_ESP8266 example included with
 * the Azure IoT SDK for C library. This MQTT client support library, helps to
 * compose and parse the MQTT topic names and messages exchanged with the
 * Azure IoT Hub.
 * 
 * For reference, please visit https://github.com/azure/azure-sdk-for-c.
 *
 * This file performs the following tasks:
 * - Synchronize the device clock with an NTP server (using Rop Gonggrijp's ezTime
 *   libary;
 * - Initialize our "az_iot_hub_client" (struct for data, part of our
 *   azure-sdk-for-c);
 * - Initialize the MQTT client (using Nick Oleary's PubSubClient, which also
 *   handles the tcp connection and TLS);
 * - Connect the MQTT client (using server-certificate validation,
 *   SAS-tokens for client authentication);
 * - Capture signals from an IR LED beam-break system and use this information to
 *   determine the occupancy of a space;
 * - Periodically send occupancy data to the Azure IoT Hub.
 */

// C99 libraries
#include <cstdlib>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// libraries for MQTT client, WiFi connection, time, and SAS-token generation.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ezTime.h>
#include <base64.h>
#include <bearssl/bearssl.h>
#include <bearssl/bearssl_hmac.h>
#include <libb64/cdecode.h>

// azure IoT SDK for C includes
#include <az_core.h>
#include <az_iot.h>
#include <azure_ca.h>

// import sensetive information
#include "mcu_secrets.h"

// follow the format '(ard;<platform>)' for developing Arduino-based platforms
#define AZURE_SDK_CLIENT_USER_AGENT "c%2F" AZ_SDK_VERSION_STRING "(ard;esp8266)"

// Utility macros and defines
#define sizeofarray(a) (sizeof(a) / sizeof(a[0]))
#define ONE_HOUR_IN_SECS 3600
#define NTP_SERVERS "pool.ntp.org", "time.nist.gov"
#define MQTT_PACKET_SIZE 1024

// declare constants for IR signal
// signal and nosignal allow for better code readability
#define noSIGNAL HIGH
#define SIGNAL LOW

// translate mcu_secrets.h into variables used by the sample
// wifi
static const char* ssid = SECRET_SSID;
static const char* password = SECRET_PASS;

//azure + mqtt
static const char* host = SECRET_IOTHUB_HOST;
static const char* device_id = SECRET_IOTHUB_ID;
static const char* device_key = SECRET_IOTHUB_KEY;
static const int port = 8883;

// memory allocated for the sample's variables and structures.
static WiFiClientSecure wifi_client;
static X509List cert((const char*)ca_pem);
static PubSubClient mqtt_client(wifi_client);
static az_iot_hub_client client;
static char sas_token[200];
static uint8_t signature[512];
static unsigned char encrypted_signature[32];
static char base64_decoded_device_key[32];
static unsigned long next_telemetry_send_time_ms = 0;
static char telemetry_topic[128];
static uint8_t telemetry_payload[100];

// set timezone
Timezone GB;

// declare pins to pull data from ir receievers
static const byte RECPIN1 = 2;
static const byte RECPIN2 = 4;

// declare bool variables for holding current and previous sensor readings
//beam 1
static bool status1;
static bool lastStatus1;
// beam 2
static bool status2;
static bool lastStatus2;

// declare variables to hold the break times of each beam
static time_t breakTime1 = 0;
static time_t breakTime2 = 0;

// declare variable to hold the occupancy count of the space
static int occupancy = 0;

// auxiliary functions ---------------------------------------------------------
// static functions can only be accessed in their source files
static void connectWifi()
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // setting the mode to "Station" and then disconnecting
  // ensures that the Wi-Fi module is in the desired state
  // before proceeding with any further Wi-Fi-related operations
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

static void initializeTime()
{
  Serial.print("Setting time using SNTP");
  // 0 * 3600 for GMT
  configTime(0 * 3600, 0, NTP_SERVERS);
  time_t now = time(NULL);
  while (now < 1510592825)
  {
    delay(500);
    Serial.print(".");
    now = time(NULL);
  }
  Serial.println("done!");
}

static char* getCurrentLocalTimeString()
{
  time_t now = time(NULL);
  return ctime(&now);
}

static void printCurrentTime()
{
  Serial.print("Current time: ");
  Serial.print(getCurrentLocalTimeString());
}

static void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
}

static void initializeClients()
{
  az_iot_hub_client_options options = az_iot_hub_client_options_default();
  options.user_agent = AZ_SPAN_FROM_STR(AZURE_SDK_CLIENT_USER_AGENT);

  wifi_client.setTrustAnchors(&cert);
  if (az_result_failed(az_iot_hub_client_init(
          &client,
          az_span_create((uint8_t*)host, strlen(host)),
          az_span_create((uint8_t*)device_id, strlen(device_id)),
          &options)))
  {
    Serial.println("Failed initializing Azure IoT Hub client");
    return;
  }

  mqtt_client.setServer(host, port);
  mqtt_client.setCallback(callback);
}

/*
 * @brief           Gets the number of seconds since UNIX epoch until now.
 * @return uint32_t Number of seconds.
 */
static uint32_t getSecondsSinceEpoch() { return (uint32_t)time(NULL); }

static int generateSasToken(char* sas_token, size_t size)
{
  az_span signature_span = az_span_create((uint8_t*)signature, sizeofarray(signature));
  az_span out_signature_span;
  az_span encrypted_signature_span
      = az_span_create((uint8_t*)encrypted_signature, sizeofarray(encrypted_signature));

  uint32_t expiration = getSecondsSinceEpoch() + ONE_HOUR_IN_SECS;

  // get signature
  if (az_result_failed(az_iot_hub_client_sas_get_signature(
          &client, expiration, signature_span, &out_signature_span)))
  {
    Serial.println("Failed getting SAS signature");
    return 1;
  }

  // base64-decode device key
  int base64_decoded_device_key_length
      = base64_decode_chars(device_key, strlen(device_key), base64_decoded_device_key);

  if (base64_decoded_device_key_length == 0)
  {
    Serial.println("Failed base64 decoding device key");
    return 1;
  }

  // SHA-256 encrypt
  br_hmac_key_context kc;
  br_hmac_key_init(
      &kc, &br_sha256_vtable, base64_decoded_device_key, base64_decoded_device_key_length);

  br_hmac_context hmac_ctx;
  br_hmac_init(&hmac_ctx, &kc, 32);
  br_hmac_update(&hmac_ctx, az_span_ptr(out_signature_span), az_span_size(out_signature_span));
  br_hmac_out(&hmac_ctx, encrypted_signature);

  // base64 encode encrypted signature
  String b64enc_hmacsha256_signature = base64::encode(encrypted_signature, br_hmac_size(&hmac_ctx));

  az_span b64enc_hmacsha256_signature_span = az_span_create(
      (uint8_t*)b64enc_hmacsha256_signature.c_str(), b64enc_hmacsha256_signature.length());

  // URl-encode base64 encoded encrypted signature
  if (az_result_failed(az_iot_hub_client_sas_get_password(
          &client,
          expiration,
          b64enc_hmacsha256_signature_span,
          AZ_SPAN_EMPTY,
          sas_token,
          size,
          NULL)))
  {
    Serial.println("Failed getting SAS token");
    return 1;
  }

  return 0;
}

static int connectToAzureIoTHub()
{
  size_t client_id_length;
  char mqtt_client_id[128];
  if (az_result_failed(az_iot_hub_client_get_client_id(
          &client, mqtt_client_id, sizeof(mqtt_client_id) - 1, &client_id_length)))
  {
    Serial.println("Failed getting client id");
    return 1;
  }

  mqtt_client_id[client_id_length] = '\0';

  char mqtt_username[128];
  // Get the MQTT user name used to connect to IoT Hub
  if (az_result_failed(az_iot_hub_client_get_user_name(
          &client, mqtt_username, sizeofarray(mqtt_username), NULL)))
  {
    printf("Failed to get MQTT clientId, return code\n");
    return 1;
  }

  Serial.print("Client ID: ");
  Serial.println(mqtt_client_id);

  Serial.print("Username: ");
  Serial.println(mqtt_username);

  mqtt_client.setBufferSize(MQTT_PACKET_SIZE);

  while (!mqtt_client.connected())
  {
    time_t now = time(NULL);

    Serial.print("MQTT connecting ... ");

    if (mqtt_client.connect(mqtt_client_id, mqtt_username, sas_token))
    {
      Serial.println("connected.");
    }
    else
    {
      Serial.print("failed, status code =");
      Serial.print(mqtt_client.state());
      Serial.println(". Trying again in 5 seconds.");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  mqtt_client.subscribe(AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC);

  return 0;
}

static void establishConnection()
{
  connectWifi();
  initializeTime();
  printCurrentTime();
  initializeClients();

  // must be restarted after SAS expiration, or the client won't be able
  // to connect/stay connected to the Azure IoT Hub.
  if (generateSasToken(sas_token, sizeofarray(sas_token)) != 0)
  {
    Serial.println("Failed generating MQTT password");
  }
  else
  {
    connectToAzureIoTHub();
  }
}

static char* getTelemetryPayload()
{
  az_span temp_span = az_span_create(telemetry_payload, sizeof(telemetry_payload));
  temp_span = az_span_copy(temp_span, AZ_SPAN_FROM_STR("{ \"Occupancy\": "));
  (void)az_span_u32toa(temp_span, occupancy, &temp_span);
  temp_span = az_span_copy(temp_span, AZ_SPAN_FROM_STR("{ \"Timestamp\": "));
  (void)az_span_i64toa(temp_span, time(NULL), &temp_span);
  //temp_span = az_span_copy(temp_span, GB.dateTime(), &temp_span);
  temp_span = az_span_copy(temp_span, AZ_SPAN_FROM_STR(" }"));
  temp_span = az_span_copy_u8(temp_span, '\0');

  return (char*)telemetry_payload;
}

static void sendTelemetry()
{
  Serial.print(millis());
  Serial.print(" ESP8266 Sending telemetry ... ");
  if (az_result_failed(az_iot_hub_client_telemetry_get_publish_topic(
          &client, NULL, telemetry_topic, sizeof(telemetry_topic), NULL)))
  {
    Serial.println("Failed az_iot_hub_client_telemetry_get_publish_topic");
    return;
  }

  mqtt_client.publish(telemetry_topic, getTelemetryPayload(), false);
  Serial.println("OK");
  delay(100);
}

// arduino setup and loop main functions.

void setup()
{
  Serial.begin(9600);
  // delay ensures serial connection before anything else
  delay(100);
  Serial.print("i'm connected");

  pinMode(RECPIN1, INPUT);
  pinMode(RECPIN2, INPUT);

  establishConnection();
}

void loop()
{
  // read ir sensors and save ir receiever status (HIGH or LOW; noSIGNAL or SIGNAL)
  status1 = digitalRead(RECPIN1);
  status2 = digitalRead(RECPIN2);
  
  // check beam 1
  if (status1 == noSIGNAL && status1 != lastStatus1) { // if nosignal & change in inputStatus, beam has been broken
      Serial.println("BEAM 1 BROKEN");
      // capture time of break for comparison; .now() gets time as an integer
      breakTime1 = time(NULL);
      Serial.println(breakTime1);
      Serial.println();
      // check if second beam has been broken recently
      if (breakTime1 > (breakTime2 + 1)) {
        // if not, occupancy inflow
        occupancy += 1;
      }
      else { // else, occupancy outlfow
        occupancy -= 1;
      }
      sendTelemetry();
    }
  
  else if (status2 == noSIGNAL && status2 != lastStatus2) { // check beam 2
      Serial.println("BEAM 2 BROKEN");
      // capture time for break comparison
      breakTime2 = time(NULL);
      Serial.println(breakTime2);
      Serial.println();
    } 
    

  // update lastStatus1 and 2
  lastStatus1 = status1;
  lastStatus2 = status2;

  // MQTT loop must be called to process Device-to-Cloud and Cloud-to-Device.
  mqtt_client.loop();
  delay(100);
}
