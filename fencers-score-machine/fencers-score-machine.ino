
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

    Report_toServer(); 
    ActivateDisplays();
    RestartBout(); 
  }

  delay(1);

}

void ActivateDisplays() // activating sound 
{
  tone(buzzer,1500);
  digitalWrite(Led, HIGH);
  delay(5000);
  noTone(buzzer);
  delay(5000);
  digitalWrite(Led, LOW);
}
void RestartBout() // reseting the equipment and resynching with the servers
{
  
  tone(buzzer,100);
  digitalWrite(Led, HIGH);
  delay(500);
  noTone(buzzer);
  digitalWrite(Led, LOW);

}

void Report_toServer()  //reporting to the central device monitoring both fencers and keeping score
{
  
}

