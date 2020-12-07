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
byte addresshigh=0;
byte addressmed=0;
byte addresslow=0;

//data buffer
char buffer [128];
byte string = 0;
byte savedata = 0;
unsigned long counter = 0;
volatile byte ports;
volatile byte pin_buffer[10] ;
volatile byte j = 0;
volatile byte voiceindex = 0;
volatile unsigned long time=0;

volatile byte eepromread = 0;
volatile byte shifter = 0;
volatile byte mask = 1;
volatile byte firstrun = 1;
volatile byte tempdata=0;
//volatile byte worddata[13] = {0x0F,0x29,0x0F,0x60,0xc8,0x71, 0x5C,0x51,0x37,0x20,0xD8,0x6F}; //ox60
                              //0x5c,0x4a,0x3f,0x20,0xe2,0x6e,    60,52,1f,20,e6,6e,   60,5a,16,27,06,5b,    64,50,fe,76,1a,3c   68,66,1d,7e,12,ff,    68,47,e5,af,28,f1
                              //68,38,e6,a7,2c,f1,      6b,41,ed,a7,2a,f0,    64,5a,fc,70,1e,fe,      67,92,e5,6f,0c,1d,      68,92,fe,a0,f4,1c,    68,92,f7,67,f2,2c,   68,92,ef,67,f6,2d,
                              //6c,92,ee,20,02,2d} ;

volatile byte maxvoice[300];
void setup() {
  DDRB = B00000000;
  DDRC = B00000000;
  pinMode(2, INPUT);  // D2  //int0
  pinMode(3, INPUT); //  D3  //int1
  pinMode(4, INPUT); //  D4 //data
   
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
  //addresslow=0x1E;
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
   Serial.println("2nd read");
  Serial.println(spi_transfer(0xFF),HEX);
   Serial.println("3rd read");
  Serial.println(spi_transfer(0xFF),HEX);
  digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer    
}





///////////////////////////////////////////////////////////////////////////////////
void loop() {

  time++;
  //voiceindex=0;
  //j=0;
if (time > 65000 & voiceindex >= 3){
//if( voiceindex >= 20) {
counter = 0;

    PORTC ^= _BV(PC4);
    // save the last time you blinked the LED 
//    PORTC |= B00100000;
//    eepromread = EEPROM.read(5);
//    PORTC &= B11011111;
    Serial.println("Max Voice");
   for (byte i = 0; i < voiceindex; i++) {
    Serial.print(counter);
    Serial.print(": ");
    Serial.print(maxvoice[i], BIN);
        Serial.print(" ");
        Serial.println(maxvoice[i], HEX);
//    Serial.println(i);
    counter++;
   }   
       Serial.print("Max ROM location: ");
      for (byte i = 0; i < j; i++) {
    Serial.print(pin_buffer[i] & B00001111, HEX);
    Serial.print("-");
    //Serial.println(i);
    pin_buffer[i] = 0;
   }  
   Serial.println("");
   Serial.print("Address High = ");
   Serial.println(addresshigh,HEX); 
   Serial.print("Address Med = ");
   Serial.println(addressmed,HEX);  
   Serial.print("Address Low = ");
   Serial.println(addresslow,HEX);  
   
   Serial.println("");
   Serial.println(shifter);
   Serial.print("J = ");
   Serial.println(j);
   Serial.print("VoiceIndex = ");
   Serial.println(voiceindex);   
    j=0 ;
    time=0;
    //digitalWrite(13, LOW);
    //PORTC ^= _BV(PC4);
savedata = 0;
Serial.println("Save Data? y/n");
    while(Serial.available() == 0 & maxvoice[0] != 0xFF){}  // continuously monitor serial port for any data;
    while (Serial.available() > 0){ // Don't read unless
      delay(100);
      string = Serial.read();
      Serial.println(string);
      if (string == 121) { //yes
        savedata = 1;
      }
      Serial.print("Save Data=");
      Serial.println(savedata);
    }
    //Serial.println("Done");
if (savedata == 1){  
  counter=0;
  digitalWrite(SLAVESELECT,HIGH); //release chip
  delay(100);
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WRITE); //write instruction
  spi_transfer((char)(addresshigh));   //send MSByte address first
  spi_transfer((char)(addressmed));   //send MSByte address first
  spi_transfer((char)(addresslow));      //send LSByte address
  //write 128 bytes
  for (int I=0;I<voiceindex;I++)
  {
    spi_transfer(maxvoice[I]); //write data byte
    Serial.println(maxvoice[I], HEX);
    counter++;
    addresslow++;
    if (addresslow == 0) {
  digitalWrite(SLAVESELECT,HIGH); //release chip
  //wait for eeprom to finish writing
  delay(3000);
  Serial.println("Changing Pages");
  addressmed++;

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN); //write enable
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
  
   Serial.println("");
   Serial.print("Address High = ");
   Serial.println(addresshigh,HEX); 
   Serial.print("Address Med = ");
   Serial.println(addressmed,HEX);  
   Serial.print("Address Low = ");
   Serial.println(addresslow,HEX); 
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WRITE); //write instruction
  spi_transfer((char)(addresshigh));   //send MSByte address first
  spi_transfer((char)(addressmed));   //send MSByte address first
  spi_transfer((char)(addresslow));      //send LSByte address    
    }
  }
  
  digitalWrite(SLAVESELECT,HIGH); //release chip
  //wait for eeprom to finish writing
  delay(3000);
 
 Serial.println("DoneWriting");
 
}
    voiceindex = 0 ;

    
  }

}

ISR(INT0_vect) {
  while (!(PIND & _BV(PD3))){};
 // pin_buffer[j] = PINC;    //read datalines and place in buffer
 //j++;

while (j<5){
  while ((PIND & _BV(PD2)) == 0){
    //PORTC ^= _BV(PC4);
    ;
  }
  
  pin_buffer[j] = PINC;    //read datalines and place in buffer
 j++;
  while (PIND & _BV(PD2)){
    //PORTC ^= _BV(PC4);
    ;
  }


    }

mask=1;
voiceindex = 0 ;
eepromread = 0;
firstrun = 1;
        PORTB |= B00000100; //digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
        //digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
        addresshigh = pin_buffer[0] & B00001111;
        addresshigh <<= 4;
        addresshigh = addresshigh | (pin_buffer[0] & B00001111);
        
        addressmed = pin_buffer[1] & B00001111;
        addressmed <<= 4;
        addressmed = addressmed + (pin_buffer[2] & B00001111);        

        addresslow = pin_buffer[3] & B00001111;
        addresslow <<= 4;
        addresslow = addresslow + (pin_buffer[4] & B00001111);
        
        PORTC ^= _BV(PC4);
        read_eeprom(addresshigh,addressmed,addresslow);
        PORTC ^= _BV(PC4);
        delayMicroseconds(30);
        shifter = spi_transfer(0xFF);
        PORTC ^= _BV(PC4);        
        mask=1;
        if (shifter & mask) PORTC |= _BV(PC5);
        else PORTC &= ~_BV(PC5);
time=0;
 EIFR = bit (INTF0); 
  EIFR = bit (INTF1); 
  
}

ISR(INT1_vect) {
  if (PIND & _BV(PD4)) tempdata |= B10000000; 
  //PORTC ^= _BV(PC4);
    mask <<= 1;
    //tempdata <<=1;
    while (mask){
          //tempdata <<=1;
          while (PIND & _BV(PD3)){
          if (PIND & _BV(PD4)) tempdata |= B10000000;         
          }
            //PORTC ^= _BV(PC4);
          //if (worddata[eepromread] & mask) PORTC |= _BV(PC5);
          if (shifter & mask) PORTC |= _BV(PC5);
          else PORTC &= ~_BV(PC5);
          //PORTC ^= _BV(PC4);
          mask <<= 1;
          tempdata >>=1;
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
      if (PIND & _BV(PD4)) tempdata |= B10000000;
      mask=1;
      maxvoice[voiceindex]= tempdata;
      voiceindex++;
      //PORTC ^= _BV(PC4);
      shifter = spi_transfer(0xFF);
      //PORTC ^= _BV(PC4);      
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
    
    tempdata=0;
    delayMicroseconds(30);
  if (PIND & _BV(PD4)) tempdata |= B10000000; 
  PORTC ^= _BV(PC4);
    tempdata <<=1;
    time=0;
    //voiceindex=0;
    EIFR = bit (INTF1); 
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
  PORTB |= B00000100; //release chip
  delayMicroseconds(100);
  PORTB &= B11111011; //digitalWrite(SLAVESELECT,LOW);  //select chip
  spi_transfer(READ); //transmit read opcode
  spi_transfer((char)(EEPROM_addresshigh));   //send MSByte address first
  spi_transfer((char)(EEPROM_addressmed));   //send MSByte address first
  spi_transfer((char)(EEPROM_addresslow));      //send LSByte address
  //data = spi_transfer(0xFF); //get data byte
  //digitalWrite(SLAVESELECT,HIGH); //release chip, signal end transfer
  data=0;
  return data;
}
