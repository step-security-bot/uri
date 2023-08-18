#!/usr/bin/env python3
import argparse
import sys
import uritools

def parse (s):
  print('URI:', s)
  asstr = lambda x: '"' + x + '"' if x is not None else None
  
  u = uritools.urisplit(s)
  #print (u)  
  print(' scheme: {0}'.format(asstr(u.scheme)))
  print(' userinfo: {0}'.format(asstr(u.userinfo)))
  print(' host: {0}'.format(asstr(u.host)))
  print(' port: {0}'.format(asstr(u.port)))
  print(' path: {0}'.format(asstr(u.path)))
  print(' query: {0}'.format(asstr(u.query)))
  print(' fragment: {0}'.format(asstr(u.fragment)))

def parse_file (path):
    with open(path) as f:
        lines = f.readlines()
        for s in lines:
            parse(s)
            
def main (args):
  parser = argparse.ArgumentParser (description='URI parser wrapper')
  parser.add_argument('infile', help='The name of a file containing URI text')
  options = parser.parse_args(args)
  parse_file (options.infile)
  
if __name__ == '__main__':
  sys.exit(main (sys.argv[1:]))
    
