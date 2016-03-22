

 
#include "fonts.h"

// LED MAtrix
int latchPin = 2;
int clockPin = 3;
int dataPin = 4;
int clock = 9;
int Reset = 8;
int latchPinPORTB = latchPin - 8;

int i = 0;
char msg[] = "";
char msgD2[] = "The number of books lent is: 11";
char msgD3[] = "Not bad at all";
char msgD4[] = "Nothing is better than a good book";
char msgD5[] = "Open a book, Open a world";
char msgD6[] = "The number of books lent is: 1l";
char msgD7[] = "Light up another by borrow 3 more books";
char msgD8[] = "Light up another by borrow 2 more books";
char msgD9[] = "Light up another by borrow 1 more books";
char* stringBuffer[]={"North,", "East,", "South,", "West,", "Rokka", "Spot", "knows", "best! ", " yay!"};
int msgLen = sizeof(msg);
char msgBuffer[58];
byte bitmap[8][5]; // Change 5 to how much matrices you want to use

int numZones = sizeof(bitmap) / 8; // one zone equals a led module
int maxZoneIndex = numZones - 1;
int numCols = numZones * 8; // total columns
int scrollSpeed = 4;

// Counters
int loopCounter2 = 0;

//Added by Chinh for testingn
int incomingByte = 0;
char inData[2]; // Allocate some space for the string
char inChar=-1; // Where to store the character read
byte index = 0;
String content = "";
char character;
char char_array[24];

void setup()
{ 
  // Only for Debug
  Serial.begin(19200);
  // delay(400);
  pinMode(13,OUTPUT);
  
  // Init LED Matrix 
  pinMode(dataPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(latchPin,OUTPUT);
  pinMode(clock,OUTPUT);
  pinMode(Reset,OUTPUT);
  // Reset Row Counter
  digitalWrite(Reset,HIGH);
  digitalWrite(Reset,LOW);
   
  // Set welcome message
  memcpy((uint8_t *)msgBuffer, (uint8_t *)msg, (sizeof(msgBuffer)));
  
}


boolean testState = true;
int stringSelect = 0;
int timeGone = millis();
char timeGoneStr[18];


void displayTimer(){
 
  // timeGoneLost();
   //RefreshDisplay();                                              
   display_word(msgBuffer, msgLen ,scrollSpeed);// calls for the display_pattern function and says that int loop = 15(if you do more loop the pattern whould scrole slower).
   //scrollText( numCols, msgBuffer);// calls for the display_pattern function and says that int loop = 15(if you do more loop the pattern whould scrole slower). 
}

void timeGoneLost(){
  timeGone = millis()- timeGone;
  sprintf(timeGoneStr, "%d", timeGone);
  drawText(8, timeGoneStr);  
  timeGone = millis();
}


// Draw to LED Matrix
void RefreshDisplay(){
   
  for(int row = 0; row < 8; row++){ // scanning the display
   
    if(row  == 1){// 4017 Counter counts till 10 so we have to reset 
      digitalWrite(Reset,HIGH);
      digitalWrite(Reset,LOW);
    }
  
    digitalWrite(latchPin, LOW);
    for(int zone = maxZoneIndex; zone >= 0; zone--){ // For each matrix module
      //byte data = bitmap[row][zone];
      //spi_transfer(data);
      shiftOut(dataPin, clockPin, LSBFIRST, bitmap[row][zone]); // Transmit Data
    }
   
    digitalWrite(latchPin, HIGH);
    //delayMicroseconds(10);
  
    digitalWrite(clock,HIGH);//counting up with the 4017
    digitalWrite(clock,LOW);
    
  
 }
}


void Plot(int col, int row, bool isOn){
  int zone = col / 8;
  int colBitIndex = col % 8;
  byte colBit = 1 << colBitIndex;
  
  if(isOn)
    bitmap[row][zone] = bitmap[row][zone] | colBit;
  else
    bitmap[row][zone] =  bitmap[row][zone] & (~colBit);
}

// this function displays your symbols
void display_word(char* lmsg, int len, int delay_length){
  i = 0;  // resets the counter for the 4017
  
  for(int charIndex = 0; charIndex < len; charIndex++ ){ // Count up Characters
    int alphabetIndex = lmsg[charIndex] - FONT_START;
    if(alphabetIndex < 0) alphabetIndex = 0;
   
    for(int col = 0; col < FONT_WIDTH + FONT_SPACE; col++){
      for(int row = 0; row < 8; row++){
        bool isOn = 0;
        if(col < FONT_WIDTH) isOn = bitRead( alphabets[alphabetIndex][col], 7-row) == 1;
        Plot(numCols-1, row, isOn);
      }
     // for(int refreshCount = 0; refreshCount < delay_length; refreshCount++) // Refresh count
       // RefreshDisplay();
  
      // Shift bitmap one column left
      for(int row = 0; row < 8; row++) {
        for(int zone = 0; zone < numZones; zone++){
          bitmap[row][zone] = bitmap[row][zone] >> 1;
          if(zone < maxZoneIndex) bitWrite(bitmap[row][zone], 7, bitRead(bitmap[row][zone+1],0));
        }
      }
      for(int refreshCount = 0; refreshCount < delay_length; refreshCount++) // Refresh count
     RefreshDisplay();
   } 
  }
  
  //memset(bitmap, 0, sizeof(bitmap)); // clear bitmap
//finish_scroll(scrollSpeed);
}

void scrollText(uint8_t x, char *s){
  i=0;
  for(int xpos = x; xpos < -(sizeof(s) * FONT_WIDTH); xpos-- ){ // Count up Characters
    drawText(xpos, s);
     RefreshDisplay();
  }
}
  
   
uint8_t drawChar(uint8_t x, char c){
  uint8_t width = FONT_WIDTH + FONT_SPACE;
  uint8_t height = FONT_HEIGHT;
 
  // Return value is x-Position for next character
  uint8_t ret = x + width;
  
  if(ret > numCols){
    return numCols + 1;
  }
  
  int alphabetIndex = c - FONT_START;
  if(alphabetIndex < 0) alphabetIndex = 0;
  
  for(int w = 0; w < FONT_WIDTH + FONT_SPACE; w++){
    for(int h = 0; h < 8; h++){
      bool isOn = 0;
      if(w < FONT_WIDTH) isOn = bitRead( alphabets[alphabetIndex][w], 7-h) == 1;
        Plot(x+w, h, isOn);
        
    }
    
  }
  
  return ret;
}

uint8_t drawText(uint8_t x, char *s){
  while(*s != 0){
    x = drawChar(x, *s++);
    if(x >= numCols){
      break;
    }
  }
  return x;
}
   

unsigned long timeGone1 = micros();
char timeGoneStr1[18];
void loop() {
  
  while(Serial.available()) {      
      character = Serial.read();
      content.concat(character);
}
  if(content == "D1"){
  memset(&msgBuffer[0], 0, sizeof(msgBuffer));
  memcpy((uint8_t *)msgBuffer, (uint8_t *)msg, (sizeof(msgBuffer)));
  content="";
  }
  if(content == "D2"){
    memset(&msgBuffer[0], 0, sizeof(msgBuffer));
      memcpy((uint8_t *)msgBuffer, (uint8_t *)msgD2, (sizeof(msgBuffer)));
  content="";  
  }
  if(content == "D3"){
    memset(&msgBuffer[0], 0, sizeof(msgBuffer));
      memcpy((uint8_t *)msgBuffer, (uint8_t *)msgD3, (sizeof(msgBuffer)));
  content="";  
  }
  if(content == "D4"){
    content="";
    memset(&msgBuffer[0], 0, sizeof(msgBuffer));
    content.toCharArray(char_array,content.length()+1);
    memcpy((uint8_t *)msgBuffer, (uint8_t *)msgD4, (sizeof(msgBuffer)));  
  }
//   if(content == "D5"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
//   if(content == "D6"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
//   if(content == "D7"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
//   if(content == "D8"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
//   if(content == "D9"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
//   if(content == "C1"){
//    content="";
//    content.toCharArray(char_array,content.length()+1);
//    memcpy((uint8_t *)msgBuffer, (uint8_t *)char_array, (sizeof(msgBuffer)));  
//  }
  
  //timeGoneLost();
   //display_word(msgBuffer, msgLen ,scrollSpeed);// calls for the display_pattern function and says that int loop = 15(if you do more loop the pattern whould scrole slower).
    //memset(bitmap, 170, sizeof(bitmap)); // clear bitmap
    //drawChar(9, 'B');
    //drawText(8, "Bambule");
    //RefreshDisplay();
   //delay(1);
 //delay(2000);
    
    /////////////////////////////////
 if(loopCounter2 >= 2){ // 25 fps = 40 ms @ 2
   displayTimer();
   loopCounter2 = 0;  
 }
 
  loopCounter2++;
}
char Comp(char* This){

 while(Serial.available() > 0) // Don't read unless
   // there you know there is data
 {
   if(index < 3) // One less than the size of the array
   {
     inChar = Serial.read(); // Read a character
     inData[index] = inChar; // Store it
     index++; // Increment where to write next
     inData[index] = '\0'; // Null terminate the string
   }
 }
 if(strcmp(inData,This)  == 0){
   for(int i=0;i<3;i++){
     inData[i]=0;
   }
   index=0;
   return(0);
 }
 else{
   return(1);
 }
}
  
