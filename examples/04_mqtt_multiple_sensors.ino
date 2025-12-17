/* --- MQTT Netpie
  Objective:
 --- Simple code for MQTT to NETPIE 
  Description:
 --- 
  Note to author(s):
 --- 

  To use:
 ---        See the bottom of the code and edit "PARAMETERS"
 ---
  Authors:  TAA (teerachart.s@mail.rmutk.ac.th)
 ---        TAA     JUN/06/2025     Initial Coding
*/

/* ************************************************************************ */
/*      Include Files                                                       */
/* ************************************************************************ */

#include <WiFi.h>

// for MQTT
// https://github.com/knolleary/pubsubclient
#include "PubSubClient.h"

/* ************************************************************************ */
/*      Parameters                                                          */
/* ************************************************************************ */

// WiFi information
char* ssid = "Username"; // เปลี่ยนเป็น ชื่อ WiFi
char* pass = "password"; // เปลี่ยนเป็น รหัส WiFi

// MQTT to NETPIE 
// https://docs.netpie.io/mqtt-api.html
const char* mqttServer = "broker.netpie.io"; // MQTT broker address
const int mqttPort = 1883; // PORT: 1883 for MQTT, 1884 for MQTTs              
const char* mqttClientID = "";
const char* mqttToken = "";
const char* mqttSecret = "";

/* ************************************************************************ */
/*      Code Section                                                        */
/* ************************************************************************ */

// สำหรับการใช้ WiFi และ MQTT
WiFiClient espClient;                // สำหรับเชื่อมต่อ WiFi
PubSubClient client(espClient);      // สำหรับ MQTT

const int number_data = 2; // จำนวนชุดข้อมูลที่จะส่ง
String topics[number_data] = {"temp", "moisture"};
float values[number_data] = {0};

// ฟังก์ชันเชื่อมต่อ WiFi
void setupWiFi() {
    Serial.println("Connecting to WiFi..."); // แสดงข้อความก่อนเชื่อม WiFi
    WiFi.begin(ssid, pass);                  // เชื่อมต่อ WiFi

    // รอจนกว่าจะเชื่อมต่อสำเร็จ หากไม่สำเร็จหน่วงเวลา 1 วินาที
    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);        
        Serial.print("."); 
    }

    // แสดงข้อความเมื่อเชื่อมต่อสำเร็จ
    Serial.println("WiFi connected!");

    // แสดง IP address
    Serial.print("Local ESP32 IP address: ");
    Serial.println(WiFi.localIP());
}

// ทำงานเมื่อได้รับข้อความจาก MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    // แสดง payload ที่ได้รับ
    Serial.print("I heard: ");
    Serial.write(payload, length);           
    Serial.println(); // ขึ้นบรรทัดใหม่
}

// ฟังก์ชันเชื่อมต่อ MQTT Broker
void connect_MQTT() {
    while (!client.connected()) {               // ถ้ายังไม่เชื่อมต่อ MQTT
        Serial.print("NETPIE-");     
        if (client.connect(mqttClientID, mqttToken, mqttSecret)) { 
            Serial.println("MQTT Connected!");  // ถ้าเชื่อมต่อสำเร็จ
            client.subscribe("@shadow/data/updated/data/chatter");   // subscribe หัวข้อ (topic)
        } else {
            Serial.print("MQTT Failed: ");      // ถ้าเชื่อมต่อไม่ได้
            Serial.println(client.state());      
            delay(5000);                        // รอ 5 วินาทีแล้วลองใหม่
        }
    }
}

// ฟังก์ชันสำหรับอัปเดตข้อมูล Shadow
void publish_shadow() {
    String shadowData = "{\"data\": {"; // สร้าง JSON
    for (int i = 0; i < number_data; i++) {
        shadowData += "\"" + topics[i] + "\": " + String(values[i], 4);

        if (i < number_data - 1) { // ถ้าไม่ใช่ตัวแปรสุดท้่ายที่จะส่ง
            shadowData += ", ";  // เติม comma "," เพื่อคั่นระหว่างตัวแปรที่จะส่ง
        }
    }
    shadowData += "}}"; // ถ้าสุดท้าย ให้ใส่ปิดวงเล็บ

    client.publish("@shadow/data/update", shadowData.c_str());    // ส่งข้อมูล
    
    // แสดงผลการอัปเดต
    Serial.print("Sent: "); 
    Serial.println(shadowData.c_str()); 
}

/* ************************************************************************ */
/*      Main Functions                                                      */
/* ************************************************************************ */

void setup() {
    Serial.begin(9600);
    setupWiFi();                             // เชื่อมต่อ WiFi
    client.setServer(mqttServer, mqttPort);  // ตั้งค่าเซิฟเวอร์ MQTT
    client.setCallback(callback);            // ตั้งค่าการทำงานเมื่อได้รับข้อความจาก MQTT
    connect_MQTT();                          // เชื่อมต่อ MQTT
}

void loop() {
    // ถ้า MQTT หลุด พยายามเชื่อมใหม่
    if (!client.connected()) { 
        connect_MQTT();
    }
    client.loop();

    // เก็บค่าใหม่เพื่อส่ง
    values[0] = random(22, 40); // ค่า สมมติ ของอุณหภูมิ
    values[1] = 1 - float(analogRead(34))/4095.0; // ตัววัดความชื้นในดิน
    publish_shadow();

    delay(10000);       // หน่วง 10 วินาที
}
