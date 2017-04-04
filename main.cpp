 #include "mbed.h"
 #include "TextLCD.h"

 I2C m_i2c(D14,D15);
 char m_addr = 0x90;
 
 TextLCD lcd(D2, D3, D4, D5, D6, D7);

 int main()

 {
     lcd.cls();
     while(1) {

         const char tempRegAddr = 0x00;
         m_i2c.write(m_addr, &tempRegAddr, 1); //Pointer to the temperature register
         char reg[2] = {0,0};
         m_i2c.read(m_addr, reg, 2); //Read
         unsigned short res = (reg[0] << 4) | (reg[1] >> 4);
         float temp =  (float) ((float)res * 0.0625);
         
         lcd.locate(2,1);
         lcd.printf("Temp: %2.2f C", temp);
         wait(1.0);
     }

 }
