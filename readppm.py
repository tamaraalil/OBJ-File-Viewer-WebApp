#!/usr/bin/python3
import sys

# P3 READER FUNCTION
def p3file(fileName):
    f = open(fileName, "r")
    c = []
    for line in f:
        if line[0] != '#' and line[0] != 'P': # Skip over comments & "P3" header
            temp = ''
            splitLine = []
            # Iterate through line, append each item after whitespace
            for ch in line:
                if ch == ' ' or ch == '\n' or ch == '\t':
                    if temp != '':
                        splitLine.append(temp)
                    temp = ''
                else:
                    temp += ch
            # Convert string numbers to integers and add them to the list
            for nums in splitLine:
                data = int(nums)
                c.append(data)
    f.close()
    return c


# P6 READER FUNCTION
def p6file(fileName):
    c = []
    bytecount = 0
    with open(fileName, "rb") as f:
        p6 = f.readline() # Read P6 header
        bytecount = bytecount + len(p6)
        while len(c) < 3: # Read height, width, and depth first
            line = f.readline()
            bytecount = bytecount + len(line)
            l = line.decode("ASCII")
            temp = ''
            splitLine = []
            if l[0] != '#': # Ignore comments
                # Iterate through line, append each item after whitespace
                for ch in l:
                    if ch == ' ' or ch == '\n' or ch == '\t':
                        if temp != '':
                            splitLine.append(temp)
                        temp = ''
                    else:
                        temp += ch
                for nums in splitLine:
                    data = int(nums)
                    c.append(data)
        i = bytecount # Byte position where data starts
        for i in f:
            for j in i:
                c.append(j) # Append data points
    f.close()
    return c
