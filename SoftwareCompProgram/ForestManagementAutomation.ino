#include <LiquidCrystal.h>
#include <dht.h>
#include <Keypad.h>
dht DHT;
const byte ROW = 4;      //four ROW
const byte COLUMNS = 4;  //four columns
//define the cymbols on the buttons of the keypads
String password = "67855";
bool pass = false;
bool hazard = 
char hexaKeys[ROW][COLUMNS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROW] = { 37, 36, 35, 34 };      //connect to the row pinouts of the keypad
byte colPins[COLUMNS] = { 33, 32, 31, 30 };  //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROW, COLUMNS);

int time = millis();

#define DHT_PIN 22
#define MOIST_PIN 0
#define GAS_PIN 1
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float average[4] = { 0.49, 2.35, 37, 25};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  delay(100);
  pinMode(8, OUTPUT);   
  
  //getAverage(average);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(pass && millis()-time > 60000){
    pass = false;
  }
  //analogWrite(8, 1);
  if(!pass){lock();}

  LCDprint(average);
}

void lock() {
  lcd.clear();
  String str1 = "Enter PIN: ";
  while (true) {
    String str2 = "";
    while (str2.length() < 5) {
      char customKey = customKeypad.getKey();
      if (customKey) {
        str2 = str2 + String(customKey);
      }
      lcd.setCursor(0, 0);
      lcd.print(str1);
      lcd.setCursor(0, 1);
      lcd.print(str2);
    }

    if (str2.equals(password)){
      lcd.clear();
      break;
    }
    lcd.clear();
  }
  pass = true;
  time = millis();
}



void getAverage(float averagePtr[]) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter #of ");
  lcd.setCursor(0, 1);
  lcd.print("samples");
  char customKey = customKeypad.getKey();
}

int check(float data, float average) {
  if (data < average * 0.95) {
    return 0;  //8
  } else if (data < average * 1.05) {
    return 1;  //6
  } else {
    return 2;  //9
  }
}

void LCDprint(float average[4]) {

  float moist = moistMeter(MOIST_PIN);
  float gas = gasMeter(GAS_PIN);
  float hum, temp;
  humTempMeter(DHT, DHT_PIN, &hum, &temp);

  String str1 = "Moist: " + String(moist) + "%";
  String str2 = "CO2%: " + String(gas) + "%";
  String str3 = "Humid: " + String(hum) + "%";
  String str4 = "Temp: " + String(temp) + "'C";

  int chk;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcd.setCursor(0, 1);
  chk = check(moist, average[0]);
  if (chk == 0) {
    lcd.print("Too low!");
  } else if (chk == 1) {
    lcd.print("Normal");
  } else {
    lcd.print("Too High!");
  }
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str2);
  lcd.setCursor(0, 1);
  chk = check(gas, average[1]);
  if (chk == 0) {
    lcd.print("Too low!");
  } else if (chk == 1) {
    lcd.print("Normal");
  } else {
    lcd.print("Too High!");
  }
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str3);
  lcd.setCursor(0, 1);
  chk = check(hum, average[2]);
  if (chk == 0) {
    lcd.print("Too low!");
  } else if (chk == 1) {
    lcd.print("Normal");
  } else {
    lcd.print("Too High!");
  }
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str4);
  lcd.setCursor(0, 1);
  chk = check(temp, average[3]);
  if (chk == 0) {
    lcd.print("Too low!");
  } else if (chk == 1) {
    lcd.print("Normal");
  } else {
    lcd.print("Too High!");
  }
  delay(1500);

  /*
  str1 = "CO2%: " + String(data[1], 2) + "%,  Humidity: " + String(data[2], 2) + "%,  Temperature: " + String(data[3], 2) + "'C  ";



  String str2 = "";
  int one = (String(data[0], 2)).length() + 11;
  int two = (String(data[1], 2)).length() + 10;
  int three = (String(data[2], 2)).length() + 14;
  int four = (String(data[3], 2)).length() + 17;

  float line[] = {one, two, three, four};
  for(int i = 0; i < 4; i++){
    if(data[i] < average[i]*0.95){
      str2 = str2 + "Too Low!";                  //8
      for(int j = 0; j < line[i] - 8; j ++){
        str2 = str2 + " ";
      }
    }
    else if(data[i] < average[i] * 1.05){
      str2 = str2 + "Normal";                  //6
      for(int j = 0; j < line[i] - 6; j ++){
        str2 = str2 + " ";
      }
    }else{
      str2 = str2 + "Too High!";                  //9
      for(int j = 0; j < line[i] - 9; j ++){
        str2 = str2 + " ";
      }
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str1);
  lcd.setCursor(0, 1);
  lcd.print(str2);
  for(int i = 0; i < str1.length(); i ++){
    lcd.scrollDisplayLeft();
    delay(250);
  }
  */
}

float moistMeter(int pin) {
  float moist = analogRead(pin) / 1023.0 * 100;
  return moist;
}

float gasMeter(int pin) {
  float gas = analogRead(pin) / 1023.0 * 100;
  //Serial.print("Gas: "); Serial.print(gas, DEC); Serial.println("%");
  return gas;
}

void humTempMeter(dht DHT, int pin, float* hum, float* temp) {
  int chk = DHT.read11(pin);
  /*
  switch (chk)
  {
    case DHTLIB_OK:  
    Serial.print("OK,\t"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.print("Checksum error,\t"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.print("Time out error,\t"); 
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default: 
    Serial.print("Unknown error,\t"); 
    break;
  }
  */
  *hum = DHT.humidity;
  *temp = DHT.temperature;
}
