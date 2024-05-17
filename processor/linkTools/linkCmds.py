'''
linkCmds.py: Implements PC-based CLI/functions for controlling RISCII "J-Link".

Script can be used in two ways: 1) Run as a python script, which implements a
CLI that interfaces with the "J-Link" (and by extension, RISCII processor) or
2) as a referenced python library with functions that interface with the
"J-Link".

linkCmds.py are written with user's intentions in mind- focused on broader
acions (e.g. read/write memory, programming, etc). At present, the script
focuses on actions that benefit RISCII processor development.

Note: Since the "J-Link" firmware/hardware mainly abstracts the JTAG signals,
this script is susceptible to changes to 1) the RISCII JTAG port 2) changes
to the SPI EEPROM chip used and 3) the "J-Link" firmware commands.
'''

import re
import serial
import sys

## -------------------------------------------------------------------------- ##

# Definitions for Serial connection.
SERIAL_COM_PORT = "COM3"
SERIAL_RATE     = 9600
SERIAL_TIMEOUT  = 1

# Definitions of special "send" chars.
CHAR_ESC_CHAR   = 0xFF
CHAR_EXE_CMD    = 0x0A

# Definitions of special CLI chars/sequences.
CLI_PREFIX      = "> "
CLI_SPACE       = ' '
CLI_TAB         = '\t'
CLI_LF          = '\n'
CLI_CR          = '\r'

# Definitions of valid command regexes.
CMD_HELP        = 'h|help'
CMD_QUIT        = 'q|quit'
CMD_COM         = '(?:c|com) [0-9]+'
CMD_PERIOD      = '(?:p|period) [0-9]+'
CMD_READ        = '(?:r|read) 0x[0-9a-fA-F]{1,4}'
CMD_WRITE       = '(?:w|write) 0x[0-9a-fA-F]{1,4} 0x[0-9a-fA-F]{1,4}'
CMD_SPI_READ    = '(?:sr|spi_read) 0x[0-9a-fA-F]{1,4} [0-9]+'
CMD_SPI_WRITE   = '(?:sw|spi_write) 0x[0-9a-fA-F]{1,4} 0x(?:[0-9a-fA-F]{2})+'
CMD_BSCAN       = '(?:bs|bscan) [0-9]+ [10zZ]'
CMD_BSCAN_RESET = '(?:br|bscan_reset)'

# Definitions for building "J-link" commands.
LINK_CMD_PERIOD = b'\x30'
LINK_CMD_INSTR  = b'\x31'
LINK_CMD_DATA   = b'\x32'
LINK_EXE_CMD    = b'\x0A'

# Definitions for building "JTAG/Hardware" commands.
JTAG_PASS       = b'\x00'
JTAG_ADDR       = b'\x01'
JTAG_READ       = b'\x02'
JTAG_WRITE      = b'\x03'
JTAG_SPI        = b'\x05'
JTAG_SCAN       = b'\x04'

# Definitions for building "SPI EEPROM" commands.
SPI_WREN        = b'\x06'
SPI_READ        = b'\x03'
SPI_WRITE       = b'\x02'

# Static variable needed for boundary scanning purposes.
g_BscanSetting = list('0' * 168) # 56 pins * 3 bit settings each

## -- Public/Library Functions ---------------------------------------------- ##

# Function to handle printing of "CLI help menu" (for CLI usage).
def pHelp():
    # Print out summary of available commands
    print("q|quit                             : exit CLI")
    print("h|help                             : print this blurb")
    print("c|com <number>                     : use COM port <number>")
    print("p|period <number>                  : set JTAG period to <number> ms")
    print("r|read 0x<address>                 : read data at RAM <address>")
    print("w|write 0x<address> 0x<value>      : write <value> to RAM <address>")
    print("sr|spi_read 0x<address> <number>   : read <number> bytes from SPI starting at <address>")
    print("sw|spi_write 0x<address> 0x<value> : write <value> to SPI starting at <address>")
    print("bs|bscan <pin number> <0|1|z>      : set <pin number> to <0|1|z>, reading back value")
    print("br|bscan_reset                     : reset bscan register")

# Function to handle setting COM port "J-Link" is connected to.
def com(tkns):
    # Set current COM port to given number (2nd token).
    global SERIAL_COM_PORT
    oldPort = SERIAL_COM_PORT
    SERIAL_COM_PORT = "COM" + str(tkns[1])
    
    # Let user know of update.
    print("%s -> %s"%(oldPort, SERIAL_COM_PORT))

# Function to handle setting period "J-Link" uses for JTAG speed.
def period(tkns):
    # Determine bytes to send (argument = 2nd token).
    asHex = '{:04x}'.format(int(tkns[1]))
    sendBytes = LINK_CMD_PERIOD + bytearray.fromhex(asHex)
    
    # Send/Receive from J-Link.
    retBytes = _transferBytes(sendBytes)
    
    # (Skip if in error).
    if retBytes == None: return
    
    # Let user know of update.
    oldPeriod = int.from_bytes(retBytes[-3:-1], "big")
    print("%s ms -> %s ms"%(oldPeriod, tkns[1]))

# Function to handle reading a word from RAM address.
def read(tkns):
    # Determine bytes to send (address = 2nd token).
    hexAddr = tkns[1][2:].zfill(4)
    addrIn  = LINK_CMD_DATA + bytearray.fromhex(hexAddr)
    addrCmd = LINK_CMD_INSTR + JTAG_ADDR
    readCmd = LINK_CMD_INSTR + JTAG_READ
    readOut = LINK_CMD_DATA + bytearray.fromhex("0000")
    
    # Set up JTAG/MCU with correct address.
    _transferBytes(addrIn)
    _transferBytes(addrCmd)
    
    # Read out data at given address.
    _transferBytes(readCmd)
    retBytes = _transferBytes(readOut)
    
    # (Skip if in error).
    if retBytes == None: return
    
    # Report read data.
    retHex = "0x" + retBytes[1:3].hex()      # Chop off overhead data
    print ("MEM[0x%s] = %s"%(hexAddr, retHex))

# Function to handle writing a word to RAM address.
def write(tkns):
    # Determine bytes to send (address = 2nd token, value = 3rd token).
    hexAddr  = tkns[1][2:].zfill(4)
    hexData  = tkns[2][2:].zfill(4)
    addrIn   = LINK_CMD_DATA + bytearray.fromhex(hexAddr)
    addrCmd  = LINK_CMD_INSTR + JTAG_ADDR
    dataIn   = LINK_CMD_DATA + bytearray.fromhex(hexData)
    writeCmd = LINK_CMD_INSTR + JTAG_WRITE
    
    # Set up JTAG/MCU with correct address.
    _transferBytes(addrIn)
    _transferBytes(addrCmd)
    
    # Write data at given address.
    _transferBytes(dataIn)
    res = _transferBytes(writeCmd)
    
    # (Skip if in error).
    if res == None: return
    
    # Report actions to user.
    print ("0x%s => MEM[0x%s]"%(hexData, hexAddr))

# Function to handle reading from SPI EEPROM address (limit to page).
def spi_read(tkns):
    # Determine bytes to send (address = 2nd token, # bytes to read = 3rd token).
    hexAddr = tkns[1][2:].zfill(4)
    fillIn = "0".zfill(int(tkns[2]) * 2)
    spiCmd = LINK_CMD_INSTR + JTAG_SPI
    readCmd = LINK_CMD_DATA + SPI_READ + bytearray.fromhex(hexAddr) + bytearray.fromhex(fillIn)
    resetCmd = LINK_CMD_INSTR + JTAG_PASS
    
    # Enable SPI mode.
    res = _transferBytes(spiCmd)
    
    # (Skip if in error).
    if res == None: return
    
    # Read SPI data.
    retBytes = _transferBytes(readCmd)

    # Report read data.
    retHex = "0x" + retBytes[4:-1].hex()      # Chop off overhead data
    print ("SPI_MEM[0x%s...] = %s"%(hexAddr, retHex))
    
    # Reset SPI mode selection.
    _transferBytes(resetCmd)

# Function to handle writing to SPI EEPROM address (limit to page).
def spi_write(tkns):
    # Determine bytes to send (address = 2nd token,  value = 3rd token).
    hexAddr  = tkns[1][2:].zfill(4)
    hexData  = tkns[2][2:]          # Regex ensures it aligns to bytes
    spiCmd = LINK_CMD_INSTR + JTAG_SPI
    wrenCmd  = LINK_CMD_DATA + SPI_WREN
    writeCmd = LINK_CMD_DATA + SPI_WRITE + bytearray.fromhex(hexAddr) + bytearray.fromhex(hexData)
    resetCmd = LINK_CMD_INSTR + JTAG_PASS
    
    # Enable SPI mode.
    retBytes = _transferBytes(spiCmd)
    
    # (Skip if in error).
    if retBytes == None: return
    
    # Enable writes on the SPI (good until actual write occurs).
    _transferBytes(wrenCmd)
    
    # Write data to SPI EEPROM.
    _transferBytes(writeCmd)
    
    # Report action to user.
    print("0x%s => SPI_MEM[0x%s...]"%(hexData, hexAddr))
    
    # Reset SPI mode selection.
    _transferBytes(resetCmd)

# Function to handle setting pin via boundary scan to certain value.
def bscan(tkns):
    # Determine mask created by pin change request.
    newMask = ''
    if (tkns[2] == '1'):   newMask = '110'
    elif (tkns[2] == '0'): newMask = '010'
    else:                  newMask = '000'
    
    # Update local copy of bscan settings
    global g_BscanSetting
    basdIdx = min(int(tkns[1]) * 3, 165)
    g_BscanSetting[basdIdx + 0] = newMask[2] # input field
    g_BscanSetting[basdIdx + 1] = newMask[1] # tristate field
    g_BscanSetting[basdIdx + 2] = newMask[0] # output field
    
    # Get byte array from settings to update MCU.
    scanData = bytearray()
    for i in range(int(len(g_BscanSetting) / 8)):
        newByte = 0
        for j in range(7, -1, -1):
            newByte = newByte << 1
            if (g_BscanSetting[8 * i + j] == '1'): newByte = newByte | 0x1
        scanData = newByte.to_bytes(1, 'little') + scanData
    
    # Determine bytes to send.
    scanCmd  = LINK_CMD_INSTR + JTAG_SCAN
    dataCmd  = LINK_CMD_DATA + scanData
    
    # Enable scan mode.
    retBytes = _transferBytes(scanCmd)
    
    # (Skip if in error).
    if retBytes == None: return
    
    # Send over data- recording shifted out settings.
    retBytes = _transferBytes(dataCmd)
    
    # Parse data for changed pin's last reading.
    byteIdx = int(basdIdx / 8)
    bitIdx  = basdIdx - (8 * byteIdx)
    readValue = (int(retBytes[20 - byteIdx + 1]) >> bitIdx) & 0x1
    print("%s => bscan[%d] (last read = %d)"%(tkns[2], int(tkns[1]), readValue))

# Function to handle setting pin via boundary scan to certain value.
def bscan_reset(tkns):
    # Update local copy of bscan settings
    global g_BscanSetting
    g_BscanSetting = list('0' * 168) # 56 pins * 3 bit settings each
    
    # Get byte array from settings to update MCU.
    scanData = bytearray()
    for i in range(int(len(g_BscanSetting) / 8)):
        newByte = 0
        for j in range(7, -1, -1):
            newByte = newByte << 1
            if (g_BscanSetting[8 * i + j] == '1'): newByte = newByte | 0x1
        scanData = newByte.to_bytes(1, 'little') + scanData
    
    # Determine bytes to send.
    scanCmd  = LINK_CMD_INSTR + JTAG_SCAN
    dataCmd  = LINK_CMD_DATA + scanData
    
    # Enable scan mode.
    retBytes = _transferBytes(scanCmd)
    
    # (Skip if in error).
    if retBytes == None: return
    
    # Send over data.
    _transferBytes(dataCmd)
    
    # Parse data for changed pin's last reading.
    print("BScan register reset to 0x0")

## -- Helper/Internal/CLI Functions ----------------------------------------- ##

# Helper function to handle actual byte transfer to "J-Link".
def _transferBytes(sendBytes):
    # (Try to) connect to serial port/link tool. # TODO- this should happen once per session, NOT command
    linkSerial = None
    try:
        global SERIAL_COM_PORT
        linkSerial = serial.Serial(port=SERIAL_COM_PORT, baudrate=SERIAL_RATE, timeout=SERIAL_TIMEOUT)
    except:
        print("Failed to connect to serial port \"%s\""%(SERIAL_COM_PORT))
        return None
    
    # Send byte array (adding escape chars as needed).
    for i in range(len(sendBytes)):
        # Add escape char if needed.
        if (sendBytes[i] == CHAR_ESC_CHAR) or (sendBytes[i] == CHAR_EXE_CMD):
            linkSerial.write(CHAR_ESC_CHAR.to_bytes(1, "big"))
        
        # Send next byte.
        linkSerial.write(sendBytes[i].to_bytes(1, "big"))
    
    # Bytes sent- trigger command execution by sending "execute" byte.
    linkSerial.write(CHAR_EXE_CMD.to_bytes(1, "big"))
    
    # Link tool response should match sent length (sendBytes + CHAR_EXE_CMD).
    retMsg = b''
    while len(retMsg) < (len(sendBytes) + 1):
        retMsg += linkSerial.readline()
        
    # Close serial.
    linkSerial.close()
    
    # Return bytes to caller.
    return retMsg

## -------------------------------------------------------------------------- ##

# Help function to conduct CLI (if script is called- vs. referenced).
def _doCli():
    # Loop until broken from within.
    while True:
        # Prompt input.
        print("> ", end="")
        sys.stdout.flush()
        
        # Get input.
        rawInput = sys.stdin.readline()
        
        # Tokenize for convenience of command execution.
        rawInput = rawInput.replace(CLI_CR, '').replace(CLI_LF, '').replace(CLI_TAB, CLI_SPACE)
        tkns = [x for x in rawInput.split(CLI_SPACE) if x]
        
        # Run appropriate procedure.
        if re.fullmatch(CMD_QUIT, rawInput): return
        elif re.fullmatch(CMD_HELP, rawInput):      pHelp()
        elif re.fullmatch(CMD_COM, rawInput):       com(tkns)
        elif re.fullmatch(CMD_PERIOD, rawInput):    period(tkns)
        elif re.fullmatch(CMD_READ, rawInput):      read(tkns)
        elif re.fullmatch(CMD_WRITE, rawInput):     write(tkns)
        elif re.fullmatch(CMD_SPI_READ, rawInput):  spi_read(tkns)
        elif re.fullmatch(CMD_SPI_WRITE, rawInput): spi_write(tkns)
        elif re.fullmatch(CMD_BSCAN, rawInput):     bscan(tkns)
        elif re.fullmatch(CMD_BSCAN_RESET, rawInput): bscan_reset(tkns)
        else: print("Unknown command: \"%s\""%(rawInput))

## -------------------------------------------------------------------------- ##

# Main program (if called- vs referenced)- run CLI operation of "J-Link".
if __name__ == "__main__":
    # Run CLI for user.
    print("=== Project RISCII Link CLI ===")
    _doCli()
