#include <ESP8266WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
#define MSG_BUFFER_SIZE  (50)
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
// Update these with values suitable for your network.

const char* ssid = "SLT-4G-CS";
const char* password = "Sandu@uom31";
const char* mqtt_server = "test.mosquitto.org";

int nvalue = 0;
int pvalue = 0;
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  nvalue = 0;
  String tdelay = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);

    tdelay += (char)payload[i];

  }
  nvalue = tdelay.toInt();
  Serial.println();

  Serial.print("Value ");
  Serial.println(nvalue);


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("Warning");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (nvalue != pvalue) {

    if (nvalue >= 500 && nvalue <= 5000) {
      for (int i = 0; i < 10000 / nvalue; i++) {
        digitalWrite(BUILTIN_LED, LOW);
        delay(nvalue);
        Serial.println("blinked");
        digitalWrite(BUILTIN_LED, HIGH);
        delay(nvalue);
      }
    }
    pvalue = nvalue;

  }
}
