// tdos - prototype.ino - serial I/O cmdline communication and reading peripherals
//
// derived from https://linoxide.com/hrdwre/develop-control-arduino-systems-linux/
//
// This is the all-inclusive proof-of-concept for several controls read by Arduino.
//
// It combines
//
//  - serial io to provide a cmdline interface
//  - blinking the arduino built-in led
//  - setting arduino digital pin
//  - reading arduino analog pins
//  - printing to an OLED display using the (somewhat old) U8glib
//  - communication with an MCP3008 A/D converter
//
// Besides that the controls from tapedeck will be electrically
// disconnected via 4N35 optocouplers, not visible in programming
// here but in the schematics.
//
// Connect to it with a serial console program, like minicom or cu.
// After connecting it should give a prompt like
//
//   Tape Deck Control.
//   $ 
//
// Enter "help" and press ENTER to get a menu of possible commands.
//


// === init ==================================================================

// for OLED display SSD1306
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

// for MCP3008 A/D converter CPI communication
#include <SPI.h>
const uint8_t CS = 10;     // Chip Select

String  inputString      = "";
String  lastcmd          = "help";
boolean stringComplete   = false;
int     pin_sensor       = A0;
int     pin_signal       = 2;
int     led_status       = 13;
int     led_status_state = LOW;
int     resetPin         = 8; // wiring from this pin to reset pin disables auto-reset
int     pin_trigger      = 7;
int     blink_time       = 50;
int     mcp_pin_m0       = 0;

// === utils ==================================================================

// Check if inputString matches given cmd string
bool cmd(String cmd) {
  //Serial.println("\rdebug: " + inputString);
  if (inputString.startsWith(cmd)) {
    return true; 
  }
  else { 
    return false; 
  }
}

// blink the led duration
void blink(int led, int duration) { 
  digitalWrite(led, HIGH);  
  delay(duration);
  digitalWrite(led, LOW);   
  delay(duration);
}

void display_reset (void) {
  u8g.firstPage();
  do { } while (u8g.nextPage());
}

void display (int x, int y, String s) {
  u8g.firstPage();
  u8g.setFont(u8g_font_profont10);
  do {
    u8g.setPrintPos(x, y);
    u8g.print(s);
  } while (u8g.nextPage());
}

void display_globalstate () {
  u8g.firstPage();
  do {
    // YELLOW
    u8g.setFont(u8g_font_profont10);
    u8g.setPrintPos(0, 10); u8g.print("H1 ([0x]:67{890})");
    u8g.setFont(u8g_font_u8glib_4);
    u8g.setPrintPos(0, 16); u8g.print("::::+++++-----");

    // BLUE
    u8g.setFont(u8g_font_profont10);
    u8g.setPrintPos(0, 24); u8g.print("A0:" + String(analogRead(A0)));
    u8g.setPrintPos(0, 34); u8g.print("M0:" + String(mcp3008_read(mcp_pin_m0)));
    u8g.setPrintPos(0, 44); u8g.print(".");
    u8g.setPrintPos(0, 54); u8g.print(".");
    u8g.setPrintPos(0, 64); u8g.print(".");
  } while (u8g.nextPage());
}

// Triple blink
void blinky(void) {
  blink(led_status, blink_time);
  blink(led_status, blink_time);
  blink(led_status, blink_time);
}

// restart the board
void(* reboot) (void) = 0;

// Read from serial port
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (int(inChar) == 13) { inChar = '\n'; }
    else { inputString += inChar; }

    Serial.print(inChar);
    if (inChar == '\n') { stringComplete = true; }
  } // while
}

void print_help () {
  Serial.println("\r<CR> ... repeat last command");
  Serial.println("\rhelp ... print command overview");
  Serial.println("\rblink ... blink led");
  Serial.println("\ruptime ... print uptime");
  Serial.println("\rreboot ... reboot device");
  Serial.println("\rled_on ... set led on");
  Serial.println("\rled_off ... set led off");
  Serial.println("\rd_reset ... clear display");
  Serial.println("\rd_hello ... display hello");
  Serial.println("\rt_on ... trigger pin on");
  Serial.println("\rt_off ... trigger pin off");
  Serial.println("\rmcp_read ... read MCP3008 A/D converter");
  Serial.println("\rS ... display global state");
  Serial.println("\rread_sensor ... read analog sensor");
}

uint16_t mcp3008_read(uint8_t channel) {
  digitalWrite(CS, LOW); // start SPI communication

  // actual communication
  SPI.transfer(0x01);                                  // 1 = measure absolute (not relative)
  uint8_t msb = SPI.transfer(0x80 + (channel << 4));   // read high byte
  uint8_t lsb = SPI.transfer(0x00);                    // read low  byte (send 0 as we only want to read)

  digitalWrite(CS, HIGH); // end SPI communication

  return ((msb & 0x03) << 8) + lsb;
}

// Perform actions based on the input
void check_input(String input) {

  // empty line repeats last command
  if (inputString.equals("\n") || inputString.equals("")) {
    inputString = lastcmd;
    Serial.println(inputString);
  }

  // Turn led on
  if (cmd("led_on")) {
    led_status_state = HIGH;
    Serial.println("\rLED was turned on");
  }

  // Turn led off
  else if (cmd("led_off")) {
    led_status_state = LOW;
    Serial.println("\rLED was turned off");
  }

  // Read sensor pin
  else if (cmd("read_sensor")) {
    Serial.println("\rSensor value was:" + String(analogRead(pin_sensor)));
  }

  // clear display on oled
  else if (cmd("d_reset")) {
    display(0, 0, "");
    Serial.print("\r");
  }

  // display hello message on oled
  else if (cmd("d_hello")) {
    display(0, 0, "");
    display(0, 11, "hello");
    display(0, 21, "world");
    display(0, 31, "dear");
    Serial.print("\r");
  }

  // trigger pin on
  else if (cmd("t_on")) {
    digitalWrite(pin_trigger, HIGH);
    Serial.print("\r");
  }

  // trigger pin on
  else if (cmd("t_off")) {
    digitalWrite(pin_trigger, LOW);
    Serial.print("\r");
  }

  // display hello message on oled
  else if (cmd("S")) {
    display_globalstate();
    Serial.print("\r");
  }

  // Read sensor pin
  else if (cmd("help") || cmd("?")) {
    print_help();
  }

  // blink function
  else if (cmd("blink")) {
    blinky();
    Serial.print("\r");
  }

  // reboot
  else if (cmd("reboot")) {
    Serial.println("\rRebooting...");
    delay(200);
    reboot();
  }

  // Show time since last boot in milliseconds
  else if (cmd("uptime")) {
    unsigned long blink_time = millis();
    Serial.println("\rUptime: " + String(blink_time));
  }

  // read MCP3008 A/D converter
  else if (cmd("mcp_read")) {
    uint16_t reading = mcp3008_read(mcp_pin_m0);
    Serial.println("\rMCP ch 0: " + String(reading));
  }

  // Input string does not have any registered command
  else {
    Serial.println("\rerror: command not found: (" + inputString + ")");
  }

  // Add the command to "history"
  lastcmd = inputString;

  //prepare for next command
  inputString = "";
  stringComplete = false;
  Serial.print("$ ");
}

// === main ===================================================================

// Initialize
void setup() {
  // Set serial port speed
  Serial.begin(9600);
  inputString.reserve(255);
  pinMode(led_status, OUTPUT);
  pinMode(pin_trigger, OUTPUT);

  // wiring from resetPin (pin 8) to the reset pin will disable auto-restart
  digitalWrite(resetPin, HIGH);

  // trigger pin default off
  digitalWrite(pin_trigger, LOW);

  // blink 3 times to show we are starting
  blink(led_status, 50);

  // print startup message and prompt
  Serial.println("\rTape Deck Control.");
  Serial.print("\r$ ");

  // OLED clear screen
  display_reset();

  // MCP3008 SPI init
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);   // default HIGH, so LOW signals communication start
}

// main loop function
void loop() {
  serialEvent();
  if (stringComplete) {
    inputString.trim();
    // Call our input handler function
    blink(led_status, 10);
    check_input(inputString);
  }
  digitalWrite(led_status, led_status_state);
}

