#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include <AsyncUDP.h>
#include <Arduino.h>

AsyncUDP udp;
std::string wifi_ssid = "";
std::string wifi_pass = "";
boolean wifi_connected = false;
boolean connecting = false;

boolean remote_love = false;
boolean remote_happy = false;
boolean remote_sad = false;
boolean remote_anger = false;
boolean remote_fear = false;

boolean try_wifi_connect(std::string ssid, std::string pass, int try_times) {
  if (!connecting) {
    WiFi.mode(WIFI_STA);
    boolean is_connected = false;
    int tries = 1;
    Serial.print("connecting to ");
    Serial.print(wifi_ssid.c_str());
    Serial.print(" with password: ");
    Serial.println(wifi_pass.c_str());
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
    connecting = false;
  }
  return false;
}

std::string millistring() {
      char buf[40];
      ltoa(millis(), buf, 10);
      return std::string(buf);
}

void send_client_update(boolean love_state, boolean happy_state, boolean sad_state, boolean anger_state, boolean fear_state) {
   if (udp.connect(IPAddress(142, 93, 30, 237), 1234)) {
      uint8_t love = love_state;
      uint8_t happy = happy_state;
      uint8_t sad = sad_state;
      uint8_t anger = anger_state;
      uint8_t fear = fear_state;
      uint8_t mood_bits = (love << 4) | (happy << 3) | (sad << 2) | (anger << 1) | (fear << 0);

      udp.printf("CLIENT_UPDATE %u %u", millis(), mood_bits);
   }
}

unsigned long long last_server_time = 0;

void setup_wifi(std::string ssid, std::string password) {
  wifi_connected = try_wifi_connect(ssid, password, 5);
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
        Serial.printf("'%s'\n", pch);
        strings[i++] = pch;
        pch = strtok(NULL, " ");
      }

      if (strings[0] == "SERVER_UPDATE") {
        //reply to the client
        // return uuid (strings[2])
        packet.printf("GOT_UPDATE %u %s", millis(), strings[2]);
        // process data
        Serial.println(strings[])
      }
    });
  }
}

#endif
