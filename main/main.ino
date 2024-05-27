// ------------------- Import libraries --------------------
// Include the servo motor library
#include <Servo.h>
// Include the interruption library
#include <avr/interrupt.h> 
// ---------------- Define pins and errors -----------------
// Define the LDR sensor pins
#define LDR1 A0
#define LDR2 A1
// Set epsilon value
#define eps 10
// Set error code
#define error 1000
// Starting point of the servo motor
int Spoint =  0;
// Create an object for the servo motor
Servo servo;
// Select the pin for the LED
const int ledPin = 12;
// Select the pin for the button
const int buttonPin = 13;
// ---------------- Define global variables ----------------
// Global variable to count milliseconds
volatile unsigned int timer2_millis = 0;
// Global variable to count the time sience last position has moved
volatile unsigned int lastPositionChange = 0;
// Global variable to check the button
volatile bool ifPressed = false;
// Current reading from the input pin
int buttonState;
// Previous reading from the input pin
int lastButtonState = LOW;
// The last time the output was toggled
unsigned  long lastDebounceTime = 0;
// The debounce time
unsigned long debounceDelay = 50;


// ---------------- Define useful functions ----------------
// Timer2 ISR to increment the millisecond counter
ISR(TIMER2_COMPA_vect) {
  timer2_millis++;
}

// Custom delay function using Timer2
void delay_ms(unsigned int ms) {
  timer2_millis = 0;
  // Set CTC mode (Clear Timer on Compare Match)
  TCCR2A = (1 << WGM21); // CTC mode
  TCCR2B = (1 << CS22) | (1 << CS20); // Prescaler 128
  OCR2A = 124; // Set compare value for 1ms interrupt (16MHz / 128 / 1000 - 1)
  TIMSK2 = (1 << OCIE2A); // Enable Timer2 compare interrupt

  // Wait until the timer reaches the desired delay
  while (timer2_millis < ms);

  // Disable Timer2 compare interrupt
  TIMSK2 = 0;
}

// Move the servo function
void move_servo(int ldr1, int ldr2) {
// Get the difference of these values
  int value = abs(ldr1 - ldr2);

  if ((value <= eps)) {
// You found a good position to stay in
  } else if (ldr1 >= error && ldr2 >= error) {
// Move in the initial state
  Spoint = 0;
// Update last position change time
  lastPositionChange = millis();
  Serial.println("Moving to the initial state!");
  } else {
    if (ldr1 > ldr2) {
      if (Spoint - 2 <= 0) {
// Be careful with the limitation of the servo
      } else {
        Spoint -= 2;
// Update last position change time
        lastPositionChange = millis();
      }
    }
    if (ldr1 < ldr2) {
      if (Spoint + 2 >= 120) {
// Be careful with the limitation of the servo
      } else {
        Spoint += 2;
// Update last position change time
        lastPositionChange = millis();
      }
    }
  }

// Write values on the servo motor
  servo.write(Spoint);
}

// Messages for the user to understand the time of the day
// according to the position of the solar panel
void user_messages() {
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
}

// Light the led only if the panel has found 
// a good position to charge
void light_led() {
  if (millis() - lastPositionChange > 2000) {
// Turn on the LED
    PORTB |= (1 << PORTB4);
  } else {
// Turn off the LED
    PORTB &= ~(1 << PORTB4);
  }
}


void setup() {
  Serial.begin(9600);
// Include servo motor PWM pin
  servo.attach(11);
// Set the starting point of the servo - East
  servo.write(Spoint);
// Declare the LED as an output
  pinMode(ledPin, OUTPUT);
// Declare the button as an input
  pinMode(buttonPin, INPUT);
// Initialize last position change time
  lastPositionChange = millis();
// Delay using Timer1
  delay_ms(80);
}

void loop() {
// --------------------- Reading data ----------------------
// Configure ADC for LDR1
// Select channel 0
  ADMUX = 0;
// AVCC with external capacitor at AREF pin
  ADMUX |= (1 << REFS0);
// Clear ADCSRA register
  ADCSRA = 0;
// Set prescaler at 128
  ADCSRA |= (7 << ADPS0);
// Enable ADC
  ADCSRA |= (1 << ADEN); 
// Start conversion
  ADCSRA |= (1 << ADSC);

// Wait until conversion is complete
  while (ADCSRA & (1 << ADSC));

// Read ADC result
  int ldr1 = ADC;

// Configure ADC for LDR2
// Select channel 0
  ADMUX = 0;
// ADC1 - channel 1
  ADMUX |= (1 << MUX0);
// AVCC with external capacitor at AREF pin
  ADMUX |= (1 << REFS0);
// Start conversion
  ADCSRA |= (1 << ADSC);

// Wait until conversion is complete
  while (ADCSRA & (1 << ADSC));

// Read ADC result
  int ldr2 = ADC;

// Read the state of the button
  int reading = digitalRead(buttonPin);

// -------------------- Implementation ---------------------

// Check if the button was pressed
  if (reading != lastButtonState) {
// Reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      ifPressed = !ifPressed;
    }
  }

// Save the reading
  lastButtonState = reading;

// Move the solar panel
  if (ifPressed == false) {
    move_servo(ldr1, ldr2);
// Write the user messages
    user_messages();
  } else {
    Serial.println("The solar panel was set to a fix position");
  }

// Light the led
  light_led();

// ------------------------ Waiting ------------------------
  delay_ms(80);
}
