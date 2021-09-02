/*
  Fade

  This example shows how to fade an LED on pin 9 using the analogWrite()
  function.

  The analogWrite() function uses PWM, so if you want to change the pin you're
  using, be sure to use another PWM capable pin. On most Arduino, the PWM pins
  are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/

int leds[] = {3, 5, 6, 9, 10, 11};

void setup() {
  
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  


int i = 0;
int j = random(10,31);
//Serial.println(j);
while (i < j) {
int pinNum = random(0,6);
analogWrite(leds[pinNum], random(0,25)*10);
Serial.println(leds[pinNum]);
delay(random(5,30)*10);
i = i+1;
//Serial.println(i);
}

for (int pin = 0; pin < 7; pin++) {
  analogWrite(leds[pin], 0);
  delay(random(3,15)*10);
}
//digitalWrite(LED_BUILTIN, LOW);
delay(random(9,22)*100);

}

