#include <Adafruit_NeoPixel.h>

#include <ArduinoJson.h>

#include <ESP8266WiFi.h>

const char* ssid     = "ocadu-embedded";
const char* password = "internetofthings";
//const char* ssid     = "acouchabedandsomepeople";
//const char* password = "ashowabouthomes42";
//const char* ssid     = "CFC-Staff-AP";
//const char* password = "sunshine";

#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>

// Kate's PubNub keys
const static char pubkey[] = "pub-c-53d4cc3b-3935-4e90-b0e9-c8f2e6017841";  //get this from your PUbNub account
const static char subkey[] = "sub-c-cb72ff40-ab27-11e9-8321-fed749823569";  //get this from your PubNub account

const static char pubChannel[] = "prototype"; //choose a name for the channel to publish messages to


unsigned long lastRefresh = 0;    //used to make the timer work
int publishRate = 50;           //how often to update the value on pubnub in milliseconds

int buttonPin1 = 14;
int buttonPin2 = 13;

int buttonState1 = 0;         // current state of the button
int buttonState2 = 0;

int lastButtonState1 = 0;     // previous state of the button
int lastButtonState2 = 0;     // previous state of the button


// How many NeoPixels are attached to the Arduino?

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(6, 15, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(6, 0, NEO_GRB + NEO_KHZ800);

byte neopix_gamma[] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
  2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
  10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
  17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
  25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
  37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
  51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
  69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
  90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
  115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
  144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
  177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
  215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
};



void setup() {
  delay(1000);
  Serial.begin(115200);

   WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

  // Serial.println("success!");
  // Serial.print("IP Address is: ");
  //Serial.println(WiFi.localIP());


  PubNub.begin(pubkey, subkey);                      //connect to the PubNub Servers
  Serial.println("PubNub Connected");

  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  strip.begin();
  strip2.begin();


  for (int i = 0; i < 6; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Moderately bright red color.
    strip.show(); // This sends the updated pixel color to the hardware.
    strip2.setPixelColor(i, strip2.Color(0, 0, 0)); // Moderately bright red color.
    strip2.show();
  }

}

void loop() {
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      Serial.println("button2 is on");

      for (int i = 0; i < 1; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255

        for (int j = 0; j < 256 ; j++) {
          for (uint16_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(j, j, j, neopix_gamma[j] ) );
          }
          delay(2);
          strip.show();
        }

        //        strip.setPixelColor(i, strip.Color(0, 255, 0)); // Moderately bright red color.
        //        strip.show(); // This sends the updated pixel color to the hardware.
        delay(50);// This sends the updated pixel color to the hardware.

      }
       publishToPubNub();                      //execute the function that sends the values to pubnub

    } else {
      for (int i = 0; i < 6; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip.setPixelColor(i, strip.Color(0, 0, 0)); // Moderately bright red color.
        delay(50);// This sends the updated pixel color to the hardware.
        strip.show();
      }
    }
  }
  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == LOW) {
      Serial.println("button2 is on");

      for (int i = 0; i < 1; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255

        for (int j = 0; j < 256 ; j++) {
          for (uint16_t i = 0; i < strip2.numPixels(); i++) {
            strip2.setPixelColor(i, strip2.Color(j, j, j, neopix_gamma[j] ) );
          }
          delay(2);
          strip2.show();
        }

        //        strip.setPixelColor(i, strip.Color(0, 255, 0)); // Moderately bright red color.
        //        strip.show(); // This sends the updated pixel color to the hardware.
        delay(50);// This sends the updated pixel color to the hardware.

      }
       publishToPubNub();                      //execute the function that sends the values to pubnub

    }  else {
      for (int i = 0; i < 6; i++) {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        strip2.setPixelColor(i, strip2.Color(0, 0, 0)); // Moderately bright red color.
        strip2.show();
        delay(50);// This sends the updated pixel color to the hardware.
      }
    }


  }
  delay(100);
  lastButtonState1 = buttonState1;
  lastButtonState2 = buttonState2;
}
void publishToPubNub()
{
  WiFiClient *client;
  DynamicJsonBuffer messageBuffer(600);                    //create a memory buffer to hold a JSON Object
  JsonObject& pMessage = messageBuffer.createObject();    //create a new JSON object in that buffer

  ///the imporant bit where you feed in values
  pMessage["button1"] = buttonState1;
  pMessage["button2"] = buttonState2;


  int mSize = pMessage.measureLength() + 1;                   //determine the size of the JSON Message
  char msg[mSize];                                            //create a char array to hold the message
  pMessage.printTo(msg, mSize);                              //convert the JSON object into simple text (needed for the PN Arduino client)

  client = PubNub.publish(pubChannel, msg);                      //publish the message to PubNub

  if (!client)                                                //error check the connection
  {
    Serial.println("client error");
    delay(1000);
    return;
  }

  if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success)  //check that it worked
  {
    Serial.print("Got HTTP status code error from PubNub, class: ");
    Serial.print(PubNub.get_last_http_status_code_class(), DEC);
  }

  while (client->available())                                 //get feedback from PubNub
  {
    Serial.write(client->read());
  }
  client->stop();                                             //stop the connection
  Serial.print("Successful Publish");
}
