// ================= PROSTY SYSTEM ALARMOWY =================
// Działa z Twoim układem

// ================= PINY LCD =================
const int RS = 7;
const int E = 6;
const int D4 = 8;
const int D5 = 9;
const int D6 = 10;
const int D7 = 11;

// ================= PINY PRZYCISKÓW =================
const int BTN_UP = A1;
const int BTN_DOWN = A0;
const int BTN_OK = A2;

// ================= PINY CZUJNIKÓW =================
const int PIR = 13;
const int TEMP = A3;

// ================= PINY ALARMU =================
const int BUZZER = 5;
const int LED_ALARM = 12;

// ================= PINY CD4511 =================
const int CD_A = 2;
const int CD_B = 3;
const int CD_C = A5;
const int CD_D = A4;

// ================= STANY =================
#define ROZBROJONY 0
#define UZBROJONY 1
#define ODLICZANIE 2
#define ALARM 3

// ================= ZMIENNE =================
int stan = ROZBROJONY;
int pozycjaMenu = 0;
unsigned long czasOdliczania = 0;
unsigned long ostatniDzwiek = 0;
unsigned long ostatniMig = 0;
bool miganie = false;
bool pirNaruszenie = false;
bool tempNaruszenie = false;
float temperatura = 0;
bool ostatniStanPIR = LOW;
unsigned long ostatniOdczytTemp = 0;
unsigned long ostatniPrzycisk = 0;
int ostatniaSekunda = -1;

// ================= FUNKCJE LCD =================
void lcdPulse() {
  digitalWrite(E, HIGH);
  delayMicroseconds(1);
  digitalWrite(E, LOW);
  delayMicroseconds(100);
}

void lcdWrite4(int v) {
  digitalWrite(D4, v & 1);
  digitalWrite(D5, (v>>1) & 1);
  digitalWrite(D6, (v>>2) & 1);
  digitalWrite(D7, (v>>3) & 1);
  lcdPulse();
}

void lcdCmd(int c) {
  digitalWrite(RS, LOW);
  lcdWrite4(c>>4);
  lcdWrite4(c & 0x0F);
  delay(2);
}

void lcdChar(char c) {
  digitalWrite(RS, HIGH);
  lcdWrite4(c>>4);
  lcdWrite4(c & 0x0F);
  delayMicroseconds(50);
}

void lcdPrint(const char* s) {
  while(*s) lcdChar(*s++);
}

void lcdClear() {
  lcdCmd(0x01);
  delay(2);
}

void lcdSetCursor(int col, int row) {
  lcdCmd(0x80 | (col + (row ? 0x40 : 0)));
}

void lcdInit() {
  pinMode(RS, OUTPUT); pinMode(E, OUTPUT);
  pinMode(D4, OUTPUT); pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT); pinMode(D7, OUTPUT);
  delay(50);
  lcdWrite4(0x03); delay(5);
  lcdWrite4(0x03); delay(5);
  lcdWrite4(0x03); delay(5);
  lcdWrite4(0x02); delay(5);
  lcdCmd(0x28);
  lcdCmd(0x0C);
  lcdCmd(0x01);
  delay(2);
  lcdCmd(0x06);
}

// ================= CD4511 =================
void pokazCyfre(int n) {
  digitalWrite(CD_A, n & 1);
  digitalWrite(CD_B, (n>>1) & 1);
  digitalWrite(CD_C, (n>>2) & 1);
  digitalWrite(CD_D, (n>>3) & 1);
}

// ================= CZUJNIKI =================
void sprawdzPIR() {
  bool aktualny = digitalRead(PIR);
  if (aktualny == HIGH && ostatniStanPIR == LOW) {
    pirNaruszenie = true;
  }
  ostatniStanPIR = aktualny;
}

void sprawdzTemperature() {
  if (millis() - ostatniOdczytTemp > 1000) {
    ostatniOdczytTemp = millis();
    int raw = analogRead(TEMP);
    float napiecie = raw * 5.0 / 1023.0;
    temperatura = (napiecie - 0.5) * 100.0;
    tempNaruszenie = (temperatura > 30.0);
  }
}

// ================= WYŚWIETLANIE =================
void wyswietlEkran() {
  if (stan == ALARM) {
    lcdClear();
    lcdSetCursor(0,0); lcdPrint("!!! ALARM !!!");
    lcdSetCursor(0,1);
    if (pirNaruszenie) lcdPrint("PIR: RUCH");
    else if (tempNaruszenie) lcdPrint("TEMP: WYSOKA");
  }
  else if (stan == UZBROJONY) {
    lcdClear();
    lcdSetCursor(0,0); lcdPrint("SYSTEM UZBROJ.");
    lcdSetCursor(0,1); lcdPrint("Monitoring...");
  }
  else if (stan == ROZBROJONY) {
    lcdClear();
    lcdSetCursor(0,0); lcdPrint("SYSTEM ROZBROJ.");
    lcdSetCursor(0,1); lcdPrint("MENU - OK");
  }
}

void wyswietlOdliczanie() {
  int pozostalo = 5 - (millis() - czasOdliczania) / 1000;
  if (pozostalo < 0) pozostalo = 0;
  
  if (pozostalo != ostatniaSekunda) {
    ostatniaSekunda = pozostalo;
    lcdClear();
    lcdSetCursor(0,0); lcdPrint("Uzbrajanie...");
    lcdSetCursor(0,1); lcdPrint("Czas: ");
    char buf[3]; itoa(pozostalo, buf, 10);
    lcdPrint(buf);
  }
}

void wyswietlMenu() {
  lcdClear();
  lcdSetCursor(0,0); lcdPrint(">");
  lcdSetCursor(1,0);
  if (pozycjaMenu == 0) lcdPrint("Uzbroj");
  else if (pozycjaMenu == 1) lcdPrint("Rozbroj");
  else if (pozycjaMenu == 2) lcdPrint("Czujniki");
  
  lcdSetCursor(0,1); lcdPrint(" ");
  lcdSetCursor(1,1);
  if (pozycjaMenu == 0) lcdPrint("Rozbroj");
  else if (pozycjaMenu == 1) lcdPrint("Czujniki");
  else if (pozycjaMenu == 2) lcdPrint("Uzbroj");
}

void wyswietlCzujniki() {
  lcdClear();
  lcdSetCursor(0,0); lcdPrint("PIR: ");
  lcdPrint(pirNaruszenie ? "NARUSZ" : "OK");
  lcdSetCursor(0,1); lcdPrint("TEMP: ");
  char buf[10];
  dtostrf(temperatura, 4, 1, buf);
  lcdPrint(buf);
  lcdPrint(" C");
  if (tempNaruszenie) lcdPrint(" NARUSZ");
}

// ================= PRZYCISKI =================
int czytajPrzycisk() {
  if (digitalRead(BTN_UP) == LOW) return 1;
  if (digitalRead(BTN_DOWN) == LOW) return 2;
  if (digitalRead(BTN_OK) == LOW) return 3;
  return 0;
}

// ================= RESET CZUJNIKÓW =================
void resetCzujnikow() {
  pirNaruszenie = false;
  tempNaruszenie = false;
}

// ================= STEROWANIE DIODĄ =================
void sterujDioda() {
  if (stan == UZBROJONY) {
    // UZBROJONY - dioda ŚWIECI ciągle
    digitalWrite(LED_ALARM, HIGH);
  }
  else if (stan == ALARM) {
    // ALARM - dioda MIGA
    if (millis() - ostatniMig > 200) {
      ostatniMig = millis();
      miganie = !miganie;
      digitalWrite(LED_ALARM, miganie);
    }
  }
  else {
    // ROZBROJONY lub ODLICZANIE - dioda ZGASZONA
    digitalWrite(LED_ALARM, LOW);
  }
}

// ================= SETUP =================
void setup() {
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(PIR, INPUT);
  pinMode(TEMP, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_ALARM, OUTPUT);
  pinMode(CD_A, OUTPUT);
  pinMode(CD_B, OUTPUT);
  pinMode(CD_C, OUTPUT);
  pinMode(CD_D, OUTPUT);
  
  lcdInit();
  lcdClear();
  lcdPrint("SYSTEM ALARMOWY");
  lcdSetCursor(0,1);
  lcdPrint("START...");
  delay(3000);
  wyswietlMenu();
}

// ================= LOOP =================
void loop() {
  sprawdzPIR();
  sprawdzTemperature();
  
  // CD4511
  if (stan == ALARM) pokazCyfre(3);
  else if (stan == ODLICZANIE) pokazCyfre(2);
  else if (stan == UZBROJONY) pokazCyfre(1);
  else if (stan == ROZBROJONY) {
    if (pirNaruszenie || tempNaruszenie) pokazCyfre(4);
    else pokazCyfre(0);
  }
  
  // Sterowanie diodą LED
  sterujDioda();
  
  // Dźwięk alarmu
  if (stan == ALARM) {
    if (millis() - ostatniDzwiek > 200) {
      ostatniDzwiek = millis();
      if (miganie) tone(BUZZER, 2000);
      else noTone(BUZZER);
    }
  } else {
    noTone(BUZZER);
  }
  
  // Alarm tylko gdy uzbrojony i naruszony
  if (stan == UZBROJONY && (pirNaruszenie || tempNaruszenie)) {
    stan = ALARM;
    wyswietlEkran();
  }
  
  // Odliczanie
  if (stan == ODLICZANIE) {
    wyswietlOdliczanie();
    
    if (millis() - czasOdliczania > 5000) {
      stan = UZBROJONY;
      ostatniaSekunda = -1;
      wyswietlEkran();
    }
  }
  
  // Obsługa przycisków
  int przycisk = czytajPrzycisk();
  
  if (przycisk != 0 && millis() - ostatniPrzycisk > 200) {
    ostatniPrzycisk = millis();
    
    // Rozbrojenie podczas alarmu
    if (stan == ALARM && przycisk == 3) {
      stan = ROZBROJONY;
      resetCzujnikow();
      wyswietlMenu();
    }
    // Anulowanie odliczania
    else if (stan == ODLICZANIE && przycisk == 3) {
      stan = ROZBROJONY;
      ostatniaSekunda = -1;
      wyswietlMenu();
    }
    // Menu (tylko gdy rozbrojony)
    else if (stan == ROZBROJONY) {
      if (przycisk == 1) {
        pozycjaMenu = (pozycjaMenu - 1 + 3) % 3;
        wyswietlMenu();
      }
      else if (przycisk == 2) {
        pozycjaMenu = (pozycjaMenu + 1) % 3;
        wyswietlMenu();
      }
      else if (przycisk == 3) {
        if (pozycjaMenu == 0) {
          resetCzujnikow();
          stan = ODLICZANIE;
          czasOdliczania = millis();
          ostatniaSekunda = -1;
          wyswietlOdliczanie();
        }
        else if (pozycjaMenu == 1) {
          stan = ROZBROJONY;
          resetCzujnikow();
          wyswietlMenu();
        }
        else if (pozycjaMenu == 2) {
          wyswietlCzujniki();
          delay(3000);
          wyswietlMenu();
        }
      }
    }
  }
  
  delay(20);
}