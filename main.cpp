#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

const char* SSID          = "WiFi SSID/Name here";
const char* PASSWORD      = "Wifi Password here";
const char* CLIENT_ID     = "Client ID Here";
const char* CLIENT_SECRET = "Client Secret here";

#define TFT_CS    10
#define TFT_DC    1
#define TFT_MOSI  0
#define TFT_SCLK  2
#define TFT_RST   3

#define ENC_CLK 7
#define ENC_DT  8

const uint8_t SW_PINS[3] = {4, 5, 6};

bool     lastState[3]        = {HIGH, HIGH, HIGH};
bool     stableState[3]      = {HIGH, HIGH, HIGH};
uint32_t lastDebounceTime[3] = {0, 0, 0};           
const uint32_t DEBOUNCE_MS   = 50;

const uint8_t ENC_CLK = 7;
const uint8_t ENC_DT  = 8;

volatile int  encoderPos    = 0;
volatile bool encoderMoved  = false;
portMUX_TYPE  mux           = portMUX_INITIALIZER_UNLOCKED;

String lastArtist;
String lastTrackname;

Spotify sp(CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void IRAM_ATTR onEncoderChange() {
  portENTER_CRITICAL_ISR(&mux);
  if (digitalRead(ENC_DT) != digitalRead(ENC_CLK))
    encoderPos++;
  else
    encoderPos--;
  encoderMoved = true;
  portEXIT_CRITICAL_ISR(&mux);
}

void volumeFunc(void* param) {
  while (true) {
    if (encoderMoved) {
      portENTER_CRITICAL(&mux);
      int delta = encoderPos;
      encoderPos   = 0;
      encoderMoved = false;
      portEXIT_CRITICAL(&mux);

      int change = constrain(delta, -5, 5);

      if (change > 0)
        sp.volume_up(change);
      else if (change < 0)
        sp.volume_down(-change);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void spotifyFunc(void* param) {
  while (true) {                                     
    String currentArtist    = sp.current_artist_names();
    String currentTrackname = sp.current_track_name();

    if (lastArtist != currentArtist &&
        currentArtist != "Something went wrong" &&
        !currentArtist.isEmpty()) {
      tft.fillScreen(ST77XX_BLACK);
      lastArtist = currentArtist;
      Serial.println("Artist: " + lastArtist);
      tft.setCursor(10, 10);
      tft.write(lastArtist.c_str());
    }

    if (lastTrackname != currentTrackname &&
        currentTrackname != "Something went wrong" &&
        currentTrackname != "null") {
      lastTrackname = currentTrackname;
      Serial.println("Track: " + lastTrackname);
      tft.setCursor(10, 40);
      tft.write(lastTrackname.c_str());
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);           
  }                                                  
}

void buttonsFunc(void* param) {
  while (true) {
    uint32_t now = millis();

    for (int i = 0; i < 3; i++) {
      bool reading = digitalRead(SW_PINS[i]);

      if (reading != lastState[i])
        lastDebounceTime[i] = now;                   

      if ((now - lastDebounceTime[i]) >= DEBOUNCE_MS) {
        if (reading != stableState[i]) {
          stableState[i] = reading;

          if (stableState[i] == LOW) {
            vTaskDelay(500);
            if (stableState[0] == LOW && stableState[1] == LOW && stableState[2] == LOW) {
              // All three buttons
              sp.start_playback("spotify:track:4jnFqNWeJCeCRHc4HCdxfd"); // I'm using Michael Jackson's Remember The Time
              break;
            }
            if (i != 2) {
              if (stableState[i++] == LOW) {
                switch (i) {
                  case 0:
                    // First two buttons
                    sp.start_playback("spotify:track:4cgjA7B4fJBHyB9Ya2bu0t"); // MJ's Human Nature
                    break;
                  case 1:
                    // Middle and last buttons
                    sp.start_playback("spotify:track:2gSNBigeWMVtY3QBIvPAEc"); // MJ's Bad
                    break;
                }
              }
            } elseif (i == 2 && stableState[0] == LOW) {
              // First and last buttons
              sp.start_playback("spotify:track:1Lhwn4PqeGpM4LTVUowW76"); // This one's Michael Jackson's You Rock My World
            }
            else {
              switch (i) {
                case 0:
                  sp.previous_track();
                  break;
                case 1:
                  sp.toggle_pause();
                  break;
                case 2:
                  sp.next_track();
                  break;
              }
            }
          }
        }
      }

      lastState[i] = reading;                        
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);             
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 3; i++)
    pinMode(SW_PINS[i], INPUT_PULLUP);               

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  Serial.println("TFT Initialized!");

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  tft.setCursor(0, 0);
  tft.write(WiFi.localIP().toString().c_str());

  sp.begin();
  while (!sp.is_auth())
    sp.handle_client();
  Serial.println("Authenticated");

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_CLK), onEncoderChange, CHANGE);

  xTaskCreate(volumeFunc, "Volume", 2048, NULL, 2, NULL);
  xTaskCreate(buttonsFunc, "Buttons", 2048, NULL, 2, NULL);
  xTaskCreate(spotifyFunc, "Spotify", 4096, NULL, 1, NULL);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}