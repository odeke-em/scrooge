#!/usr/bin/python3

def main():
  values = [120, 23, -12, 10, 1450, 210, 1]
  testV = values + [i for i in range(10, 1000000)] + values
  testV.sort()
  # print(testV)

if __name__ == '__main__':
  main()
