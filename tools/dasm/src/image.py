################################################################################
# image.py: handles reading/parsing of binary image (based on uP design).
# by John Eslinger
################################################################################

import os

################################################################################

# Binary image specific parameters.
__MIN_BIN_SIZE = 8
__MAX_BIN_SIZE = 65536

################################################################################
# Broken down summary of the image's contents.
class BinImage:
    readFile  = False # Fail until proven otherwise
    resultMsg = None
    text      = None
    data      = None

################################################################################
# Reads/parses file into basic components.
def parse_image(fpath: str):
    # Return value (fail until otherwise).
    res = BinImage()
    res.readFile = False

    # Ensure file exists.
    if os.path.exists(fpath) is False:
        res.resultMsg = f"File {fpath} not found"
        return res
    
    # Ensure file is within expected size/word chunks.
    fsize = os.path.getsize(fpath)
    if fsize < __MIN_BIN_SIZE:
        res.resultMsg = f"File is too small ({fsize} < {__MIN_BIN_SIZE})"
        return res
    if fsize > __MAX_BIN_SIZE:
        res.resultMsg = f"File is too large ({fsize} > {__MAX_BIN_SIZE})"
        return res
    if fsize % 2 != 0:
        res.resultMsg = f"File is not a multiple a 2 bytes (ie 16-bit word)"
        return res
    
    # Open/parse file.
    with open(fpath, 'rb') as f:
        # Step #1: Extract text metadata.
        textSize = f.read(2)
        if not textSize:
            res.resultMsg = "Failed to read text metadata"
            return res
        textSize = int.from_bytes(textSize, byteorder='big', signed=False)
        
        # Step #2: Extract text section.
        res.text = f.read(textSize * 2)
        if not res.text:
            res.resultMsg = "Failed to read text section"

        # Step #3: Extract data metadata.
        dataSize = f.read(2)
        if not dataSize:
            res.resultMsg = "Failed to read data metadata"
            return res
        dataSize = int.from_bytes(dataSize, byteorder='big', signed=False)
        
        # Step #4: Extract data section.
        res.data = f.read(dataSize * 2)
        if not res.data:
            res.resultMsg = "Failed to read data section"

        # Step #5: Check lengths make sense.
        if len(res.text) != textSize * 2:
            res.resultMsg = f"Text metadata is incorrect ({textSize * 2} vs {len(res.text)})"
            return res
        if len(res.data) != dataSize * 2:
            res.resultMsg = f"Data metadata is incorrect ({dataSize * 2} vs {len(res.data)})"
            return res
        if ((textSize + dataSize) * 2) + 4 != fsize:
            res.resultMsg = f"Image size is inconsistent {((textSize + dataSize) * 2) + 4} vs {fsize}"
            return res
    
    # All good!
    res.resultMsg = "Success"
    res.readFile = True
    return res