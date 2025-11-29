void setup() {
    Serial.begin(9600);             // baud rate = 9600 bps
    Serial.println("Print ONCE!");  // พิมพ์มาแค่ครั้งเดียวตอนเริ่ม
}

void loop() {
    Serial.print("Hello");      // พิมพ์
    Serial.println(" World!");  // พิมพ์และจะขึ้นบรรทัดใหม่หลังจากนี้
}
