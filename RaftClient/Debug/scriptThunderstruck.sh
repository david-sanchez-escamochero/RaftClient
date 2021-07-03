#!/bin/bash
exec play Thunderstruck.mp3 &> >(tee -a log.txt)
echo "hola"
