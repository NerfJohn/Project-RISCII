// TODO- name/desc of firmware.

#include <Serial.h>

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

/*
 * General Idea(s):
 * 
 * Objective (For now): Relay data between CPU and MCU (USB and 'JTAG' respectively), explicitly for memory map accessing.
 * 
 * Commands/Actions to cover:
 *  -> Set 16-bit address
 *  -> Read (and report back) 16-bits from set address
 *  -> Write 16-bits to set address
 *  
 * Approach:
 *  -> Python script intakes live (or file?) commands- structured "a30", "r", "w-2", etc
 *  -> Script sends message over USB to link, which uses built-in functions to handle request types
 *  -> Link sends back status code + read back data to python script for display/reporting
 *  
 *  -> Link understands specific commands- will want to generic-icize and let python heavy lift that as possible
 *  -> Link can only perform one command at a time- could buffer, but likely best way for simplicity.
 *  
 * Possible link functions:
 *  -> jtagSend(tdiBit, tmsBit): sets input bits, hold high then low for halves, then returns tdo read
 *  -> sendInstr(cmdByte): sends cmdByte to jtag (into instruction register), returns tdo output (or nothing- should be 0s)
 *  -> sendData(dataWord): sends dataWord to jtag (into data register), returns tdo output
 */
