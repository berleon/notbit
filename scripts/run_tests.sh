#! /usr/bin/env bash

# propagate errors
set -e

test_file="*"
if [ -n "$1" ]
then
  test_file=$1
fi
echo "$test_file"
make check
echo
echo "> Start running Tests..."
find tests/ -maxdepth 1 -name "$test_file" -executable -type f | while read line;
do
  echo
  echo "> Test >  $line"
  echo
  $(echo $(pwd)/$line)
done
echo
echo "> Finished Tests"
