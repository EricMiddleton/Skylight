#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <RemoteDebug.h>
#include <EspMQTTClient.h>
#include "driver/ledc.h"

#include "ColorLUT.hpp"

const int W1 = 18;
const int Y1 = 5;
const int W2 = 19;
const int Y2 = 21;

const int output_pins[] = {Y1, Y2, W1, W2};

//Actual PWM frequency will be slightly under 20khz for 12bit resolution
const int PWM_FREQ = 20000;
const int PWM_BIT_DEPTH = 12;
const int PWM_FADE_TIME = 500; //in ms

//Control packets will have 16 bits resolution
const int CONTROL_BIT_DEPTH = 16;

constexpr float LED_MAX_LEVEL = (1 << PWM_BIT_DEPTH) - 1;

EspMQTTClient client(
  "414net",
  "3ricn3t1",
  "192.168.0.138",  // MQTT Broker server ip
  "lightpanel"      // Client name that uniquely identify your device
);

static constexpr char* CCT_TOPIC_NAME = "daylight/actuator/temp";
static constexpr char* BRIGHTNESS_TOPIC_NAME = "daylight/actuator/brightness";

static constexpr char* HOST_NAME = "daylight";
const char* ssid = "414net";
const char* pwd = "3ricn3t1";
const uint16_t port = 12345;

WiFiUDP udp;

RemoteDebug Debug;

struct LightState
{
  float cct;
  float brightness;
};

static LightState lightState = 
  {
    9000.0f,
    0.0f
  };

static constexpr uint32_t CONFIG_ID = 0xAABBCCDD;

void setup() {
  Serial.begin(115200);

  client.enableDebuggingMessages();
  client.enableHTTPWebUpdater("eric", "3ricn3t1");

  Serial.println("Configuring PWM Output");
  for(auto i = 0; i < 4; ++i)
  {
    ledcSetup(i, PWM_FREQ, PWM_BIT_DEPTH);
    ledcWrite(i, 0);
    ledcAttachPin(output_pins[i], i);
  }

  ledc_fade_func_install(0);

/*
  Serial.print("Starting WiFi");

  WiFi.begin(ssid, pwd);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print('.');
  };

  Serial.println("Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  */
/*
  String hostNameWifi = HOST_NAME;
  hostNameWifi.concat(".local");
  WiFi.hostname(hostNameWifi);

  MDNS.begin(HOST_NAME);
  MDNS.addService("telnet", "tcp", 23); //Debug logging
*/

  //udp.begin(port);

  Serial.println("Setup complete\n");
}

void onConnectionEstablished() {
  Serial.println("MQTT Connection Established");
  client.subscribe(CCT_TOPIC_NAME, onCCTUpdate);
  client.subscribe(BRIGHTNESS_TOPIC_NAME, onBrightnessUpdate);
}

void onCCTUpdate(const String& payload)
{
  lightState.cct = payload.toFloat();
  UpdateLights();
}

void onBrightnessUpdate(const String& payload)
{
  lightState.brightness = payload.toFloat();
  UpdateLights();
}

void loop() {
  client.loop();
/*
  auto len = udp.parsePacket();
  
  if(len == 4)
  {
    unsigned char packet[4];
    udp.read(packet, 4);
    
    Serial.printf("Received packet: %d, %d, %d, %d\r\n",
      packet[0], packet[1], packet[2], packet[3]);
    debugD("Received packet: %d, %d, %d, %d\r\n",
      packet[0], packet[1], packet[2], packet[3]);

    uint16_t warm, cool;
    uint16_t shift = CONTROL_BIT_DEPTH - PWM_BIT_DEPTH;

    warm = ((packet[0] << 8) | packet[1]) >> shift;
    cool = ((packet[2] << 8) | packet[3]) >> shift;

    Serial.printf("LED Values: warm=%d, cool=%d\r\n",
      warm, cool);
    //debugI("LED Update: warm=%d, cool=%d\r\n",
      //warm, cool);

    //Warm white
    //ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, warm, PWM_FADE_TIME);
    //ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, warm, PWM_FADE_TIME);
    ledcWrite(0, warm);
    ledcWrite(1, warm);
    //Cool white
    //ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, cool, PWM_FADE_TIME);
    //ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, cool, PWM_FADE_TIME);
    ledcWrite(2, cool);
    ledcWrite(3, cool);

    //ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
    //ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
    //ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
    //ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, LEDC_FADE_NO_WAIT);
  }
  else if(len > 0)
  {
    Serial.printf("Error: Received packet of length %d\r\n", len);
    debugE("Error: Received packet of length %d\r\n", len);
  }

  //Debug.handle();
  */
}

void UpdateLights()
{
  auto ledLevels = ColorLUT::CalculateLedLevel(lightState.cct, lightState.brightness, LED_MAX_LEVEL);
  
  ledcWrite(0, ledLevels.warm);
  ledcWrite(1, ledLevels.warm);

  ledcWrite(2, ledLevels.cool);
  ledcWrite(3, ledLevels.cool);

  Serial.printf("Light update. CCT=%f, Brightness=%f, Warm Level=%d, Cool Level=%d\n",
    lightState.cct, lightState.brightness, ledLevels.warm, ledLevels.cool);
  
  /*
  //Warm white
  ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, config.warm, PWM_FADE_TIME);
  ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, config.warm, PWM_FADE_TIME);

  //Cool white
  ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, config.cool, PWM_FADE_TIME);
  ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_3, config.cool, PWM_FADE_TIME);
  */
}
