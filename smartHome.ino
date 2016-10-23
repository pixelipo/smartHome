#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <RTClib.h>

RTC_DS1307 RTC;

int pozycja,
    pos_1,
    pos_2,
    swiatlo_wys = 0,
    i = 0,
    czytaj = 85, // default middle position for servo
    pozycja_ost = 0;

LiquidCrystal lcd(12, 11, 5, 4, 10, 8);
Servo servomech;
Servo servomech2;

void setup() {
  Serial.begin(57600);
  Wire.begin();    
  lcd.begin(16, 2);
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));

  pinMode(1, OUTPUT);
  pinMode(0, INPUT);
  pinMode(13, OUTPUT);

  servomech.attach(9);
  servomech2.attach(6);
}

void loop() {
  if (Serial.available() > 0) {
    czytaj = Serial.read();
  }

  Wire.requestFrom(8, 14);
  servomech2.write(czytaj);

  String k = "";
  if(i==20) { // read/send data 20 times slower than delay (50ms)
    while (Wire.available()) { // slave may send less than requested
      char c = Wire.read(); // receive a byte as character
      k += c; // save all in a string
    }

    String Temp_1,Temp_2,Wilg_1,Wilg_2,Pote;

    pos_1=k.indexOf(',');
    Wilg_1=k.substring(0,pos_1);
    k=k.substring(pos_1+1);
    pos_1=k.indexOf(',');
    Temp_1=k.substring(0,pos_1);
    k=k.substring(pos_1+1);

    pos_1=k.indexOf(',');
    Wilg_2=k.substring(0,pos_1);
    k=k.substring(pos_1+1);
    pos_1=k.indexOf(',');
    Temp_2=k.substring(0,pos_1);
    k=k.substring(pos_1+1);

    pos_1=k.indexOf(',');
    Pote=k.substring(0);

    // send to Android app
    String tosend = Wilg_1 + Temp_1 + Pote;
    Serial.println(tosend);
    tosend = "";

    // print to LCD
    DateTime now = RTC.now();
    lcd.setCursor(0,0);
    lcd.print("T: " + Temp_1 + "C ");
    lcd.print(String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
    lcd.setCursor(0,1);
    lcd.print("Temp zad: " + Pote + "C");

    if(Temp_1 < Pote) {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }

    i = 0; //reset count
  }
  i++; // count loop()

  int swiatlo = analogRead(A0); // read photoresitor value
  pozycja = map(swiatlo,0,1023,5,175); // map photoresistor range to servo range
  if(abs(pozycja_ost - pozycja) > 14) {
    pozycja_ost = swiatlo;
    servomech.write(pozycja);
  }

  delay(50);
}

