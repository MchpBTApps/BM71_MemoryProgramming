import argparse
from binascii import hexlify
import os

def ReadHexFile(fileName):
    bankByteArray = bytearray()

    with open(fileName,'r') as file:
        while True:
            data_line = file.readline()
            if (not data_line) or (data_line[:-1] == ':00000001FF'):
                break

            if print_debug > 3:
                print("Data line read: {} and length: {}".format(data_line, len(data_line)))

            if (data_line[0] == ':') and (data_line[len(data_line)-1] == '\n'):
                expectedLengthDataInLine = int(data_line[1:3],16)
                startAddressDataInLine = data_line[3:7]
                numberOfBytesinDataLine = len(data_line)/2
                if (expectedLengthDataInLine != 32) or (expectedLengthDataInLine != (numberOfBytesinDataLine-6)):      #Header: 1+2+4+2 (9); checksum: 2; return: 1; total 12 characters = 6 bytes
                    if data_line[1:9] == '02000004':
                        bankNumber = data_line[6:8]
                        if print_debug > 2:
                            print("Bank number {} identified. ".format(bankNumber))
                        bytesCollected = 0
                    if data_line[1:9] != '02000004':
                        if bytesCollected != int(startAddressDataInLine,16) :
                            linesMissing = int(((int(startAddressDataInLine, 16)) - bytesCollected)/32)
                            if print_debug > 2:
                                print("Number of lines missing: {} ".format(linesMissing))
                            for i in range(0, linesMissing):
                                for j in range(0,32):
                                    bankByteArray.append(int('FF',16))
                                    bytesCollected += 1

                            numberOfBytesMissingInLine = 32 - expectedLengthDataInLine
                            data_line_bytes = data_line[9:((expectedLengthDataInLine*2)+9)]
                            tempByteArray = bytearray()
                            characterPosition = 0
                            while characterPosition < expectedLengthDataInLine*2:
                                bankByteArray.append(int(data_line_bytes[characterPosition:characterPosition+2],16))
                                tempByteArray.append(int(data_line_bytes[characterPosition:characterPosition+2],16))
                                bytesCollected += 1
                                characterPosition += 2
                            while characterPosition < 63:
                                bankByteArray.append(int('FF', 16))
                                tempByteArray.append(int('FF', 16))
                                bytesCollected += 1
                                characterPosition += 2

                        if bytesCollected == int(startAddressDataInLine,16) :
                            numberOfBytesMissingInLine = 32 - expectedLengthDataInLine
                            if print_debug > 2:
                                print("Number of missing bytes: {}".format(numberOfBytesMissingInLine))
                            data_line_bytes = data_line[9:((expectedLengthDataInLine*2)+9)]
                            tempByteArray = bytearray()
                            characterPosition = 0
                            while characterPosition < expectedLengthDataInLine*2:
                                bankByteArray.append(int(data_line_bytes[characterPosition:characterPosition+2],16))
                                tempByteArray.append(int(data_line_bytes[characterPosition:characterPosition+2],16))
                                bytesCollected += 1
                                characterPosition += 2
                            while characterPosition < 63:
                                bankByteArray.append(int('FF', 16))
                                tempByteArray.append(int('FF', 16))
                                bytesCollected += 1
                                characterPosition += 2

                if expectedLengthDataInLine == 32:
                    if bytesCollected != int(startAddressDataInLine, 16):
                        linesMissing = int(((int(startAddressDataInLine, 16)) - bytesCollected)/32)
                        for i in range(0, linesMissing):
                            for j in range(0,32):
                                bankByteArray.append(int('FF',16))
                                bytesCollected += 1
                    if bytesCollected == int(startAddressDataInLine, 16):
                        characterPosition = 9                   # Data starts from bit#9. Note indexing starts at 0.
                        while characterPosition < 73:           # There are 64 characters to collect;
                            bankByteArray.append(int(data_line[characterPosition:characterPosition+2],16))
                            bytesCollected += 1
                            characterPosition += 2

    return(bankByteArray, bytesCollected)

def PackHexArray(data, endingByteNumber):
    bankEndByte = 0xFFFF
    finalByteNumber = endingByteNumber
    #print("Starting padding number: {}, Padding ending number: {}".format(endingByteNumber, bankEndByte))
    for i in range(endingByteNumber,bankEndByte):
        data.append(int('FF',16))
        finalByteNumber += 1
    return data, finalByteNumber

def ConvertByteToStr(data, endByteNumber):
    #bankStrArray = []
    bankStrArray = str(hexlify(data),"utf-8")
    #print("Converted Bytes: {}".format(bankStrArray))
    return bankStrArray


def main():
    global print_debug

    parser = argparse.ArgumentParser(description='Search a file for a repeating sequence')
    parser.add_argument('-f1', '--file1', type=str, default='RN487x.H00', help="First Hex file with .H00 extension")
    parser.add_argument('-f2', '--file2', type=str, default='RN487x.H01', help="Second hex file")
    parser.add_argument('-f3', '--file3', type=str, default='RN487x.H02', help="Third hex file")
    parser.add_argument('-f4', '--file4', type=str, default='RN487x.H03', help="Fourth hex file")
    parser.add_argument('-o', '--output', type=str, default='RN487x_array.txt', help="Output Txt filename")
    parser.add_argument('-d', '--debug', type=int, default=1,            help="debugging print level")

    args = parser.parse_args()
    file_name = args.file1
    file_name2 = args.file2
    file_name3 = args.file3
    file_name4 = args.file4
    print_debug = args.debug
    print("Firmware files to decode: '{}', '{}', '{}', '{}' debug level ={}".format(file_name, file_name2, file_name3, file_name4, print_debug))

    fileArray = [file_name, file_name2, file_name3, file_name4]
    bankNumber = 0
    #fileNameToCreate = "RN487x_Array.txt"
    fileNameToCreate = args.output
    #currentFilePath = os.getcwd()
    checkFileNameExists = os.getcwd() + '/' + fileNameToCreate
    if os.path.exists(checkFileNameExists):
        print("###########################################################")
        print("File with same name already exists. Deleting the old file!!")
        print("###########################################################")
        os.remove(checkFileNameExists)

    for eachFile in fileArray:
        #print("--------- Decoding {}  -----------------".format(eachFile))
        data, numberOfBytes = ReadHexFile(eachFile)
        if print_debug > 2:
            print("Number of bytes read: {} and the number of byte in array: {}".format(numberOfBytes, len(data)))
        bankData, finalByteNo = PackHexArray(data, numberOfBytes)
        bankStrArray = str(hexlify(data), "utf-8")

        hexFile = open(fileNameToCreate, "a")
        newLineCharacterCtr = 0

        for i in range(0,len(bankStrArray)+1,2):
            if not(i == len(bankStrArray)):
                byteToWrite = bankStrArray[i].upper()+bankStrArray[i+1].upper()
                hexFile.write("0x{}, ".format(byteToWrite))
                newLineCharacterCtr += 1
                if (newLineCharacterCtr == 16):
                    newLineCharacterCtr = 0
                    hexFile.write("\n")
            #The last byte in each bank, except bank 3, needs to be appended differently. Hence this code below.
            if (i == len(bankStrArray)) and not(bankNumber == 3):
                byteToWrite = "FF"
                hexFile.write("0x{}, ".format(byteToWrite))
                hexFile.write("\n")

        hexFile.close()
        print("---------Decoded {} ---------------------".format(eachFile))
        bankNumber += 1
    print("#######################################################################################")
    print("######### Decoded and merged hex files to an C array file: {} ######".format(fileNameToCreate))
    print("#######################################################################################")

if __name__ == '__main__':
    main()