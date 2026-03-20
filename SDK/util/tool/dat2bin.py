import os
import sys

if len(sys.argv)>=2:
    inputFilePath=sys.argv[1]
    print("inputFilePath:",inputFilePath,"\n")
    name=os.path.splitext(inputFilePath)
    outputFilePath=name[0]+".bin"
    print("outputFilePath:",outputFilePath,"\n")

    # 逐行读取dat文件转化后写入到bin文件
    with open(inputFilePath, 'r') as inputFile, open(outputFilePath, 'wb') as outputFile:
        for lineData in inputFile:
            lineData = lineData.strip() # 去除行末尾的回车符和换行符
            data = bytes.fromhex(lineData)
            outputFile.write(data)
    print("finish\n")
else:
    print("version:1.0.0")
    print("args:[input_dat_file_path]")
    print("example:")
    print("python dat2bin.py flash.dat")
    print("or dat2bin.exe flash.dat")
