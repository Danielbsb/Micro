/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin (9600);
}

// the loop function runs over and over again forever
void loop() {
  
//int main(void) {
    int i = 0;
    int j = 0b00;
    int IN = 11;
    int LOAD = 12;
    int BIT2 = 0x04,BIT4 = 0x10;
    int sDATA = 0b00110011001100000;
    digitalWrite(13,LOW);
	    for(i = 0; i < 16; i++){
                if((BIT2 & (sDATA >> i))){
                  digitalWrite (11, HIGH);
                }else{
                  digitalWrite(11,LOW);
                }
	        if(i == 15){
	          digitalWrite(LOAD,HIGH);
                  j += 0b1;
                }
                if(digitalRead(13) == HIGH){
                  digitalWrite(13,LOW);
                }else{
                  digitalWrite(13,HIGH);
                }
                delay(50);
                Serial.println("IN:        LOAD:         CLK:");
                Serial.print(digitalRead(11));
                Serial.print("        ");
                Serial.print(digitalRead(12));
                Serial.print("        ");          
                Serial.print(digitalRead(13));
                Serial.println("        ");  
	    }
	    digitalWrite(LOAD,LOW);
//}
}

