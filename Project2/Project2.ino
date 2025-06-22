#include <Servo.h> // Include the Servo library for controlling servos
#include <LiquidCrystal.h> // Include the LiquidCrystal library for controlling LCDs

const int rs = 4, en = 3, d4 = 13, d5 = 12, d6 = 11, d7 = 10;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); // Create an LCD object with the specified pins

Servo myservo; // Create a Servo object

int pos = 0; // Initialize position for the servo
int ledPin = 8; // Pin for the LED indicator
const int trigPin = 6; // Pin for the ultrasonic sensor trigger
const int echoPin = 5; // Pin for the ultrasonic sensor echo
float duration, distance; // Variables to store duration and distance measurements
int freeSpaces = 10; // Initialize the number of free parking spaces
int InVal = 0; // Variable to store input value (e.g., from a button)
int previousFreeSpaces = freeSpaces; // Store the previous number of free spaces

void setup()
{
  pinMode(2, INPUT); // Set pin 2 as an INPUT for the button
  myservo.attach(9); // Attach the servo to pin 9
  myservo.write(0); // Initialize the servo position to 0 degrees

  pinMode(trigPin, OUTPUT); // Set the trigPin as an OUTPUT for the ultrasonic sensor
	pinMode(echoPin, INPUT); // Set the echoPin as an INPUT for the ultrasonic sensor

  Serial.begin(9600); // Start the serial communication at 9600 baud rate
  pinMode(ledPin, OUTPUT); // Set the ledPin as an OUTPUT for the LED

  // Set up the LCD's number of columns and rows:
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  // Print a message to the LCD.
  lcd.print("Parking Spaces:"); // Display the label on the first row of the LCD
  lcd.setCursor(0, 1); // Move the cursor to the beginning of the second row
  lcd.print(freeSpaces); // Display the initial number of free spaces on the LCD
}

void checkDistance()
{
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);  
  duration = pulseIn(echoPin, HIGH);  
  distance = (duration*.0343)/2;
  delay(1);
}

void openGate()
{
  for (pos = 0; pos <= 90; pos += 1)
  {
    myservo.write(pos); 
    delay(15);
  }
}

void closeGate()
{
  for (pos = 90; pos >= 0; pos -= 1)
  {
    myservo.write(pos);
    delay(15);
  }
}

void buttonpressed()
{
  InVal = digitalRead(2);
  if(InVal == 1 && freeSpaces < 10) //If button is pressed
  {
    openGate();
    delay(3000);
    closeGate();
    freeSpaces++;
  }
}

void checkLight()
{
  int value = analogRead(A0);
  if (value > 450)
  {
    digitalWrite(ledPin, LOW);
  }
  else
  {
    digitalWrite(ledPin,  HIGH);
  }
}

void loop()
{
  checkLight();
  buttonpressed();
  if (freeSpaces != previousFreeSpaces) // Update the LCD if free spaces change
  {
    lcd.setCursor(0, 1);
    lcd.print("    "); // Clear previous value
    lcd.setCursor(0, 1);
    
    if (freeSpaces == 0)
    {
      lcd.print("FULL"); // Display "FULL" if no free spaces
    }
    else
    {
      lcd.print(freeSpaces); // Display the number of free spaces
    }

      previousFreeSpaces = freeSpaces;
  }

  digitalWrite(trigPin, LOW);  
	delayMicroseconds(2);  
	checkDistance();

  if (distance < 10 && freeSpaces > 0) // If car is detected within 10 centimeters
  {
    openGate(); // Open the gate
    delay(2000);

    for(int i = 0; i < 10; i++) // Check if car moved from front of sensor
    {
      checkDistance();
    }

    if (distance > 10) // If car moved from front of sensor
    {
      closeGate(); // Close the gate
      freeSpaces--;
    }
    else // If car still in front of sensor
    {
      while (distance < 10)
      {
        myservo.write(90); // Leave the gate open
        checkDistance(); // While the car still there (distance < 10), check the distance continuously

        if (distance > 10) // If car moved from front of sensor
        {
          delay(2000); // Close gate 2 seconds after the car left the sensor
          closeGate(); // Close the gate
          freeSpaces--;
        }
      }
    }
  }    
  delay(250);
}