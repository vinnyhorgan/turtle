#!/bin/bash

swc *.ts -o main.js --quiet
../build/turtle .
rm main.js.map
rm main.js