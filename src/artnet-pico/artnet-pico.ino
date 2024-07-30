/*
  Art-Net DMX gateway
  For Raspberry Pi Pico W
  This node has one DMX output, supports one universe (configurable which universe).
  The device as an OLED screen (128x64) to display status. Connecting the display is optinal buit recommended.

  Dependencies:
    - ArtnetWifi by Nathanaël Lécaudé, Stephan Ruloff (Tested with 1.6.1)
    - Pico-DMX by Jostein (Tested with 3.1.0)
    - Adafruit SSD1306 (Tested with 2.5.10)
    - Adafruit GFX Library (Tested with 1.11.10)
    - Adafruit BusIO (Tested with 1.16.1)

  Connections:

  Raspberry Pico to RS-485 adapter
  ---------------------------------------------
  | GPIO0       | TX on RS-485 adapter module |
  | 3V3         | VCC on adapter module       |
  | GND (Pin 3) | GND on adapter module       |
  ---------------------------------------------

   Raspberry Pico to SSD1306 OLED 128x64
  -------------------------
  | GP4 (I2C0 SDA) | SDA  |
  | GP5 (I2C0 SCK) | SCL  |
  | 3V3            | VCC  |
  | GND (Pin 8)    | GND  |
  -------------------------
  GND is Pin 3, 8, 13, 18, 23, 28, 33, or 38
  3.3V is Pin 36, you have to connect and share this with both

  Code by Janos Noll 2024-07-27
  The reworked and remixed original project is by Taylor Mingos 10/18/2023
*/

#include <Arduino.h>
#include <ArtnetWifi.h>
#include <DmxOutput.h>
#include "oled.h"
#include "config.h"

DmxOutput dmx[NUM_OUTPUTS];
uint8_t channels[UNIVERSE_LENGTH + 1];
ArtnetWifi artnet;
OLED oled(OUTPUT_A_UNIVERSE, WIFI_SSID);
int lastMillisDmxPortSent[NUM_OUTPUTS];

// Connect to Wifi
bool connectToWifi(void)
{
  bool connected = true;
  int attempts = 0;

  // Connect to WiFi  
  oled.displayWifiConnecting();
  Serial.print("Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (attempts > 30){
      connected = false;
      break;
    }
    attempts++;
  }

  // Display WiFi connection result
  if (connected) {
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP: ");
    Serial.println(IPAddress(WiFi.localIP()));
    oled.displayWifiConnected(WiFi.localIP().toString().c_str());
  } else {
    Serial.println("WiFi Connection Failed");
    oled.displayWifiError();
    delay(3000);
  }
  
  return connected;
}

void printDmxFrameInfoToConsole(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data) {
  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Data Length: ");
  Serial.print(length, DEC);
  Serial.print(", Sequence: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data: ");

  for (int i = 0; i < length; i++)
  {
    if (i < 16) { // only print first 16 channels
      Serial.print(data[i], DEC);
      Serial.print(" ");
    }
  }
  Serial.println("...");
}

void setChannelsFromDmxFrameData(uint8_t* data, uint16_t length) {
    for (int i = 0; i < length; i++)
    {
      channels[i + 1] = data[i];
    }
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
#ifdef DEBUG_PACKETS_SERIAL
  // Note: we capture and print other universes on the network too, we just won't send them out
  printDmxFrameInfoToConsole(universe, length, sequence, data);
#endif
  // Send out the universe if it's a universe we care about
  if (universe == OUTPUT_A_UNIVERSE) {
    setChannelsFromDmxFrameData(data, length);
    dmx[0].write(channels, UNIVERSE_LENGTH);
    lastMillisDmxPortSent[0] = millis();
    while (dmx[0].busy())
    {
      // Do nothing while the DMX frame transmits
    }
  }
}

void initializeDmxOutputPorts() {
  dmx[0].begin(OUTPUT_A_GPIO);
  lastMillisDmxPortSent[0] = 0;
}

void turnOnPicoOnboardLed() {
  // Turn on Pico's on-board LED
  // This shows that the Pico is turned on
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setAllChannelsToZero() {
  for (int i = 1; i < UNIVERSE_LENGTH + 1; i++)
  {
    channels[i] = 0;
  }
}

// Some DMX software (QLC+) can only send a packet once a second over the network
// lights might turn off after a delay
// to make things easier, we will hold the last packet and keep sending it for a bit
void writeHoldPacketIfNeeded() {
  if (millis() - 100 > lastMillisDmxPortSent[0]) {
    lastMillisDmxPortSent[0] = millis();
    dmx[0].write(channels, UNIVERSE_LENGTH);
    while (dmx[0].busy())
    {
      // Do nothing while the DMX frame transmits
    }
  }
}
//-------------------------------------------
void setup()
{
  Serial.begin(115200);
  oled.initDisplay();
  turnOnPicoOnboardLed();
  initializeDmxOutputPorts();
  setAllChannelsToZero();
  // Connect to WiFi
  while(!connectToWifi()) { }
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}
//-------------------------------------------
void loop()
{
  artnet.read();
  writeHoldPacketIfNeeded();
}
