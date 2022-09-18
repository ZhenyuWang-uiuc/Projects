#! /usr/bin/python

"""
Author: Zhenyu Wang
Date: May 12 / 2021
"""

def main():
    file1 = open('routes.dat', 'r')
    file2 = open('output.dat', 'w')
    for i in file1.readlines():
        i = i.strip('\n').split(',')[2:]
        file2.write(','.join(i) + '\n')
    file1.close()
    file2.close()


if __name__ == "__main__":
    main()
