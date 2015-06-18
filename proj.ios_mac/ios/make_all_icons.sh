#!/bin/bash

for res in 29 40 50 57 58 72 76 80 100 114 144 152; do
  convert -resize ${res}x${res} Icon-120.png Icon-$res.png
done
