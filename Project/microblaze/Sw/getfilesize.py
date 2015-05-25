import os
import sys

def main():
  """
  Main function
  """

  # Error checking
  if(len(sys.argv) != 2):
    print "Usage:"
    print "\tpython getfilesize.py name-of-file"
    sys.exit(-1);
  # Check if file exists
  elif(not os.path.isfile(sys.argv[1])):
    print "File does not exist or is not valid"
    sys.exit(-3);
  # We only support .txt files
  elif(not sys.argv[1].endswith(".txt")):
    print "We only support .txt files."
    sys.exit(-2);




main()
