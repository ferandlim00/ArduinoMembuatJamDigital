// --- LIBRARY ---
#include <Wire.h>
#include <TM1637Display.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

// --- PIN DEFINISI ---
#define CLK 2
#define DIO 3
#define BTN_MODE 4
#define BTN_UP 5
#define BTN_DOWN 6
#define BTN_ALARM 7
#define BUZZER 8
#define LM35_PIN A0

// --- OBJEK ---
TM1637Display display(CLK, DIO);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- VARIABEL WAKTU ---
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

// --- VARIABEL LOGIKA ---
unsigned long buttonPressStart = 0;
bool modePressed = false;
bool settingMode = false;
bool alarmSetMode = false;
byte editStep = 0;  
bool modeLongPressTriggered = false;

unsigned long blinkTimer = 0;
bool blinkState = true;

int setHour, setMinute, setDay, setMonth, setYear;

// --- ALARM ---
int alarmHour = 6;
int alarmMinute = 30;
bool alarmActive = false;
bool alarmRinging = false;
unsigned long alarmStart = 0;

// --- SUHU ---
unsigned long tempDisplayTimer = 0;
float cachedTemp = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  display.setBrightness(7);
  lcd.init();
  lcd.backlight();

  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_ALARM, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  alarmHour = EEPROM.read(0);
  alarmMinute = EEPROM.read(1);
  alarmActive = EEPROM.read(2);
}

void loop() {
  if (!settingMode && !alarmSetMode) {
    readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  }

  handleButtons();

  unsigned long now = millis();
  if (now - blinkTimer >= 500) {
    blinkTimer = now;
    blinkState = !blinkState;
  }

  if (alarmSetMode || (settingMode && editStep < 2)) {
    uint8_t segments[4];
    int showHour = settingMode ? setHour : alarmHour;
    int showMinute = settingMode ? setMinute : alarmMinute;

    if (blinkState) {
      display.showNumberDecEx(showHour * 100 + showMinute, 0b01000000, true);
    } else {
      if (editStep == 0) {
        segments[0] = segments[1] = 0x00;
        segments[2] = display.encodeDigit(showMinute / 10);
        segments[3] = display.encodeDigit(showMinute % 10);
      } else if (editStep == 1) {
        segments[0] = display.encodeDigit(showHour / 10);
        segments[1] = display.encodeDigit(showHour % 10);
        segments[2] = segments[3] = 0x00;
      }
      segments[1] |= 0x80;
      display.setSegments(segments);
    }
  } else {
    display.showNumberDecEx(hour * 100 + minute, 0b01000000, true);
  }

  if (millis() - tempDisplayTimer > 1000) {
    cachedTemp = analogRead(LM35_PIN) * 5.0 / 1023.0 * 100;
    tempDisplayTimer = millis();
  }

  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(cachedTemp);
  lcd.write(byte(223));
  lcd.print("C   ");

  lcd.setCursor(0, 1);
  if (settingMode) {
    lcd.print("Set: ");
    lcd.print(setDay < 10 ? "0" : ""); lcd.print(setDay); lcd.print("/");
    lcd.print(setMonth < 10 ? "0" : ""); lcd.print(setMonth); lcd.print("/");
    lcd.print("20");
    lcd.print(setYear < 10 ? "0" : ""); lcd.print(setYear);
  } else if (alarmSetMode) {
    lcd.print("Alarm: ");
    if (alarmHour < 10) lcd.print("0");
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMinute < 10) lcd.print("0");
    lcd.print(alarmMinute);
    lcd.print(alarmActive ? " On " : " Off");
  } else {
    lcd.print("Tgl: ");
    if (dayOfMonth < 10) lcd.print("0");
    lcd.print(dayOfMonth);
    lcd.print("/");
    if (month < 10) lcd.print("0");
    lcd.print(month);
    lcd.print("/20");
    if (year < 10) lcd.print("0");
    lcd.print(year);
  }

  if (hour == alarmHour && minute == alarmMinute && second == 0 && alarmActive && !alarmRinging) {
    tone(BUZZER, 1000);
    alarmRinging = true;
    alarmStart = millis();
  }

  if (alarmRinging && millis() - alarmStart >= 20000) {
    noTone(BUZZER);
    alarmRinging = false;
  }

  delay(100);
}

void handleButtons() {
  static bool lastModeState = HIGH;
  bool currentModeState = digitalRead(BTN_MODE);

  if (currentModeState == LOW && lastModeState == HIGH) {
    buttonPressStart = millis();
    modePressed = true;
    modeLongPressTriggered = false;
  }

  if (modePressed && currentModeState == LOW && !settingMode && !alarmSetMode) {
    if ((millis() - buttonPressStart > 1000) && !modeLongPressTriggered) {
      settingMode = true;
      setHour = hour;
      setMinute = minute;
      setDay = dayOfMonth;
      setMonth = month;
      setYear = year;
      editStep = 0;
      modeLongPressTriggered = true;
    }
  }

  if (currentModeState == HIGH && lastModeState == LOW) {
    modePressed = false;
  }

  if (settingMode) {
    if (digitalRead(BTN_UP) == LOW) {
      if (editStep == 0) setHour = (setHour + 1) % 24;
      else if (editStep == 1) setMinute = (setMinute + 1) % 60;
      else if (editStep == 2) setDay = constrain(setDay + 1, 1, 31);
      else if (editStep == 3) setMonth = constrain(setMonth + 1, 1, 12);
      else if (editStep == 4) setYear = (setYear + 1) % 100;
      delay(200);
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      if (editStep == 0) setHour = (setHour + 23) % 24;
      else if (editStep == 1) setMinute = (setMinute + 59) % 60;
      else if (editStep == 2) setDay = constrain(setDay - 1, 1, 31);
      else if (editStep == 3) setMonth = constrain(setMonth - 1, 1, 12);
      else if (editStep == 4) setYear = (setYear + 99) % 100;
      delay(200);
    }
    if (digitalRead(BTN_MODE) == LOW && lastModeState == HIGH) {
      editStep = (editStep + 1) % 5;
      delay(300);
    }
    if (digitalRead(BTN_ALARM) == LOW) {
      setDS3231time(0, setMinute, setHour, dayOfWeek, setDay, setMonth, setYear);
      settingMode = false;
      delay(300);
    }
  } else if (digitalRead(BTN_ALARM) == LOW && !alarmSetMode) {
    alarmSetMode = true;
    editStep = 0;
    delay(500);
  } else if (alarmSetMode) {
    if (digitalRead(BTN_UP) == LOW) {
      if (editStep == 0) alarmHour = (alarmHour + 1) % 24;
      else if (editStep == 1) alarmMinute = (alarmMinute + 1) % 60;
      delay(200);
    }
    if (digitalRead(BTN_DOWN) == LOW) {
      if (editStep == 0) alarmHour = (alarmHour + 23) % 24;
      else if (editStep == 1) alarmMinute = (alarmMinute + 59) % 60;
      delay(200);
    }
    if (digitalRead(BTN_MODE) == LOW && lastModeState == HIGH) {
      editStep = (editStep + 1) % 2;
      delay(300);
    }
    if (digitalRead(BTN_ALARM) == LOW) {
      alarmActive = !alarmActive;
      if (alarmActive) {
        EEPROM.write(0, alarmHour);
        EEPROM.write(1, alarmMinute);
      }
      EEPROM.write(2, alarmActive);
      alarmSetMode = false;
      editStep = 0;
      delay(500);
    }
  }

  lastModeState = currentModeState;
}

byte decToBcd(byte val) {
  return (val / 10 * 16 + val % 10);
}

byte bcdToDec(byte val) {
  return (val / 16 * 10 + val % 16);
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(0x68);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7);
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  Wire.beginTransmission(0x68);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}
