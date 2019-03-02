/* General configuration
   ---------------------
   INITIAL_DELAY = delay after reset to initialize serial port in ms
   PERFORM_FACTORY_RESET = define to perform BT module factory reset on restart
*/
#define INITIAL_DELAY                       500 // ms
//#define PERFORM_FACTORY_RESET

/* Interval shooting feature configuration
   ---------------------------------------
   USE_INTERVAL = define to enable interval shooting
   INTERVAL_DOT = show "." on serial every X ms
   INTERVAL_SHUTTER = press shutter every X ms
*/
//#define USE_INTERVAL
#define INTERVAL_SHUTTER                    30000 // ms

/* Trigger pin feature configuration
   ---------------------------------
   USE_TRIGGER = define to enable triggered shooting
   TRIGGER_ON_LH = true to trigger on switching from low to high
   TRIGGER_ON_HL = true to trigger on switching from high to low
   TRIGGER_PIN = GPIO pin used for trigger
*/
#define USE_TRIGGER
#define TRIGGER_PIN                         5
#define TRIGGER_ON_LH                       true
#define TRIGGER_ON_HL                       false

/* Bluefruit BLE module configuration
   ----------------------------------
   If you use the recommended board Adafruit Feather 32u4 Bluefruit LE,
   (product #2829), then do not modify anything below -- these settings 
   are required for this board.
*/
#define BLUEFRUIT_MINIMUM_FIRMWARE_VERSION  "0.6.6"
#define BLUEFRUIT_SPI_CS                    8
#define BLUEFRUIT_SPI_IRQ                   7
#define BLUEFRUIT_SPI_RST                   4
#define BLUEFRUIT_VERBOSE_MODE              false
