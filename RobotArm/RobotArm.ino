#include <ESP32Servo.h>  
#include <WiFi.h>        

const char *ssid = "";       
const char *password = "";   

const char *host = "";  //ipv4 của host
const int port = 8082;               

WiFiClient client;

IPAddress local_IP(192, 168, 50, 150);  
IPAddress gateway(192, 168, 50, 133);    
IPAddress subnet(255, 255, 255, 0);     

Servo servo1, servo2, servo3, servo4;

const int servo1Pin = 2;   
const int servo2Pin = 4;   
const int servo3Pin = 5;   
const int servo4Pin = 18;  

bool objectPicked = false; 
bool hostConnected = false; 

void setup() {
    Serial.begin(115200);

    servo1.attach(servo1Pin, 500, 2500);
    servo2.attach(servo2Pin, 500, 2500);
    servo3.attach(servo3Pin, 500, 2500);
    servo4.attach(servo4Pin, 500, 2500);  

    servo1.write(0);
    servo2.write(70);
    servo3.write(40);
    servo4.write(80);
    delay(2000);    

    if (!WiFi.config(local_IP, gateway, subnet)) {
        Serial.println("❌ Không thể cấu hình IP tĩnh!");
    }

    Serial.print("Đang kết nối WiFi ");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n✅ Kết nối WiFi thành công!");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());

    connectToServer();
}

void loop() {
    if (!client.connected()) {
        if (hostConnected) {
            Serial.println("⚠️ Mất kết nối với host! Dừng xử lý lệnh.");
            hostConnected = false;
        }
        delay(2000);
        connectToServer();
        return;
    }

    if (client.available()) {
        String command = client.readStringUntil('\n'); 
        command.trim(); 
        Serial.println("📥 Nhận lệnh từ host: " + command);

        if (command == "purple") {  
            placeObject1();
            objectPicked = false;
        } 
        else if (command == "green") {  
            placeObject2();
            objectPicked = false;
        } 
        else if (command == "orange") {  
            placeObject3();
            objectPicked = false;
        }

        client.println("OK");  // Phản hồi host rằng đã xử lý lệnh
    }

    if (!objectPicked && hostConnected) {
        delay(2000);
        pickObject3();
        objectPicked = true;
        client.println("Đã gắp từ vị trí 3");
    }

    delay(100);
}

void connectToServer() {
    while (!client.connected()) {
        Serial.println("🔄 Đang kết nối tới server " + String(host) + ":" + String(port));
        if (client.connect(host, port)) {
            Serial.println("✅ Đã kết nối tới server!");
            client.println("ESP32 đã kết nối.");
            hostConnected = true;
            Serial.println("⚙️ Hệ thống đã sẵn sàng. Đang chờ lệnh từ host...");
        } else {
            Serial.println("Kết nối thất bại. Thử lại sau 2 giây...");
            delay(2000);
        }
    }
}
// 🎯 Hàm gắp vật từ vị trí 3
void pickObject3() {
    Serial.println("🤖 Gắp vật từ vị trí 3...");
    for (int angle = 70; angle >= 0; angle -= 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 100; angle >= 40; angle -= 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 0; angle <= 70; angle += 2) {
        servo1.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 0; angle <= 70; angle += 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 40; angle <= 100; angle += 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }

    delay(500); // Chờ 0.5 giây sau khi gắp
}

// 📍 Đặt vật vào vị trí 1 (thùng tím)
void placeObject1() {
    Serial.println("📦 Đặt vật vào vị trí 1 (thùng tím)...");
    for (int angle = 90; angle <= 130; angle += 2) { // Thay đổi từ 90-130 thành 80-150
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 70; angle >= 0; angle -= 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 100; angle >= 40; angle -= 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 120; angle >= 0; angle -= 2) {
        servo1.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 40; angle <= 100; angle += 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 0; angle <= 70; angle += 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 130; angle >= 90; angle -= 2) { // Thay đổi từ 130-90 thành 150-80
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    delay(250); // Chờ 0.25 giây sau khi đặt
}

// 📍 Đặt vật vào vị trí 2 (thùng xanh)
void placeObject2() {
    Serial.println("📦 Đặt vật vào vị trí 2 (thùng xanh)...");
    for (int angle = 90; angle <= 130; angle += 2) { // Thay đổi từ 90-130 thành 80-150
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 70; angle >= 0; angle -= 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 100; angle >= 40; angle -= 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 120; angle >= 0; angle -= 2) {
        servo1.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 40; angle <= 60; angle += 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 0; angle <= 50; angle += 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 130; angle >= 90; angle -= 2) { // Thay đổi từ 130-90 thành 150-80
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    delay(250); // Chờ 0.25 giây sau khi đặt
}

// 📍 Đặt vật vào vị trí 3 (thùng cam)
void placeObject3() {
    Serial.println("📦 Đặt vật vào vị trí 3 (thùng cam)...");
    for (int angle = 90; angle <= 130; angle += 2) { // Thay đổi từ 90-130 thành 80-150
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 70; angle >= 0; angle -= 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 100; angle >= 40; angle -= 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 120; angle >= 0; angle -= 2) {
        servo1.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 40; angle <= 40; angle += 2) {
        servo3.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 0; angle <= 10; angle += 2) {
        servo2.write(angle);
        delay(50); // Delay 50ms
    }
    for (int angle = 130; angle >= 90; angle -= 2) { // Thay đổi từ 130-90 thành 150-80
        servo4.write(angle);
        delay(50); // Delay 50ms
    }
    delay(250); // Chờ 0.25 giây sau khi đặt
     delay(10000);
}