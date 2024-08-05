#include "main.h"
//#define F_CPU 8000000UL
#define MatCon 20
#define RED OCR1B
#define GREEN OCR1A
#define BLUE OCR2

uint8_t sec, min, hour, day, date, month, year; 
uint8_t min_alarm, hour_alarm;
uint8_t WAKE_UP, i_date, i_day, i_setday, key_day, ii_day, dotFlag;
uint8_t set_option, set_time, set_day, set_date, alarm_option;
uint8_t ADCnow;
uint8_t _day[7];

int8_t Keyboard [4][4] = { {1, 2, 3, 0xA},
               {4, 5, 6, 0xB},
               {7, 8, 9, 0xC},
               {0xF,0,0xE,0xD}};

uint8_t iPB, jPB, MatKey;
unsigned int TIMEOCR;
unsigned int MatrixTime;
uint8_t portState[4]= {0b11101111,0b11011111,0b10111111,0b01111111};
uint8_t inputState[4]={0x01,0x02,0x04,0x08};
unsigned int indiCounter;   // счётчик каждого индикатора (0-24)
uint8_t indiDigits[6];    // цифры, которые должны показать индикаторы (0-10)
//int8_t opts[6] = {1, 2, 3, 4, 5, 6};
uint8_t count;
uint8_t i;
 
void readDS(void)
{
    I2C_SendByteByADDR(0,0b11010000);
    I2C_StartCondition(); //Отправим условие START
    I2C_SendByte(0b11010001); //отправим в устройство бит чтения
    sec   = RTC_ConvertFromDec(I2C_ReadByte());
    min   = RTC_ConvertFromDec(I2C_ReadByte());
    hour  = RTC_ConvertFromDec(I2C_ReadByte());
    day   = RTC_ConvertFromDec(I2C_ReadByte());
    date  = RTC_ConvertFromDec(I2C_ReadByte());
    month = RTC_ConvertFromDec(I2C_ReadByte());
    year  = RTC_ConvertFromDec(I2C_ReadLastByte());
      I2C_StopCondition(); //Отправим условие STOP        
}

//*********************
//-------SHOW ON IN----------
//*********************

void outIN (void)
{ 

  //if (indiCounter >= 400){
    indiCounter = 0;
    PORTC &= ~0xFC;
    PORTA &= ~0xFE;
    PORTC |= 1<<(count+2);
    PORTA |= indiDigits [count] << 4;
    count++;
    if (count>=6) count = 0;
    _delay_ms(25);
  //}
}


void SetMass(int F, int S , int T)
{
  indiDigits [0] = F/10;
  indiDigits [1] = F%10;
  indiDigits [2] = S/10;
  indiDigits [3] = S%10;
  indiDigits [4] = T/10;
  indiDigits [5] = T%10;
  
  outIN ();
  /*if (indiCounter >=25){
  indiCounter = 0;
  //if ((TIMEOCR/10) >=50){
  PORTC &= ~0xFE;
  //PORTA = 0;
  PORTA = 1<<count;
  PORTC |= indiDigits [count] << 4;
  //_delay_ms(200);
  count++;
  if (count>=6) count =0;
  }*/
}

//*********************
//-------SHOW DATE----------
//*********************

void DSDate(void)
{
  TIMEOCR = 0;
  TCNT1 = 0;
  while ((TIMEOCR/12) <=4000)
  {
    SetMass (date, month, year);
    if (MatKey == 0xF) break;
  }
  TIMEOCR = 0;
  TCNT1 = 0;
}

//*********************
//-------SETTINGS----------
//*********************

void TimeSet(void)
{ 
  MatKey = MatCon;
  set_time = 1;
  for (i = 0; i<6; i++) indiDigits [i] = 0;
  i = 0;
  while (set_time == 1)
  {
    switch (MatKey){
    case 0: {indiDigits [i] = 0; i++; MatKey = MatCon; break;}
    case 1: {indiDigits [i] = 1; i++; MatKey = MatCon; break;}
    case 2: {indiDigits [i] = 2; i++; MatKey = MatCon; break;}
    case 3: {indiDigits [i] = 3; i++; MatKey = MatCon; break;}
    case 4: {indiDigits [i] = 4; i++; MatKey = MatCon; break;}
    case 5: {indiDigits [i] = 5; i++; MatKey = MatCon; break;}
    case 6: {indiDigits [i] = 6; i++; MatKey = MatCon; break;}
    case 7: {indiDigits [i] = 7; i++; MatKey = MatCon; break;}
    case 8: {indiDigits [i] = 8; i++; MatKey = MatCon; break;}
    case 9: {indiDigits [i] = 9; i++; MatKey = MatCon; break;}
    case 0xf: {
          if (i <= 0) {
                set_time = 0;}
          else{
            i--; 
            indiDigits[i] = 0;}
          MatKey = MatCon;
          break;
          }
    case 0xe: { set_time = 0;
          hour= indiDigits[0]*10+indiDigits[1];
          min = indiDigits[2]*10+indiDigits[3];
          sec = 0;
          
          if (hour > 23) hour = 0;
          if (min > 60) min = 0;
          
      
          I2C_SendByteByADDR(0,0b11010000);
          I2C_StartCondition(); //Отправим условие START
          I2C_SendByte(0b11010000);
          I2C_SendByte(0x00);//Переходим на 0x00
          I2C_SendByte(RTC_ConvertFromBinDec(0)); //секунды
          I2C_SendByte(RTC_ConvertFromBinDec(min)); //минуты
          I2C_SendByte(RTC_ConvertFromBinDec(hour)); //часы
          /*I2C_SendByte(RTC_ConvertFromBinDec(1)); 
          I2C_SendByte(RTC_ConvertFromBinDec(15)); 
          I2C_SendByte(RTC_ConvertFromBinDec(07)); 
          I2C_SendByte(RTC_ConvertFromBinDec(07));*/
          I2C_StopCondition();
          MatKey = MatCon;
          }
    case 0xA: { set_time = 0; break;}
    case 0xC: { set_time = 0; break;}
    }
    outIN ();
    if (i>=4) i=0;
  } 
}

void DateSet(void){
  MatKey = MatCon;
  for (i = 0; i<6; i++) indiDigits [i] = 0;
//  set_time = 1;
  //PORTD|=1<<PD6;
  i = 0;
  while (set_time == 1)
  {
    switch (MatKey){
    case 0: {indiDigits [i] = 0; i++; MatKey = MatCon; break;}
    case 1: {indiDigits [i] = 1; i++; MatKey = MatCon; break;}
    case 2: {indiDigits [i] = 2; i++; MatKey = MatCon; break;}
    case 3: {indiDigits [i] = 3; i++; MatKey = MatCon; break;}
    case 4: {indiDigits [i] = 4; i++; MatKey = MatCon; break;}
    case 5: {indiDigits [i] = 5; i++; MatKey = MatCon; break;}
    case 6: {indiDigits [i] = 6; i++; MatKey = MatCon; break;}
    case 7: {indiDigits [i] = 7; i++; MatKey = MatCon; break;}
    case 8: {indiDigits [i] = 8; i++; MatKey = MatCon; break;}
    case 9: {indiDigits [i] = 9; i++; MatKey = MatCon; break;}
    case 0xf: {
          if (i <= 0) {
                set_time = 0;}
          else{
            i--; 
            indiDigits[i] = 0;}
          MatKey = MatCon;
          break;
          }
    case 0xe: { set_time = 0;
          date= indiDigits[0]*10+indiDigits[1];
          month = indiDigits[2]*10+indiDigits[3];
          year = indiDigits[4]*10+indiDigits[5];
          
          if ((date > 29) && (month == 2) &&  ((year % 4 == 0) && (year % 100 != 0)))  {
          date = 29;
          }
          else if ((date > 28) && (month == 2)) {
              date = 28;
              }
             else if ((30 + (month + (month/ 8)) % 2 == 0) && (date > 31))  {
                date = 31;
                  }
                else if (date > 30) {
                  date = 30;
                  }
          if (month > 12) month = 12;
          if (year > 99) year = 99;
      
          I2C_SendByteByADDR(0,0b11010000);
          I2C_StartCondition(); //Отправим условие START
          I2C_SendByte(0b11010000);
          I2C_SendByte(0x04);//Переходим на 0x00
          I2C_SendByte(RTC_ConvertFromBinDec(date)); 
          I2C_SendByte(RTC_ConvertFromBinDec(month)); 
          I2C_SendByte(RTC_ConvertFromBinDec(year));
          I2C_StopCondition();
          MatKey = MatCon;
          }
    case 0xA: { set_time = 0;  break;}
    case 0xB: { set_time = 0;  break;}
    }
    outIN();
    if (i>=6) i=0;
    }
}

/*ISR(ADC_vect)
{
  //ADCnow= (ADCL)|(ADCH<<8);
  //low_adc = ADCL;
  ADCnow = ADCH;
  //high_adc = ADCH;//Верхняя часть регистра ADC должна быть считана последней, иначе не продолжится преобразование
}

uint8_t set_colour, k, red_old, green_old, blue_old, red_now, green_now, blue_now ,set_colour_mode, ADC_poh, ADC_poh1, ADC_poh2,ADC_poh3;*/
void RGB(void)
{
  //for (i = 0; i<6; i++) indiDigits [i] = 0;
  /*ADCSRA |=(1<<ADSC);
  red_old = RED;
  green_old = GREEN;
  blue_old = BLUE;
  set_colour_mode = 0;
  while (set_time == 1)
  {
    if (MatKey == 0xD) {set_colour_mode = !set_colour_mode;}
    if (MatKey == 0xF) {set_time = 0; RED = red_old; GREEN = green_old; BLUE = blue_old ; ADCSRA &=~(1<<ADSC);}
    if (MatKey == 0xE) {set_time = 0; RED = red_now; GREEN = green_now; BLUE = blue_now; ADCSRA &=~(1<<ADSC);}
  
    if (set_colour_mode == 0){
      if (MatKey == 0xA) {red_now = ADCnow; RED = red_now; GREEN = 0; BLUE = 0;}
      if (MatKey == 0xB) {green_now = ADCnow; GREEN = green_now; RED = 0; BLUE = 0;}
      if (MatKey == 0xC) {blue_now = ADCnow; BLUE  = blue_now; RED = 0; GREEN = 0;}
    }
    if (set_colour_mode == 1){
      if (MatKey == 0xA) {red_now = ADCnow; RED = red_now; GREEN = green_now; BLUE = blue_now;}
      if (MatKey == 0xB) {green_now = ADCnow; GREEN = green_now; RED = red_now; BLUE = blue_now;}
      if (MatKey == 0xC) {blue_now = ADCnow; BLUE  = blue_now; RED = red_now; GREEN = green_now;}
    }
      ADC_poh = ADCnow;
      
      ADC_poh1 = ADCnow/100%10;
      indiDigits[0] = ADC_poh1;
      ADC_poh2 = ADCnow/10%10;
      indiDigits[1] = ADC_poh2;
      ADC_poh3 = ADCnow%10;
      indiDigits[2] = ADC_poh3;

    
    for (k = 0; k<4;k++)
    {
      
      PORTC |= 1<<(k+2);
      PORTA |= indiDigits[k]<<4;
    }
  }*/
}


void Settings(void){
  set_time = 1;
  PORTA |=0xF0;
  while (set_time == 1)
  {
      if (MatKey == 0xA){RGB();}
      if (MatKey == 0xB){TimeSet(); break;}
      if (MatKey == 0xC){DateSet(); break;}
      if (MatKey == 0xF){set_time = 0; break;}
      
  }
}

//*********************
//------ALARM----------
//*********************

void dayVerify (char key_day)
{
  for (i_day = 0; i_day<7; i_day++)
  {
    if(_day[i_day] == key_day)
    {
      return;
    }
  }
  _day[ii_day] = key_day; ii_day++; 
}
void special_for_optin_4(void)
{
  while (set_time == 1)
  {
    
    switch (MatKey){
    case 1: {key_day = 1;  MatKey = MatCon; break;}
    case 2: {key_day = 2;  MatKey = MatCon; break;}
    case 3: {key_day = 3;  MatKey = MatCon; break;}
    case 4: {key_day = 4;  MatKey = MatCon; break;}
    case 5: {key_day = 5;  MatKey = MatCon; break;}
    case 6: {key_day = 6;  MatKey = MatCon; break;}
    case 7: {key_day = 7;  MatKey = MatCon; break;}
    case 0xE: { if (ii_day == 7) ii_day = 0;
          dayVerify(key_day); MatKey = MatCon;}
    case 0xF:{  
          ii_day--; 
          if (ii_day < 0) ii_day = 0;
          _day[ii_day] = 0;
          MatKey = MatCon;
         }
    case 0xD:{  set_time = 0;
          for (i_day = 0; i_day<7; i_day++)
          {
            if(_day[i_day] != 0)
            {
              return;
            }
          }
          set_option = 1;
          }
    //вывод
    PORTC |= 1<<PC3;
    PORTA |= (ii_day+1)<<4;
    _delay_ms(50);
    PORTA &= ~0xFE;
    PORTC &= ~0xFE;
    
    PORTC |= 1<<PC5;
    PORTA |= (key_day)<<4;
    _delay_ms(50);
    PORTA &= ~0xFE;
    PORTC &= ~0xFE;
    }
  }
}
void AlarmSetting (void) // 0 - выкл. 1 - один раз; 2 - всю неделю; 3 - с пн по пт; 4 - как хочу
{
  while (set_time == 1)
  {
    PORTC |= 1<<PC5;
    PORTA |= set_option <<4;
    switch (MatKey){
    case 0: {set_option = 0; MatKey = MatCon; break;}
    case 1: {set_option = 1; MatKey = MatCon; break;}
    case 2: {set_option = 2; MatKey = MatCon; break;}
    case 3: {set_option = 3; MatKey = MatCon; break;}
    case 4: {set_option = 4; MatKey = MatCon; break;}
    case 0xf: {set_time = 0;}
    case 0xE: {
          alarm_option = set_option; 
          if (alarm_option == 4) //один раз и на след день
          {
            special_for_optin_4();
          }
          set_time = 0; 
          PORTA &= ~0xFE;
          PORTC &= ~0xFE;
           }
    }

  }
}
void AlarmSet (void)
{ 
  
  MatKey = MatCon;
  //set_time = 1;
  for (i = 0; i<6; i++) indiDigits [i] = 0;
  i = 0;
  while (set_time == 1)
  {
    switch (MatKey){
    case 0: {indiDigits [i] = 0; i++; MatKey = MatCon; break;}
    case 1: {indiDigits [i] = 1; i++; MatKey = MatCon; break;}
    case 2: {indiDigits [i] = 2; i++; MatKey = MatCon; break;}
    case 3: {indiDigits [i] = 3; i++; MatKey = MatCon; break;}
    case 4: {indiDigits [i] = 4; i++; MatKey = MatCon; break;}
    case 5: {indiDigits [i] = 5; i++; MatKey = MatCon; break;}
    case 6: {indiDigits [i] = 6; i++; MatKey = MatCon; break;}
    case 7: {indiDigits [i] = 7; i++; MatKey = MatCon; break;}
    case 8: {indiDigits [i] = 8; i++; MatKey = MatCon; break;}
    case 9: {indiDigits [i] = 9; i++; MatKey = MatCon; break;}
    case 0xf: {
          if (i <= 0) {set_time = 0; readDS();}
          else{
            i--; 
            indiDigits[i] = 0;}
          MatKey = MatCon;
          break;
          }
    case 0xE: { set_time = 0;
          hour_alarm= indiDigits[0]*10+indiDigits[1];
          min_alarm = indiDigits[2]*10+indiDigits[3];
                    
          if (hour_alarm > 23) hour_alarm = 0;
          if (min_alarm > 60) min_alarm = 0;
          
          MatKey = MatCon;
          }
    case 0xD: { AlarmSetting(); set_time = 1; break;}
    }
    outIN();
    if (i>=4) i=0;
  }
}

ISR(INT1_vect)
{
  WAKE_UP=0;
  PORTD&=0<<PD7;
  if (alarm_option == 1) alarm_option = 0;
}

void wake_up(void) // переделать
{
      //GICR|= 1<<INT1; //you see...
      //MCUCR|= 1<<ISC01 | 1<<ISC00; //FRONT*/
      WAKE_UP = 1;
      //MatKey = 20;
      /*while(WAKE_UP == 1)
      {
        PORTD|=1<<PD7;
        _delay_ms(1000);
        PORTD&=0<<PD7;
        _delay_ms(400);
        PORTD|=1<<PD7;
        _delay_ms(400);
        PORTD&=0<<PD7;
        _delay_ms(400);
        PORTD|=1<<PD7;
        _delay_ms(200);
        
        }
      }*/
}

//*********************
//--------BURN----------
//*********************

int Kat, Anod;
//uint8_t burnOut;
void burnIndicators() {
  //burnOut = 1;
  for (Kat = 0; Kat < 10; Kat++) {
    for (Anod = 0; Anod < 6; Anod++) {
      PORTA |= indiDigits[Anod] << 4;
      PORTC |= 1<<(Anod+2);
      indiDigits[Anod]--; 
      if (indiDigits[Anod] < 0) indiDigits[Anod] = 9;
      _delay_ms(100);
      PORTC &= ~0xFE;
      PORTA &= ~0xFE;
    }
  }
}

//*********************
//-----TIMER-----
//*********************

 //unsigned long int COUNTER_TIMER0;          
//ISR(TIMER1_COMPA_vect)
ISR (TIMER0_COMP_vect)
{
  TIMEOCR++;
  //TIMEOCR = TIMEOCR*20;
  indiCounter++;
  MatrixTime++; 
  if ((MatrixTime/16)>=50) // 50 для TIMER1
  {
    MatrixTime = 0;
    for(iPB=0; iPB<4; iPB++)
    {
      PORTB=portState[iPB];
      for(jPB=0; jPB<4; jPB++)
      { 
        if(((PINB&inputState[jPB]) == 0))
        {
          while((PINB&inputState[jPB])!=inputState[jPB])
          {outIN();}
          MatKey = Keyboard [iPB][jPB];
        }  
      } 
    }
  } 
}

//*********************
//-------MAIN----------
//*********************

void initTimer(void){
    //TCCR1B|= (1<<WGM12)|(1<<CS10); // CTC + presclaer = 1
    //TIMSK |= 1<< OCIE1A ; // прерывание при совпадении TCNT и OCR
    /*TCCR1A|= (1<<COM1B1)|(1<<WGM10)|(1<<COM1A1);
    TCCR1B|= (1<<WGM12)|(1<<CS12)|(0<<CS11)|(0<<CS10);
    TCCR2 |= (1<<WGM20)|(1<<CS22)|(0<<CS21)|(0<<CS20)|(1<<WGM21)|(1<<COM21);  */
    /*RED = 255;
    GREEN = 255;
    BLUE  = 255;*/
    
    //TCCR2 |= (1<<WGM21)|(1<<CS22)|(0<<CS21)|(0<<CS20);
    //TIMSK |= 1<<OCIE2;
    TCCR0 |= (1<<WGM01)|(1<<CS02)|(0<<CS01)|(1<<CS00);
    TIMSK |= 1<<OCIE0;
    OCR0 = 0;
    sei();
}

/*unsigned int ADC_convert (void)
{
  ADCSRA |= (1<<ADSC); //Начинаем преобразование
  while((ADCSRA & (1<<ADSC))); //проверим закончилось ли аналого-цифровое преобразование
  return (unsigned int) ADC;
}*/

//uint8_t low_adc,high_adc;

void initADC(void){
  /*ADCSRA |= (1<<ADEN) //Разрешить работу АЦП
  |(1<<ADATE)//Непрерывный режим работы АЦП
  |(1<<ADIE)//Разрешение прерывания от АЦП
  |(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // делитель на 128
  ADMUX |= (0<<REFS1)|(0<<REFS0); // 1|1 -опорное напряжение 2,56В
  ADMUX |= (1<< ADLAR);
  //ADCSRA |=(1<<ADSC); // включить преобразование
   */

}

int main(void)
{
  hour= 14;
  min = 28;
  sec = 29;
  I2C_Init();
  DDRA = 0xF0;
  DDRB = 0xF0;
  DDRC = 0b11111100;
  DDRD = 0xF0;
  PORTD = 1<<PD3;
  
  PORTB = 0xFF;
  //PORTD= 0xF0;
  //_delay_ms(50); 
  //set_day = 10;
  //readDS();
  //initADC();
  //initTimer();
    while(1)
    {
    SetMass (hour, min, sec);

    if (TIMEOCR>=(7800*2)) { // 500 - TIMER1; 7800 - timer0
      TIMEOCR = 0;
      TCNT0 = 0;
      
        sec++;
        if (sec % 2 == 0) 
        {
          PORTD = 1<<PD6; 
        }
        else
        {
          PORTD = 0<<PD6;
        }
        
        if (sec > 59) {
          sec = 0;
          min++;
          if ((min == 1 || min == 30)) {    // каждые полчаса
            burnIndicators(); 
            readDS(); 
          }
        }
        if (min > 59) {
          min = 0;
          hour++;
          //burnOut = 0;  
        }
        if (hour > 23) hour = 0;
        
      }
      //dotFlag = !dotFlag;
    
    if (MatKey == 0xA) {DSDate(); MatKey = MatCon; sec+=3;}
    if (MatKey == 0xB) {AlarmSet(); MatKey = MatCon;} // это в меню
    if (MatKey == 0xC) {}//wake_up(); //BMP();
    if (MatKey == 0xD) {Settings (); MatKey = MatCon;}

    if (((hour == hour_alarm) && (min == min_alarm) && ((alarm_option == 1)||(alarm_option == 2))) || ((hour == hour_alarm) && (min == min_alarm) && (alarm_option == 3)&& (day>1)&&(day<7)))
    {wake_up();}

    if ((hour == hour_alarm) && (min==min_alarm)&&(alarm_option == 4))
    {
      for (i_day=0; i_day<7; i_day++)
      {
        if (_day[i_day] == day)
        {
          wake_up();
        }
      }
    }
//A bt - Date now
//B bt - Alarm setting -> B bt - Time Switch ; C bt - Date Swith   
//C bt - Devices -> A - MP3; B - Radio; C - BMP (may remape);
//D bt - Optins (in development)
//----------------------------------------------
//Sourse:
/*I2C_SendByteByADDR(0,0b11010000);
    I2C_StartCondition(); //Отправим условие START
    I2C_SendByte(0b11010001); //отправим в устройство бит чтения
    sec   = RTC_ConvertFromDec(I2C_ReadLastByte());
    I2C_StopCondition(); //Отправим условие STOP  */    
    //DSTime();


    /*if (~PIND&(1<<PD1))  {buttons();}
    if (~PIND&(1<<PD0))  {buttons_date();}
    if (~PIND&(1<<PD3))  {timer();}
    if (~PIND&(1<<PD2))  {set_day = 10;}
    if (((hour == hour_timer) && (min==min_timer)&&(sec==sec_timer) && (set_day == 0))||((hour == hour_timer) && (min==min_timer)&&(sec==sec_timer) && (set_day == 1)) || ((hour == hour_timer) && (min==min_timer)&&(sec==sec_timer) && (set_day == 2) && (day>=2)&&(day<=5)))
    {
      wake_up();
    }
    if ((hour == hour_timer) && (min==min_timer)&&(sec==sec_timer) && (set_day == 3))
    {
      for (k_day=0; k_day<7; k_day++)
      {
        if (_day[k_day] == day)
        {
          
        }
      }
    }*/
      //Запсить в DS
      /*I2C_SendByteByADDR(0,0b11010000);
          I2C_StartCondition(); //Отправим условие START
          I2C_SendByte(0b11010000);
          I2C_SendByte(0x00);//Переходим на 0x00
          I2C_SendByte(RTC_ConvertFromBinDec(10)); //секунды
          I2C_SendByte(RTC_ConvertFromBinDec(30)); //минуты
          I2C_SendByte(RTC_ConvertFromBinDec(indiDigits[0]*10+indiDigits[1])); //часы
          I2C_SendByte(RTC_ConvertFromBinDec(1)); 
          I2C_SendByte(RTC_ConvertFromBinDec(15)); 
          I2C_SendByte(RTC_ConvertFromBinDec(07)); 
          I2C_SendByte(RTC_ConvertFromBinDec(07));
          I2C_StopCondition();*/
    }
}
