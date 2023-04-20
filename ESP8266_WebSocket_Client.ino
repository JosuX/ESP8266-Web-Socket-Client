#include <ESP8266WiFi.h>

const char* ssid PROGMEM = "PLDTHOMEFIBRe3960";
const char* password PROGMEM = "PLDTWIFIjhq97";
//const char* ssid = "Josup";
//const char* password = "wazup321";
constexpr int port PROGMEM = 80;
constexpr int max_servers PROGMEM = 1; // Maximum number of socket servers to connect to
int preVal[2] = {HIGH, HIGH};

class ServerSocket {
  public:
    IPAddress ip;
    WiFiClient client;
    bool connected;
};

std::array<ServerSocket, max_servers> servers;
int num_servers = 0;

void setup() {
  pinMode(1, INPUT);
  //Serial.begin(115200);
  WiFi.begin(ssid, password);
  //Serial.print(F("Connecting to WiFi..."));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("."));
    delay(500);
  }
  //Serial.println(F(""));
  //Serial.println(F("WiFi connected"));

  IPAddress localIP = WiFi.localIP();
  //Serial.print(F("Local IP address: "));
  //Serial.println(localIP);
}

void loop() {
int currentValues[2] = {!digitalRead(1), !digitalRead(3)};  
  if (num_servers < max_servers) {
    // Connect to all socket servers in the same subnet
    for (int i = 2; i <= 10; i++) {
      const IPAddress& ip = IPAddress(WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], i); // Use the same subnet as the local IP address
      if (ip != WiFi.localIP()) {
        bool found = false;
        for (const auto& server : servers) {
          if (server.ip == ip) {
            found = true;
            break;
          }
        }
        if (!found) {
          auto newClient = WiFiClient();
          //Serial.print(F("Connecting to "));
          //Serial.println(ip);
          if (newClient.connect(ip, port)) {
            //Serial.println(F("Success"));
            servers[num_servers].ip = ip;
            servers[num_servers].client = newClient;
            servers[num_servers].connected = true;
            num_servers++;
          } else {
            //Serial.println(F("Failed to connect to server"));
          }
        }
      }
      if(num_servers == max_servers){
        break;
      } 
    }
  }

  // Send data to all connected socket servers
  for (auto& server : servers) {
    if (server.connected) {      
      if (server.client.connected()) {
  boolean changed = false;
  char output[32];
  strcat(output, "key=32l7xjel ");
  for (int i = 0; i < 2; i++) {
    if (currentValues[i] != preVal[i]) {
      char pinNumber[2];
      itoa(((2*i) + 1), pinNumber, 10);
      strcat(output, pinNumber);
      strcat(output, "=");
      char pinValue[2];
      itoa(currentValues[i], pinValue, 10);
      strcat(output, pinValue);
      strcat(output, " ");
      preVal[i] = currentValues[i];
      changed = true;
    }
  }      
        if(changed){
          if (server.client.write(output) == -1) {
          //Serial.print(F("Failed to write to server "));
          //Serial.println(server.ip);
          server.connected = false;
        }
        }
        else if(!changed){
          if (server.client.write("p") == -1) {
          //Serial.print(F("Failed to write to server "));
          //Serial.println(server.ip);
          server.connected = false;
        }
        }
      } else {
        //Serial.print(F("Disconnected from "));
        //Serial.println(server.ip);
        server.connected = false;
      }
    } else {
      //Serial.print(F("Reconnecting to "));
      //Serial.println(server.ip);
      if (server.client.connect(server.ip, port)) {
        //Serial.println(F("Success"));
        server.connected = true;
      } else {
        //Serial.println(F("Failed to connect to server"));
      }
    }
    delay(500);
  }
}
