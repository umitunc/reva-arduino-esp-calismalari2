# Reva - PlatformIO Microcontroller Project

Bu proje, **Arduino Uno**, **ESP32** ve **ESP8266** geliştirme kartları için ortak bir kod tabanı oluşturmak amacıyla yapılandırılmıştır.

## Proje Yapısı

- `platformio.ini`: PlatformIO yapılandırma dosyası (kartların tanımları, kütüphaneler ve hız ayarları buradadır).
- `src/main.cpp`: Projenin ana kod dosyası. Hangi kart derleniyorsa ona göre otomatik pin ve isim ayarlarını seçer.
- `include/`: Kendi başlık (.h) dosyalarınızı koyabileceğiniz klasör.
- `lib/`: Özel veya yerel kütüphanelerinizi yerleştirebileceğiniz klasör.

## Nasıl Kullanılır?

1. **Gereksinimler:**
   - VS Code kurulu olmalıdır.
   - VS Code üzerinde **PlatformIO IDE** eklentisi kurulmalıdır.

2. **Kart Seçimi ve Derleme:**
   - PlatformIO eklentisini kurduktan sonra VS Code'un sol alt köşesinde veya sol menüdeki PlatformIO simgesinde (Karınca kafası) ortam seçiciyi göreceksiniz.
   - Buradan çalışmak istediğiniz kart ortamını seçebilirsiniz:
     - `env:uno` (Arduino Uno)
     - `env:esp32` (ESP32 Dev Module)
     - `env:esp8266` (NodeMCU v2)

3. **Yükleme (Upload):**
   - Kartınızı USB ile bilgisayarınıza bağlayın.
   - Sol alt bardaki **PlatformIO: Upload** (Sağ ok `→` simgesi) butonuna basarak kodu karta yükleyebilirsiniz.
