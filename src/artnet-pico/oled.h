#ifndef OLED_H
#define OLED_H
#include <Adafruit_SSD1306.h>

class OLED {
  private:
    Adafruit_SSD1306& displayRef;
    int universeA;
    const char* wifiNetwork;
    void _drawWifiLogo(void);
    void _addNetworkName();
    void _displayUniverseConfig();
    void _displayCommonInfo();

  public:
    OLED(int univA, const char* ssid);
    void initDisplay();
    void displayWifiConnecting();
    void displayWifiError();
    void displayWifiConnected(const char *ipAddress);
};

#endif // OLED_H
