#include <GSM.h>
#define PINNUMBER ""

GSM gsmAccess;
GSM_SMS sms;

const int MAX_MSG = 50;

char senderNumber[20];
char sms_buf[MAX_MSG]; 
char c;

int LPIROut = 0 ;
int RPIROut = 1 ;
int en_motor_1 = 5; //left motor. controlls speed
int motor1_A = 4;   //left motor +ve : together with -ve controls forward/backward direction for a motion
int motor1_B = 6;   //left motor -ve
int en_motor_2 = 9; //right motor
int motor2_A = 8;   //right motor +ve : together with -ve controls forward/backward direction for a motion
int motor2_B = 7;   //right motor -ve
int trigPin = 10;
int echoPin = 11;
int transPin = 12;
int BPIROut = 13;
int l_pir=0, r_pir=0, b_pir=0;

int TURNING_DURATION = 250; //in ms
int FWD_MOTION_DURATION = 250; //in ms
int BACKWARD_MOTION_DURATION = 350; //in ms

float duration;
float distance;

void setup() 
  {
    pinMode(LPIROut,INPUT);
    pinMode(RPIROut,INPUT);
    pinMode(BPIROut,INPUT);
    
    pinMode(en_motor_1, OUTPUT);
    pinMode(en_motor_2, OUTPUT);
    
    pinMode(motor1_A, OUTPUT);
    pinMode(motor1_B, OUTPUT);
    pinMode(motor2_A, OUTPUT);
    pinMode(motor2_B, OUTPUT);
    
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    pinMode(transPin, OUTPUT);
    
    //Serial.begin(9600);
      
  set_both_motors_speed_to_stationary();
    //analogWrite(en_motor_2,0);
    //analogWrite(en_motor_1,0);
}

void left_turn()
  {
    //set right wheel move direction to forward
    digitalWrite(motor2_A,1);    
    digitalWrite(motor2_B,0);
  
  //set right wheel move direction to none
    digitalWrite(motor1_A,0);
    digitalWrite(motor1_B,0);
  
    //set left wheel speed to 0
    analogWrite(en_motor_1,0);   
  
  //set right wheel speed to 150
    analogWrite(en_motor_2,150); 
  
   // Serial.println("Turning Left");
    delay(TURNING_DURATION); //make left turn for 200ms
  
  //Set both motors speed to zero
  set_both_motors_speed_to_stationary();
    //analogWrite(en_motor_1,0);
    //analogWrite(en_motor_2,0);
               
}

void right_turn()
  {
  //set left wheel move direction to forward
    digitalWrite(motor1_A,1);
    digitalWrite(motor1_B,0);
  
  //set right wheel move direction to none
    digitalWrite(motor2_A,0);
    digitalWrite(motor2_B,0);
  
   //set right wheel speed to 0
    analogWrite(en_motor_2,0);
  
  //set left wheel speed to 150
    analogWrite(en_motor_1,150);
  
    //Serial.println("Turning Right");
    delay(TURNING_DURATION);  //make left turn for 500ms
  
  //Set both motors speed to zero
  set_both_motors_speed_to_stationary();
    //analogWrite(en_motor_1,0);
    //analogWrite(en_motor_2,0);           
    
}

void evaluate_range_and_chase()
{
    //determine distance of an obstacle in front by calculating signal return time 
  digitalWrite(trigPin,HIGH);
  delay(1000);
  digitalWrite(trigPin,LOW);
  
  duration = pulseIn(echoPin,HIGH);
  distance = (duration/2)/29.1;
//  Serial.println("Distance="+distance);
  if(distance <= 50 && distance >10) //make a chase by moving forward since obstacle is winthin 50>range>10 
  {
//    Serial.println("Forward Intrusion");
    forward();
    send_RF(); //let buzzer receive signal to start alarm
  }
  else //the obstacle is outside of range, robo don't chase
  {
//    Serial.println("the obstacle is outside of range, robo doesn't chase");
    /* commented by Kausik
    analogWrite(en_motor_2,0);
    analogWrite(en_motor_1,0);
    */
  }
}

void forward()
  {
  //set left wheel move direction to forward
    digitalWrite(motor1_A,1);
    digitalWrite(motor1_B,0);
  
  //set right wheel move direction to forward
    digitalWrite(motor2_A,1);
    digitalWrite(motor2_B,0);
  
  //set right wheel speed to 150
    analogWrite(en_motor_2,150);
  
  //set left wheel speed to 150
    analogWrite(en_motor_1,150);
  
   // Serial.println("Moving Forward");
    delay(FWD_MOTION_DURATION); //move forward for 250 ms
  
  //Set both motors speed to zero
  set_both_motors_speed_to_stationary();
    //analogWrite(en_motor_1,0);
    //analogWrite(en_motor_2,0);    
  
  //send_RF(); //kausik has commented out. send_RF() is invoked from main loop now.
}


void turn_back()
  {
    
  //set left wheel move direction to backward
    digitalWrite(motor1_A,0);
    digitalWrite(motor1_B,1);
  
  //set right wheel move direction to forward 
    digitalWrite(motor2_A,1);
    digitalWrite(motor2_B,0);
  
    //set right wheel speed to 150
    analogWrite(en_motor_2,150);
  
  //set right wheel speed to 150
    analogWrite(en_motor_1,150);
 
  //  Serial.println("Turning Back");
    delay(BACKWARD_MOTION_DURATION); //make left turn for 500ms
  
  //Set both motors speed to zero
  set_both_motors_speed_to_stationary();
    //analogWrite(en_motor_1,0);
    //analogWrite(en_motor_2,0);
    
}


void send_RF()
  {
    digitalWrite(transPin,HIGH);
    delay(500);
    digitalWrite(transPin,LOW);
    //delay(500);     //kausik has commented out
  delay (100); //let's not take too long to reduce the signal ato LOW nd make the robo wait total 500+500 ms for send_RF()
   }

void set_both_motors_speed_to_stationary(){
  analogWrite(en_motor_1,0);
    analogWrite(en_motor_2,0);
}

void alarm()
  {
    sms.beginSMS(""); //Insert user's phone number
    sms.print("intruder alert..should the police be informed");
    sms.endSMS();
}
void alarm_police()
  {
    sms.beginSMS(""); //Insert police contact number
    sms.print("intruder detected at *address*");
    sms.endSMS();
}

void loop() 
  {
    l_pir = digitalRead(LPIROut);
    r_pir = digitalRead(RPIROut);
    b_pir = digitalRead(BPIROut);
  

    //Print PIR states
//    Serial.println(l_pir+", "+r_pir+", "+b_pir, DEC); //print all variables in DECIMAL number system 
    
    
    if ( l_pir == HIGH && r_pir == LOW  ) //equivalent to only-left=HIGH or both Left & back=high
    {
      left_turn();
    }
    else if ( l_pir == LOW && r_pir == HIGH ) //equivalent to only-Right=HIGH or both Right & back=high
    {
      right_turn();
    }
    else if ( (l_pir == HIGH && r_pir == HIGH & b_pir == HIGH) || (l_pir == LOW && r_pir == LOW & b_pir == HIGH)  ) 
    {
      turn_back();
    }
    else if ( l_pir == HIGH && r_pir == HIGH & b_pir == LOW) 
    {
      evaluate_range_and_chase();
    }
    else
    {
      Serial.println("from main loop: do nothing");
      //do nothing
    }
    
        if (transPin == HIGH) {
          alarm();
          }

  /*  if (sms.available()) {    // this part should be in loop()
  sms.remoteNumber(senderNumber, 20);
  if (strcmp(senderNumber, "") == 0) { // compare numbers //Insert Sender Ph Number
    while (sms.available() == 0);   // wait for a char
    char i = 0;
    while (i < MAX_MSG - 1){    // till max len is reached; 1 byte for null
      c = sms.read();
      if (c == '\0')  // assuming the modem terminates sms with null
        break;
      sms_buf[i++] = c;   // write char to the buffer
      while (sms.available() == 0);   //wait for a char
    }
    sms_buf[i] = 0;   // terminate the string, just in case
    if (strstr(sms_buf, "yes") != NULL)  // look for 'yes' in buffer
      alarm_police();
  }
  
  }*/
    }
