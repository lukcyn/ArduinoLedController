#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define LED_PIN 6
#define LED_COUNT 20
#define ARR_SIZE 64

#define HUE_MAX 65536L

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
#define WAIT 'W' // The higher, the slower animation is playing

// Function flag values
#define FUNCTION 'F'

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
    strip.fill(strip.Color(rgb.red, rgb.green, rgb.blue));
    strip.show();
  }

  void rainbow(const int& wait, const byte& brightness, bool rightDirection) {
    static int firstPixelHue = 0;
    displayRainbowStrip(firstPixelHue, brightness);
    delay(wait);

    if(rightDirection)
      firstPixelHue += HUE_MAX / 90;
    else 
      firstPixelHue -= HUE_MAX / 90;
  }

  void pulse(const unsigned int& wait, const RGB& rgb) {
    clear();
    inOutBrightness(wait, strip.Color(rgb.red, rgb.green, rgb.blue));
    clear();
  }

  void pulseRainbow(const unsigned int& wait) {
    static int hue = 0;
    hue += HUE_MAX/10;
    uint32_t color = strip.gamma32(strip.ColorHSV(hue));
    clear();
    inOutBrightness(wait, color);
    clear();
  }

private:
  void displayRainbowStrip(const int& firstPixelHue, const byte& brightness) {
    strip.setBrightness(brightness);
    for(int a=0; a<30; a++) {
        strip.clear();
        for(int c=0; c<strip.numPixels(); c++) {
          int hue = firstPixelHue + c * HUE_MAX / strip.numPixels();
          uint32_t color = strip.gamma32(strip.ColorHSV(hue));
          strip.setPixelColor(c, color); 
        }
        strip.show();
    }
  }

  /**
  * Starts with brightness==0, goes to 255, then goes back to 0
  */
  void inOutBrightness(int unsigned wait, const uint32_t& color) {
    const static byte INCREMENT = 5;

    // Prevent flashing from bad input data
    if(wait > 10)
      wait = wait/10;
    else
      wait = 1;

    Serial.println(wait);
    clear();

    for(unsigned int i=0; i<=255; i+=INCREMENT) {
      strip.setBrightness(i);
      strip.fill(color);
      strip.show();
      delay(wait);
    }

    for(unsigned int i=255; i-INCREMENT > 0; i-=INCREMENT) {
      strip.setBrightness(i);
      strip.fill(color);
      strip.show();
      delay(wait);
    }
  }
};
LedStrip* LedStrip::ledStrip = nullptr;


// uint8_t fFlag=0;
// char dataRecieved[ARR_SIZE];
// bool isDecoded = true;
LedStrip* ledStrip = LedStrip::getInstance();

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  Serial.begin(38400);
  ledStrip->init();
}

RGB pallete = { 0, 0, 0 };

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

void testFadeInOut() {
  int wait = 300;
  Serial.println("Fade in out RED");
  ledStrip->pulse(wait, {255, 0, 0});

  Serial.println("Fade in out GREEN");
  ledStrip->pulse(wait, {0, 255, 0});

  Serial.println("Fade in out BLUE");
  ledStrip->pulse(wait, {0, 0, 255});
}

void ledStripTest() {
  Serial.println("LED STRIP TEST");
  // testFadeInOut();
  // testColorSwitch();

  // Serial.println("Rainbow left");
  // for(int i=0; i<50; i++)
  //   ledStrip->rainbow(30, 255, false);

  // Serial.println("Rainbow right");
  // for(int i=0; i<50; i++)
  //   ledStrip->rainbow(30, 255, true);

  ledStrip->pulseRainbow(30);
}

void loop() {
  ledStripTest();
  //recieving the data from HC-05 (which function will be used?)
  // recieveData();  


  // //change it to while(!Serial.available()>0)
  // if(Serial.peek()!=START){
  //     switch(fFlag){
        
  //     case CLEAR:
  //     strip.clear();
  //     Serial.println("Cleaning");
  //     fFlag = 0;
  //     break;
    
  //     case FILL:
  //     strip.fill(strip.Color(pallete.getGreen(), pallete.getBlue(), pallete.getRed()));   //TODO: color chooser, currently function turns strip red
  //     strip.show();
  //     fFlag = 0;
  //     break; 
  //   }
  // }
}

// void recieveData(){
//     char pos = 0;
//     //TODO: Serial cleaning mechanisim in case START does not occur
    
//     if(Serial.peek()==START){
//       Serial.println("START");
//       do{
//             if(Serial.available()>0){
//                 dataRecieved[pos] = Serial.read();
//                 //dataRecieved[0]=46;
//                 Serial.println(dataRecieved[pos]);
//                 pos++;
//               }            

//       }while(dataRecieved[pos-1]!=STOP);
      
//       //This step proves that data has beed recovered fully
//       isDecoded = false;
//     }
//     //in case of error
//     if(Serial.peek() != START && Serial.available()>0){
//       flushBuffor();
//     }
   
//   if(!isDecoded){
//       decoder(dataRecieved, fFlag, pallete);
//       isDecoded=true;
//       for(short i=0; i<64; i++){
//       dataRecieved[i]=0;

//       //This step allows the function to be executed after succesfull decoding
//       //dataAvailable = true;
//       }
//   }
// }

// //Flushing the buffor in case of incomplete data
// void flushBuffor(){
//   while(Serial.available()>0){
//     Serial.read();
//   }
//   Serial.println("Flushing buffor...");
// }

// //Handles the values connected with flags
// short charToInt(char arr[], short &index){
//   short solution; 
//   short temp[]={0,0,0};
//   short i=0;
  
//   while(arr[index]!=END && i<=2 && arr[index]!=STOP){
//     temp[i] = arr[index]-'0';
//     i++;
//     index++;
//   }
//   solution = temp[0]*100+temp[1]*10+temp[2];
//   return solution; 
// }

// //Handles the flags and values in code send by bluetooth
// void decoder(char arr[], uint8_t &fFlag, Pallete &pallete){
//   short i=0;
//   while(arr[i]!=STOP){
//     switch (arr[i]){

//       case START:
//       i++;
//       Serial.println("START DETECTED");
//       break;

//       case END:
//       i++;
//       Serial.println("END DETECTED");
//       break;
      
//       case FUNCTION:
//       i++;
//       Serial.println("FUNCTION FLAG DETECTED");
//       fFlag = charToInt(arr,i);
//       break;
      
//       case RED:
//       i++;
//       Serial.println("RED FLAG DETECTED");
//       pallete.setRed(charToInt(arr,i));
//       break;
      
//       case GREEN:
//       i++;
//       Serial.println("GREEN FLAG DETECTED");

//       pallete.setGreen(charToInt(arr,i));
//       break;
      
//       case BLUE:
//       i++;
//       Serial.println("BLUE FLAG DETECTED");
//       pallete.setBlue(charToInt(arr,i));
//       break;

//       case BRIGHTNESS:
//       i++;
//       strip.setBrightness(charToInt(arr, i));
//       Serial.println("BRIGHTNESS FLAG DETECTED");
//     }
//   }
//   Serial.println("Data stopped flowing succesfully");
// }