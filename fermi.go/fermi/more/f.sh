#!/bin/bash

function f(){
  sed -u "s|#.*||" | 
  sed -u "s|//.*||" | 
  sed -u 's|K|000|g' | 
  sed -u 's|M|000000|g' | 
  sed -u 's|B|000000000|g' | 
  /usr/bin/f
}

# f
