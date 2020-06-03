# Editor for bitmap images in C

Editing-Manipulating bitmap images in C

## Makefile

A simple makefile is provided <br>
Use the command: make -f makefile.make

## Help - Available Commands

./blind [image source name] [image output name] -> to copy the image <br>
./blind -attrs [image name] -> to see image's info and attributes about width,height,size etc. <br>
./blind -scale <up/down> [scalefactor] [image source name] [image output name] -> to scale up/down by a factor <br>
./blind -flip <v/h> [image source name] [image output name] -> to mirror it vertically or horizontally <br>
./blind -contrast <number> [image source name] [image output name] -> to change image contrast <br>
./blind -contrast <auto> [image source name] [image output name] -> to automatically change image contrast <br>
./blind -flipm2 <v/h> [image source name] [image output name] -> to flip it vertically or horizontally <br>
./blind -need help -> to see this info <br>