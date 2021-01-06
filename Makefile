LEPTON_INC = -I/opt/lepton/leptonSDKEmb32PUB/
INCLUDES = $(LEPTON_INC)
CFLAGS = $(INCLUDES)
CPPFLAGS = -march=armv7-a -mtune=cortex-a7

LEPTON_LIBS = -L/usr/lib/arm-linux-gnueabihf -L/opt/lepton/leptonSDKEmb32PUB/Debug/ -lLEPTON_SDK
LEPTON_OBJ = /opt/lepton/leptonSDKEmb32PUB/Debug/*.o
LDFLAGS = $(LEPTON_LIBS)

# Enkel echte tabs gebruiken!
# Geen spaces!
# LeptonSPIMeasTest zal de naam zijn van onze output.
# hievoor hebben we nodig Main.o SPI.o en Lepton_I2C.o
# de makefile gaat voor elk van deze naar de respectievelijke regel om deze te compileren
LeptonSPIMeasTest: Main.o SPI.o Lepton_I2C.o
# Welk commando willen we concreet uitvoeren?
	cc -o LeptonSPIMeasTest Main.o SPI.o Lepton_I2C.o $(LDFLAGS)

# regel om Main.c te compileren naar Main.o
Main.o: Main.c
	cc -c Main.c

SPI.o: SPI.c
	cc -c SPI.c

Lepton_I2C.o: Lepton_I2C.c
	cc -c Lepton_I2C.c $(CFLAGS)