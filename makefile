all: smartHome

#Production des exécutables 

# options pour la précompilation 
CPPFLAGS=-I$(TARGET_NCURSES)/include/ncurses -I$(TARGET_NCURSES)/include -I$(TARGET_WIRINGPI)/include -I$(TARGET_WIRINGPI)/include/wiringPi
# options pour l'édition de liens 
LDFLAGS=-L$(TARGET_NCURSES)/lib -L$(TARGET_WIRINGPI)/lib

smartHome : smartHome.c
	$(CC_CROISE) $(CPPFLAGS) $(LDFLAGS) smartHome.c -o smartHome -lncurses -lmenu -lwiringPi -lpthread

clean: 
	rm -rf *.o smartHome







