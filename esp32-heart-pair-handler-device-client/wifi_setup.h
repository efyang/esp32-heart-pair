#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H
/*
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
          strncpy((char*)packet.data(), data_s, packet.length());
          data_s[packet.length()] = '\0';
          char * pch = strtok(data_s, " ");
          int i = 1;
          while (pch != NULL) {
            i += 1;
            pch = strtok(NULL, " ");
            Serial.println(pch);
          }
          //reply to the client
          //packet.printf("GOT_UPDATE %u bytes of data", packet.length());
        });
        connecting = false;
        return true;
      }
    }
    connecting = false;
  }
  return false;
}

void send_client_update(boolean love_state, boolean happy_state, boolean sad_state, boolean anger_state, boolean fear_state) {
   if (udp.connect(IPAddress(142, 93, 30, 237), 1234)) {
      uint8_t love = love_state;
      uint8_t happy = happy_state;
      uint8_t sad = sad_state;
      uint8_t anger = anger_state;
      uint8_t fear = fear_state;
      uint8_t mood_bits = (love << 4) | (happy << 3) | (sad << 2) | (anger << 1) | (fear << 0);

      char buf[40];
      ltoa(millis(), buf, 10);

      char s[4];
      snprintf(s, 4, "%d", mood_bits);
      
      udp.print((std::string("CLIENT_UPDATE ") + std::string(buf) + std::string(" ") + std::string(s)).c_str());
   }
}



void setup_wifi(std::string ssid, std::string password) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    while (1) {
      delay(1000);
    }
  }
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
      //reply to the client
      packet.printf("Got %u bytes of data", packet.length());
    });
    //Send unicast
    udp.print("CLIENT_UPDATE 0 60 50");
  }
}
*/
#endif
