#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define LED_PIN 6
#define LED_COUNT 20
#define ARR_SIZE 64

#define HUE_MAX 65536L

#define BUFFER_SIZE 64

// Message flags
// Steering
#define STOP '!'        
#define END '.'         
#define START '?'

// Variables
#define RED 'R'
#define GREEN 'G'
#define BLUE 'B'
#define BRIGHTNESS 'b'
#define WAIT 'w' // The higher, the slower animation is playing

// Function flag values
#define FUNCTION 'f'

class LedStrip;
struct ProgramVariables;

LedStrip* ledStrip;
ProgramVariables* variables;

char receivedData[BUFFER_SIZE];
int dataIndex = 0;
bool isDataReceiving = false;


enum LedStripFunction {
  CLEAR = 1, 
  FILL, 
  RAINBOW_LEFT, 
  RAINBOW_RIGHT, 
  PULSE, 
  PULSE_RAINBOW
};


struct RGB {
  byte red = 0;
  byte green = 0;
  byte blue = 0;

  void setColor(const byte& r, const byte& g, const byte& b){
    red = r;
    green = g;
    blue = b;
  }

  void black(){
    red = 0;
    green = 0;
    blue = 0;
  }
  
  RGB(unsigned char r, unsigned char g, unsigned char b){
    setColor(r, g, b);
  }
  
  RGB() {
    black();
  }
};


class LedStrip {
  Adafruit_NeoPixel strip;
  static LedStrip* ledStrip;

  LedStrip() {
    strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GBR + NEO_KHZ800);
  }

public:
  LedStrip(LedStrip &other) = delete;
  void operator=(const LedStrip&) = delete;

  static LedStrip* getInstance() {
    if(ledStrip == nullptr){
        ledStrip = new LedStrip();
    }
    return ledStrip;
  }

  void init() {
    strip.begin();
    strip.show();
    strip.setBrightness(255);
  }

  void clear() {
    strip.clear();
    strip.show();
  }

  void fillColor(const RGB& rgb, const byte& brightness) {
    strip.setBrightness(brightness);
    strip.fill(strip.Color(rgb.green, rgb.red, rgb.blue));
    strip.show();
  }

  void rainbow(const byte& brightness, bool rightDirection) {
    static int firstPixelHue = 0;
    displayRainbowStrip(firstPixelHue, brightness);

    if(rightDirection)
      firstPixelHue += HUE_MAX / 90;
    else 
      firstPixelHue -= HUE_MAX / 90;
  }

  void pulse(const RGB& rgb) {
    inOutBrightness(strip.Color(rgb.red, rgb.green, rgb.blue));
  }

  void pulseRainbow() {
    static int hue = 0;
    uint32_t color = strip.gamma32(strip.ColorHSV(hue));
    
    // change color when the animation starts over
    if(inOutBrightness(color))
      hue += HUE_MAX/10;
  }

private:
  void displayRainbowStrip(const int& firstPixelHue, const byte& brightness) {
    strip.setBrightness(brightness);
    strip.clear();

    for(int c=0; c<strip.numPixels(); c++) {
      int hue = firstPixelHue + c * HUE_MAX / strip.numPixels();
      uint32_t color = strip.gamma32(strip.ColorHSV(hue));
      strip.setPixelColor(c, color); 
    }

    strip.show();
  }

  /**
  * Starts with brightness==0, goes to 255, then goes back to 0
  * @returns true if the animation completes
  */
  bool inOutBrightness(const uint32_t& color) {
    const static byte INCREMENT = 5;
    static bool ascending = true;
    static byte currentBrightness = 0;

    strip.setBrightness(currentBrightness);
    strip.fill(color);
    strip.show();

    return adjustBrightness(currentBrightness, ascending, INCREMENT);
  }

  /**
  * @returns true if the animation completes
  */
  bool adjustBrightness(byte& currentBrightness, bool& ascending, const byte& increment) {
    if(ascending) {
      if(currentBrightness + increment > 255) {
        ascending = false;
        currentBrightness = 255;
      }
      else
        currentBrightness += increment;
    }
    else {
      if(currentBrightness < increment) {
        ascending = true;
        currentBrightness = 0;
        return true;
      }
      else
        currentBrightness -= increment;
    }
    return false;
  }
};
LedStrip* LedStrip::ledStrip = nullptr;

struct ProgramVariables {
  RGB rgb;
  byte brightness;
  unsigned int wait;
  LedStripFunction ledFunction;
};

void testColorSwitch() {
  int DELAY = 500;
  Serial.println("RED");
  ledStrip->fillColor({255,0,0}, 255);
  delay(DELAY);

  Serial.println("GREEN");
  ledStrip->fillColor({0,255,0}, 255);
  delay(DELAY);

  Serial.println("BLUE");
  ledStrip->fillColor({0,0,255}, 255);
  delay(DELAY);
  
  Serial.println("WHITE");
  ledStrip->fillColor({255,255,255}, 255);
  delay(DELAY);
  
  ledStrip->clear();
}

void testPulse() {
  int wait = 300;

  Serial.println("Fade in out RED");
  for(int i=0; i<102; i++) {
    delay(wait);
    ledStrip->pulse({255, 0, 0});
  }

  Serial.println("Fade in out GREEN");
  for(int i=0; i<102; i++) {
    delay(wait);
    ledStrip->pulse({0, 255, 0});
  }

  Serial.println("Fade in out BLUE");
  for(int i=0; i<102; i++) {
    delay(wait);
    ledStrip->pulse({0, 0, 255});
  }
  
  for(int i=0; i<1000; i++) {
    delay(wait);
    ledStrip->pulseRainbow();
  }
}

void testRainbow() {
  Serial.println("Rainbow left");
  for(int i=0; i<50; i++) {
    delay(30);
    ledStrip->rainbow(255, false);
  }

  Serial.println("Rainbow right");
  for(int i=0; i<50; i++) {
    delay(30);
    ledStrip->rainbow(255, true);
  }
}

void runLedStripTest() {
  Serial.println("LED STRIP TEST");
  testPulse();
  // testColorSwitch();
  testRainbow();
  Serial.println("END OF TEST");
}

void receiveBluetoothData() {
  // TODO: timeout if STOP not received (lost or invalid data)
  while (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == START) {
      // Start flag received
      isDataReceiving = true;
      dataIndex = 0;
    } else if (receivedChar == STOP && isDataReceiving) {
      // End of the string
      isDataReceiving = false;
      receivedData[dataIndex] = '\0'; // Null-terminate the string
      Serial.print("Received: ");
      Serial.println(receivedData);
    } else if (isDataReceiving) {
      // Add the character to the string buffer
      if (dataIndex < BUFFER_SIZE - 1) {
        receivedData[dataIndex] = receivedChar;
        dataIndex++;
      }
    }
  }
}

/**
* Checks if an array is structured like 'char''num'num'num''.''char''num'num'num'...'
* @returns true if the elemnts in array are correct
*/
bool validateCommand(char* arr, const unsigned int& size) {
  for(int i=0; i<size-1; i++) {
    if(i%5 == 0 && isdigit(arr[i])) // letter check
      return false;
    else if(i%5 > 0 && i%5 < 4 && !isdigit(arr[i])) // 3 digits in a row
      return false;
    else if(i%5 == 4 && arr[i] != '.') // dot between commands
      return false;
  }
  return arr[size-1] == '\0';
}


unsigned int parseInt(char* arr, const unsigned int& begin, const unsigned int& end) {
  unsigned int result = 0;

  for (unsigned int i = begin; i < end; ++i) {
    if (isdigit(arr[i])) {
      result = result * 10 + (arr[i] - '0');
    } else {
      Serial.println("Error parsing command. Command invalid even though was validated");
      break;
    }
  }
  return result;
}

void printArray(char* arr, const unsigned int& size) {
  for (unsigned int i = 0; i < size; ++i) {
    Serial.print(arr[i]);
    if (i < size - 1) {
      Serial.print(" ");
    }
  }
  Serial.println();
}

void parseBufferedData(char* arr, const unsigned int& size) {
  if(!validateCommand(arr, size)) {
    Serial.print("INVALID DATA ");
    printArray(arr, size);
    return;
  }


  // TODO: implement parsing
}


void executeDelayed(const unsigned int delay, void (*fun)()) {
  static unsigned long lastExecution = millis();
  unsigned long now = millis();

  if(now - delay > lastExecution) {
    lastExecution = now;
    fun();
  }
}

// function wrappers
void pulseWrapped() {
  ledStrip->pulse(variables->rgb);
}

void pulseRainbowWrapped() {
  ledStrip->pulseRainbow();
}

void rainbowRightWrapped() {
  ledStrip->rainbow(variables->brightness, true);
}

void rainbowLeftWrapped() {
  ledStrip->rainbow(variables->brightness, false);
}

void runLedStrip() {
  switch(variables->ledFunction) {
    case CLEAR:
      ledStrip->clear();
      break;
    
    case FILL:
      ledStrip->fillColor(variables->rgb, variables->brightness);
      break;

    case PULSE:
      executeDelayed(variables->wait, pulseWrapped);
      break;

    case PULSE_RAINBOW:
      executeDelayed(variables->wait, pulseRainbowWrapped);
      break;

    case RAINBOW_RIGHT:
      executeDelayed(variables->wait, rainbowRightWrapped);
      break;

    case RAINBOW_LEFT:
      executeDelayed(variables->wait, rainbowLeftWrapped);
      break;
  }
}

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  Serial.begin(38400);

  ledStrip = LedStrip::getInstance();
  ledStrip->init();
  // runLedStripTest();

  *variables = {
    RGB(0, 255, 0),   // Default green
    255,              // Default brightness MAX
    10,               // Delay 10  
    FILL              // Fill strip with color
  };
}

// TODO: log messages
void loop() {
  receiveBluetoothData();

  // Parse data if not currently gathering data and there is data available to parse
  if(!isDataReceiving && dataIndex > 0)
    parseBufferedData(receivedData, BUFFER_SIZE);

  runLedStrip();
}
