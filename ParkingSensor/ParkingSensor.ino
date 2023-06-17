#include <LiquidCrystal.h>

// Pin Definitions
#define HCSR04_PIN_TRIG	3
#define HCSR04_PIN_ECHO	2
#define LCD_PIN_RS	12
#define LCD_PIN_E	11
#define LCD_PIN_DB4	4
#define LCD_PIN_DB5	7
#define LCD_PIN_DB6	8
#define LCD_PIN_DB7	10
#define LEDG_PIN_VIN	5
#define LEDY_PIN_VIN	6
#define LEDR_PIN_VIN	9
#define PB_LCD_TOGGLE 13
#define POTENTIOMETER_PIN_SIG_A	A0

#define YELLOW_LED_DISTANCE 60
#define ACTIVATION_DISTANCE 250

#define TIMEOUT 30000

LiquidCrystal lcd(LCD_PIN_RS,LCD_PIN_E,LCD_PIN_DB4,LCD_PIN_DB5,LCD_PIN_DB6,LCD_PIN_DB7);

int potentiometerValue = 0;
long duration;
int distance;

int pbCurrent;
int pbPrior = 0;

int lcdDisplaySetup = 0;

unsigned long sleepTime = TIMEOUT; 

void setup() {
  pinMode(POTENTIOMETER_PIN_SIG_A, INPUT);
  pinMode(HCSR04_PIN_TRIG, OUTPUT);
  pinMode(HCSR04_PIN_ECHO, INPUT);
  pinMode(PB_LCD_TOGGLE, INPUT_PULLUP);
  pinMode(LEDG_PIN_VIN, OUTPUT);
  pinMode(LEDY_PIN_VIN, OUTPUT);
  pinMode(LEDR_PIN_VIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) ; // wait for serial port to connect. Needed for native USB
  Serial.println("Start.");

  lcd.begin(16, 2);

  // Print welcome message to LCD
  lcd.setCursor(0, 0);
  lcd.print("Parking Helper");
  lcd.setCursor(0, 1);
  lcd.print("Made by Sarah :)");

  digitalWrite(LEDR_PIN_VIN, HIGH);
  digitalWrite(LEDY_PIN_VIN, HIGH);
  digitalWrite(LEDG_PIN_VIN, HIGH);

  delay(5000);
  lcd.clear();

  digitalWrite(LEDR_PIN_VIN, LOW);
  digitalWrite(LEDY_PIN_VIN, LOW);
  digitalWrite(LEDG_PIN_VIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Stop Distance");
  lcd.setCursor(0, 1);
  lcd.print("0 cm");
}

void loop() {
  unsigned long currentMillis = millis();

  // Retrieve mode toggle state
  pbCurrent = digitalRead(PB_LCD_TOGGLE);

  if(pbPrior == 0 && pbCurrent == 1)
  {
    switch(lcdDisplaySetup)
    {
      case 0:
        Serial.println("Changing LCD to display setup.");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Stop Distance");
        lcd.setCursor(0, 1);
        lcd.print(potentiometerValue);
        lcd.print(" cm");

        lcdDisplaySetup = 1;
        break;
      case 1:
        Serial.println("Changing LCD to display vehicle distance.");

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Car Distance");
        lcd.setCursor(0, 1);
        lcd.print(distance);
        lcd.print(" cm");

        lcdDisplaySetup = 0;
        break;
      default:
        break;
    }
  }
  pbPrior = pbCurrent;

  // Clear Trig PIN
  digitalWrite(HCSR04_PIN_TRIG, LOW);
  delayMicroseconds(2);
  
  // Sets the Trig PIN on HIGH state for 10 micro seconds
  digitalWrite(HCSR04_PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(HCSR04_PIN_TRIG, LOW);

  // Reads the Echo PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(HCSR04_PIN_ECHO, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  Serial.println(distance);

  // Only activate the LCD and LED's if we are within 8 feet.
  if(distance > ACTIVATION_DISTANCE)
  {
    lcd.noDisplay();
    turnOffLeds();
    return;
  }
  else
  {
    lcd.display();
  }

  int tempValue = potentiometerValue;
  potentiometerValue = analogRead(A0) / 10;

  if(distance <= potentiometerValue)
  {
    toggleRedLed();
  }
  else if(distance <= (potentiometerValue + YELLOW_LED_DISTANCE))
  {
    toggleYellowLed();
  }
  else
  {
    toggleGreenLed();
  }

  if(tempValue != potentiometerValue)
  {
    if(lcdDisplaySetup == 1)
    {
      lcd.setCursor(0, 1);
      lcd.print(potentiometerValue);
      lcd.print(" cm       ");
    }

    Serial.print("Stop distance: ");
    Serial.print(potentiometerValue);
    Serial.println(" cm       ");
  }

  // Display vehicle distance
  if(lcdDisplaySetup == 0)
  {
    lcd.setCursor(0, 1);
    lcd.print(distance);
    lcd.print(" cm       ");
  }

  delay(50);
}

void clearLcdLine()
{
  lcd.print("                ");
}

// Turns on red LED
void toggleRedLed()
{
  digitalWrite(LEDR_PIN_VIN, HIGH);
  digitalWrite(LEDY_PIN_VIN, LOW);
  digitalWrite(LEDG_PIN_VIN, LOW);
}

// Turns on yellow LED
void toggleYellowLed()
{
  digitalWrite(LEDR_PIN_VIN, LOW);
  digitalWrite(LEDY_PIN_VIN, HIGH);
  digitalWrite(LEDG_PIN_VIN, LOW);
}

// Turns on green LED
void toggleGreenLed()
{
  digitalWrite(LEDR_PIN_VIN, LOW);
  digitalWrite(LEDY_PIN_VIN, LOW);
  digitalWrite(LEDG_PIN_VIN, HIGH);
}

// Turn off all LEDs
void turnOffLeds()
{
  digitalWrite(LEDR_PIN_VIN, LOW);
  digitalWrite(LEDY_PIN_VIN, LOW);
  digitalWrite(LEDG_PIN_VIN, LOW);
}