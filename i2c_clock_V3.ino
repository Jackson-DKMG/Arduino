#include <LowPower.h>
//#include <avr/sleep.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
#include <DS3231.h>
#include <Fonts/visitor218pt7b.h>
#include <Fonts/visitor27pt7b.h>
//#include <SPI.h>

#define OLED_ADDR 0x3C
//#define OLED_RESET 11
Adafruit_SH1106 display(-1);

DS3231 Clock;
byte time[7]; // Temp buffer used to hold BCD time/date values
char buf[12]; // Buffer used to convert time[] values to ASCII strings

bool Century = false;
bool h12;
bool PM;

bool dot = true;
bool screenOn = true;
byte timeOut = 0;
const unsigned int interval = 1000;
byte i = 0;
unsigned long newTime;
unsigned long curTime;
String DoW; // Day of week
String Y;   // Year
String M;   // Month
String D;   // Day
String h;   // hours
String m;   // minutes
String s;   // seconds
byte temp;  // temporary number for getting the date & time values
byte set;
bool stopSetup = false;
bool battWarning = false;
byte battLevel;
byte getBattInterval = 0;
byte Am; // alarm minutes
byte Ah; // alarm hours
byte AD;
byte AM;
bool A2Dy = false;
byte AlarmBits;
int AY;
String AlarmM;
String AlarmH;
String AlarmD;
unsigned long interruptTime = 0;

void setup() {

  getBattLevel();
  if (battLevel < 2) {
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  Wire.begin();
  // Serial.begin(9600);
  Clock.setClockMode(false);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(7, OUTPUT);
  pinMode(12, OUTPUT);  // alarm / low power led
  // digitalWrite(7, HIGH);  //switch + led. Proudly introducing "The Craplight"

  // attachInterrupt(digitalPinToInterrupt(3), vibrate, RISING);
  // attachInterrupt(digitalPinToInterrupt(4), lightsOn, RISING);   THERE ARE NO
  // FREAKING INTERRUPTS EXCEPT ON 2 AND 3 !!
  // attachInterrupt(digitalPinToInterrupt(5), vibrate, RISING);
  display.begin(SH1106_SWITCHCAPVCC);
  // display.dim(true);
  display.setFont(&visitor27pt7b);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 40);
  display.print("starting");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  // PORTB &= ~_BV(PB5);

  if (Clock.checkAlarmEnabled(2) == true) {
    Clock.getA2Time(AD, Ah, Am, AlarmBits, A2Dy, h12, PM);
    if (AD < 10) {
      AlarmD = '0' + String(AD);
    } else {
      AlarmD = String(AD);
    }
    if (Ah < 10) {
      AlarmH = '0' + String(Ah);
    } else {
      AlarmH = String(Ah);
    }
    if (Am < 10) {
      AlarmM = '0' + String(Am);
    } else {
      AlarmM = String(Am);
    }
  }
  if (battLevel < 16) {
    battWarning = true;
    lowBat();
  }
}

void loop() {

  // Serial.println(screenOn);
  if (Clock.checkIfAlarm(A2) == true) {
    alarm();
  }

  getBattInterval = getBattInterval + 1;
  if (getBattInterval == 12) {
    getBattInterval = 0;
    getBattLevel();
    if (battWarning == false && battLevel < 16) {
      battWarning = true;
      lowBat();
    }
    if (battWarning == true && battLevel > 17) {
      battWarning = false;
    }
  }

  // Serial.println(getBattInterval);
  // Serial.println(battLevel);

  if (screenOn == true) {

    curTime = millis();

    if (dot == true) {
      dot = false;
    } else {
      dot = true;
    }

    if (digitalRead(5) == LOW) {
      for (int i = 0; i < 26; i++) {
        delay(50);
        if (digitalRead(5) == HIGH) {
          break;
        }
        if (i == 25) {
          vibrate();
          setTimeDate();
        }
      }
    }

    if (digitalRead(4) == LOW) {
      for (int i = 0; i < 26; i++) {
        delay(50);
        if (digitalRead(4) == HIGH) {
          break;
        }
        if (i == 25) {
          vibrate();
          setAlarm();
        }
      }
    }

    temp = Clock.getDoW();
    switch (temp) {
    case 1:
      DoW = "Sun";
      break;
    case 2:
      DoW = "Mon";
      break;
    case 3:
      DoW = "Tue";
      break;
    case 4:
      DoW = "Wed";
      break;
    case 5:
      DoW = "Thu";
      break;
    case 6:
      DoW = "Fri";
      break;
    case 7:
      DoW = "Sat";
      break;
    }

    temp = Clock.getMonth(Century);
    switch (temp) {
    case 1:
      M = "Jan";
      break;
    case 2:
      M = "Feb";
      break;
    case 3:
      M = "Mar";
      break;
    case 4:
      M = "Apr";
      break;
    case 5:
      M = "May";
      break;
    case 6:
      M = "Jun";
      break;
    case 7:
      M = "Jul";
      break;
    case 8:
      M = "Aug";
      break;
    case 9:
      M = "Sep";
      break;
    case 10:
      M = "Oct";
      break;
    case 11:
      M = "Nov";
      break;
    case 12:
      M = "Dec";
      break;
    }

    temp = Clock.getDate();
    if (temp < 10) {
      D = '0' + String(temp);
    } else {
      D = String(temp);
    }
    Y = String(Clock.getYear() + 2000);

    temp = Clock.getHour(h12, PM);
    if (temp < 10) {
      h = '0' + String(temp);
    } else {
      h = String(temp);
    }
    temp = Clock.getMinute();
    if (temp < 10) {
      m = '0' + String(temp);
    } else {
      m = String(temp);
    }

    display.clearDisplay();
    display.setFont(&visitor27pt7b);
    display.setTextSize(1);

    display.setCursor(0, 10);
    display.print(DoW + ',' + ' ' + M + ' ' + D + ',' + ' ' + Y);

    if (Clock.checkAlarmEnabled(2) == true) {
      display.setCursor(0, 62);
      display.print(AlarmH + ':' + AlarmM + ' ' + '-' + '-' + AlarmD);
    }

    display.setCursor(100, 62);
    display.print(battLevel);
    display.print("%");

    display.setCursor(0, 45);
    display.setFont(&visitor218pt7b);
    display.print(h + ':' + m);
    if (dot == true) {
      display.setCursor(120, 45);
      display.print('.');
    }
    display.display();

    while (millis() - curTime < interval) {
      delay(50);
    }
    timeOut = timeOut + 1;
    if (timeOut == 8) {
      screenOn = false;
      display.clearDisplay();
      display.display();
    }
  } else {
    // LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
    attachInterrupt(0, screenToggle, LOW);
    LowPower.idle(SLEEP_2S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
                  SPI_OFF, USART0_OFF, TWI_OFF);
  }
}

void screenToggle() {

  if (millis() - interruptTime > 200) {
    if (screenOn == true) {
      attachInterrupt(0, screenToggle, LOW);
      screenOn = false;
      timeOut = 7;
      // display.clearDisplay();
      // display.display();
    } else {
      detachInterrupt(0);
      screenOn = true;
      timeOut = 0;
    }
  }
  interruptTime = millis();
  vibrate();
}

void vibrate() { // haptic feedback
  if (millis() - interruptTime > 50) {
    PORTD |= _BV(PD7); //= digitalWrite(7, HIGH), by changing the port register.
                       //Faster than digitalWrite, otherwise wouldn't work
    delay(50);
    PORTD &= ~_BV(PD7); // digitalWrite(7, LOW);
  }
  interruptTime = millis();
}

void alarm() {
  display.clearDisplay();
  display.setCursor(15, 45);
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.print("ALARM");
  display.display();
  while (1) {
    digitalWrite(7, HIGH);
    digitalWrite(12, HIGH);
    delay(200);
    digitalWrite(7, LOW);
    digitalWrite(12, LOW);
    delay(75);
    digitalWrite(7, HIGH);
    digitalWrite(12, HIGH);
    delay(200);
    digitalWrite(7, LOW);
    digitalWrite(12, LOW);
    delay(75);
    digitalWrite(7, HIGH);
    digitalWrite(12, HIGH);
    delay(200);
    digitalWrite(7, LOW);
    digitalWrite(12, LOW);
    delay(500);
    digitalWrite(12, LOW);
    
    if (digitalRead(2) == LOW) {
      vibrate();
      Clock.turnOffAlarm(2);
      delay(1000);
      screenOn = true;
      timeOut = 0;
      return;
    }

    else if (digitalRead(3) == LOW) {
      vibrate();
      display.clearDisplay();
      display.setCursor(5, 45);
      display.print("SNOOZE");
      display.display();
      delay(2000);

      Am = Clock.getMinute() + 10;

      if (Am > 59) {
        Am = Am - 60;
      }
      Ah = Clock.getHour(h12, PM);
      AD = Clock.getDate();
      if (Am < 10) {
        Ah = Ah + 1;
      }
      if (Ah > 23) {
        Ah = 0;
        AD = AD + 1;
      }

      AM = Clock.getMonth(Century);

      if (AD > 28 && AM == 2) {
        AY = Clock.getYear() + 2000;
        if (AY % 4 == 0) {
          if (AY % 100 != 0) {
            AD = 1;
          } else {
            if (AY % 400 == 0) {
              AD = 1;
            }
          }
        }
      }

      else if (AD == 31) {
        if (AM == 4 || AM == 6 || AM == 9 || AM == 11) {
          AD = 1;
        }
      }

      if (Ah < 10) {
        AlarmH = '0' + String(Ah);
      } else {
        AlarmH = String(Ah);
      }

      if (Am < 10) {
        AlarmM = '0' + String(Am);
      } else {
        AlarmM = String(Am);
      }

      if (AD < 10) {
        AlarmD = '0' + String(AD);
      } else {
        AlarmD = String(AD);
      }

      Clock.setA2Time(AD, Ah, Am, 0x0, false, false, false);
      Clock.turnOnAlarm(2);
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void setTimeDate() {
  detachInterrupt(0);
  display.setFont(&visitor27pt7b);
  display.setTextSize(2);
  display.clearDisplay();
  display.display();
  display.setCursor(5, 40);
  display.print("Set time");
  display.display();
  delay(2000);
  stopSetup = false;
  display.clearDisplay();
  display.display();
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  delay(400);

  set = Clock.getHour(h12, PM);
  changeRTC_hours(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setHour(set);
  display.clearDisplay();
  display.display();
  delay(400);

  set = Clock.getMinute();
  changeRTC_minutes(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setMinute(set);
  Clock.setSecond(0);
  display.clearDisplay();
  display.display();
  delay(400);

  set = Clock.getDate();
  changeRTC_date(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setDate(set);
  display.clearDisplay();
  display.display();
  delay(400);

  set = Clock.getMonth(Century);
  changeRTC_month(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setMonth(set);
  display.clearDisplay();
  display.display();
  delay(400);

  set = Clock.getYear();
  changeRTC_year(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setYear(set);
  display.clearDisplay();
  display.display();
  delay(400);

  set = Clock.getDoW();
  changeRTC_DoW(set);
  if (stopSetup == true) {
    return;
  }
  Clock.setDoW(set);
  display.clearDisplay();
  display.display();
  delay(400);

  screenOn = true;
  timeOut = 0;
  display.clearDisplay();
  display.display();
  delay(400);
}

void setAlarm() {
    detachInterrupt(0);
    display.setFont(&visitor27pt7b);
    display.setTextSize(2);
    display.clearDisplay();
    display.display();
    display.setCursor(40, 25);
    display.print("Set");
    display.setCursor(25, 50);
    display.print("Alarm");
    display.display();
    delay(2000);
    stopSetup = false;
    display.clearDisplay();
    display.display();
    display.setFont(&visitor218pt7b);
    display.setTextSize(1);
    delay(400);

    set = Clock.getHour(h12, PM);
    //Serial.println(String(set));
    changeRTC_hours(set);
    if (stopSetup == true) {
      Clock.turnOffAlarm(2);
      screenOn = true;
      timeOut = 0;
      return;
    }

    Ah = set;
    if (Ah < 10) {
      AlarmH = '0' + String(Ah);
    }
    else {
      AlarmH = String(Ah);
    }
    display.clearDisplay();
    display.display();
    delay(400);

    set = Clock.getMinute();
    changeRTC_minutes(set);
    if (stopSetup == true) {
      Clock.turnOffAlarm(2);
      screenOn = true;
      timeOut = 0;
      return;
    }

    Am = set;
    if (Am < 10) {
      AlarmM = '0' + String(Am);
    }
    else {
      AlarmM = String(Am);
    }
    display.clearDisplay();
    display.display();
    delay(400);

    set = Clock.getDate();
    changeRTC_date(set);
    if (stopSetup == true) {
      Clock.turnOffAlarm(2);
      screenOn = true;
      timeOut = 0;
      return;
    }

    AD = set;
    if (AD < 10) {
      AlarmD = '0' + String(AD);
    }
    else {
      AlarmD = String(AD);
    }
    Clock.setA2Time(AD, Ah, Am, 0x0, false, false, false);
    Clock.turnOnAlarm(2);

    screenOn = true;
    timeOut = 0;
    display.clearDisplay();
    display.display();
    delay(400);
}

void changeRTC_minutes(byte x) {

  display.clearDisplay();
  display.setCursor(10, 40);
  display.setFont(&visitor27pt7b);
  display.setTextSize(1);
  display.print("Minutes: ");
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.setCursor(70, 40);

  if (x < 10) {
    display.print('0' + String(x));
  } else {
    display.print(String(x));
  }
  display.display();

  while (1) {
    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      if (x > 59) {
        x = 0;
      }
      display.clearDisplay();
      display.setCursor(10, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);

      display.print("Minutes: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);

      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(5) == LOW) {
      delay(150);
      vibrate();
      x = x - 1;
      if (x == 255) {
        x = 59;
      }
      display.clearDisplay();
      display.setCursor(10, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);

      display.print("Minutes: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);

      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      delay(150);
      vibrate();
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void changeRTC_hours(byte x) {

  display.clearDisplay();
  display.setCursor(15, 40);
  display.setFont(&visitor27pt7b);
  display.setTextSize(1);
  display.print("Hours: ");
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.setCursor(70, 40);

  if (x < 10) {
    display.print('0' + String(x));
  } else {
    display.print(String(x));
  }
  display.display();
  while (1) {
    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      if (x > 23) {
        x = 0;
      }
      display.clearDisplay();
      display.setCursor(15, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Hours: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(5) == LOW) {
      delay(150);
      vibrate();
      x = x - 1;
      if (x == 255) {
        x = 23;
      }
      display.clearDisplay();
      display.setCursor(15, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Hours: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      vibrate();
      delay(150);
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void changeRTC_month(byte x) {

  display.clearDisplay();
  display.setCursor(20, 40);
  display.setFont(&visitor27pt7b);
  display.setTextSize(1);
  display.print("Month: ");
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.setCursor(70, 40);
  if (x < 10) {
    display.print('0' + String(x));
  } else {
    display.print(String(x));
  }
  display.display();

  while (1) {
    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      if (x > 12) {
        x = 1;
      }
      display.clearDisplay();
      display.setCursor(20, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Month: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(5) == LOW) {
      delay(150);
      vibrate();
      x = x - 1;
      if (x == 255) {
        x = 12;
      }
      display.clearDisplay();
      display.setCursor(20, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Month: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(70, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      delay(150);
      vibrate();
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void changeRTC_DoW(byte x) {

  display.clearDisplay();
  display.setCursor(30, 40);

  switch (x) {
  case 1:
    DoW = "Sun";
    break;
  case 2:
    DoW = "Mon";
    break;
  case 3:
    DoW = "Tue";
    break;
  case 4:
    DoW = "Wed";
    break;
  case 5:
    DoW = "Thu";
    break;
  case 6:
    DoW = "Fri";
    break;
  case 7:
    DoW = "Sat";
    break;
  }
  display.print(DoW);
  display.display();
  while (1) {
    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      if (x > 7) {
        x = 1;
      }
      display.clearDisplay();
      display.setCursor(30, 40);

      switch (x) {
      case 1:
        DoW = "Sun";
        break;
      case 2:
        DoW = "Mon";
        break;
      case 3:
        DoW = "Tue";
        break;
      case 4:
        DoW = "Wed";
        break;
      case 5:
        DoW = "Thu";
        break;
      case 6:
        DoW = "Fri";
        break;
      case 7:
        DoW = "Sat";
        break;
      }

      display.print(DoW);
      display.display();
    } else if (digitalRead(5) == LOW) {
      delay(150);
      vibrate();
      x = x - 1;
      if (x == 255) {
        x = 7;
      }
      display.clearDisplay();
      display.setCursor(30, 40);

      switch (x) {
      case 1:
        DoW = "Sun";
        break;
      case 2:
        DoW = "Mon";
        break;
      case 3:
        DoW = "Tue";
        break;
      case 4:
        DoW = "Wed";
        break;
      case 5:
        DoW = "Thu";
        break;
      case 6:
        DoW = "Fri";
        break;
      case 7:
        DoW = "Sat";
        break;
      }

      display.print(DoW);
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      delay(150);
      vibrate();
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void changeRTC_year(byte x) {
  display.clearDisplay();
  display.setCursor(1, 40);
  display.setFont(&visitor27pt7b);
  display.setTextSize(1);
  display.print("Year: ");
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.setCursor(40, 40);
  display.print(2000 + x);
  display.display();

  while (1) {

    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      display.clearDisplay();
      display.setCursor(1, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Year: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(40, 40);
      display.print(2000 + x);
      display.display();
    } else if (digitalRead(5) == LOW) {
      delay(150);
      vibrate();
      x = x - 1;
      display.clearDisplay();
      display.setCursor(1, 40);
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.print("Year: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(40, 40);
      display.print(2000 + x);
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      delay(150);
      vibrate();
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
}

void changeRTC_date(byte x) {

  display.clearDisplay();
  display.setCursor(25, 40);
  display.setFont(&visitor27pt7b);
  display.setTextSize(1);
  display.print("Day: ");
  display.setFont(&visitor218pt7b);
  display.setTextSize(1);
  display.setCursor(60, 40);
  if (x < 10) {
    display.print('0' + String(x));
  } else {
    display.print(String(x));
  }
  display.display();

  while (1) {

    if (digitalRead(4) == LOW) {
      delay(150);
      vibrate();
      x = x + 1;
      if (x > 31) {
        x = 1;
      }
      display.clearDisplay();
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.setCursor(25, 40);
      display.print("Day: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(60, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(5) == LOW) {
      vibrate();
      delay(150);
      x = x - 1;
      if (x == 0) {
        x = 31;
      }
      display.clearDisplay();
      display.setFont(&visitor27pt7b);
      display.setTextSize(1);
      display.setCursor(25, 40);
      display.print("Day: ");
      display.setFont(&visitor218pt7b);
      display.setTextSize(1);
      display.setCursor(60, 40);
      if (x < 10) {
        display.print('0' + String(x));
      } else {
        display.print(String(x));
      }
      display.display();
    } else if (digitalRead(3) == LOW) {
      delay(150);
      vibrate();
      stopSetup = true;
      screenOn = true;
      timeOut = 0;
      break;
    } else if (digitalRead(2) == LOW) {
      delay(150);
      vibrate();
      set = x;
      screenOn = true;
      timeOut = 0;
      return;
    }
  }
  display.clearDisplay();
  display.display();
  delay(400);
}

unsigned int analogReadReference(void) {

  /* Elimine toutes charges résiduelles */
  ADMUX = 0x4F;
  delayMicroseconds(5);

  /* Sélectionne la référence interne à 1.1 volts comme point de mesure, avec
   * comme limite haute VCC */
  ADMUX = 0x4E;
  delayMicroseconds(200);

  /* Active le convertisseur analogique -> numérique */
  ADCSRA |= (1 << ADEN);

  /* Lance une conversion analogique -> numérique */
  ADCSRA |= (1 << ADSC);

  /* Attend la fin de la conversion */
  while (ADCSRA & (1 << ADSC))
    ;

  /* Récupère le résultat de la conversion */
  return ADCL | (ADCH << 8);
}

void getBattLevel() {

  float batteryVoltage = (1023 * 1.1) / analogReadReference();
  battLevel = round(((((1023 * 1.1) / analogReadReference()) - 2.75) / 1.45) *
                    100); // Vcur - Vmin / Vmax (4.2V) - Vmin (2.75V, board
                          // stops working below that), rounded, percentage
  if (battLevel < 2) {
    display.clearDisplay();
    display.display();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }
  // Serial.println(batteryVoltage);

  if (battLevel > 100) {
    battLevel = 100;
  }
}

void lowBat() {
  display.clearDisplay();
  display.display();
  display.setCursor(40, 25);
  display.setFont(&visitor27pt7b);
  display.setTextSize(2);
  display.print("LOW");
  display.setCursor(10, 50);
  display.print("BATTERY");
  display.display();
  PORTD |= _BV(PD7); //= digitalWrite(7, HIGH), by changing the port register.
                     //Faster than digitalWrite, otherwise wouldn't work
  digitalWrite(12, HIGH);
  delay(500);
  PORTD &= ~_BV(PD7);
  digitalWrite(12, LOW);
  delay(200);
  PORTD |= _BV(PD7);
  digitalWrite(12, HIGH);
  delay(500);
  PORTD &= ~_BV(PD7);
  digitalWrite(12, LOW);
  delay(2000);
  display.clearDisplay();
  display.display();
  screenOn = true;
  timeOut = 0;
}
