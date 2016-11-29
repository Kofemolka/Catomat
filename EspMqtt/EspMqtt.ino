/*
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Config.h"

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);   

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
	Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Feeder", mqtt_user, mqtt_pwd))
	{
      Serial.println("connected");
      client.subscribe(in_topic_mode);
	  client.subscribe(in_topic_cmd);
	  client.subscribe(in_topic_adj);
    }
	else
	{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  if (Serial.available())
  {
	  String str = Serial.readString();
	  int ndx = str.indexOf(":");
	  if (ndx != -1)
	  {
		  String topic = str.substring(0, ndx - 1);
		  String value = str.substring(ndx + 1);

		  Serial.print(topic);
		  Serial.print(":");
		  Serial.println(value);

		  client.publish(topic, value.c_str());
	  }
  }  
}

void callback(char* topic, byte* payload, unsigned int length)
{
	Serial.print(topic);
	Serial.print(":");

	for (int i = 0; i < length; i++)
	{
		Serial.print((char)payload[i]);
	}

	Serial.println();
}
