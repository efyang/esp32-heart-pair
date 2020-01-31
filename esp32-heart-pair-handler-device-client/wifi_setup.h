#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include "esp_wpa2.h"
#include <AsyncUDP.h>
#include <Arduino.h>
#include "FastLED.h"

#define WIFI_TRY_CONNECT_TIMES 3

AsyncUDP udp;
std::string wifi_ssid = "";
std::string wifi_pass = "";
std::string wifi_user = "";
uint8_t WIFI_MODE_NORMAL = 0;
uint8_t WIFI_MODE_EDUROAM = 1;
uint8_t wifi_mode = WIFI_MODE_NORMAL;

boolean wifi_connected = false;
boolean connecting = false;

boolean remote_love = false;
boolean remote_happy = false;
boolean remote_sad = false;
boolean remote_anger = false;
boolean remote_fear = false;

boolean try_wifi_connect(std::string ssid, std::string pass, int try_times) {
  if (!connecting && ssid.length() > 1) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    boolean is_connected = false;
    int tries = 1;
    Serial.print("connecting to ");
    Serial.print(ssid.c_str());
    Serial.print(" with password: ");
    Serial.println(pass.c_str());
    while (!is_connected && (tries <= try_times)) {
      WiFi.begin(ssid.c_str(), pass.c_str());
      connecting = true;
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        tries += 1;
        Serial.println("wifi failed");
      } else {
        is_connected = true;
        Serial.println("wifi connected");

        connecting = false;
        return true;
      }
    }
    WiFi.disconnect(true);
    connecting = false;
  }
  return false;
}

boolean try_wifi_connect_wpa2_enterprise(std::string ssid, std::string user, std::string pass, int try_times) {
  if (!connecting && ssid.length() > 1) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)wifi_user.c_str(), wifi_user.length()); //provide identity
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)wifi_user.c_str(), wifi_user.length()); //provide username --> identity and username is same
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)wifi_pass.c_str(), wifi_pass.length()); //provide password
    esp_wpa2_config_t conf = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
    esp_wifi_sta_wpa2_ent_enable(&conf); //set config settings to enable function

    boolean is_connected = false;
    int tries = 1;
    Serial.print("connecting to ");
    Serial.print(ssid.c_str());
    Serial.print(" with password: ");
    Serial.println(pass.c_str());
    Serial.print(" with username: ");
    Serial.println(user.c_str());
    while (!is_connected && (tries <= try_times)) {
      WiFi.begin(ssid.c_str(), pass.c_str());
      connecting = true;
      if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        tries += 1;
        Serial.println("wifi failed");
      } else {
        is_connected = true;
        Serial.println("wifi connected");

        connecting = false;
        return true;
      }
    }
    WiFi.disconnect(true);
    connecting = false;
  }
  return false;
}

std::string millistring() {
      char buf[40];
      ltoa(millis(), buf, 10);
      return std::string(buf);
}

void send_client_update(boolean love_state, boolean happy_state, boolean sad_state, boolean fear_state, boolean anger_state) {
   if (udp.connect(IPAddress(142, 93, 30, 237), 1234)) {
      uint8_t love = love_state;
      uint8_t happy = happy_state;
      uint8_t sad = sad_state;
      uint8_t fear = fear_state;
      uint8_t anger = anger_state;
      uint8_t mood_bits = (love << 4) | (happy << 3) | (sad << 2) | (fear << 1) | (anger << 0);

      udp.printf("CLIENT_UPDATE %u %u", millis(), mood_bits);
   }
}

long long last_server_time = 0;


void setup_udp_callback() {
  if (udp.connect(IPAddress(142, 93, 30, 237), 1234)) {
    Serial.println("UDP connected");
    udp.onPacket([] (AsyncUDPPacket packet) {
      Serial.print("UDP Packet Type: ");
      Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
      Serial.print(", From: ");
      Serial.print(packet.remoteIP());
      Serial.print(":");
      Serial.print(packet.remotePort());
      Serial.print(", To: ");
      Serial.print(packet.localIP());
      Serial.print(":");
      Serial.print(packet.localPort());
      Serial.print(", Length: ");
      Serial.print(packet.length());
      Serial.print(", Data: ");
      Serial.write(packet.data(), packet.length());
      Serial.println();

      char data_s[packet.length() + 1];
      strncpy(data_s, (char*)packet.data(), packet.length());
      data_s[packet.length()] = '\0';
      char * pch = strtok(data_s, " ");
      char * strings[6];
      int i = 0;
      while (pch != NULL) {
        strings[i++] = pch;
        pch = strtok(NULL, " ");
      }
      Serial.println(strings[0]);
      if (strcmp(strings[0], "SERVER_UPDATE") == 0) {
        //reply to the client
        // return uuid (strings[2])
        packet.printf("GOT_UPDATE %u %s", millis(), strings[2]);
        // process data
        long long message_time = atoll(strings[1]);
        if (message_time > last_server_time) {
          last_server_time = message_time;
          // is in order, use this info
          uint8_t remote_bitflags = (uint8_t)atoi(strings[3]);
          remote_love = (remote_bitflags >> 4) & 1;
          remote_happy = (remote_bitflags >> 3) & 1;
          remote_sad = (remote_bitflags >> 2) & 1;
          remote_fear = (remote_bitflags >> 1) & 1;
          remote_anger = (remote_bitflags >> 0) & 1;
          Serial.printf("love: %d happy: %d sad: %d fear: %d anger: %d\n",
            remote_love,
            remote_happy,
            remote_sad,
            remote_fear,
            remote_anger);
        }
      }
    });
  }
}


void setup_wifi_wpa2_enterprise(std::string ssid, std::string user, std::string password) {
  wifi_connected = try_wifi_connect_wpa2_enterprise(ssid, user, password, WIFI_TRY_CONNECT_TIMES);
  setup_udp_callback();
}

void setup_wifi(std::string ssid, std::string password) {
  wifi_connected = try_wifi_connect(ssid, password, WIFI_TRY_CONNECT_TIMES);
  setup_udp_callback();
}

void try_wifi_connect(CRGB* heart_leds) {
  Serial.println("Try wifi connect now");
  Serial.printf("\"%s\"", wifi_ssid.c_str());
  Serial.printf("\"%s\"", wifi_user.c_str());
  Serial.printf("\"%s\"", wifi_pass.c_str());
  heart_leds[13] = CRGB::Red;
  FastLED.show();
  if (wifi_mode == WIFI_MODE_NORMAL) {
    setup_wifi(wifi_ssid, wifi_pass);
  } else if (wifi_mode == WIFI_MODE_EDUROAM) {
    setup_wifi_wpa2_enterprise(wifi_ssid, wifi_user, wifi_pass);
  }
  if (wifi_connected) {
    Serial.println("wifi connected");
    for (int i = 0; i < 5; i++) {
      heart_leds[13] = CRGB::Green;
      FastLED.show();
      delay(500);
      heart_leds[13] = CRGB::Black;
      FastLED.show();
      delay(500);
    }
  } else {
    Serial.println("wifi failed to connect");
    for (int i = 0; i < 5; i++) {
      heart_leds[13] = CRGB::Red;
      FastLED.show();
      delay(500);
      heart_leds[13] = CRGB::Black;
      FastLED.show();
      delay(500);
    }
  }
}

#endif
