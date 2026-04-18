# Digital Clock with Temperature Monitoring & Dual Display

Proyek ini adalah implementasi sistem jam digital berbasis mikrokontroler yang dirancang untuk akurasi tinggi dan fungsionalitas ganda (Waktu & Suhu). Proyek ini mengintegrasikan modul RTC untuk ketepatan waktu dan sensor analog untuk pemantauan lingkungan.

## 🚀 Fitur Utama
- **Real-Time Accuracy:** Menggunakan modul RTC DS3231 dengan presisi tinggi.
- **Dual Display System:** Output waktu ditampilkan melalui Seven Segment (High Visibility) dan informasi detail melalui LCD.
- **Ambient Temperature Sensing:** Monitoring suhu ruangan secara real-time menggunakan sensor LM35.
- **Interactive Time Adjustment:** Pengaturan jam dan menit secara manual melalui dua push button.
- **Alarm/Indicator:** Dilengkapi dengan Buzzer dan LED sebagai indikator visual dan audio.

## 🛠️ Komponen Elektronika
| Komponen | Deskripsi |
| --- | --- |
| **Microcontroller** | Arduino / ESP32 (Sesuaikan dengan yang Anda gunakan) |
| **RTC DS3231** | Real Time Clock Module (I2C Interface) |
| **Sensor LM35** | Analog Temperature Sensor |
| **LCD Display** | 16x2 Character Display (I2C/Parallel) |
| **7-Segment** | 4-Digit Display untuk Jam & Menit |
| **Push Buttons** | 2x Tactile Switch (Menu & Adjust) |
| **Buzzer & LED** | Indikator Alarm/Detik |

## 📐 Skema Rangkaian
*(Opsional: Anda bisa menambahkan gambar skema rangkaian di sini)*


## 💻 Instalasi & Penggunaan
1. Clone repositori ini:
   ```bash
   git clone [https://github.com/username-anda/digital-clock-project.git](https://github.com/username-anda/digital-clock-project.git)
