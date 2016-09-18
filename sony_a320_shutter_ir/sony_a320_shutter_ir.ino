/* Original source for this was found at http://playground.arduino.cc/Main/SonyShutterIR
* Original author: Jim Park  
* I have modfied it for my purposes  
* You may have to modify the timings if you have problems         
* *
*            IR Circuit
*         IR LED     150R
* Pin 2 ----|>|-----VVVVV----GND
*/

// Some Declarations
unsigned long TimeBetweenShots = 1000;    // just defaulting to 1 second here
int IRled = 12;                // IR Output Signal to Anode of IR LED 
// Cathode of IR LED to ground through a 150 Ohm Resistor.
int Begin = 13;               // Indicates Shutter Start on LED

//pins used for dip switches
int dipPins[] = {2, 3, 4, 5, 6, 7, 8}; 
int dipValue = 1;

// Code to send Shutter release command B4B8F
int SCodeBits[] = {1,0,1,1,0,1,0,0,1,0,1,1,1,0,0,0,1,1,1,1};

// Code to send 2 X delay Shutter release command ECB8F (Not used)
int S2CodeBits[] = {1,1,1,0,1,1,0,0,1,0,1,1,1,0,0,0,1,1,1,1};

void setup()
{
  Serial.begin(9600);
  int i;
  for(i = 0; i<=6; i++){
    pinMode(dipPins[i], INPUT_PULLUP);      // set the digital pins (defined above) as input
  }
  // Setup IRLed and Begin Led Pins as outputs
  pinMode(IRled, OUTPUT);      // sets the digital pin as output
  pinMode(Begin, OUTPUT);      // sets the digital pin as output

  //set timer interval based on dip switches
  dipValue = getDipValue();
  TimeBetweenShots = dipValue * 1000L; //the L is important, keeps the value a long
  Serial.println(dipValue);
  Serial.println(TimeBetweenShots);
}

// Infinite Loop 
void loop()
{
  digitalWrite(Begin, HIGH);  // Signal Begin LED ON

  for (int i=1; i <= 3; i++)  // Send Command 3 times as per Sony Specs
  {
    header();                    // Send the Start header
    for (int i=0; i <= 19; i++)  // Loop to send the bits
    {
          if(SCodeBits[i] == 1)  // Is Data_is_One to be sent ?
          {
            Data_is_One();              // Yes, send a Data_is_One bit
          }
          else                  // No, Data_is_Zero to be sent
          {
            Data_is_Zero();              // Send a Data_is_Zero bit
          }
    }
    delay(11);                  // Delay Padding to give approx 45mS between command starts
  }
  digitalWrite(Begin, LOW);     // Begin LED OFF
  delay(TimeBetweenShots);      // Hang about wasting time before next shot
}

// Routine to give the 40kHz burst signal
void burst()                   // 40KHz burst
{
  digitalWrite(IRled, HIGH);   // sets the pin on
  delayMicroseconds(10);       // pauses for 13 microseconds  (fudged to 10uS Delay)   
  digitalWrite(IRled, LOW);    // sets the pin off
  delayMicroseconds(8);        // pauses for 12 microseconds   (fudged to 8uS Delay)
}

// Routine to give a quiet period of data
void quiet()                   // Quiet burst
{
  digitalWrite(IRled, LOW);    // sets the pin off
  delayMicroseconds(10);       // pauses for 13 microseconds   (fudged to 10uS Delay)  
  digitalWrite(IRled, LOW);    // sets the pin off
  delayMicroseconds(8);        // pauses for 12 microseconds    (fudged to 8uS Delay)
}

// Routine to send header data burst
// This allows the IR receiver to set its AGC (Gain)
// Header Burst Timing is 96 * 0.025uS = 2.4mS
// Quiet Timing is 24 * 0.025uS = 600uS
void header() 
{
    for (int i=1; i <= 96; i++){
      burst();                // 40kHz burst
    }
    for (int i=1; i <= 24; i++){
      quiet();                // No 40 kHz
    }
}

// Routine to send one data burst
// Burst Timing is 48 * 0.025uS = 1.2mS
// Quiet Timing is 24 * 0.025uS = 600uS
void Data_is_One()
{
    for (int i=1; i <= 48; i++){
      burst();                // 40kHz burst
    }
    for (int i=1; i <= 24; i++){
      quiet();                // No 40 kHz
    }
}

// Routine to send zero data burst
// Burst Timing is 24 * 0.025uS = 600uS
// Quiet Timing is 24 * 0.025uS = 600uS
void Data_is_Zero()
{
    for (int i=1; i <= 24; i++){
      burst();                // 40 kHz burst
    }
    for (int i=1; i <= 24; i++){
      quiet();                // No 40 kHz 
    }
}

//Read state from DIP Switch (7 positions used)
int getDipValue(){
  int i,j=0;
  //Get the switches state
  for(i=0; i<7; i++){
    j = (j << 1) | !digitalRead(dipPins[i]);   // read each input pin
    
  }
  return j; //return address
}
