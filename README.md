Messen des Co2 Gehalts der Luft, um daraus den Airosolgehalt
bzw. die COV2 Ansteckungsgefahr abzuleiten.


Main Features:
Der Co2 Gehalt sowie die Temperatur und die rel. Luftfeuchtigkeit werden
auf einem Display angezeigt.

Die Anzeige hat 7 Modi:
1. Co2 Gehalt [ppm]
2. Temp. [°C]
3. rel. Luftfeuchte [%]
4. Modi 1-3 wechseln automatisch im 5sec Rytmus
5. Co2, Temp. und rel. Luftfeuchte untereinsnder als Übersicht
6. Historie von Co2 Gehalt der letzten 2 Stunden (soweit vorhanden)
7. Historie von Co2 Gehalt der letzten 10 Stunden (soweit vorhanden)
Durch drücken des Tasters 1 (rechts) kann der Modus durchgeschalten werden.

Kalibrierung: Zur Aktivierung der Kalibrierung des Co2 Sensors müssen Taster 1 und Taster 2 (links innen) gleichzeitig gedrückt werden.
Die Kalibrierung wird erst 8 Stunden nach Aktivierung durchgeführt. 
Kalibirierung muss in frischer und von Mensch/Tier/Pflanzen unbeeinflusster Luft durchführt werden. Wind bzw. Luftbewegungen können sich dabei negativ auswirken.
Bei frischer Luft sollte der Wert (z.B. im Freien) bei ca. 400 ppm liegen.
Ablaufbeispiel: Kalibirierung um 21 Uhr in einem unbewohnten und gut belüfteten Raum starten. Um 5 Uhr wird dann die Kalibrierung durchgeführt.

Grün/Gelb/Rot Anzeige: Angelehnt an der Empfehlung durch das RKI (2020) leuchtet die grühne LED solange der Co2 Gehalt unter 1000 ppm liegt.
Die Gelbe LED leuchtet sobald der Co2 Gehalt über 1000 ppm liegt und gelüftet werden sollte.
Die Rote LED leuchtet sobald der Co2 Gehalte über 1500 ppm liegt und dringend gelüftet werden sollte.
Ab einem Co2 Gehalt von über 2000 ppm blinkt die Rote LED 500/500 msec um die steigende Dringlichkeit zu unterstreichen.

Hardware: 
Microcontroller   NodeMCU ESP8266
Display           adafruit SSD1306
CO2 Sensor        Sensirion SCD30

Pinbelegung NodeMCU ESP8266:
Taster 1 (rechts)         D33
Taster 2 (links innen)    D32
LED grün                  D18
LED rot                   D19
LED gelb                  D25
LED Intern                D2
Display                   I2C 3,3V, GRD, SCL, SDA - Adresse: 0x3C
Co2 Sensor                I2C 3,3V, GRD, SCL, SDA - Adresse: 0x61
# CO2-sensor_DIY
