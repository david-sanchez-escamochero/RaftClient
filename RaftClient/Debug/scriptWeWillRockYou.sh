#!/bin/bash
exec play WeWillRockYou.mp3 &> >(tee -a log.txt)
echo "hola"
