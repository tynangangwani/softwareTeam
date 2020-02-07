#define C1Pin 2
#define C2Pin 3
#define C3Pin 6
#define C4Pin 7
int interruptPins[4]={C1Pin, C2Pin, C3Pin, C4Pin}; //sets the array with appropriate pin number
//initializing the rest of the global vairables
volatile bool currentPinValue[4]={0,0,0,0};
volatile unsigned long startTime[4]={0,0,0,0};
volatile unsigned long endTime[4]={0,0,0,0};
volatile bool flag1[4]={false,false,false,false};
volatile bool flag2[4]={false,false,false,false};
int new_throttleValue[4]={0,0,0,0};
int throttleValue[4]={0,0,0,0};
//functions
void enable_interrupts(int i);
int calculate_throttle(int arr_num);
void interrupt(int arr_num); //measures PWM

void setup() {

  for(int i=0; i<4; i++){
    currentPinValue[i]=digitalRead(interruptPins[i]);//initializing the current pin values
    pinMode(interruptPins[i], INPUT);
    enable_interrupts(i);
  }

  Serial.begin(9600);
}

void loop() {
   for(int i=0; i<4;i++){
    if(flag2[i]&&flag1[i]){ //checking if a throttle value for a channel should be changed

      flag1[i]=false;
      flag2[i]=false;//resetting the flags
      new_throttleValue[i]=calculate_throttle(i); //calculating the new throttle
      if(new_throttleValue[i]!=throttleValue[i]){
        throttleValue[i]=new_throttleValue[i]; //updating the throttle if it changed
        Serial.print("pin ");
        Serial.print(interruptPins[i]);
        Serial.print(": ");
        Serial.println(throttleValue[i]);
    }

     }
   }

}

ISR (PCINT2_vect)  {//interrupt triggered when any of the D register pin changes
   for(int i=0; i<4; i++){
     if(currentPinValue[i]!=digitalRead(interruptPins[i])){//checks which one changed
     interrupt(i);
     }
  }
}

void enable_interrupts(int i){//enables interrupts
  *digitalPinToPCMSK(interruptPins[i]) |= bit (digitalPinToPCMSKbit(interruptPins[i]));  // enable pin
      PCIFR  |= bit (digitalPinToPCICRbit(interruptPins[i])); // clear any outstanding interrupt
      PCICR  |= bit (digitalPinToPCICRbit(interruptPins[i])); // enable interrupt for the group
}

int calculate_throttle(int arr_num){//finds the difference in the start time and end time of the rising
                                    //and falling edge respectively
 return ((int)(endTime[arr_num]-startTime[arr_num]-1000)/25);//makes it between 0-1000

}

void interrupt(int arr_num){ //measures pulse width modulation

  if((digitalRead(interruptPins[arr_num])==HIGH)&&(flag2[arr_num]==false)){
    flag1[arr_num]=true;
    currentPinValue[arr_num]=1;
    startTime[arr_num]=(int)micros();
  }

  if((digitalRead(interruptPins[arr_num])==LOW)&&(flag1[arr_num]==true)){
    endTime[arr_num]=(int)micros();
    currentPinValue[arr_num]=0;
    flag2[arr_num]=true;
  }
}
