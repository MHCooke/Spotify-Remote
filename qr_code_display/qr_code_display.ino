// for hardware SPI
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
const int OLED_DC = 21;
const int OLED_CS = 5;
const int OLED_RESET = 22;

Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RESET, OLED_CS);

// Hardware Buttons
struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button b_list[] = {{32, false}, {33, false}, {25, false}};
const int prev = 0;const int play = 1;const int next = 2;

int last_millis;
const int loop_time = 1000; 

// QR Code settings
#include <qrcode.h>
// Estimated URL length: 216 chars
const int QR_CODE_VERSION = 9;
const int QR_CODE_EC = ECC_MEDIUM;
const int QR_SCALE = 1;
const int QR_BORDER = 4;

// WiFi 
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <URLEncoding.h>
#include "wifi_secret.h"
// #include "WiFinder.h"

// Spotify Access Token
struct Token {
  String token;
  Token() {
    token = "";
  }
  Token(String supplied_token) {
    token = supplied_token;
  }
  bool exists() {
    return token != "";
  }
  String& operator=(const String& new_token) {
    token = new_token;
  }
  operator String() const {
    return token;
  }
};

#include <FS.h>
#include <SPIFFS.h>
const char* TOKEN_FILE_NAME = "/token_file";
Token access_token(secret_access_code);
Token refresh_token;

// JSON
#include <Arduino_JSON.h>

// Users Data
JSONVar user_data;
JSONVar player_data;

class restAPI: public HTTPClient {
  String base_URL;
  const char * CA_cert;
  public:
    String custom_header_key;
    String custom_header_value;
  
    restAPI (const char * base_URL, const char * CA_cert, const char * custom_header_key = "", String custom_header_value = "") {
      this->base_URL = String(base_URL);
      this->CA_cert = CA_cert;
      this->custom_header_key = String(custom_header_key);
      this->custom_header_value = custom_header_value; 
    }
  
    String request(const char * type, const char * endpoint, String payload = "") {
      begin(String(base_URL + endpoint), CA_cert);
      addHeader(custom_header_key, custom_header_value);
      Serial.println("Request headers:");Serial.print(_headers);
      int response_code;
      String return_payload;
      if (type == "GET") {
        response_code = sendRequest(type);
      } else {
        response_code = sendRequest(type, (uint8_t *) payload.c_str(), payload.length());
      }
      if (response_code == 200) {
        return_payload = getString();
      } else {
        return_payload = "";
        Serial.print("Request failed, reason: ");Serial.println(getString());
      }
      end();
      return return_payload;
    }

    static String easy_request(String url) {
      HTTPClient http;
      http.begin(url);
      int response_code = http.GET();
      String response_string;
      if (response_code == 200) {
        response_string = http.getString();
      } else {
        response_string = "";
        Serial.print("Request failed, reason: ");Serial.println(http.getString());
      }
      http.end();
      return response_string;
    }
};

restAPI spotify("https://api.spotify.com", trusted_root, "Authorization");
restAPI accounts("https://accounts.spotify.com", trusted_root);
  
String noQuote(JSONVar source) {
  String string = JSONVar::stringify(source);
  int len = string.length();
  return string.substring(1,len-1);
}

void display_qr(const char* url) {
  QRCode auth_qr;
  uint8_t qrcode_data[qrcode_getBufferSize(QR_CODE_VERSION)];
  qrcode_initText(&auth_qr, qrcode_data, QR_CODE_VERSION, QR_CODE_EC, url);
  
  display.clearDisplay();
  display.fillRect(0, 0, (auth_qr.size * QR_SCALE) + (QR_BORDER * QR_SCALE * 2), (auth_qr.size * QR_SCALE) + (QR_BORDER * QR_SCALE * 2), SSD1306_WHITE);
  for (uint16_t y = 0; y < auth_qr.size; y++) {
    for (uint16_t x = 0; x < auth_qr.size; x++) {
      if (qrcode_getModule(&auth_qr, x, y)) {
        display.fillRect((x*QR_SCALE)+QR_BORDER, (y*QR_SCALE)+QR_BORDER, QR_SCALE, QR_SCALE, SSD1306_BLACK);
      }
    }
  }
  display.display();
}

void IRAM_ATTR press_button(void* arg) {
  Button* b = static_cast<Button*>(arg);
  b->pressed = true;
}

void update_access_token() {
  File token_file = SPIFFS.open("token_file");
  refresh_token = token_file.readString();
  String encoded_refresh_token = urlEncode(refresh_token);
  String payload = "grant_type=refresh_token&refresh_token=";
  payload += encoded_refresh_token;
  JSONVar response = JSON.parse(accounts.request("POST", "/api/token", payload));
  access_token = noQuote(response["access_token"]);
}

void setup() {
  Serial.begin(115200);

  // Initialise pins
  for (int i = 0; i < 3; i++) {
    pinMode(b_list[i].PIN, INPUT_PULLUP);
    attachInterruptArg(b_list[i].PIN, press_button, &b_list[i], FALLING);
  }

  // Initialise WiFi  
  Serial.print(F("Connecting to: "));
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println(F("Connection successful.\nUpdating time from internet."));
  configTime(0,0, "pool.ntp.org");

  // Initialise file system
  // SPIFFS.begin(bool formatIfFailed)
  if (!SPIFFS.begin(true)) {
    Serial.println(F("SPIFFS Mount failed, reboot"));
  }
  
  // Initialise display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(/*SSD1306_SWITCHCAPVCC*/)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.clearDisplay();

  // API key stuff
  accounts.setAuthorization(client_id, client_secret);
  
  if(SPIFFS.exists(TOKEN_FILE_NAME)) {
    update_access_token();
  } else {
    // ask user to authenticate, step 1, get middleman id
    String middle_man_id = restAPI::easy_request("http://mhcooke.uk/api/spotify/get_id.php");

    // construct spotify auth url
    String spotify_auth_url = "https://accounts.spotify.com/authorize?client_id=";
    spotify_auth_url += client_id;
    spotify_auth_url += "&response_type=code&redirect_uri=http%3A%2F%2Fmhcooke.uk%2Fapi%2Fspotify%2Fset_code.php&scope=user-modify-playback-state&state=";
    spotify_auth_url += middle_man_id;

    // show auth to user
    display_qr(spotify_auth_url.c_str());

    // wait for user to authorise access, query get_code.php
    delay(15000);
    String get_code_url = "http://mhcooke.uk/api/spotify/get_code.php?id=";
           get_code_url += middle_man_id;
    String response;
    do {
      response = restAPI::easy_request(get_code_url);
    } while (!response.startsWith("S"));
    display.clearDisplay();
    display.print("Success! ");
    String authorisation_code = response.substring(1);
    String payload = "grant_type=authorization_code&code=";
    payload += urlEncode(authorisation_code);
    payload += "&redirect_uri=http%3A%2F%2Fmhcooke.uk%2Fapi%2Fspotify%2Fset_code.php";
    JSONVar tokens = JSON.parse(accounts.request("POST", "/api/token", payload));
    access_token = noQuote(tokens["access_token"]);
    refresh_token = noQuote(tokens["refresh_token"]);

    // update api object
    spotify.custom_header_value = String("Bearer ") + secret_access_code;

    // write refresh token to SPIFFS
    File token_file = SPIFFS.open(TOKEN_FILE_NAME);
    token_file.print(refresh_token);
  }

  // Fetch user data
  JSONVar user_data = JSON.parse(spotify.request("GET", "/v1/me"));
  Serial.println(user_data);
  if (user_data.hasOwnProperty("display_name")) {
    display.printf("Welcome %s\n", noQuote(user_data["display_name"]));
  } else {
    display.print("Something went wrong :(");
    display.display();
    while (true) {
      // halt
    }
  }  
  display.display();
  display.setTextSize(1);
  delay(2000);
  last_millis = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - last_millis > loop_time) {
    player_data = JSON.parse(spotify.request("GET", "/v1/me/player"));
    display.clearDisplay();
    display.setCursor(0,0);
    if (player_data.hasOwnProperty("is_playing")) {
      if (player_data.hasOwnProperty("item")) {
        display.println(noQuote(player_data["item"]["name"]));
        display.print("by ");display.print(noQuote(player_data["item"]["artists"][0]["name"]));
      }
      if (player_data["is_playing"]) {
        display.fillRect(56, 35, 6, 25, SSD1306_WHITE);
        display.fillRect(66, 35, 6, 25, SSD1306_WHITE);
      } else {
        display.fillTriangle(54, 35, 54, 60, 73, 47, SSD1306_WHITE);
      }
    }
    display.display();  
    last_millis = millis();
  }

  if (b_list[prev].pressed == true) {
    spotify.request("PUT", "/v1/me/player/previous");
    b_list[prev].pressed = false;
  } else if (b_list[play].pressed == true) {
    if (player_data["is_playing"]) {
      spotify.request("PUT", "/v1/me/player/pause");
    } else {
      spotify.request("PUT", "/v1/me/player/play");
    }
    b_list[play].pressed = false;
  } else if (b_list[next].pressed == true) {
    spotify.request("PUT", "/v1/me/player/next");
    b_list[play].pressed = false;
  }  
}
