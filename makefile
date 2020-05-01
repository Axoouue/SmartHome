all: smartHome

#Production des exécutables EN UTILISANT LA LIBRAIRIE CROISEE 
# et un compilateur CROISE 
#TARGET_NCURSES=/home/axelle/Desktop/Projet/target_ncurses_rpi
#TARGET_WIRINGPI=/home/axelle/Desktop/Projet/target_wiringpi_rpi
# options pour la précompilation 
CPPFLAGS=-I$(TARGET_NCURSES)/include/ncurses -I$(TARGET_NCURSES)/include -I$(TARGET_WIRINGPI)/include -I$(TARGET_WIRINGPI)/include/wiringPi
# options pour l'édition de liens 
LDFLAGS=-L$(TARGET_NCURSES)/lib -L$(TARGET_WIRINGPI)/lib
#chemin du compilateur croisé 
#CC_CROISE=/home/axelle/Desktop/Projet/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc

smartHome : smartHome.c
	$(CC_CROISE) $(CPPFLAGS) $(LDFLAGS) smartHome.c -o smartHome -lncurses -lmenu -lwiringPi -lpthread

clean: 
	rm -rf *.o smartHome







