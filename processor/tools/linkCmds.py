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

################################################################################
## -- Constants/Globals --
################################################################################

# Definitions for Serial connection.
SERIAL_COM_PORT = "COM3"
SERIAL_RATE     = 9600
SERIAL_TIMEOUT  = 1

# Definitions of special CLI chars/sequences.
CLI_PREFIX      = "> "
CLI_SPACE       = ' '
CLI_TAB         = '\t'
CLI_LF          = '\n'
CLI_CR          = '\r'

# Definitions of valid command regexes.
CMD_QUIT        = 'q|quit'
CMD_HELP        = 'h|print_help'
CMD_COM         = '(?:c|set_com) [0-9]+'
CMD_PERIOD      = '(?:p|set_period) [0-9]+'
CMD_MEM_READ    = '(?:mr|mem_read) 0x[0-9a-fA-F]{1,4}'
CMD_MEM_WRITE   = '(?:mw|mem_write) 0x[0-9a-fA-F]{1,4} 0x[0-9a-fA-F]{1,4}'
CMD_STORE_READ  = '(?:sr|store_read) 0x[0-9a-fA-F]{1,4} [0-9]+'
CMD_STORE_WRITE = '(?:sw|store_write) 0x[0-9a-fA-F]{1,4} 0x(?:[0-9a-fA-F]{2})+'
CMD_SCAN_SET    = '(?:bs|bscan_set) [0-9]+ [10zZ]'
CMD_SCAN_RESET  = '(?:br|bscan_reset)'
CMD_JTAG_PAUSE  = '(?:jp|jtag_pause)'
CMD_JTAG_RUN    = '(?:jr|jtag_run)'

# Definitions for building "J-Link" messages.
LINK_CMD_PERIOD = b'\x30'
LINK_CMD_INSTR  = b'\x31'
LINK_CMD_DATA   = b'\x32'
LINK_CHAR_ESC   = b'\xFF'
LINK_CHAR_EXE   = b'\x0A'
LINK_RESP_GOOD  = b'\x01'
LINK_RESP_FAIL  = b'\x00'

# Definitions for building "JTAG/Hardware" commands.
JTAG_PASS       = b'\x00'
JTAG_ADDR       = b'\x01'
JTAG_READ       = b'\x02'
JTAG_WRITE      = b'\x03'
JTAG_SCAN       = b'\x04'
JTAG_STORE      = b'\x05'
JTAG_PAUSE      = b'\x06'
JTAG_RUN        = b'\x07'

# Definitions for commands on EEPROM chip (ie storage on MCU).
SPI_READ        = b'\x03'
SPI_WRITE       = b'\x02'
SPI_WREN        = b'\x06'

# Definitions for MCU states (as read over JTAG).
MCU_BOOTING     = 0
MCU_RUNNING     = 1
MCU_PAUSED      = 3

# Static variable needed for boundary scanning purposes.
g_bscanSetting = list('0' * 168) # 56 pins * 3 bit settings each

################################################################################
## -- Public/Library Functions --
################################################################################

'''
Prints help blurb for available CLI commands.
'''
def print_help():
    # Print out summary of available commands.
    print("q|quit                          : exit CLI program")
    print("h|print_help                    : print this blurb")
    print("c|set_com <num>                 : set COM port of RISCII 'J-Link'")
    print("p|set_period <num>              : set JTAG's TCK period (in ms)")
    print("mr|mem_read 0x<addr>            : read word to memory")
    print("mw|mem_write 0x<addr> 0x<val>   : write word to memory")
    print("sr|store_read 0x<addr> <val>    : read bytes from MCU's storage")
    print("sw|store_write 0x<addr> 0x<val> : write bytes to MCU's storage")
    print("bs|bscan_set <num> <0|1|z>      : set boundary scan pin")
    print("br|bscan_reset                  : reset all boundary scan pins")
    print("jp|jtag_pause                   : pause MCU using JTAG")
    print("jr|jtag_run                     : run MCU using JTAG")

## -----------------------------------------------------------------------------
'''
Sets COM port used to communicate to "J-Link".

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return previous COM port (as string)
'''
def set_com(tkns, isSilent = True):
    # Swap current/new COM ports.
    global SERIAL_COM_PORT
    oldPort = SERIAL_COM_PORT
    SERIAL_COM_PORT = "COM" + str(tkns[1])
    
    # Report results.
    retVal = oldPort
    if not isSilent:
        print("%s -> %s"%(oldPort, SERIAL_COM_PORT))
    return retVal

## -----------------------------------------------------------------------------
'''
Sets speed of JTAG connection (good way to check if J-Link is connected).

Note: limit given period to uint16_t range for best results.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def set_period(tkns, isSilent = True):
    # Determine bytes to send.
    asHex = '{:04x}'.format(int(tkns[1]))
    sendBytes = LINK_CMD_PERIOD + bytearray.fromhex(asHex)
    
    # Send/Receive from J-Link.
    retBytes = _transferBytes(sendBytes, isSilent)
    
    # (Skip if in error).
    if retBytes == None or retBytes[0] == LINK_RESP_FAIL: return
    
    # Report results.
    oldPeriod = int.from_bytes(retBytes[-3:-1], "big")
    if not isSilent:
        print("%s ms -> %s ms"%(oldPeriod, tkns[1]))
    return retBytes

## -----------------------------------------------------------------------------
'''
Reads word from memory on MCU.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def mem_read(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None

    # Determine bytes to send.
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
    
    # Report results.
    retHex = "0x" + retBytes[1:3].hex()      # Chop off overhead data
    if not isSilent:
        print ("MEM[0x%s] = %s"%(hexAddr, retHex))
    return retBytes

## -----------------------------------------------------------------------------
'''
Writes word to memory on MCU.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def mem_write(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None

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
    retBytes = _transferBytes(writeCmd)
    
    # Report results.
    if not isSilent:
        print ("0x%s => MEM[0x%s]"%(hexData, hexAddr))
    return retBytes

## -----------------------------------------------------------------------------
'''
Reads multiple bytes from storage on MCU.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def store_read(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None

    # Determine bytes to send.
    hexAddr = tkns[1][2:].zfill(4)
    fillIn = "0".zfill(int(tkns[2]) * 2)
    storeCmd = LINK_CMD_INSTR + JTAG_STORE
    readCmd = LINK_CMD_DATA + SPI_READ                   \
                            + bytearray.fromhex(hexAddr) \
                            + bytearray.fromhex(fillIn)
    resetCmd = LINK_CMD_INSTR + JTAG_PASS
    
    # Enable storage mode.
    _transferBytes(storeCmd)
    
    # Read storage data.
    retBytes = _transferBytes(readCmd)
    
    # Reset storage mode selection.
    _transferBytes(resetCmd)

    # Report results.
    retHex = "0x" + retBytes[4:-1].hex()      # Chop off overhead data
    if not isSilent:
        print ("STORE_MEM[0x%s...] = %s"%(hexAddr, retHex))
    return retBytes

## -----------------------------------------------------------------------------
'''
Writes multiple bytes to storage on MCU.

Note: Based on underlying chip, writes should be limited to 128 byte pages with
5 ms pauses in-between writes.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def store_write(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None

    # Determine bytes to send (address = 2nd token,  value = 3rd token).
    hexAddr  = tkns[1][2:].zfill(4)
    hexData  = tkns[2][2:]          # Regex ensures it aligns to bytes
    storeCmd = LINK_CMD_INSTR + JTAG_STORE
    wrenCmd  = LINK_CMD_DATA + SPI_WREN
    writeCmd = LINK_CMD_DATA + SPI_WRITE                  \
                             + bytearray.fromhex(hexAddr) \
                             + bytearray.fromhex(hexData)
    resetCmd = LINK_CMD_INSTR + JTAG_PASS
    
    # Enable storage mode.
    _transferBytes(storeCmd)

    # Write data to storage.
    _transferBytes(wrenCmd)  # enables write on underlying until write occurs
    _transferBytes(writeCmd)
    
    # Reset SPI mode selection.
    retBytes = _transferBytes(resetCmd)
    
    # Report results.
    if not isSilent:
        print("0x%s => SPI_MEM[0x%s...]"%(hexData, hexAddr))
    return retBytes

## -----------------------------------------------------------------------------
'''
Sets boundary scan register for given pin to given value.

Updates local copy of boundary scan register and shifts it into MCU's boundary
scan register. Previous value of changed pin is reported along with change.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def scan_set(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None
    
    # Determine mask created by pin change request.
    newMask = ''
    if (tkns[2] == '1'):   newMask = '110'
    elif (tkns[2] == '0'): newMask = '010'
    else:                  newMask = '000'
    
    # Update local copy of bscan settings
    global g_bscanSetting
    basdIdx = min(int(tkns[1]) * 3, 165)
    g_bscanSetting[basdIdx + 0] = newMask[2] # input field
    g_bscanSetting[basdIdx + 1] = newMask[1] # tristate field
    g_bscanSetting[basdIdx + 2] = newMask[0] # output field
    
    # Get byte array from settings to update MCU.
    scanData = bytearray()
    for i in range(int(len(g_bscanSetting) / 8)):
        newByte = 0
        for j in range(7, -1, -1):
            newByte = newByte << 1
            if (g_bscanSetting[8 * i + j] == '1'): newByte = newByte | 0x1
        scanData = newByte.to_bytes(1, 'little') + scanData
    
    # Determine bytes to send.
    scanCmd  = LINK_CMD_INSTR + JTAG_SCAN
    dataCmd  = LINK_CMD_DATA + scanData
    
    # Enable scan mode.
    _transferBytes(scanCmd)
    
    # Send over data- recording shifted out settings.
    retBytes = _transferBytes(dataCmd)
    
    # Report results.
    byteIdx = int(basdIdx / 8)
    bitIdx  = basdIdx - (8 * byteIdx)
    readValue = (int(retBytes[20 - byteIdx + 1]) >> bitIdx) & 0x1
    if not isSilent:
        print("%s => bscan[%d] (last read = %d)"%(tkns[2],
                                                  int(tkns[1]), 
                                                  readValue)
                                                 )
    return retBytes

## -----------------------------------------------------------------------------
'''
Resets boundary scan register (local copy and MCU copy).

Updates local copy of boundary scan register and shifts it into MCU's boundary
scan register. Previous value of changed pin is reported along with change.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def scan_reset(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None

    # Update local copy of bscan settings.
    global g_bscanSetting
    g_bscanSetting = list('0' * 168) # 56 pins * 3 bit settings each
    
    # Get byte array from settings to update MCU.
    scanData = bytearray()
    for i in range(int(len(g_bscanSetting) / 8)):
        newByte = 0
        for j in range(7, -1, -1):
            newByte = newByte << 1
            if (g_bscanSetting[8 * i + j] == '1'): newByte = newByte | 0x1
        scanData = newByte.to_bytes(1, 'little') + scanData
    
    # Determine bytes to send.
    scanCmd  = LINK_CMD_INSTR + JTAG_SCAN
    dataCmd  = LINK_CMD_DATA + scanData
    
    # Enable scan mode.
    _transferBytes(scanCmd)
    
    # Send over data.
    retBytes = _transferBytes(dataCmd)
    
    # Report results.
    if not isSilent:
        print("BScan register reset to 0x0")
    return retBytes

## -----------------------------------------------------------------------------
'''
Sets MCU to 'paused' state.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def jtag_pause(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_RUNNING, isSilent):
        if not isSilent: print("MCU not in 'running' state")
        return None
    
    # Send pause command.
    pauseCmd = LINK_CMD_INSTR + JTAG_PAUSE
    retBytes = _transferBytes(pauseCmd)
    
    # Report results.
    if not isSilent:
        print("MCU is pausing/paused")
    return retBytes

## -----------------------------------------------------------------------------
'''
Sets MCU to 'running' state.

@param tkns list of stringified token variables (includes command name)
@param isSilent bool controlling if results print to stdout- True by default
@return returned bytes from J-Link is successful, None otherwise
'''
def jtag_run(tkns, isSilent = True):
    # Make sure JTAG/MCU is in state to run necessary commands.
    if not _inMCUState(MCU_PAUSED, isSilent):
        if not isSilent: print("MCU not in 'paused' state")
        return None
    
    # Send pause command.
    runCmd = LINK_CMD_INSTR + JTAG_RUN
    retBytes = _transferBytes(runCmd)
    
    # Report results.
    if not isSilent:
        print("MCU is now running")
    return retBytes

################################################################################
## -- Helper/Internal Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Confirms is MCU is in a certain state.

Used to ensure MCU is in a state where commands can be run successfully. Checks
MCU directly, so it effectively can also confirm if COM/J-Link connection is
in usable state as well.

@param desiredState MCU state/status bits to match to
@param isSilent bool controlling if results print to stdout- True by default
@return true if MCU is in desired state, false otherwise.
'''
def _inMCUState(desiredState, isSilent = True):
    # Craft message to check MCU's status bits.
    statCmd = LINK_CMD_INSTR + JTAG_PASS
    
    # Transfer message.
    retBytes = _transferBytes(statCmd, isSilent)
    
    # Check transfer was successul (COM/J-Link connection).
    if retBytes == None or retBytes[0] == LINK_RESP_FAIL: return False

    # Check state of MCU.
    retBool = retBytes[1] == desiredState
    
    # Return result of state check.
    return retBool

## -----------------------------------------------------------------------------
'''
Helper function to manage main transfer between PC, J-Link, and MCU.

Function opens and closes connection to J-Link while fufilling request. If the
COM port cannot be connected to, the function returns 'None' in error.

@param sendBytes ordered bytearray of bytes to send to J-Link.
@param isSilent bool controlling if results print to stdout- True by default
@return ordered bytearray of bytes returned from J-Link
'''
def _transferBytes(sendBytes, isSilent = True):
    # (Try to) connect to serial port/link tool.
    linkSerial = None
    try:
        global SERIAL_COM_PORT
        linkSerial = serial.Serial(port=SERIAL_COM_PORT,  \
                                   baudrate=SERIAL_RATE,  \
                                   timeout=SERIAL_TIMEOUT \
                                  )
    except:
        if not isSilent:
            print("Failed to connect to serial port \"%s\""%(SERIAL_COM_PORT))
        return None
    
    # Send byte array (adding escape chars as needed).
    for i in range(len(sendBytes)):
        # Add escape char if needed.
        if (sendBytes[i] == LINK_CHAR_ESC) or (sendBytes[i] == LINK_CHAR_EXE):
            linkSerial.write(LINK_CHAR_ESC.to_bytes(1, "big"))
        
        # Send next byte.
        linkSerial.write(sendBytes[i].to_bytes(1, "big"))
    
    # Bytes sent- trigger command execution by sending "execute" byte.
    linkSerial.write(LINK_CHAR_EXE)
    
    # Link tool response should match sent length (sendBytes + LINK_CHAR_EXE).
    retMsg = b''
    while len(retMsg) < (len(sendBytes) + 1):
        retMsg += linkSerial.readline()
        
    # Close serial.
    linkSerial.close()
    
    # Return bytes to caller.
    return retMsg

################################################################################
## -- CLI-related Functions --
################################################################################

## -----------------------------------------------------------------------------
'''
Main loop to conduct CLI program.
'''
def _doCli():
    # Loop until broken from within.
    while True:
        # Prompt input.
        print("> ", end="")
        sys.stdout.flush()
        
        # Get input.
        rawInput = sys.stdin.readline()
        
        # Tokenize for convenience of command execution.
        rawInput = rawInput.replace(CLI_CR, '')        \
                           .replace(CLI_LF, '')        \
                           .replace(CLI_TAB, CLI_SPACE)
        tkns = [x for x in rawInput.split(CLI_SPACE) if x]
        
        # Run appropriate procedure.
        if re.fullmatch(CMD_QUIT, rawInput)          : return
        elif re.fullmatch(CMD_HELP, rawInput)        : print_help()
        elif re.fullmatch(CMD_COM, rawInput)         : set_com(tkns, False)
        elif re.fullmatch(CMD_PERIOD, rawInput)      : set_period(tkns, False)
        elif re.fullmatch(CMD_MEM_READ, rawInput)    : mem_read(tkns, False)
        elif re.fullmatch(CMD_MEM_WRITE, rawInput)   : mem_write(tkns, False)
        elif re.fullmatch(CMD_STORE_READ, rawInput)  : store_read(tkns, False)
        elif re.fullmatch(CMD_STORE_WRITE, rawInput) : store_write(tkns, False)
        elif re.fullmatch(CMD_SCAN_SET, rawInput)    : scan_set(tkns,False)
        elif re.fullmatch(CMD_SCAN_RESET, rawInput)  : scan_reset(tkns, False)
        elif re.fullmatch(CMD_JTAG_PAUSE, rawInput)  : jtag_pause(tkns, False)
        elif re.fullmatch(CMD_JTAG_RUN, rawInput)    : jtag_run(tkns, False)
        else: print("Unknown/Malformed command: \"%s\""%(rawInput))

## -----------------------------------------------------------------------------
'''
Main program- when called directly, run as CLI program for J-Link.
'''
if __name__ == "__main__":
    # Run CLI for user.
    print("=== Project RISCII Link CLI ===")
    _doCli()
