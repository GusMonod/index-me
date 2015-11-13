#!/bin/bash

#(cd tokenizer && node node_modules/.bin/gulp --silent)

# Bash only version:
#
for file in $(ls corpus/*.z)
do
  gzcat "$file" \
   | tr '[:upper:]' '[:lower:]' \
   | sed -E "s/<.*>//g" \
   | sed -E "s/[^a-z]/ /g" \
   | tr '\n' ' ' \
   | sed -E 's/[[:space:]]{2,}/ /g' \
   | sed -E 's/(^[[:space:]])|([[:space:]]$)//g'
done
