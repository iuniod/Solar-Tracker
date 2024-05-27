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

// Last displayed message
String lastMessage = "";
// Current reading from the input pin
int buttonState;
// Previous reading from the input pin
int lastButtonState = LOW;
// The last time the output was toggled
unsigned  long lastDebounceTime = 0;
// The debounce time
unsigned long debounceDelay = 50;