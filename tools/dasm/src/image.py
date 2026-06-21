################################################################################
# image.py: handles reading/parsing of binary image (based on uP design).
# by John Eslinger
################################################################################

'''
Interpretation Expectations (based on PR2000):
1) Image is measured in 16-bit, big endian values (ie words)
2) Image is between 4-32768 words long (ie 8-65536 bytes)
3) Image is organized in this order:
    a) text metadata (1 word- # of words in following section)
    b) text section  (1+ words- instructions of image)
    c) data metadata (1 word- # of words in following section)
    d) data section  (1+ words- global data of image)
4) Image contains no additional data besides that stated above

For ease, all checks/etc will be done in bytes in this file.
'''

################################################################################

import os

################################################################################

# Binary image sizing parameters.
__MIN_IMG_SIZE = 8
__MAX_IMG_SIZE = 65536
__IMG_MULTIPLE = 2

################################################################################
# "Extracted Image" object- created by parsing the image.
class BinImage:
    text    = None  # contents/size of text section
    data    = None  # contents/size of data section

################################################################################
# Parses file into given image object. Returns (isValid, errMsg).
def parse_image(fpath: str, img: BinImage):
    # Fail if file doesn't exist.
    if os.path.exists(fpath) is False: 
        return (False, f"File '{fpath}' does not exist")

    # Fail if file is not within expected size/multiple constraints.
    imgSize = os.path.getsize(fpath)
    if imgSize < __MIN_IMG_SIZE: 
        return (False, f"File ({imgSize} bytes) is too small ({__MIN_IMG_SIZE} bytes)")
    if imgSize > __MAX_IMG_SIZE: 
        return (False, f"File ({imgSize} bytes) is too large ({__MAX_IMG_SIZE} bytes)")
    if imgSize % __IMG_MULTIPLE != 0:
        return (False, f"File ({imgSize} bytes) is not a word multiple ({__IMG_MULTIPLE} bytes)")
    
    # Open/Parse file.
    with open(fpath, 'rb') as f:
        # Fail if unable to extract text metadata.
        textSize = f.read(2) # 1 word section
        if not textSize: return (False, "Failed to read text metadata")
        textSize = 2 * int.from_bytes(textSize, byteorder='big', signed=False)

        # Fail if unable to extract text section.
        img.text = f.read(textSize)
        if not img.text: return (False, "Failed to read text section")

        # Fail if unable to extract data metadata.
        dataSize = f.read(2) # 1 word section
        if not dataSize: return (False, "Failed to read data metadata")
        dataSize = 2 * int.from_bytes(dataSize, byteorder='big', signed=False)

        # Fail if unable to extract data section.
        img.data = f.read(dataSize)
        if not img.data: return (False, "Failed to read data section")

        # Fail if file/metadata sizes are semantically wrong.
        totalSize = (2 + textSize + 2 + dataSize)
        if len(img.text) != textSize:
            return (False, f"Text metadata ({textSize} bytes) is incorrect ({len(img.text)} bytes)")
        if len(img.data) != dataSize:
            return (False, f"Data metadata ({dataSize} bytes) is incorrect ({len(img.data)} bytes)")
        if imgSize != totalSize:
            return (False, f"Image size ({imgSize} bytes) is incorrect ({totalSize} bytes)")
    
    # Still here? Image parsed successfully.
    return (True, "Success")
