#!/bin/bash

(cd tokenizer && node node_modules/.bin/gulp --silent)

# Bash only version:
#
# for file in $(ls corpus/*.z)
# do
#   gzcat corpus/la011490.z \
#   | tr '[:upper:]' '[:lower:]' \
#   | sed -E "s/(<\!--.*-->)|(<[[:space:]]*\/?[a-z-]+([[:space:]]+[a-z-]+[[:space:]]*=[[:space:]]*\"(\\\"|[^\"])*\")*>)//g" \
#   | sed -E "s/[_'-]?[^a-z_'-]+[_'-]*[^a-z_'-]?/ /g" | tr '\n' ' ' \
#   | sed -E 's/[[:space:]]{2,}/ /g' \
#   | sed -E 's/(^[[:space:]])|([[:space:]]$)//g'
# done
