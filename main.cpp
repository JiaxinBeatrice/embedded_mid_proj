 #include "mbed.h"
 #include "TextLCD.h"
 #include <string>

 I2C m_i2c(D14,D15);
 char m_addr = 0x90;
 Ticker TmpOnLCD;
 
 TextLCD lcd(D2, D3, D4, D5, D6, D7);
 
 InterruptIn goingOut(D8);
 Serial rn4020(D1, D0);
 Serial pc(USBTX, USBRX);
 DigitalOut AWAKE(A3);

 void init_ble(void);
 void sendString(string msg);
 string getString(void);
 char getChar(void);

 string buff;
 float temp;

 void displayLCD(){
   const char tempRegAddr = 0x00;
   m_i2c.write(m_addr, &tempRegAddr, 1); //Pointer to the temperature register
   char reg[2] = {0,0};
   m_i2c.read(m_addr, reg, 2); //Read
   unsigned short res = (reg[0] << 4) | (reg[1] >> 4);
   temp =  (float) ((float)res * 0.0625);
         
   lcd.locate(2,1);
   lcd.printf("Temp: %2.2f C", temp); 
 }

 void ISR(){
   AWAKE = 0;
   wait(1.0);
   AWAKE = 1;
   pc.printf("Mode : ",getString());
   sendString("SUW,111213141516171819101A1B1C1D1E1F,");  
   rn4020.printf("%2.2f",temp);
   sendString("\r\n");
   pc.printf("Write temp: %2.2f : %s",temp, getString());
 }
 
 int main(){
  lcd.cls();
  TmpOnLCD.attach(&displayLCD, 1.0);
  goingOut.rise(&ISR);  
  
  AWAKE = 0;
  rn4020.baud(115200);
  wait(1.0);
  init_ble();
  
  AWAKE = 1;
  pc.printf("Mode: %s",getString());
  
  sendString("A\r\n");
  pc.printf("Annotation start: %s",getString());  
     
  while(1){ 
    buff =  getString();
    pc.printf("%s",buff);
    if(buff == "Connection End\r\n"){
        pc.printf("Annotation restart\r\n");
        sendString("A\r\n");
        pc.printf("%s",getString());  
    }        
  }
 }
 
 void init_ble() {
   AWAKE = 1;
   pc.printf(" CMD mode :%s",getString());

   wait(0.1);
   sendString("SF,1\r\n");
   pc.printf("Set to factory default configuration: %s",getString());

   wait(0.1);
   sendString("SS,C0000001\r\n");
   pc.printf("Enable support of the Device Information and Battery services: %s",getString());

   wait(0.1);
   sendString("PZ\r\n");
   pc.printf("Clear current private service: %s",getString());
   
   wait(0.1);
   sendString("PS,11223344556677889900AABBCCDDEEFF\r\n");
   pc.printf("Private service: %s",getString());

   wait(0.1);
   sendString("PC,111213141516171819101A1B1C1D1E1F,12,05\r\n");
   pc.printf("Private characteristic set: %s",getString());

   wait(0.1);
   sendString("SR,00000000\r\n");
   pc.printf("Peripheral mode: %s",getString());

   wait(0.1);
   sendString("R,1\r\n");
   pc.printf("%s",getString());

   AWAKE = 0;
 } 
 
 void sendString(string msg){
   rn4020.printf("%s",msg);
 }
 
 char getChar(){
    return rn4020.getc();
 }
 
 string getString(){
   string msg = "";
   char prev = ' ';
   char curr = ' ';
   while(1){
     if(rn4020.readable()){
          prev = curr;
          curr = getChar();
          msg += curr;
          if(prev=='\r' and curr=='\n'){
              break;
          }
     }
   }
   return msg;
 } 
