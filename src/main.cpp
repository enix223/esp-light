#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_client.h>
#include <json_parser.h>
#include <FastLED.h>

#define WIFI_SSID CONFIG_NET_SSID
#define WIFI_PASSWORD CONFIG_NET_PASSWORD
#define DEVICE_ID CONFIG_DEVICE_ID

#define MQTT_BROKER_URI CONFIG_BROKER_URL
#define MQTT_SUBSCRIBE_TOPIC "eiot/" DEVICE_ID "/properties/downstream"
#define MQTT_PUBLISH_TOPIC "eiot/" DEVICE_ID "/properties/upstream"
#define MQTT_CLIENT_ID "eio-" DEVICE_ID

#define LED_GPIO CONFIG_RGB_LED_PIN
#define LED_NUM_LEDS 64
#define LED_TYPE WS2812B
#define LED_COLOR_ORDER GRB
#define LED_BRIGHTNESS 128
static CRGB leds[LED_NUM_LEDS];

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t mqtt_client;

// JSON format: {"command":"SET_PROPERTIES","payload":{"r":96,"b":255,"g":0}}
static void OnReceiveData(esp_mqtt_event_handle_t event)
{
  int ret;
  if (event->data_len == 0)
  {
    Serial.println("topic not match or payload is empty");
    return;
  }

  jparse_ctx_t ctx;
  ret = json_parse_start(&ctx, event->data, event->data_len);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to parse json command");
    return;
  }

  ret = json_obj_get_object(&ctx, "payload");
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get payload");
    return;
  }

  int i, a, r, g, b;
  ret = json_obj_get_int(&ctx, "i", &i);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get i");
    return;
  }
  ret = json_obj_get_int(&ctx, "r", &r);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get r");
    return;
  }
  ret = json_obj_get_int(&ctx, "g", &g);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get g");
    return;
  }
  ret = json_obj_get_int(&ctx, "b", &b);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get b");
    return;
  }
  ret = json_obj_get_int(&ctx, "a", &a);
  if (ret != OS_SUCCESS)
  {
    Serial.println("failed to get a");
    return;
  }
  Serial.printf("set led[%d] to r = %d, g = %d, b = %d\n", i, r, g, b);
  leds[i] = CRGB(r & 0xff, g & 0xff, b & 0xff);
  leds[i].nscale8_video(a & 0xff);
  FastLED.show();
}

// MQTT event handler function
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
  esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

  switch ((esp_mqtt_event_id_t)event_id)
  {
  case MQTT_EVENT_CONNECTED:
    Serial.println("MQTT_EVENT_CONNECTED");
    // Once connected, subscribe to a topic
    esp_mqtt_client_subscribe(mqtt_client, MQTT_SUBSCRIBE_TOPIC, 0);
    Serial.printf("subscribe topic %s\n", MQTT_SUBSCRIBE_TOPIC);
    // And publish a hello message
    // esp_mqtt_client_publish(mqtt_client, "esp32/topic", "Hello from ESP32!", 0, 1, 0);
    break;
  case MQTT_EVENT_DATA:
    Serial.println("MQTT_EVENT_DATA");
    // Print received topic and message
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);
    OnReceiveData(event);
    break;
  case MQTT_EVENT_DISCONNECTED:
    Serial.println("MQTT_EVENT_DISCONNECTED");
    break;
  case MQTT_EVENT_ERROR:
    Serial.println("MQTT_EVENT_ERROR");
    break;
  default:
    break;
  }
}

// Function to initialize and start the MQTT client
void mqtt_app_start(void)
{
  // Configure the MQTT client with the broker's URI
  esp_mqtt_client_config_t mqtt_cfg = {};
  mqtt_cfg.broker.address.uri = MQTT_BROKER_URI;
  mqtt_cfg.credentials.client_id = MQTT_CLIENT_ID;

  // Initialize the client
  mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
  // Register the event handler
  esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
  // Start the MQTT client
  esp_mqtt_client_start(mqtt_client);
}

void setup()
{
  Serial.begin(115200);
  delay(3000);

  FastLED.addLeds<LED_TYPE, LED_GPIO, LED_COLOR_ORDER>(leds, LED_NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  // Connect to Wi-Fi (you need to implement this part)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
  }

  // Start the MQTT application
  mqtt_app_start();
}

void loop()
{
  delay(1000);
}
