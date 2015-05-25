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

  # Get file stats
  file_stats = os.stat(sys.argv[1])

  print "Size of file is " + str(file_stats.st_size) + " bytes"

  new_file = sys.argv[1].strip(".txt") + "_size" + ".txt"

  print "New file is " + new_file

  with open(new_file, "w+") as outfile:
    outfile.write(str(file_stats.stats) + "\n")
    with open(sys.argv[1], "r") as infile:
      for line in infile:
        outfile.write(line)


if __name__ == '__main__':
  main()
