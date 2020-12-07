# maxxsteele
//Maxx Steele Voice LSI LC8100 and LSI LC3100 voice 

//1984 Ideal Robot CBS Toys

//Int0 is called to read the address sent to the LC3100 ROM

//Int1 is called to read the contents that is returned by the LC3100.  it will also be used to inject new data from an EEPROM.

Cut trace between LC3100 Pin 22 and LC8100 Pin 8  (Serial Data) so I can feed eeprom data back to voice processor.
