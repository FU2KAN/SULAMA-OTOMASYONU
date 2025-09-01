#define BLYNK_TEMPLATE_ID "TMPL6-yp09HNu"
#define BLYNK_TEMPLATE_NAME "Sulama Otomasyonu"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

char auth[] = "Blynk Tokeniniz";     // Blynk token yada website bağlantı yolu
char ssid[] = "Wifi Adınız";         // WiFi adı
char pass[] = "Wifi Şifreniz";       // WiFi şifresi

#define RELAY_PIN D1   // Röle pini

WidgetRTC rtc;

bool motorZamanlayiciCalisti = false;
unsigned long motorAcikKalma = 0;
const unsigned long motorSure = 60000; // 1 dakika

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Röle pasif (motor kapalı)

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  rtc.begin();
  setSyncInterval(10*60); // 10 dakikada bir saat güncellenir
}

// Blynk switch için sanal pin V1
BLYNK_WRITE(V1) {
  int value = param.asInt();
  if(value) {
    digitalWrite(RELAY_PIN, LOW); // Motor aç
    Serial.println("Motor açıldı (Blynk switch)");
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Motor kapat
    Serial.println("Motor kapandı (Blynk switch)");
  }
}

void loop() {
  Blynk.run();

  int h = hour();
  int m = minute();

  // Motoru sadece bir kez 19:05’te aç
  if(h == 19 && m == 11 && !motorZamanlayiciCalisti){
    Serial.println("Motor açıldı (19:05)");
    digitalWrite(RELAY_PIN, LOW); // Röle aktif
    motorAcikKalma = millis();
    motorZamanlayiciCalisti = true;
  }

  // Motor açık kalma süresini kontrol et
  if(!digitalRead(RELAY_PIN) && motorZamanlayiciCalisti){
    if(millis() - motorAcikKalma >= motorSure){
      digitalWrite(RELAY_PIN, HIGH); // Motor kapandı
      Serial.println("Motor kapandı (1 dk sonra)");
    }
  }
}
