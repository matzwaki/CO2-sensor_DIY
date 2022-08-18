// Title: Programm zum messen des Co2 Gehalts der Luft, um daraus den Airosolgehalt
//        bzw. die COV2 Ansteckungsgefahr abzuleiten.
// Autor: Matthias Wagner
// Version: 1.05 (12.03.2021)

// Main Features:
// Der Co2 Gehalt sowie die Temperatur und die rel. Luftfeuchtigkeit werden
// auf einem Display angezeigt.

// Die Anzeige hat 7 Modi:
// 1. Co2 Gehalt [ppm]
// 2. Temp. [°C]
// 3. rel. Luftfeuchte [%]
// 4. Modi 1-3 wechseln automatisch im 5sec Rytmus
// 5. Co2, Temp. und rel. Luftfeuchte untereinsnder als Übersicht
// 6. Historie von Co2 Gehalt der letzten 2 Stunden (soweit vorhanden)
// 7. Historie von Co2 Gehalt der letzten 10 Stunden (soweit vorhanden)
// Durch drücken des Tasters 1 (rechts) kann der Modus durchgeschalten werden.

// Kalibrierung: Zur Aktivierung der Kalibrierung des Co2 Sensors müssen Taster 1 und Taster 2 (links innen) gleichzeitig gedrückt werden.
// Die Kalibrierung wird erst 8 Stunden nach Aktivierung durchgeführt. 
// Kalibirierung muss in frischer und von Mensch/Tier/Pflanzen unbeeinflusster Luft durchführt werden. Wind bzw. Luftbewegungen können sich dabei negativ auswirken.
// Bei frischer Luft sollte der Wert (z.B. im Freien) bei ca. 400 ppm liegen.
// Ablaufbeispiel: Kalibirierung um 21 Uhr in einem unbewohnten und gut belüfteten Raum starten. Um 5 Uhr wird dann die Kalibrierung durchgeführt.

// Grün/Gelb/Rot Anzeige: Angelehnt an der Empfehlung durch das RKI (2020) leuchtet die grühne LED solange der Co2 Gehalt unter 1000 ppm liegt.
// Die Gelbe LED leuchtet sobald der Co2 Gehalt über 1000 ppm liegt und gelüftet werden sollte.
// Die Rote LED leuchtet sobald der Co2 Gehalte über 1500 ppm liegt und dringend gelüftet werden sollte.
// Ab einem Co2 Gehalt von über 2000 ppm blinkt die Rote LED 500/500 msec um die steigende Dringlichkeit zu unterstreichen.

// Hardware: NodeMCU ESP8266

// Pinbelegung:
// Taster 1 (rechts)         D33
// Taster 2 (links innen)    D32
// LED grün                  D18
// LED rot                   D19
// LED gelb                  D25
// LED Intern                D2
// Display                   I2C 3,3V, GRD, SCL, SDA - Adresse: 0x3C
// Co2 Sensor                I2C 3,3V, GRD, SCL, SDA - Adresse: 0x61

#include <SparkFun_SCD30_Arduino_Library.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Konstanten Definitions

const int LED_GRUEN = 18;
const int LED_ROT = 19;
const int LED_GELB = 25;
const int LED_INT = 2;
const int TASTE1 = 33;
const int TASTE2 = 32;

SCD30 airSensor;
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);

// Vareablen Definition

int ButtonState1 = 0;                   // Tastenstatus für Display Anzeigevariante
int ButtonState2 = 0;                   // Tastenstatus für die Aktivierung der Kalibrierung
int Co2 = 0;                            // Co2 Gehalt der Luft
float Temp = 0.0;                       // Lufttemperatur
float Feuchte = 0.0;                    // rel. Luftfeuchte 
unsigned long mess_timer = 0;           // Timer für für den Abstand der Messungen
unsigned long flash_timer = 0;          // Timer für Blinkende LEDs
unsigned long flip_timer = 0;           // Timer für Wechselinterval Anzeigewariante 4
unsigned long kalib_timer = 0;          // Timer für Kalibrierung
int kalib_aktiv = 0;                    // Status Kalibrierung aktiv
int display_anz_var = 1;                // Display Anzeigevariante
int flankenerkennung1 = 0;              // Zur Erkennung ob Taste 1 ihren zustand verändert
int flankenerkennung2 = 0;              // Zur Erkennung ob Taste 2 ihren zustand verändert   
int einzelwert_2 = 0;                   // Einzelwert für die Speicherung in der Co2 Historie (2 Stunden)
int einzelwert_10 = 0;                  // Einzelwert für die Speicherung in der Co2 Historie (10 Stunden)
int einzelwertzaehler_2 = 0;            // Zähler zur Ermittlung des Einzelwerts (Co2 Historie 2 Stunden)
int einzelwertzaehler_10 = 0;           // Zähler zur Ermittlung des Einzelwerts (Co2 Historie 10 Stunden)
int co2_historie_2[127];                // Array zur Darstellung der Co2 Historie (2 Stunden)
int co2_historie_10[127];               // Array zur Darstellung der Co2 Historie (10 Stunden)
int co2_hist_wert = 0;                  // Zum hochzählen der Werte im Array


void setup() // ***********************************************************************
{
  
  Serial.begin(115200);
  Serial.println("Corona-Co2 Lüftungswarner");
  Wire.begin();

  // Verbindung zum Airsensor wird geprüft
  if (airSensor.begin(Wire, false) == false)
  {
    Serial.println("Air Sensor wurde nicht erkannt. Bitte Kabel-Verbindung prüfen");
    while (1)
      ;
  }
  airSensor.setAutoSelfCalibration(false);      // Autokalibrierung deaktiviert
  airSensor.setMeasurementInterval(5);          // Messintervall auf 5 sec gesetzt
  airSensor.setAltitudeCompensation(340);       // Höhenparameter auf 340m über Mehreshöhe gesetzt (Jesingen)
  // airSensor.setAmbientPressure(835);         // Aktueller Umgebungsdruck in mBar: 700 - 1200 (einstellbar)
  // airSensor.setTemperatureOffset(0);         // Optional kann ein Themperatur Offset von bis zu 5°C gesetzt werden


  Serial.println("OLED SSD 1306 test");
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x64
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  Serial.println("OLED begun");
  display.display();
  delay(1000);
  display.setTextColor(SSD1306_WHITE);
  
  pinMode(LED_GRUEN, OUTPUT);
  pinMode(LED_ROT, OUTPUT);
  pinMode(LED_GELB, OUTPUT);
  pinMode(LED_INT, OUTPUT);
  pinMode(TASTE1, INPUT_PULLUP);
  pinMode(TASTE2, INPUT_PULLUP);
}

void loop() // ***********************************************************************
{

// Sensordaten abfragen und ausgeben (Serieler Monitor + Display) --------------------
  
  if (millis() > mess_timer + 1000) {
     if (airSensor.dataAvailable()) {
        Co2 = airSensor.getCO2();
        Temp = airSensor.getTemperature();
        Feuchte = airSensor.getHumidity();
        
        Serial.print("co2(ppm):");
        Serial.print(Co2);

        Serial.print(" temp(C):");
        Serial.print(Temp, 1);

        Serial.print(" humidity(%):");
        Serial.print(Feuchte, 1);

        Serial.println();

        einzelwert_ermittlung();

        switch (display_anz_var) {
          case 1:
            disp_anz_1();
            break;
          case 2:
            disp_anz_2();
            break;
          case 3:
            disp_anz_3();
            break;
          case 4:
            disp_anz_4();
            break;
          case 5:
            disp_anz_5();
            break;
          case 6:
            disp_anz_6();
            break;
            case 7:
            disp_anz_7();
            break;
          default:
            disp_anz_1();
        }
     }
     else {
        Serial.println("Waiting for new data");
     }
     mess_timer = millis();
  }

// LED Anzeige -----------------------------------------------------------------------

   if (Co2 < 2000) {
      if (Co2 < 1500) {
         if (Co2 < 1000) {
            digitalWrite(LED_ROT, LOW);
            digitalWrite(LED_GELB, LOW);
            digitalWrite(LED_GRUEN, HIGH);
         }
         else {
            digitalWrite(LED_ROT, LOW);
            digitalWrite(LED_GRUEN, LOW);
            digitalWrite(LED_GELB, HIGH);
         }
      }
      else {
         digitalWrite(LED_GRUEN, LOW);
         digitalWrite(LED_GELB, LOW);
         digitalWrite(LED_ROT, HIGH);
      }
   }
   else {
      digitalWrite(LED_GRUEN, LOW);
      digitalWrite(LED_GELB, LOW);
      led_R_blinkt();
   }   
   
// Display Anzeigevariante ------------------------------------------------------------

    ButtonState1 = digitalRead(TASTE1);
     if (ButtonState1 == LOW && flankenerkennung1 == 0) { // geschaltet
        display_anz_var += 1;
        if (display_anz_var > 7) {
           display_anz_var = 1;
        }
        switch (display_anz_var) {
          case 1:
            disp_anz_1();
            break;
          case 2:
            disp_anz_2();
            break;
          case 3:
            disp_anz_3();
            break;
          case 4:
            disp_anz_4();
            break;
          case 5:
            disp_anz_5();
            break;
          case 6:
            disp_anz_6();
            break;
            case 7:
            disp_anz_7();
            break;
          default:
            disp_anz_1();
        }
        flankenerkennung1 = 1; }
     else if (ButtonState1 == HIGH && flankenerkennung1 == 1) {
        flankenerkennung1 = 0;
        delay(50);
     }
     
// Kalibrierung -----------------------------------------------------------------------

    ButtonState2 = digitalRead(TASTE2);
    if (ButtonState1 == LOW && ButtonState2 == LOW && flankenerkennung2 == 0) { // beide geschaltet
       kalib_aktiv = 1;
       flankenerkennung2 = 1;
       kalib_timer = millis(); }
    else if (ButtonState1 == HIGH && ButtonState2 == HIGH && flankenerkennung2 == 1) {
        flankenerkennung2 = 0;
        delay(50);
    }
    if (kalib_aktiv == 1) {
       led_int_blinkt();
       kalibrieren();
    }


} // ENDE loop ************************************************************************


// Funktionen

void led_R_blinkt() { // ------------Rote LED Blinkt 500/500 msec ---------------------

  if (millis() > flash_timer + 100) {
     digitalWrite(LED_ROT, LOW);
  }
  if (millis() > flash_timer + 500) {
     digitalWrite(LED_ROT, HIGH);
     flash_timer = millis();
  }
}

void led_int_blinkt() { // ------------Interne LED Blinkt 500/500 msec ------------------

  if (millis() > flash_timer + 100) {
     digitalWrite(LED_INT, LOW);
  }
  if (millis() > flash_timer + 500) {
     digitalWrite(LED_INT, HIGH);
     flash_timer = millis();
  }
}

void kalibrieren() { // ------------ Funktion zur Kalibrierung des Co2 Sensors---------
  
  if (millis() > kalib_timer + 28800000 && kalib_aktiv == 1) {  // Kalibrierung startet nach 8 h
     airSensor.setForcedRecalibrationFactor(400);  // Frische Luft
     kalib_aktiv = 0;
  }   
}

void disp_anz_1() { // --------Display: Standard Anzeige -> Co2 in ppm -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print("CO2 in ppm");
    display.setTextSize(4);
    display.setCursor(20,30);
    display.print(Co2);
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.display();
}

void disp_anz_2() { // --------Display: Anzeige -> Temperatur in °C -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.print("Temperatur");
    display.setTextSize(3);
    display.setCursor(14,22);
    display.print(Temp, 1); display.print("\xF7" "C");
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.display();
}

void disp_anz_3() { // --------Display: Anzeige -> rel. Luftfeuchte in % -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.println("Rel.Luft-"); display.print("feuchte");
    display.setTextSize(3);
    display.setCursor(30,40);
    display.print(Feuchte, 0); display.println(" %");
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.display();
}

void disp_anz_4() { // --------Display: Anzeige -> Co2, Temp und Feuchte im Wechsel-----------------

    if (millis() > flip_timer + 12000) {
       flip_timer = millis();
    }
    if (millis() > flip_timer + 4000 && millis() < flip_timer + 7999) {
       disp_anz_2();
    }
    else if (millis() > flip_timer + 8000) {
       disp_anz_3();
    }
    else {
      disp_anz_1();
    }
}

void disp_anz_5() { // --------Display: Anzeige -> Co2, Temp. und Feuchte untereinander als Übersicht -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.print("CO2: ");
    display.print(Co2);
    display.println(" ppm");
    display.println();
    display.print("Temp.: ");
    display.print(Temp, 1);
    display.println(" " "\xF7" "C");
    display.println();
    display.print("Luftfeuchte.: ");
    display.print(Feuchte, 0);
    display.println(" %");
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.display();
}

void disp_anz_6() { // --------Display: Anzeige -> Co2 Historie der letzten 2 Stunden -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.println("Co2 Historie (2 h)");
    display.print("Akt. Wert: ");
    display.print(Co2);
    display.println(" ppm");
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.drawLine(127, 63, 125, 63, WHITE);
    display.drawLine(127, 53, 125, 53, WHITE);
    display.drawLine(127, 43, 125, 43, WHITE);
    display.drawLine(127, 33, 125, 33, WHITE);
    display.drawLine(127, 23, 125, 23, WHITE);
    for (int i=0; i <= 111; i++) {
      display.drawLine(121-i, 63, 121-i, 63 - co2_historie_2[i]/50, WHITE);
    }
    display.display();  
}

void disp_anz_7() { // --------Display: Anzeige -> Co2 Historie der letzten 10 Stunden -----------------
  
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.println("Co2 Historie (10 h)");
    display.print("Akt. Wert: ");
    display.print(Co2);
    display.println(" ppm");
    display.setCursor(0,57);
    display.setTextSize(1);
    display.print(display_anz_var);
    yield();
    display.drawLine(127, 63, 125, 63, WHITE);
    display.drawLine(127, 53, 125, 53, WHITE);
    display.drawLine(127, 43, 125, 43, WHITE);
    display.drawLine(127, 33, 125, 33, WHITE);
    display.drawLine(127, 23, 125, 23, WHITE);
    for (int i=0; i <= 111; i++) {
      display.drawLine(121-i, 63, 121-i, 63 - co2_historie_10[i]/50, WHITE);
    }
    display.display();  
}

void einzelwert_ermittlung() {

   einzelwert_2 += Co2;
   einzelwert_10 += Co2;
   einzelwertzaehler_2 += 1;
   einzelwertzaehler_10 += 1;
   if (einzelwertzaehler_2 == 12) {
      for (int i=0; i <= 110; i++) {
          co2_historie_2[111-i] = co2_historie_2[110-i];
      }
      co2_historie_2[0] = einzelwert_2/12;
      einzelwert_2 = 0;
      einzelwertzaehler_2 = 0;
   }
   if (einzelwertzaehler_10 == 60) {
      for (int i=0; i <= 110; i++) {
          co2_historie_10[111-i] = co2_historie_10[110-i];
      }
      co2_historie_10[0] = einzelwert_10/60;
      einzelwert_10 = 0;
      einzelwertzaehler_10 = 0;
   }
}
