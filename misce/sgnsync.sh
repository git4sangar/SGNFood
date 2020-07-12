#!/bin/bash

# Supposed to run on rsync-host01, change rsync-host02 to rsync-host01 to make a script that is meant to run on rsync-host02.

while true; do
  inotifywait -r -e modify,attrib,close_write,move,create,delete ../../SGNFood
  rsync -azP -e "ssh -i /home/tstone10/sgn/bkup/private/projs/SGNFood/misce/rsync-key -o StrictHostKeyChecking=no" ../../SGNFood svaithiy@172.16.176.129:/home/svaithiy/sgn/projs
done
