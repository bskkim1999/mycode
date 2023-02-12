#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>

//<<<<<LCD>>>>>>
// Define some device parameters
#define I2C_ADDR   0x27 // I2C device address

// Define some device constants
#define LCD_CHR  1 // Mode - Sending data
#define LCD_CMD  0 // Mode - Sending command

#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line

#define LCD_BACKLIGHT   0x08  // On
// LCD_BACKLIGHT = 0x00  # Off

#define ENABLE  0b00000100 // Enable bit


//함수원형
void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// added by Lewis
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);

int fd;  // seen by all subroutines




//<<<<<step motor>>>>>
//실험결과, 한바퀴 = 520(steps)이다. 1도 = 약 1.4444step 이다.
void setsteps(int w1, int w2, int w3, int w4);
void forward (int del,int steps);
void backward (int del,int steps);

//핀은 순서대로 장착 (lcd)
#define OUT1 12    
#define OUT2 16     
#define OUT3 20      
#define OUT4 21       



int main(void)
{
    //if(wiringPiSetup() == -1)
    //return 1;
        
        fd = wiringPiI2CSetup(I2C_ADDR);   //i2c통신 준비
        
        wiringPiSetupGpio();   //gpio핀을 사용
        lcd_init(); // setup LCD

        while(1){
            int angle=0;
            int direction=0;

            printf("write an angle and direction(CW=1 OR CCW=-1):");
            typeln("WAIT...");
            scanf("%d %d", &angle, &direction);
            
            //clockwise
            if(direction==1 && angle>0){
                ClrLcd();
                delay(500);
                typeln("WORKING...");
                forward(2, 1.4444*angle);   //#(속도, 스텝 수)#
                
                printf("clockwise\n");
                ClrLcd();
                
                lcdLoc(LINE1);
                typeInt(angle);

                lcdLoc(LINE2);
                typeln("clockwise");

                delay(10000);
            }
            //counterclockwise
            else if(direction==-1 && angle>0){
                ClrLcd();
                delay(500);
                typeln("WORKING...");
                backward(2, 1.4444*angle);   //#(속도, 스텝 수)#
                
                printf("counterclockwise\n");
                ClrLcd();
                
                lcdLoc(LINE1);
                typeInt(angle);

                lcdLoc(LINE2);
                typeln("counterclockwise");

                delay(10000);
            }

            else{
                printf("please write direction again!!!!!");
            }

            ClrLcd();

        }
           
            
            
       

    return 0;
}

//<<<<<step motor>>>>>
void setsteps(int w1, int w2, int w3, int w4)
    {
         pinMode(OUT1,OUTPUT);
         digitalWrite(OUT1,w1);
         pinMode(OUT2,OUTPUT);
         digitalWrite(OUT2,w2);
         pinMode(OUT3,OUTPUT);
         digitalWrite(OUT3,w3);
         pinMode(OUT4,OUTPUT);
         digitalWrite(OUT4,w4);
     }

//clockwise
 void forward (int del,int steps)
     {
          int i;
          
          for(i=0;i<=steps;i++)
          {
                setsteps(1,1,0,0);
                delay(del);
                //printf("a\n");

                setsteps(0,1,1,0);
                delay(del);
                //printf("b\n");

                setsteps(0,0,1,1);
                delay(del);
                //printf("c\n");

                setsteps(1,0,0,1);
                delay(del);
                //printf("d\n");

          }

      }

//counterclockwise
void backward (int del,int steps)
      {
           int k;
           for(k=0;k<=steps;k++)
           {
                   setsteps(1,0,0,1);
                   delay(del);

                   setsteps(0,0,1,1);
                   delay(del);

                   setsteps(0,1,1,0);
                   delay(del);

                   setsteps(1,1,0,0);
                   delay(del);

           }
      }

//<<<<<LCD>>>>>

// float to string
void typeFloat(float myFloat)   {
  char buffer[20];
  sprintf(buffer, "%4.2f",  myFloat);
  typeln(buffer);
}

// int to string
void typeInt(int i)   {
  char array1[20];
  sprintf(array1, "%d",  i);
  typeln(array1);
}

// clr lcd go home loc 0x80
void ClrLcd(void)   {
  lcd_byte(0x01, LCD_CMD);
  lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void lcdLoc(int line)   {
  lcd_byte(line, LCD_CMD);
}

// out char to LCD at current position
void typeChar(char val)   {

  lcd_byte(val, LCD_CHR);
}


// this allows use of any size string
void typeln(const char *s)   {

  while ( *s ) lcd_byte(*(s++), LCD_CHR);

}

void lcd_byte(int bits, int mode)   {

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT ;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT ;

  // High bits
  wiringPiI2CReadReg8(fd, bits_high);
  lcd_toggle_enable(bits_high);

  // Low bits
  wiringPiI2CReadReg8(fd, bits_low);
  lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits)   {
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fd, (bits & ~ENABLE));
  delayMicroseconds(500);
}


void lcd_init()   {
  // Initialise display
  lcd_byte(0x33, LCD_CMD); // Initialise
  lcd_byte(0x32, LCD_CMD); // Initialise
  lcd_byte(0x06, LCD_CMD); // Cursor move direction
  lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
  lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
  lcd_byte(0x01, LCD_CMD); // Clear display
  delayMicroseconds(500);
}
