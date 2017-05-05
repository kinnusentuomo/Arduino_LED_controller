//Tuomo Kinnunen 2016 / Kello / Etäisyysanturi / Ledinauha
//
//
// DS1302:  RST pin    -> Arduino Digital 3
//          DATA pin   -> Arduino Digital 4
//          CLK pin    -> Arduino Digital 5
//
//------------------------------------------------------------
//
// ASETUKSET:

//etäisyys, jonka yli ledit palavat tasaisen himmeästi
  int ledDistance = 11;
  
//Pisin mahdollinen etäisyys, jolloin ledit vilkuttavat
  int longestDistance = 100;
  
//Välys, jolla ledit kirkastuvat/himmenevät
  int ledDelay = 25;
  
// Kuinka monta pistettä LED-nauha himmenee/kirkastuu kerralla
  int fadeAmount = 5;    

//Kellonaika, jolloin ledinauha alkaa palaa muodossa HH 
  int ledStartTime = 10;

//Kellonaika, jolloin ledinauha sammuu muodossa HH
  int ledStopTime = 22;
  
//
//------------------------------------------------------------
#include <DS1302.h>                //Kellosirun oma kirjasto
#include <Wire.h>                  //LCD-näytön tarvitsema kirjasto
#include <LiquidCrystal_I2C.h>     //LCD-näytön kirjasto
            
//LCD-näytön alustaminen
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Asettaa LCD I2C -osoitteen ja määrittelee lcd-näytön

//Alustaa DS1302 pinnit (RST, DATA, CLK)
DS1302 rtc(3, 4, 5);

//Alustaa kellosirun RAM-muistin käyttöön puskuriksi
DS1302_RAM ramBuffer;

//Tarvittavat muuttujat
//kesto ja etäisyys etäisyysanturille
long duration, distance;
//Muuttuja tunteja varten
int hours = 00;

//etäisyysanturin datapinnit
#define trigPin 13
#define echoPin 12

//ledille tarkoitettu PWM pinni
#define led 11
// LED:in kirkkaus
int brightness = 0;    

void setup()
{
  //Alustaa sarjamonitorin käyttöön halutulla nopeudelle BDS
  Serial.begin(19200);
  Serial.println("DS1302 Testiohjelma - Tuomo Kinnunen 2016");

  //ledi ulostuloksi
  pinMode(led, OUTPUT);

  //Etäisyysanturin pinnit ulostuloksi
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //LCD-asetukset
  setupLCD();
  //Aika-asetukset
  setupClock();
}

void loop()
{
  printTime();
  saveHours();

//Alla oleva if else rakenne tarkistaa kellonajan
  if(hours >= ledStartTime && hours <= ledStopTime)
  {
    //Kun tunnit on enemmän tai yhtä paljon kuin 10, mutta vähemmän kuin 22 alla oleva koodi toteutuu
    checkDistance();
    setLedMode();
    lcd.backlight();
  }

  else
  {
    //Yöllä alla oleva koodi toteutuu
    analogWrite(led, 0);
    lcd.noBacklight();
  }
}

//Funktio ajan näyttämistä varten LCD-näytöllä ja Sarjamonitorissa
void printTime(){
  // Näyttää ajan ylhäällä 5. riviltä alkaen
  lcd.setCursor(8, 0);
  lcd.print(rtc.getTimeStr());
  
  // Näyttää viikonpäivän alarivin vasemmassa nurkassa
  lcd.setCursor(0, 0);
  lcd.print(rtc.getDOWStr(2)); //FORMAT_SHORT 1 - FORMAT_LONG 2
  
  // Asettaa päivämäärän alariville
  lcd.setCursor(3, 1);
  lcd.print(rtc.getDateStr());

  //Tulostaa kellonajan sarjamonitoriin tarkistamista varten
  //Serial.print("Kello on: ");
  //Serial.println(rtc.getTimeStr());
  Serial.print("Etäisyys: ");
  Serial.println(distance);
}

//Funktio tuntien tallettamista varten
void saveHours(){
  //Ottaa kellonajan merkkijonona talteen kellonaika muuttujaan
  String kellonaika = rtc.getTimeStr(2);
  //Muuttaa kellonajan ainoastaan tunneiksi ja Int -tyyppiseksi hours -muuttujaa varten
  hours = kellonaika.toInt();
}

//Funktio aikatietojen muuttamista varten
void setupClock(){
  //Asettaa kellon "run-modeen" ja ottaa kirjoitussuojan pois päältä
  rtc.halt(false);
  rtc.writeProtect(false);
  
  //Alla olevilla riveillä voi alustaa DS1302 arvot
//rtc.setDOW(5);        // Asettaa viikonpäivän 
/* 
 *1 = Maanantai
 *2 = Tiistai
 *3 = Keskiviikko
 *4 = Torstai
 *5 = Perjantai
 *6 = Lauantai
 *7 = Sunnuntai
 */
//rtc.setTime(18, 31, 30);     // Asettaa ajan muodossa HH:MM:SS (24h muotoon)
//rtc.setDate(01, 02, 2017);  // Asettaa päivämäärän muodossa DD.MM.YYYY

Serial.println("Kellonaika asetettu onnistuneesti");
}

//Funktio LCD-näytön asetuksia varten
void setupLCD(){
  //Alustaa LCD-näytön koon 16x2 merkkiin
  lcd.begin(16, 2);
  #define   CONTRAST_PIN   9
  #define   BACKLIGHT_PIN  7
  #define   CONTRAST       110
  pinMode(CONTRAST_PIN, OUTPUT);
}

//Funktio joka tarkistaa etäisyyden etäisyysanturista
void checkDistance(){
  //Etäisyysanturin tiedot
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  //Etäiysyys lasketaan seuraavalla kaavalla
  distance = (duration/2) / 29.1;
}

//Funktio, joka määrittelee millä tavalla LED-valo(t) palaa/palavat
void setLedMode(){
  
  //Jos etäisyys suurempi kuin määritelty etäisyys (10cm) ja kirkkaus on 10
  if (distance > ledDistance && brightness == 10) 
  {
  int launch = 0;
    analogWrite(led, brightness);
  }

  //Jos ylläolevat ehdot eivät täyty, "hengittävä" led-mode on päällä kunnes ne täyttyvät
  else
  {
  // Asettaa ledin kirkkauden
  analogWrite(led, brightness);

  //Lisää/vähentää jatkuvasti kirkkautta riippuen fadeamountin etumerkistä
  brightness = brightness + fadeAmount;

  //Kääntää kirkkauden pienenemään, kun se on saavuttanut korkeimman rajan 255 ja kun se on saavuttanut pienimmän rajan 0
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  //Odottaa määrätyn ajan, jotta himmennys-efekti näkyy
  delay(ledDelay);
  }
}
