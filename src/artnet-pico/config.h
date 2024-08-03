#define WIFI_SSID "yourssid"
#define WIFI_PASSWORD "yourpassword"

// Which universe maps to output A
#define OUTPUT_A_UNIVERSE 1
// the GPIO pin used for output A
#define OUTPUT_A_GPIO 0

// Number of output ports, this sketch only supports one but it's extensible
#define NUM_OUTPUTS 1
// Universe length (you can decrease this if you don't use the full 512 channels)
#define UNIVERSE_LENGTH 512

// set to true to see packets on serial console (only needed for debugging)
#define DEBUG_PACKETS_SERIAL true
