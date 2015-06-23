#!/bin/bash

# 100, 114, 120, 144 and 152 were rendered from paths
for res in 29 40 50 57 58 72 76 80; do
  convert -resize ${res}x${res} Icon-152.png Icon-$res.png
done
