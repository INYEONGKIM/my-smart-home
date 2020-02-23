#include <core_build_options.h> // system clock
#include <swRTC.h>

#include <SoftwareSerial.h> // bluetooth
#include <Servo.h> // serve motor

swRTC rtc;
Servo myServo;

int servoLightOnPosition = 180;
int servoDefaultPosition = 90;
int servoLightOffPosition = 0;
int servoPinNum = 9;
int ledPinNum = 8;

SoftwareSerial BTSerial(4, 5);

int isAlarmOn = 1; // 0 == false, 1 == true

String mobileInputString, nowTime, alarmTime = "190010";

void setup() {
  rtc.stopRTC();
  rtc.setTime(23, 59, 00); // hh, mm, ss
  rtc.setDate(23, 2, 2020); // dd, mm, yyyy
  rtc.startRTC();

  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.println("ATcommand");  // ATcommand Start

  pinMode(ledPinNum, OUTPUT); // Set LED
}

void loop() {
  if (isAlarmOn) {
    /* alarm on */
    digitalWrite(ledPinNum, HIGH);
    nowTime = makeNowTime2String();
    Serial.println(nowTime);
    if (!nowTime.compareTo(alarmTime)) {
      /* Time to wake up */
      turnOnTheLight();
      isAlarmOn = 0;
    }
  } else {
    /* alarm off */
    digitalWrite(ledPinNum, LOW);
  }
  
  /* BlueTooth == async */
  if (BTSerial.available()) { 
    mobileInputString = BTSerial.readStringUntil('\n');

    if (mobileInputString == "on") {
      /* LIGHT ON */
      turnOnTheLight();
    } else if (mobileInputString == "off") {
      /* LIGHT OFF */
      turnOffTheLight();
    } else if (mobileInputString == "cancel") {
      /* CANCELLED */
      Serial.println("Alarm is cancelled!");
      isAlarmOn = 0;
    } else if (mobileInputString.length() == 6) {
      /* Alarm format must be hhmmss */
      if (checkValidAlarmTime(mobileInputString)) {
        Serial.print("Alarm is on ");
        Serial.println(mobileInputString);
        alarmTime = mobileInputString;
        isAlarmOn = 1;
      }
    } else {
      /* Do somthing Default*/
      Serial.println(mobileInputString);
    }

  }

  delay(1000);
}

String makeNowTime2String() {
  String res = "";
  int h = rtc.getHours();
  int m = rtc.getMinutes();
  int s = rtc.getSeconds();

  if (h < 10) { res.concat("0"); }
  res.concat(String(h));

  if (m < 10) { res.concat("0"); }
  res.concat(String(m));

  if (s < 10) { res.concat("0"); }
  res.concat(String(s));
  return res;
}

int checkValidAlarmTime(String mobileInputString) {
  for (int i = 0; i < 6; i++) {
    if (!(mobileInputString[i] >= '0' && mobileInputString[i] <= '9')) {
      Serial.print("Wrong Str!");
      Serial.println(mobileInputString[i]);
      return 0;
    }
  }
  
  /* Valid Check */
  char buf[3];
  mobileInputString.substring(0,2).toCharArray(buf, 3);
  int hour = atoi(buf);
  if (!(0<=hour && hour<=23)){ return 0; }
    
  mobileInputString.substring(2,4).toCharArray(buf, 3);
  int minutes = atoi(buf);
  if (!(0<=minutes && minutes<=59)){ return 0; }
  
  mobileInputString.substring(4,6).toCharArray(buf, 3);
  int seconds = atoi(buf);
  if (!(0<=seconds && seconds<=59)){ return 0; }
  
  return 1;
}

void turnOnTheLight() {
  myServo.attach(servoPinNum);
  myServo.write(servoLightOnPosition);
  delay(1000);
  myServo.write(servoDefaultPosition);
  delay(1000);
  myServo.detach();
}

void turnOffTheLight() {
  myServo.attach(servoPinNum);
  myServo.write(servoLightOffPosition);
  delay(1000);
  myServo.write(servoDefaultPosition);
  delay(1000);
  myServo.detach();
}

void printNowTime() {
  Serial.print(rtc.getYear(), DEC);
  Serial.print("/");
  Serial.print(rtc.getMonth(), DEC);
  Serial.print("/");
  Serial.print(rtc.getDay(), DEC);
  Serial.print(" ");
  Serial.print(rtc.getHours(), DEC);
  Serial.print(":");
  Serial.print(rtc.getMinutes(), DEC);
  Serial.print(":");
  Serial.print(rtc.getSeconds(), DEC);
  Serial.print("\n");
}
