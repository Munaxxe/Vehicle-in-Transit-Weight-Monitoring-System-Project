#include <Arduino.h>
#include "HX711.h"
#include <LiquidCrystal.h>

const int rs = 9, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Variable declarations 
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
const int analogInPin = A0; // Analog input pin that the potentiometer is attached to
const int analogOutPin = 12; // Analog output pin that the LED is attached to

int in1 = 10;
int red = 11;
int orange = A5;
int green = A2;
int buzzer = A1;
int sensorValue = 0; // Value read from the pot
int outputValue = 0; // Value output to the PWM (analog out)
float weightAvg =0;
float weight;

HX711 scale;

void setup() {
 
  pinMode(orange, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(57600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("\t|Calibrating|");

 //Initialising variables
  digitalWrite(red, LOW);
  digitalWrite(in1, LOW);
  analogWrite(orange, 0);
  analogWrite(green, 0);
  analogWrite(buzzer, 0);  

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read()); // Printing a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20)); // Printing the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5)); // Printing the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1); // Print the average of 5 readings from the ADC minus tare weight (not set) divided


  scale.set_scale(36.059); // Calibration factor
  scale.tare(); // Reseting the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read()); // Print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20)); // Print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5)); // Print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1); // Print the average of 5 readings from the ADC minus tare weight, divided

  Serial.println("Readings:");

}

// Motor start function
void forward() {
  digitalWrite(in1, HIGH);
}

void loop() {

  Serial.print("Weight in KG:\t");
  Serial.print(scale.get_units() / 1000, 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 5);

  // Reading the analog-in value of the potentiometer
  sensorValue = analogRead(analogInPin);
  // mapping it to the range of the analog-out
  outputValue = map(sensorValue, 0, 1023, 0, 99);
  // Changing the analog-out value
  analogWrite(analogOutPin, outputValue);
  // Print the results to the Serial Monitor:

  weight = scale.get_units() / 1000;
  lcd.setCursor(1, 1);
  Serial.print(scale.get_units() / 1000, 1);
  lcd.setCursor(0, 2);

  weightAvg = weight *2;

  Serial.println(scale.get_units(10), 5);
  delay(3000);

  lcd.clear();

  // Less than the threshold weight function
  if (int(round(weightAvg)) <= 7.00){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("N-Weight |");
  lcd.write("\tSpeed");
  
  lcd.setCursor(0,1);
  lcd.print(weightAvg); 
  lcd.print("\tKg |\t");
  lcd.print(outputValue);
  lcd.print("\tKm/h");  

  analogWrite(green, 255);
  digitalWrite(red, LOW);
  analogWrite(orange, 0);
  analogWrite(buzzer, 0);
  forward();
    
  }
  
  // Approching overweight function 
  if( int(round(weightAvg)) == 8.00){
   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("A-O-Weight|");
  lcd.write("Speed");
  
  lcd.setCursor(0,1);
  lcd.print(weightAvg); 
  lcd.print("\tKg |\t");
  lcd.print(outputValue);
  lcd.print("\tKm/h");  
  
  analogWrite(orange, 255);
  digitalWrite(red, LOW);
  analogWrite(green, 0);
  analogWrite(buzzer, 0);
  forward();

  }
  
  // Greater thsn the threshold voltage
  if (int(round(weightAvg) >= 9.00)) {
    
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("O-Weight |");
  lcd.write("\tSpeed");

  lcd.setCursor(0,1);
  lcd.print(weightAvg); 
  lcd.print("\tKg |\t");
  lcd.print(outputValue);
  lcd.print("\tKm/h");
  
  analogWrite(green, 0);
  digitalWrite(red, HIGH);
  analogWrite(orange, 0);
  analogWrite(buzzer, 255); 
  digitalWrite(in1, LOW);
  
  }
  // Catastrophic system failure
  if (outputValue > 60){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("------ |");
  lcd.write("\tSpeed");
  
  lcd.setCursor(0,1);
  lcd.print(weightAvg); 
  lcd.print("\tKg |\t");
  lcd.print(outputValue);
  lcd.print("\tKm/h");  

  analogWrite(orange, 255);
  digitalWrite(red, HIGH);
  analogWrite(buzzer, 0);
  analogWrite(green, 255);          
     }
 }
