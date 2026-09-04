// ======================================================
//        TCS3200 8-COLOR DETECTOR WITH SERVO
//        Arduino UNO
//
//        SELF-CALIBRATING VERSION 
//
//        Features:
//        - 8 color calibration
//        - 10 readings averaged per color
//        - Calibration stored in EEPROM
//        - Nearest-profile color detection
//        - Servo moves to calibrated color angle
//
//        Colors:
//        BLACK, WHITE, RED, GREEN, BLUE,
//        YELLOW, PURPLE, ORANGE
//
//        Commands in Serial Monitor:
//        C = Start calibration
//        R = Reset stored calibration
//        S = Show stored calibration
// ======================================================

#include <Servo.h>
#include <EEPROM.h>


// ======================================================
//                     PIN CONNECTIONS
// ======================================================

const int S0 = 4;
const int S1 = 5;
const int S2 = 6;
const int S3 = 7;
const int SENSOR_OUT = 8;
const int SERVO_PIN = 9;


// ======================================================
//                     SERVO ANGLES
// ======================================================

// Change these only if the physical position
// of the color wheel changes.

const int BLACK_ANGLE  = 170;
const int WHITE_ANGLE  = 145;
const int RED_ANGLE    = 125;
const int GREEN_ANGLE  = 105;
const int BLUE_ANGLE   = 80;
const int YELLOW_ANGLE = 60;
const int PURPLE_ANGLE = 35;
const int ORANGE_ANGLE = 10;


// ======================================================
//                  CALIBRATION SETTINGS
// ======================================================

const int NUM_COLORS = 8;
const int SAMPLES_PER_COLOR = 10;


// ======================================================
//                    COLOR NAMES
// ======================================================

const char* colorNames[NUM_COLORS] =
{
  "BLACK",
  "WHITE",
  "RED",
  "GREEN",
  "BLUE",
  "YELLOW",
  "PURPLE",
  "ORANGE"
};


// ======================================================
//                 EEPROM IDENTIFICATION
// ======================================================

// This number tells the Arduino whether valid
// calibration data already exists in EEPROM.

const unsigned long EEPROM_MAGIC = 0xC0A81234;


// ======================================================
//               COLOR PROFILE STRUCTURE
// ======================================================

struct ColorProfile
{
  unsigned long red;
  unsigned long green;
  unsigned long blue;
};


// ======================================================
//                  EEPROM DATA STRUCTURE
// ======================================================

struct CalibrationData
{
  unsigned long magic;
  ColorProfile colors[NUM_COLORS];
};


// ======================================================
//              GLOBAL OBJECTS / VARIABLES
// ======================================================

Servo colorServo;

CalibrationData calibration;


// ======================================================
//                  SENSOR FUNCTIONS
// ======================================================

// ---------- Read RED ----------

unsigned long readRed()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  delay(50);

  return pulseIn(SENSOR_OUT, LOW, 100000);
}


// ---------- Read GREEN ----------

unsigned long readGreen()
{
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  delay(50);

  return pulseIn(SENSOR_OUT, LOW, 100000);
}


// ---------- Read BLUE ----------

unsigned long readBlue()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  delay(50);

  return pulseIn(SENSOR_OUT, LOW, 100000);
}


// ======================================================
//                READ COMPLETE RGB VALUE
// ======================================================

ColorProfile readColor()
{
  ColorProfile result;

  result.red = readRed();
  result.green = readGreen();
  result.blue = readBlue();

  return result;
}


// ======================================================
//              AVERAGE MULTIPLE READINGS
// ======================================================

ColorProfile readAverageColor()
{
  unsigned long redTotal = 0;
  unsigned long greenTotal = 0;
  unsigned long blueTotal = 0;

  int validSamples = 0;

  for (int i = 0; i < SAMPLES_PER_COLOR; i++)
  {
    ColorProfile reading = readColor();

    // Ignore a reading if pulseIn timed out.
    if (reading.red == 0 ||
        reading.green == 0 ||
        reading.blue == 0)
    {
      Serial.println("Warning: invalid sensor reading. Retrying...");
      delay(100);
      continue;
    }

    redTotal += reading.red;
    greenTotal += reading.green;
    blueTotal += reading.blue;

    validSamples++;

    delay(100);
  }

  ColorProfile average;

  if (validSamples == 0)
  {
    average.red = 0;
    average.green = 0;
    average.blue = 0;
  }
  else
  {
    average.red = redTotal / validSamples;
    average.green = greenTotal / validSamples;
    average.blue = blueTotal / validSamples;
  }

  return average;
}


// ======================================================
//                PRINT COLOR PROFILE
// ======================================================

void printProfile(ColorProfile profile)
{
  Serial.print("R = ");
  Serial.print(profile.red);

  Serial.print("   G = ");
  Serial.print(profile.green);

  Serial.print("   B = ");
  Serial.println(profile.blue);
}


// ======================================================
//              SAVE CALIBRATION TO EEPROM
// ======================================================

void saveCalibration()
{
  calibration.magic = EEPROM_MAGIC;

  EEPROM.put(0, calibration);

  Serial.println();
  Serial.println("Calibration saved to EEPROM.");
}


// ======================================================
//              LOAD CALIBRATION FROM EEPROM
// ======================================================

bool loadCalibration()
{
  EEPROM.get(0, calibration);

  if (calibration.magic == EEPROM_MAGIC)
  {
    return true;
  }

  return false;
}


// ======================================================
//              RESET STORED CALIBRATION
// ======================================================

void resetCalibration()
{
  calibration.magic = 0;

  EEPROM.put(0, calibration);

  Serial.println();
  Serial.println("Stored calibration has been erased.");
  Serial.println("Run calibration again using C.");
}


// ======================================================
//              SHOW STORED CALIBRATION
// ======================================================

void showCalibration()
{
  Serial.println();
  Serial.println("================================");
  Serial.println("       STORED CALIBRATION");
  Serial.println("================================");

  for (int i = 0; i < NUM_COLORS; i++)
  {
    Serial.print(colorNames[i]);
    Serial.print(" -> ");

    printProfile(calibration.colors[i]);
  }

  Serial.println("================================");
}


// ======================================================
//                     CALIBRATION
// ======================================================

void calibrate()
{
  Serial.println();
  Serial.println("================================");
  Serial.println("        CALIBRATION MODE");
  Serial.println("================================");

  Serial.println();
  Serial.println("Calibration uses 10 readings");
  Serial.println("for each color.");
  Serial.println();
  Serial.println("Keep the sensor position and");
  Serial.println("lighting unchanged.");
  Serial.println();


  for (int i = 0; i < NUM_COLORS; i++)
  {
    Serial.println("--------------------------------");
    Serial.print("Place ");
    Serial.print(colorNames[i]);
    Serial.println(" in front of the sensor.");

    Serial.println("Press ENTER when ready.");

    // Wait for ENTER
    while (Serial.available() == 0)
    {
      delay(10);
    }

    // Clear Serial input
    while (Serial.available() > 0)
    {
      Serial.read();
    }

    Serial.println();
    Serial.print("Reading ");
    Serial.print(colorNames[i]);
    Serial.println("...");

    ColorProfile average = readAverageColor();

    calibration.colors[i] = average;

    Serial.print(colorNames[i]);
    Serial.print(" profile: ");

    printProfile(average);

    Serial.println();
  }


  // Save everything
  saveCalibration();

  Serial.println();
  Serial.println("================================");
  Serial.println("     CALIBRATION COMPLETE");
  Serial.println("================================");

  showCalibration();

  Serial.println();
  Serial.println("Returning to NORMAL MODE...");
  Serial.println();
}


// ======================================================
//             CALCULATE COLOR DISTANCE
// ======================================================

// Smaller distance = more similar color.

unsigned long calculateDistance(
  ColorProfile current,
  ColorProfile reference)
{
  long redDifference =
    (long)current.red - (long)reference.red;

  long greenDifference =
    (long)current.green - (long)reference.green;

  long blueDifference =
    (long)current.blue - (long)reference.blue;

  unsigned long distance =
    (unsigned long)(
      redDifference * redDifference +
      greenDifference * greenDifference +
      blueDifference * blueDifference
    );

  return distance;
}


// ======================================================
//                FIND CLOSEST COLOR
// ======================================================

int detectColor(ColorProfile current)
{
  unsigned long smallestDistance = 0xFFFFFFFFUL;

  int bestColor = -1;

  for (int i = 0; i < NUM_COLORS; i++)
  {
    unsigned long distance =
      calculateDistance(current, calibration.colors[i]);

    if (distance < smallestDistance)
    {
      smallestDistance = distance;
      bestColor = i;
    }
  }

  return bestColor;
}


// ======================================================
//                  MOVE THE SERVO
// ======================================================

void moveServoForColor(int colorIndex)
{
  switch (colorIndex)
  {
    case 0:
      colorServo.write(BLACK_ANGLE);
      break;

    case 1:
      colorServo.write(WHITE_ANGLE);
      break;

    case 2:
      colorServo.write(RED_ANGLE);
      break;

    case 3:
      colorServo.write(GREEN_ANGLE);
      break;

    case 4:
      colorServo.write(BLUE_ANGLE);
      break;

    case 5:
      colorServo.write(YELLOW_ANGLE);
      break;

    case 6:
      colorServo.write(PURPLE_ANGLE);
      break;

    case 7:
      colorServo.write(ORANGE_ANGLE);
      break;
  }
}


// ======================================================
//                       SETUP
// ======================================================

void setup()
{
  // TCS3200 pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(SENSOR_OUT, INPUT);


  // Frequency scaling = 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  // Servo
  colorServo.attach(SERVO_PIN);
  colorServo.write(90);


  // Serial Monitor
  Serial.begin(9600);

  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println("   TCS3200 8-COLOR DETECTOR");
  Serial.println("================================");
  Serial.println();


  // Try to load calibration
  if (loadCalibration())
  {
    Serial.println("Calibration found in EEPROM.");
    Serial.println("Starting NORMAL MODE.");
    Serial.println();

    showCalibration();
  }
  else
  {
    Serial.println("No calibration found.");
    Serial.println("Type C and press ENTER");
    Serial.println("to start calibration.");
    Serial.println();
  }
}


// ======================================================
//                        LOOP
// ======================================================

void loop()
{
  // ----------------------------------------------------
  // Check for Serial commands
  // ----------------------------------------------------

  if (Serial.available() > 0)
  {
    char command = Serial.read();

    // Convert lowercase to uppercase
    if (command >= 'a' && command <= 'z')
    {
      command -= 32;
    }


    // ---------- CALIBRATE ----------

    if (command == 'C')
    {
      calibrate();
    }


    // ---------- RESET ----------

    else if (command == 'R')
    {
      resetCalibration();
    }


    // ---------- SHOW CALIBRATION ----------

    else if (command == 'S')
    {
      if (loadCalibration())
      {
        showCalibration();
      }
      else
      {
        Serial.println("No valid calibration found.");
      }
    }


    // Clear remaining input
    while (Serial.available() > 0)
    {
      Serial.read();
    }
  }


  // ----------------------------------------------------
  // Only detect colors if calibration exists
  // ----------------------------------------------------

  if (calibration.magic != EEPROM_MAGIC)
  {
    delay(100);
    return;
  }


  // ----------------------------------------------------
  // Read current color
  // ----------------------------------------------------

  ColorProfile current = readColor();


  // Ignore invalid reading
  if (current.red == 0 ||
      current.green == 0 ||
      current.blue == 0)
  {
    delay(100);
    return;
  }


  // ----------------------------------------------------
  // Print raw values
  // ----------------------------------------------------

  Serial.print("Red: ");
  Serial.print(current.red);

  Serial.print("  Green: ");
  Serial.print(current.green);

  Serial.print("  Blue: ");
  Serial.println(current.blue);


  // ----------------------------------------------------
  // Detect closest calibrated color
  // ----------------------------------------------------

  int detectedColor = detectColor(current);


  if (detectedColor >= 0)
  {
    Serial.print("COLOR DETECTED: ");
    Serial.println(colorNames[detectedColor]);

    moveServoForColor(detectedColor);
  }


  Serial.println("--------------------------------");

  delay(500);
}