
/**************************Libraries*****************************/
#include <LiquidCrystal.h>
#include <dht.h>
#include <Keypad.h>
/***************************************************************/


/**************************Initilization of Keypad*****************************/
const byte ROW = 4;      //four ROW
const byte COLUMNS = 4;  //four columns
//define the cymbols on the buttons of the keypads
String password = "67855";
bool pass = false;
bool hazard = true;
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
/******************************************************************************/


/*******************Sensor Initilization*************************/
int time = millis();

dht DHT;
#define DHT_PIN 22
#define MOIST_PIN 0
#define GAS_PIN 1
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float average[4] = { 0.49, 2.35, 37, 25};
/***************************************************************/

void setup() {
  Serial.begin(9600);  //start serial
  lcd.begin(16, 2);    //start lcd
  delay(100);
  pinMode(8, OUTPUT);   
  
  //getAverage(average);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(pass && millis()-time > 60000){  //check if the lcd has been displaying for more than one minutes, if yes turn it off
    pass = false;
  }
  if(!pass){lock();}    //halt if the password is not entered the 
  if(!hazard){
    digitalWrite(8, HIGH);   //turn the hazard light on if the reading is abnormal
    }
  else{
    digitalWrite(8, LOW);    //otherwise keep the light off 
    }

  LCDprint(average);         //read the sensor data and print it to the LCD screen
}

void lock() {
  lcd.clear();
  String str1 = "Enter PIN: ";     //prompt the question for the user to enter password
  while (true) {                   //halt until the correct password is entered
    String str2 = "";              //the user input string
    while (str2.length() < 5) {
      char customKey = customKeypad.getKey();   //get the keypad input
      if (customKey) {
        str2 = str2 + String(customKey);        //append the newly entered character to the end of the user input string
      }
      lcd.setCursor(0, 0);                    //print the question
      lcd.print(str1);
      lcd.setCursor(0, 1);                    //print the updated user input code
      lcd.print(str2);
    }
    if (str2.equals(password)){                  //if the inputed code match with the set passcode break from the infinite loop
      lcd.clear();
      break;
    }
    lcd.clear();
  }
  pass = true;                                 //allowed to read data
  time = millis();                             //start timer for lock out
}





int check(float data, float average) {           //check the incoming data is within +- 10% of the average value
  if (data < average * 0.9) {
    return 0;                                             //output 0 for more than 10% lower than the average
  } else if (data < average * 1.1) {
    return 1;                                           //output 1 for within margin of error
  } else {
    return 2;                                         //output 2 for more than 10% higher than the average
  }
}

void warning(String str, float data, float average){  //call the check for hazard function and turn the LED on if the value is abnormal
  int chk;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(str);
  lcd.setCursor(0, 1);
  chk = check(data, average);        //interpurating the code generated by the check() function
  if (chk == 0) {
    lcd.print("Too low!");
    hazard = hazard && 0;
  } else if (chk == 1) {
    lcd.print("Normal");
  } else {
    lcd.print("Too High!");
    hazard = hazard && 0;
  }
  delay(1500);
}

void LCDprint(float average[4]) {          //call the sensor functions and print functions to display the testing result

  float moist = moistMeter(MOIST_PIN);
  float gas = gasMeter(GAS_PIN);
  float hum, temp;
  humTempMeter(DHT, DHT_PIN, &hum, &temp);

  String str1 = "Moist: " + String(moist) + "%";
  String str2 = "CO2%: " + String(gas) + "%";
  String str3 = "Humid: " + String(hum) + "%";
  String str4 = "Temp: " + String(temp) + "'C";

  warning(str1, moist, average[0]);
  warning(str2, gas, average[1]);
  warning(str3, hum, average[2]);
  warning(str4, temp, average[3]);
}

float moistMeter(int pin) {                              //measure the moisture of the soil, and convert it to percent, 100 is in water, 0 is dry
  float moist = analogRead(pin) / 1023.0 * 100;
  return moist;
}

float gasMeter(int pin) {
  float gas = analogRead(pin) / 1023.0 * 100;          //measure the percentage of carbo deoxide in the air
  //Serial.print("Gas: "); Serial.print(gas, DEC); Serial.println("%");
  return gas;
}

void humTempMeter(dht DHT, int pin, float* hum, float* temp) {  //measure the temperature and humidity 
  int chk = DHT.read11(pin);
  *hum = DHT.humidity;
  *temp = DHT.temperature;
}
