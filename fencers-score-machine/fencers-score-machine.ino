
#define buzzer 9
#define Led 6

#define WeaponOut 2

int WeaponState = 0;

void setup() {
  
  pinMode(WeaponOut, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // sound / led checkup
  tone(buzzer,1500);
  digitalWrite(Led, HIGH);
  delay(300);
  noTone(buzzer);
  digitalWrite(Led, LOW);

}

void loop() {

  WeaponState = digitalRead(WeaponOut);

  if (WeaponState == 1)
  {
    ActivateBuzzer();
    ActivateLED();
    Report_toServer(); 
  }

}

void ActivateBuzzer()
{
  tone(buzzer,1500);
  delay(1000);
  noTone(buzzer);
  
}

void ActivateLED()
{
  digitalWrite(Led, HIGH);
  delay(5000);
  digitalWrite(Led, LOW);
}

void Report_toServer()
{
  
}

