import argparse
from binascii import hexlify
import os

def ReadInputFile(fileName, outputFileName):
    hexFile = open(outputFileName, "a")
    newLineCharacterCtr = 0

    with open(fileName,'r') as file:
        for eachLine in file.readlines():
            if eachLine.startswith(';') or len(eachLine) < 2:
                pass
                #print("This can be ignored: {}".format(eachLine))
            else:
                #firstSetOfBytesInLine = eachLine[8:(15+(8*2))]
                setOfBytesInLine = eachLine[8:-2]
                setOfBytesInLine = setOfBytesInLine.replace(" ","")
                #print("First set: {}".format(setOfBytesInLine))
                #print("Length of line: {}".format(len(setOfBytesInLine)))


                for i in range(0,len(setOfBytesInLine),2):
                    byteToWrite = setOfBytesInLine[i].upper() + setOfBytesInLine[i + 1].upper()
                    hexFile.write("0x{}, ".format(byteToWrite))
                    newLineCharacterCtr += 1
                if (newLineCharacterCtr == 128):
                    newLineCharacterCtr = 0
                    hexFile.write("\n")

    hexFile.close()
    print("Finished writing file: {}".format(outputFileName))

def main():
    global print_debug

    parser = argparse.ArgumentParser(description='Creating a C array for BM7x Memory configuration file')
    parser.add_argument('-f1', '--file1', type=str, default='BM70_def.txt', help="Memory config Txt created by UI tool")
    parser.add_argument('-o', '--output', type=str, default='BM7x_array.txt', help="Output array Txt filename")
    parser.add_argument('-d', '--debug', type=int, default=1,            help="debugging print level")

    args = parser.parse_args()
    input_name = args.file1
    print_debug = args.debug
    print("Input file to merge: '{}', debug level ={}".format(input_name, print_debug))

    outputFileNameToCreate = args.output
    checkFileNameExists = os.getcwd() + '/' + outputFileNameToCreate
    if os.path.exists(checkFileNameExists):
        print("###########################################################")
        print("File with same name already exists. Deleting the old file!!")
        print("###########################################################")
        os.remove(checkFileNameExists)
    else:
        print("###########################################################")
        print("Will create new file: {}".format(outputFileNameToCreate))
        print("###########################################################")

    ReadInputFile(input_name, outputFileNameToCreate)

if __name__ == '__main__':
    main()