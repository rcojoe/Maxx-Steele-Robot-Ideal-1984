//Maxx Steele Voice LSI LC8100 and LSI LC3100 voice 
//1984 Ideal Robot CBS Toys
//Int0 is called to read the address sent to the LC3100 ROM
//Int1 is called to read the contents that is returned by the LC3100.  it will also be used to inject new data from an EEPROM.

//#include <EEPROM.h>
#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss

//opcodes
#define WREN  6
#define WRDI  4
#define RDSR  5
#define WRSR  1
#define READ  3
#define WRITE 2

byte eeprom_output_data;
byte eeprom_input_data=0;
byte clr;
int addresshigh=0;
int addressmed=0;
int addresslow=0;

//data buffer
char buffer [128];

volatile byte ports;
volatile byte pin_buffer[10] ;
volatile byte j = 0;
volatile unsigned long time=0;
volatile byte eepromread = 0;
volatile byte shifter = 0;
volatile byte mask = 1;
volatile byte firstrun = 1;
volatile byte worddata[13] = {0x0F,0x29,0x0F,0x60,0xc8,0x71, 0x5C,0x51,0x37,0x20,0xD8,0x6F}; //ox60
                              //0x5c,0x4a,0x3f,0x20,0xe2,0x6e,    60,52,1f,20,e6,6e,   60,5a,16,27,06,5b,    64,50,fe,76,1a,3c   68,66,1d,7e,12,ff,    68,47,e5,af,28,f1
                              //68,38,e6,a7,2c,f1,      6b,41,ed,a7,2a,f0,    64,5a,fc,70,1e,fe,      67,92,e5,6f,0c,1d,      68,92,fe,a0,f4,1c,    68,92,f7,67,f2,2c,   68,92,ef,67,f6,2d,
                              //6c,92,ee,20,02,2d} ;
void setup() {
  DDRB = B00000000;
  DDRC = B00000000;
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(18, OUTPUT); //A4  PC4
  pinMode(19, OUTPUT); //A5  PC5
  Serial.begin(9600); // open the serial port at 9600 bps:
  // put your setup code here, to run once  
EICRA &= ~(bit(ISC00) | bit (ISC01));  // clear existing flags
EICRA |= bit (ISC01);    // set wanted flags (falling level interrupt)
EICRA |= bit (ISC00); 
EIMSK |= bit (INT0);     // enable it

EICRA &= ~(bit(ISC10) | bit (ISC11));  // clear existing flags
EICRA |= bit (ISC11);    // set wanted flags (falling level interrupt)
EICRA |= bit (ISC10); 
EIMSK |= bit (INT1);     // enable it
//attachInterrupt(0,myfunction,FALLING);

Serial.println("Maxx Steele VoiceReader Int Pin D2, data PORTC 11/09/2020");

  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  digitalWrite(SLAVESELECT,HIGH); //disable eeprom
  // SPCR = 01010000
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  SPCR = (1<<SPE)|(1<<MSTR);
  clr=SPSR;
  clr=SPDR;
  delay(10);


  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN); //write enable
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
  addresshigh=0x0;
  addressmed=0x73;
  addresslow=0x1D;
  /*
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WRITE); //write instruction


  spi_transfer((char)(addresshigh));   //send MSByte address first
  spi_transfer((char)(addressmed));   //send MSByte address first
  spi_transfer((char)(addresslow));      //send LSByte address
  //write 128 bytes
  //for (int I=0;I<128;I++)
  //{
  //  spi_transfer(buffer[I]); //write data byte
  //}
  spi_transfer(0x60); //write data byte
  spi_transfer(0x29); //write data byte
  digitalWrite(SLAVESELECT,HIGH); //release chip
  //wait for eeprom to finish writing
  
  delay(3000);
 
 Serial.println("DoneWriting");
 */
  eeprom_output_data = read_eeprom(addresshigh,addressmed,addresslow);
  Serial.println("Finished reading");
  Serial.println(eeprom_output_data,HEX);
  Serial.println(spi_transfer(0xFF),HEX);
  Serial.println(spi_transfer(0xFF),HEX);
  digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer    
}





///////////////////////////////////////////////////////////////////////////////////
void loop() {
//  digitalWrite(13,LOW);
//PORTC ^= _BV(PC4);
if (j >=9){
        digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
        read_eeprom(addresshigh,addressmed,addresslow);
        PORTC ^= _BV(PC4);
        shifter = spi_transfer(0xFF);
        PORTC ^= _BV(PC4);

}
   unsigned long currentMillis = millis();
  // put your main code here, to run repeatedly:
  if (time == 0 && j > 0) time = millis();
  //j=0;
//  if(currentMillis - time >= 1  && j > 0 ) {
//    j=0;
//    time=0;
//    mask=1;
//  }
  
//  if(currentMillis - time < 15  && j > 4) {
      if( j >= 9) {

   // PORTC ^= _BV(PC4);
    // save the last time you blinked the LED 
//    PORTC |= B00100000;
//    eepromread = EEPROM.read(5);
//    PORTC &= B11011111;
   for (byte i = 4; i < j; i++) {
    Serial.println(pin_buffer[i] & B00001111, BIN);
    Serial.println(i);
   }   
      for (byte i = 4; i < j; i++) {
    Serial.print(pin_buffer[i] & B00001111, HEX);
    Serial.print("-");
    //Serial.println(i);
   }  
   Serial.println("");
   Serial.println(shifter);
   Serial.print("J = ");
   //Serial.println(j);
    j=0 ;
    time=0;
    
    Serial.println("Done");
    
    //digitalWrite(13, LOW);
    //PORTC ^= _BV(PC4);
  }
if (j>0) Serial.println(j);
//Serial.println(rotateRight(0xF0,shifter) & B00100000);
//shifter++;
/*PORTC ^= _BV(PC4);
  if (!(PIND & _BV(PD3))) {
      if (eepromread & mask) PORTC |= _BV(PC5);
     else PORTC &= ~_BV(PC5);
  }
  */
//      if (mask==1 & (worddata[eepromread] & mask)) PORTC |= _BV(PC5);
//     else PORTC &= ~_BV(PC5);
}
ISR(INT0_vect) {
//  PORTC ^= _BV(PC4);

//void myfunction(){
//    delayMicroseconds(100);
// digitalWrite(13,HIGH);
// PORTC |= B00100000;

  pin_buffer[j] = PINC;    //read datalines and place in buffer
 j++;
// EIFR = bit (INTF0);  // clear flag for interrupt 0
// digitalWrite(13, !digitalRead(13));
//digitalWrite(13,HIGH);
//if (time==0) time = millis();
//digitalWrite(13,LOW);
//PORTC &= B11011111;
//shifter = EEPROM.read(5);
mask=1;
eepromread = 0;
if(j <=5) firstrun = 1;

//PORTC ^= _BV(PC4);
//digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer  
}

ISR(INT1_vect) {
 // PORTC ^= _BV(PC4);

    //PORTC |= B00100000;
    //eepromread = EEPROM.read(5);
    //if(rotateRight(85,shifter) & B00100000) 
    //shifter &= B0000111;
    //eepromread = 1;
    //PORTC &= (B11011111 | (eepromread & B00100000)); //off
    //PORTC |= ((eepromread & B00100000));     //on
    //PORTC ^= _BV(PC5);
    //delayMicroseconds(3); 
    //PORTC &= B11011111;
/*
 if (!(PIND & _BV(PD3))) {
      if (eepromread & mask) PORTC |= _BV(PC5);
     else PORTC &= ~_BV(PC5);
  } else{
*/
    
    //read_eeprom(addresshigh,addressmed,addresslow);
    
//    if (worddata[eepromread] & mask) PORTC |= _BV(PC5);
    if (shifter & mask) PORTC |= _BV(PC5);
     else PORTC &= ~_BV(PC5);
          
  if (firstrun == 1){
    firstrun=0;
    mask <<= 1;
    //j=0;
    return;
  }
    mask <<= 1;
    while (mask){
          while (PIND & _BV(PD3)){
            //PORTC ^= _BV(PC4);
            ;
          }
          //if (worddata[eepromread] & mask) PORTC |= _BV(PC5);
          if (shifter & mask) PORTC |= _BV(PC5);
          else PORTC &= ~_BV(PC5);
          //PORTC ^= _BV(PC4);
          mask <<= 1;
          while (!(PIND & _BV(PD3))){
            ;
          }
    }
/*    if (worddata[eepromread] & mask) PORTC |= _BV(PC5);
     else PORTC &= ~_BV(PC5);
//    if ((worddata[eepromread] & mask) == (PC5)) PORTC ^= _BV(PC5);
     mask <<= 1;
*/
//     if (!mask) {
      mask=1;
      //PORTC ^= _BV(PC4);
      shifter = spi_transfer(0xFF);
      //PORTC ^= _BV(PC4);
      if (eepromread < 11 ) { 
        eepromread++;
        //PORTC ^= _BV(PC4);
      }
        else eepromread = 0;
      
//     }
//  }
    //shifter++;
   // mask=1;
             while ((PIND & _BV(PD3))){
            ;
          }
//      shifter = EEPROM.read(eepromread);
   //if (worddata[eepromread] & mask) PORTC |= _BV(PC5);
   if (shifter & mask) PORTC |= _BV(PC5);
          else PORTC &= ~_BV(PC5);
   // PORTC ^= _BV(PC4);

   //PORTC ^= _BV(PC4);
    EIFR = bit (INTF1); 
}

void fill_buffer()
{
  for (int I=0;I<128;I++)
  {
    buffer[I]=I;
  }
}

char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

byte read_eeprom(int EEPROM_addresshigh, int EEPROM_addressmed, int EEPROM_addresslow)
{
  //READ EEPROM
  int data;
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(READ); //transmit read opcode
  spi_transfer((char)(EEPROM_addresshigh));   //send MSByte address first
  spi_transfer((char)(EEPROM_addressmed));   //send MSByte address first
  spi_transfer((char)(EEPROM_addresslow));      //send LSByte address
  //data = spi_transfer(0xFF); //get data byte
  //digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
  data=0;
  return data;
}
