//Paul Oh

//servo variables

int pos = 0; //servo position
int dir = 0; //direction of the servo

//LED variables
int ledPins[] = {11, 12, 13}; //3 LED pins to indicate modes

//motor: Standard PWM DC control
int E1 = 5; //M1 Speed Control
int E2 = 6; //M2 Speed Control
int M1 = 4; //M1 Direction Control
int M2 = 7; //M1 Direction Control

//button variables
const int buttonPins[] = {3, 2}; //2 buttons used for the project
const int adc_key_val[2] ={200, 400}; //maximum ADC values for each button, 200 is button 1, 400 is button 2
int adc_key_in; //ADC value that is inputted
int key = -1; //the button that is pressed, initialized as not pressed
const int NUM_KEYS = 2; //2 buttons in total

//ultrasonic variables
int trig_pin = 9; //ultrasonic pin
float range; //actual distance from the wall known by ultrasonic

void setup(){ //setup for all 4 motors
  for(int i = 4; i < 8; i++)
    pinMode(i, OUTPUT); //4 motor setup as output
  for(int i = 0; i < 3; i++)
    pinMode(ledPins[i], OUTPUT); //attach all 3 LED as outputs
  Serial.begin(9600); //serial monitor
  myservo.attach(3); //attaching standard servo
  pinMode(trig_pin, OUTPUT); //ultrasonic sensor as output
  digitalWrite(trig_pin, HIGH); // Trig pin is normally HIGH
} 

void loop(){
  detect(); //sets the mode of the vehicle, turns the servo, and detects objects with the ultrasonic sensor
}

void detect(){
  for(pos = 0; pos < 180; pos++){ //the servo turns clockwise till pos is 180
    for (int i = 0; i < 100; i++){//check 50 times in case it was not detected
      buttonSetting(); //detect if the button was pressed
    }
    ledMode(); //after the buttonSetting, the LED lights up according to the mode that this is in
    myservo.write(pos); //servo moves according to pos
    delay(10);
    if (distance() < 20){ //if the ultrasonic detects an object within 20 cm
      if (key == 0){
        gogo(); //advance mode goes forward
      }
      else if (key == 1){
        backback(); //back mode goes backward
      //nothing happens if none of the buttons are pressed
      }
    }
    digitalWrite(ledPins[0], LOW); //the yellow light, which indicates if the vehicle is detecting something, goes off
  }
  for(pos = 180; pos>=1; pos--){ //the servo turns counter-clockwise till pos is 0
    for (int i = 0; i < 50; i++){ //check 50 times in case it was not detected
      buttonSetting(); //detect if the button was pressed
    }
    ledMode(); //after the buttonSetting, the LED lights up according to the mode that this is in
    myservo.write(pos); //servo moves according to 
    delay(10);
    if (distance() < 20){ //if the ultrasonic detects an object within 20 cm
      if (key == 0){
        gogo(); //advance mode goes forward
      }
      else if (key == 1){
        backback(); //back mode goes backward
      } 
      //nothing happens if none of the buttons are pressed
    }
    digitalWrite(ledPins[0], LOW); //the yellow light, which indicates if the vehicle is detecting something, goes off
  }
}

void buttonSetting(){ //sets the mode of the button
  //setting the mode of the buttons
  //button 1 means advance in the correct direction when ultrasonic detects
  //button 2 means back off in the correct direction when ultrasonic detects
  adc_key_in = analogRead(0); //read ADC values for each button
  if (adc_key_in < 1000){ //if a key is pressed
    //this if statement is used so that the key does not change unless something is pressed
    key = get_key(adc_key_in); //key is recognized and the mode is chosen, cannot be altered until another button is pressed
  }
  //nothing happens if none of the buttons are pressed
}

void ledMode(){
  if (key == 0){ //indicates mode 1
    digitalWrite(ledPins[2], HIGH); //Red LED turns on
    digitalWrite(ledPins[1], LOW); //Blue LED turns off
   }
  else if (key == 1){ //indicates mode 2
    digitalWrite(ledPins[1], HIGH); //Blue LED turns on
    digitalWrite(ledPins[2], LOW); //Red LED turns off
  }
}

//the vehicle goes to the direction of where it detected the object
void gogo(){ //Mode 1, going forward method
  while (distance() < 20){ //when it detects an object within 30 cm
    show (distance()); //show how far away the object is on the serial monitor
    int turning = pos-90; //if motor is at left, turning is negative, if motor is at right, turning is positive
    Serial.println(turning);
    if (turning <= 0){ //when negative, turns left, when 0, go straight forwards
      digitalWrite(ledPins[0], HIGH); //Yellow LED (detecting led) turns on
      advance(255, 255+turning*2); //turn smoothly towards the left direction forward
    }
    else if (turning > 0){ //when positive, turns right
      digitalWrite(ledPins[0], HIGH); //Yellow LED (detecting led) turns on
      advance(255-turning*2, 255); //turn smoothly towards the right direction forward
    }
  }
  stopped(); //stop the motor once the ultrasonic doesn't detect anything
}

//the vehicle moves back in the direction of where it detected the object
void backback(){ //Mode 2, going backward method
  while (distance() < 20){ //when it detects an object within 30 cm
    show (distance()); //show how far away the object is on the serial monitor
    int turning = pos-90; //if motor is at left, turning is negative, if motor is at right, turning is positive
    Serial.println(turning);
    if (turning <= 0){ //when negative, turns left, when 0, go straight backwards
      digitalWrite(ledPins[0], HIGH); //Yellow LED (detecting led) turns on
      back_off(255+turning*2, 255); //turn smoothly towards the left direction backward
    }
    else if (turning > 0){ //when positive, turns right
      digitalWrite(ledPins[0], HIGH); //Yellow LED (detecting led) turns on
      back_off(255, 255-turning*2); //turn smoothly towards the right direction backward
    }
  }
  stopped(); //stop the motor once the ultrasonic doesn't detect anything
}

void show(float range){ //display the distance from the object
  Serial.print(" Distance: ");
  Serial.print(range);
  Serial.println(" cm");
}

//ultrasonic sensor distance calculating method
float distance(){
  unsigned long time_1;
  unsigned long sizeofpulse;
  float range;

  pinMode(trig_pin, OUTPUT); //return digital pin to OUTPUT mode after reading
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(25);
  digitalWrite(trig_pin, HIGH); //Trig pin pulsed LOW for 25usec
  time_1 = micros(); //record timer
  pinMode(trig_pin, INPUT); //change pin to INPUT to read the echo pulse 
  sizeofpulse = pulseIn(trig_pin, LOW, 18000); //should be approx 150usec, timeout at 18msec
  time_1 = micros() - time_1 - sizeofpulse; // amount of time elapsed since we sent the trigger pulse and detect the echo pulse, then subtract the size of the echo pulse
  range = (time_1*340.29/2/10000)-3; // convert to distance in centimeters
  return range; //distance in cm
}

//motor methods
void stopped(){ //stop method
  digitalWrite(E1,LOW);
  digitalWrite(E2,LOW);      
}

void advance(char a,char b){ //move forward
  analogWrite (E1,a);  
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void back_off (char a,char b){ //goes backward
  analogWrite (E1,a);
  digitalWrite(M1,LOW);   
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

void turn_L (char a,char b){ //turns left
  analogWrite (E1,a);
  digitalWrite(M1,LOW);    
  analogWrite (E2,b);    
  digitalWrite(M2,HIGH);
}

void turn_R (char a,char b){ //turns right
  analogWrite (E1,a);
  digitalWrite(M1,HIGH);    
  analogWrite (E2,b);    
  digitalWrite(M2,LOW);
}

//button reader method
int get_key(unsigned int input){ //convert ADC value to key number
  int k; //declare variable
  for (k = 0; k < NUM_KEYS; k++){ //goes through all keys
    if (input < adc_key_val[k]){ //when the ADC value of the reading is lower than the maximum ADC value for the button
      return k; //returns the key value
    }
  }
  if (k >= NUM_KEYS) //none of the keys are pressed
    k = -1; //set k to a non-positive integer
  return k; //return k
}
