# Sulama-Otomasyonu
Seyahat ederken bitkileri sulama sorununu çözen, ESP8266 tabanlı bir IoT Projesi


Seyahate çıktığımde yada çalışırken  balkonumda yetiştidiğim bitkilerim soluyordu bu sorunu çözmek için Otomatik bir sulama sistemi geliştirdim

## Proje Bileşenleri

* **Beyin:** ESP8266 (NodeMCU)
* **Kas:** 5V Tek Kanallı Röle
* **Kalp:** 2 adet 5V Dalgıç Motor
* **Enerji:** 2 adet 5V Adaptör
* **Damar Yolu:** 6mm şeffaf hortum, damlatma uçları ve damacana su depom

* Projede Karşılaştığım Zorluklar ve Çözümlerim

Bu proje bana şunu öğretti: sadece devre kurmak ya da kod yazmak değil, beklenmedik sorunları pratik çözümlerle aşmak da işin yarısıymış. İşte benim başıma gelen iki sıkıntı ve onları nasıl çözdüğüm:

1. Motorların Debisi Suyu Yukarıya Taşımadı

*İlk denemede motorları damacananın altına koydum, suyu yukarıya basacaklarını düşündüm. Ama 5V’luk motorların debisi yetmedi
*Çözüm: Motorlara yüklenmek yerine damacanayı rafların en üstüne koyup yer çekimini kullandım.

2. Sifonlama Sorunu

*Damacana yukarıya çıkınca bu sefer de motorlar zaman ayarı olarak çalıştıktan sonra su akmaya devam ediyordu motor kapnadıktan sonra sifonlama oluyordu
*Çözüm: Hortumun damacananın içindeki kısmına  (1 mm) bir delik açtım. Bu sayede hortuma hava girdi ve akış anında kesildi. Ufak bir delik, problemi çözdü.

Sonuç

ESP8266 ile kurduğum sistem şu an kendi kendine çalışıyor. Her gün akşam 19:11’de otomatik olarak 1 dakika sulama yapıyor. İstersem Blynk uygulamasından tek tuşla uzaktan sulamayı da başlatabiliyorum.

Kısacası artık nerede olursam olayım bitkilerim susuz kalmıyor. 


*Bu projeyi yapmak istersen, en önemli kısım bağlantı şeması. Tüm parçaları nasıl bir araya getirdiğimi görmek için aşağıya göz atabilirsin. İki ayrı güç kaynağı kullandım, biri beyin (ESP8266) diğeri kaslar (motorlar) için.



## Proje Fotoğrafları ve Devre Şeması

Sistemin kurulumunu ve görünümünü merak edenler için birkaç kare:
<img width="1388" height="805" alt="image" src="https://github.com/user-attachments/assets/cf7a8086-3d19-4dff-b83a-98f61f188d51" />

![Sulama Sistemimin Genel Görünümü]
![IMG-20250831-WA0011](https://github.com/user-attachments/assets/8ef2702e-b2ee-4eda-8f49-8db7529330df)


![Sistemin Beyni: ESP8266 ve Röle Kutusu](https://i.imgur.com/your-IMG-20250831-WA0008.jpg)
![IMG-20250831-WA0008](https://github.com/user-attachments/assets/8b969894-579d-4796-adcf-e2448ccb551a)


![Damacana ve Motor Bağlantısı]
![IMG-20250831-WA0009](https://github.com/user-attachments/assets/d475d909-e53b-489b-ba7d-7c6d60cac38a)
![IMG-20250831-WA0007](https://github.com/user-attachments/assets/4917067d-f216-4326-b4ea-5b0c706179f8)


![Blynk Uygulaması ile Aç/Kapat Butonu]
![IMG-20250831-WA0006](https://github.com/user-attachments/assets/dd3d934e-4029-41f3-b719-abb57f1ceba2)


## Yazılım (Kod)

Sistemimin beyni olan ESP8266, bu kod sayesinde çalışıyor. Kod, akşamları otomatik sulama yapıyor ve istediğim zaman telefondan açıp kapatmama olanak tanıyor.

```cpp
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_BURAYA"
#define BLYNK_TEMPLATE_NAME "TEMPLATE_ADI_BURAYA"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

char auth[] = "BLYNK_TOKEN_BURAYA";
char ssid[] = "WIFI_ADINIZ";
char pass[] = "WIFI_SIFRENIZ";

#define RELAY_PIN D1

WidgetRTC rtc;

bool motorZamanlayiciCalisti = false;
unsigned long motorAcikKalma = 0;
const unsigned long motorSure = 60000; // 1 dakika

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); 

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  rtc.begin();
  setSyncInterval(10*60); 
}

BLYNK_WRITE(V1) {
  int value = param.asInt();
  if(value) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Motor açıldı (Blynk switch)");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Motor kapandı (Blynk switch)");
  }
}

void loop() {
  Blynk.run();

  int h = hour();
  int m = minute();

  if(h == 19 && m == 11 && !motorZamanlayiciCalisti){
    Serial.println("Motor açıldı (19:11)");
    digitalWrite(RELAY_PIN, LOW);
    motorAcikKalma = millis();
    motorZamanlayiciCalisti = true;
  }

  if(!digitalRead(RELAY_PIN) && motorZamanlayiciCalisti){
    if(millis() - motorAcikKalma >= motorSure){
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("Motor kapandı (1 dk sonra)");
      motorZamanlayiciCalisti = false;
    }
  }
}
