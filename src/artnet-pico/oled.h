#ifndef OLED_H
#define OLED_H
#include <Adafruit_SSD1306.h>

enum ConnectionState {
  connecting,
  connected,
  error
};

class OLED {
  private:
    Adafruit_SSD1306& displayRef;
    int universeA;
    const char* wifiNetwork;
    char _ipAddress[20];
    ConnectionState _connectionState = ConnectionState::connecting;
    void drawWifiLogo();
    void drawNetworkName();
    void drawIPAddress();
    void drawUniverseConfig();
    void drawScreen();

  public:
    OLED(int univA, const char* ssid);
    void initDisplay();
    void displayWifiConnecting();
    void displayWifiError();
    void displayWifiConnected(const char *ipAddress);
};

#endif // OLED_H
