#include <SoftwareSerial.h>

//define the pins. Note use of define here saves memory but weware of 
//compliation replacing a string containing it
#define BT_TX_PIN 1
#define BT_RX_PIN 0
#define SONAR_1_TX_PIN 12
#define SONAR_1_RX_PIN 13
#define SONAR_2_TX_PIN 10
#define SONAR_2_RX_PIN 11
#define LED_RED_PIN 6
#define LED_BLUE_PIN 5
#define LED_GREEN_PIN 3

//control when the sonar runs
unsigned long sonarTime = 0;
unsigned long sonarInterval = 30;

//used to average 3 readings before sending to smooth out readings
int sonarMeasurments[2];
int sonarCounter[2];
int sonarSamples = 3; //increase for smoother


//creates the software serial port
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);




void setup(){
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  pinMode(SONAR_1_TX_PIN, OUTPUT);
  pinMode(SONAR_1_RX_PIN, INPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);

  sonarMeasurments[0] = 0;
  sonarMeasurments[1] = 0;
  sonarCounter[0] = 0;
  sonarCounter[1] = 0;
}

void loop(){
  CommunicateBluetooth();

  //do things async like
  unsigned long currentTime = millis();
  if( currentTime - sonarTime > sonarInterval){
    sonarTime = currentTime;
    DoSonar(0);
    DoSonar(1);
  }
}

void CommunicateBluetooth(){
  if (BTSerial.available())
    Serial.write(BTSerial.read());
  if (Serial.available())
    BTSerial.write(Serial.read());
}

int DoSonar(int index){
  int distance = SenseSonar();
  sonarMeasurments[index] += distance;
  sonarCounter[index]++;

  if(sonarCounter[index] > sonarSamples){
    int smoothDistance = sonarMeasurments[index]/sonarSamples;
    sonarCounter[index] = 0;
    sonarMeasurments[index] = 0;
   

    OnboardSonarDisplay(smoothDistance);
    //return sonar 1 as negative
    if(index == 0){
      smoothDistance *= -1;
    }
    Serial.println(smoothDistance);
  }
  
}

int SenseSonar(){
  long duration;

  digitalWrite(SONAR_1_TX_PIN, HIGH);
  digitalWrite(SONAR_1_TX_PIN, LOW);
  duration = pulseIn(SONAR_1_RX_PIN, HIGH);
  return (duration/2) /29.1;
}

void OnboardSonarDisplay(int distance){
  int red = 0;
  int green = 0;
  int blue = 0;

  if(distance < 90){
    red = map(distance, 0, 90, 255, 0);
  }
  if(distance > 30 && distance <=120){
    green = map(distance, 30, 120, 0, 255);
  }
  if(distance > 90 && distance <=150){
    blue = map(distance, 90, 150, 0, 255);
  }
  if(distance > 120 && distance <=180){
    red = map(distance, 120, 180, 0, 255);
  }
  if(distance > 150){
    blue = 255;
  }
  if(distance > 180){
    red = 255;
  }

  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_BLUE_PIN, blue);
  
}
