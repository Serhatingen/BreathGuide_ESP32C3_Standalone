# BreathGuide ESP32-C3 Standalone

ESP32-C3 Super Mini tabanlı, tek butonla çalışan bağımsız nefes rehberi cihazı.

## Donanım
| Parça | Detay |
|---|---|
| MCU | ESP32-C3 Super Mini |
| LED | 10 × WS2812 |
| Ekran | 0.91" I2C 128×32 OLED (SSD1306, 0x3C) |
| Buton | 1 × buton |
| Ses | 1 × pasif piezo / buzzer |

## Varsayılan pinler
| Sinyal | GPIO |
|---|---|
| WS2812 DATA | 7 |
| BUTTON | 4 |
| BUZZER | 3 |
| OLED SDA | 5 |
| OLED SCL | 6 |

> C3 Super Mini klonlarında pin eşleşmeleri değişebilir. `Config.h` içindeki sabitleri kart üzerindeki silk-screen ile doğrula.

## Temel akış
1. Ayarlanan aralık dolunca intro LED animasyonu çalar ve minik uyarı sesi verir.
2. OLED üzerinde `Seans basliyor` mesajı çıkar.
3. Komik göz animasyonu ile kullanıcı beklenir.
4. **Kısa basış** → nefes seansı başlar.
5. **Uzun basış** → AP açılır; OLED'de SSID / IP / şifre görünür.
6. Tarayıcıdan ayar sayfasına (captive portal) bağlanılır.

## Proje yapısı

```
BreathGuide_ESP32C3_Standalone/
├── BreathGuide_ESP32C3_Standalone.ino  ← sadece setup() ve loop()
├── Config.h          ← pin tanımları, sabitler, enum'lar, Settings struct
├── Utils.h / .cpp    ← clampValue<T>, easeInOutSine, shortChipId
├── AppState.h / .cpp ← tüm global nesneler ve değişkenler (tek tanım noktası)
├── Audio.h / .cpp    ← buzzer / tone yönetimi
├── Display.h / .cpp  ← OLED ekran fonksiyonları
├── LedControl.h /.cpp← WS2812 LED animasyonları
├── SettingsMgr.h/.cpp← NVS (Preferences) yükleme / kaydetme + hatırlatma zamanlayıcısı
├── Session.h / .cpp  ← nefes seansı yaşam döngüsü
├── Portal.h / .cpp   ← WiFi AP + captive-portal web sunucusu
└── Button.h / .cpp   ← buton debounce, kısa / uzun basış
```

### Bağımlılık ağacı (döngüsel bağımlılık yok)
```
Config.h
  └─ Utils.h/cpp
  └─ AppState.h/cpp  (Config + tüm kütüphaneler)
       └─ Audio
       └─ Display
       └─ LedControl  (+ Utils)
       └─ SettingsMgr (+ Utils)
            └─ Session  (Audio + Display + LedControl + SettingsMgr)
                 └─ Portal  (+ Utils + Session)
                      └─ Button  (Session + Portal)
                           └─ .ino
```

## Kullanılan kütüphaneler
- WiFi, WebServer, DNSServer *(ESP32 Arduino core)*
- Preferences *(ESP32 Arduino core)*
- Wire *(ESP32 Arduino core)*
- Adafruit_GFX
- Adafruit_SSD1306
- FastLED

## Portal davranışı
| Özellik | Değer |
|---|---|
| AP adı | `BreathGuide-XXXX` |
| Varsayılan şifre | `nefes123` |
| IP | 192.168.4.1 |
| Timeout | 600 sn (ayarlanabilir) |

Web arayüzünden ayarlanabilenler:
- Hatırlatma aralığı, tur sayısı
- Alış / tutuş / veriş süreleri
- LED parlaklığı, ses seviyesi
- Intro animasyon süresi
- Portal otomatik kapanma süresi
- Ses açık / kapalı
- Otomatik hatırlatma açık / kapalı

## Dikkat
ESP32-C3 + WS2812 + Wi-Fi/AP aynı anda bazı kurulumlarda flicker yapabilir.
Bu firmware bu yüzden portal açıkken ağır LED animasyonlarını kapatır.

## Sonraki mantıklı geliştirmeler
- Batarya besleme ve low-battery ikonu
- NTP veya RTC ile saat bazlı hatırlatma pencereleri
- Gece modu / sessiz mod
- Farklı nefes presetleri (4-7-8, kutu nefesi, vb.)
- Web arayüzünden firmware bilgisi ve OTA
