// Include the servo motor library
#include <Servo.h>
// Include the interruption library
#include <avr/interrupt.h> 
// Define the LDR sensor pins
#define LDR1 A0
#define LDR2 A1
// Set error code
#define error 10
// Starting point of the servo motor
int Spoint =  0;
// Create an object for the servo motor
Servo servo;
// Select the pin for the LED
int ledPin = 12;

// Global variable to count milliseconds
volatile unsigned int timer2_millis = 0;

// Timer2 ISR to increment the millisecond counter
ISR(TIMER2_COMPA_vect) {
  timer2_millis++;
}

// Custom delay function using Timer2
void delay_ms(unsigned int ms) {
  timer2_millis = 0;
// Set CTC mode (Clear Timer on Compare Match)
// CTC mode
  TCCR2A = (1 << WGM21);
// Prescaler 128
  TCCR2B = (1 << CS22) | (1 << CS20);
// Set compare value for 1ms interrupt (16MHz / 128 / 1000 - 1)
  OCR2A = 124;
// Enable Timer2 compare interrupt
  TIMSK2 = (1 << OCIE2A);

// Wait until the timer reaches the desired delay
  while (timer2_millis < ms);

// Disable Timer2 compare interrupt
  TIMSK2 = 0;
}


void setup() {
  Serial.begin(9600);
// Include servo motor PWM pin
  servo.attach(11);
// Set the starting point of the servo - East
  servo.write(Spoint);
// Declare the LED as an output
  pinMode(ledPin, OUTPUT);
// Delay using Timer1
  delay_ms(80);
}

void loop() {
// Get the LDR sensor value
  int ldr1 = analogRead(LDR1);
// Get the LDR sensor value
  int ldr2 = analogRead(LDR2);
// Get the difference of these values
  int value1 = abs(ldr1 - ldr2);
  int value2 = abs(ldr2 - ldr1);

// Check these values using a IF condition
  if ((value1 <= error) || (value2 <= error)) {
// The night has come!
// Move slowly to the east - Spoint = 0
  Spoint -= 5;
  } else {
    if (ldr1 > ldr2) {
      if (Spoint - 2 <= 0) {
// Be careful with the limitation of the servo
      } else {
        Spoint -= 2;
      }
    }
    if (ldr1 < ldr2) {
      if (Spoint + 2 >= 120) {
// Be careful with the limitation of the servo
      } else {
        Spoint += 2;
      }
    }
  }
// Write values on the servo motor
  servo.write(Spoint);

// Check time of the day and print the correct message
  if (Spoint - 2 <= 0) {
    Serial.println("Good Night!");
  } else if (Spoint < 60) {
    Serial.println("Good Morning sunshine!");
  } else if (Spoint >= 60 && Spoint <= 80) {
    Serial.println("It's time for lunch!");
  } else if (Spoint > 80 && Spoint < 110) {
    Serial.println("Good Afternoon!");
  } else {
    Serial.println("Good Night!");
  }

// Read the light intensity
  int solarValue = ldr1 + ldr2;
// Map the solar value to LED brightness (0 to maxBrightness)
  int brightness = map(solarValue, 108, 1948, 0, 255);
// Set the LED brightness
  digitalWrite(ledPin, brightness);

  delay_ms(80);
}
