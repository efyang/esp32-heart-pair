#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <WiFi.h>
#include <AsyncUDP.h>

AsyncUDP udp;

void setup_wifi(std::string ssid, std::string password) {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("WiFi Failed");
        while(1) {
            delay(1000);
        }
    }
    if(udp.connect(IPAddress(142,93,30,237), 1234)) {
        Serial.println("UDP connected");
        udp.onPacket([] (AsyncUDPPacket packet) {
          Serial.print("UDP Packet Type: ");
          Serial.print(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
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

#endif
