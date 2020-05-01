/** 
 * \file smartHome.c
 * \brief Fichier source de l'assistant Smart Home.
 * \authors Charles Destombes, Julien Bossut, Axelle Saas
 * \version 0.3
 * \date 5 avril 2020
 */

#include <curses.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>

#define MAXTIMINGS	85
#define DHTPIN		7
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4
#define GPIOIN         23
#define GPIOOUT         1 // buzzer

/*-------------------- V A R I A B L E S  G L O B A L E S --------------------*/

// variables globales pour la température et l'humidité
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
int old[4] = { 0, 0, 0, 0 };

// variables globales pour le menu
char *choices[] = {
                        "Afficher la temperature et l'humidite.",
                        "Je suis absent(e).",
                        (char *)NULL,
                  };
int c, n_choices, i;
ITEM **my_items;
ITEM *cur_item;			
MENU *my_menu;

// variables globales pour l'alarme
bool absent = false;
pthread_t th;
void * ret;

/*--------------------------- P R O T O T Y P E S ----------------------------*/
void func(char *name);
void afficher_temperature_humidite();
void * activer_alarme(void * arg);

/*----------------------------- F O N C T I O N S ----------------------------*/
/**
 *\fn int main()
 *Cette fonction s'occupe principalement d'initialiser wiringPi et ncurses. Pour ce
 *qui est de l'initialisation de ncurses, on met en place un menu et on initalise le clavier.
 *Tant que l'utilisateur n'appuie pas sur la touche F1, le programme ne se terminera pas.
 *\brief Fonction principale du programme.
 */
int main() {
	
	/* Initialiser wiringPi */
	if ( wiringPiSetup() == -1 )
		exit(1);

	/* Initialiser curses */	
	initscr();
	start_color(); // initialise les couleurs supportées par le terminal
        cbreak(); // désactive le buffer de ligne et CTRL-Z, CTRL-C sont interprétés
        noecho(); // désactive l'echo (autorise la fonction getch())
	keypad(stdscr, TRUE); // active l'utilisation du clavier
	init_pair(1, COLOR_RED, COLOR_BLACK); // (num de paire, premier plan, arrière plan)
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	/* Initialiser les items */
    n_choices = ARRAY_SIZE(choices);
    my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

    for(i = 0; i < n_choices; ++i){
		my_items[i] = new_item(choices[i], ""); // crée un nouvel item
		set_item_userptr(my_items[i], func);
	}

	my_items[n_choices] = (ITEM *)NULL;

	/* Créer le menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Afficher le menu */
	mvprintw(LINES - 3, 0, "Appuyez sur <ENTER> pour choisir une option.");
	mvprintw(LINES - 2, 0, "Utilisez les touches haut et bas pour navguer. (F1 pour quitter)");
	post_menu(my_menu);
	refresh();
	
	
	while((c = getch()) != KEY_F(1)) { // F1 pour quitter      
		switch(c) {	
			case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: {	// si ENTREE
				ITEM *cur;
				void (*p)(char *);

				cur = current_item(my_menu);
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
				break;
			}
			break;
		}
	}	
		
	pthread_join(th, &ret);	
			
	/* Terminer */
	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
}

/** 
 *\fn void func(char *name) 
 *Trois options du menu sont traitées dans cette fonction. 
 *Si l'utilisateur choisit l'option "Je suis absent(e).", la variable absent devient vraie , on met en place un thread qui va lancer la fonction activer_alarme puis on met à jour le menu avec l'option "Je suis present(e).".
 *Si l'utilisateur choisit l'option "Je suis present(e).", la variable absent devient fausse, la fonction activer_alarme se termine, on attend la fin du thread puis on met à jour le menu avec l'option "Je suis absent(e).". 
 *\brief Effectue une action en fonction de l'option choisie dans le menu.
 *\param name Chaîne de caractère de l'option choisie.
 */

void func(char *name) {	
	move(10, 0);
	clrtoeol();

	/* L'utilisateur choisi l'option "Je suis absent(e)." */
	if(strcmp(name, "Je suis absent(e).") == 0){

		absent = true;

		pthread_create(&th, NULL, activer_alarme, NULL);

		// mettre à jour le menu avec la nouvelle option
		my_items[1] = new_item("Je suis present(e).", "");
		unpost_menu(my_menu);
		free_menu(my_menu);
		set_menu_items(my_menu, my_items);

		mvprintw(LINES - 3, 0, "Appuyez sur <ENTER> pour choisir une option.");
		mvprintw(LINES - 2, 0, "Utilisez les touches haut et bas pour naviguer. (F1 pour quitter)");
		post_menu(my_menu);
		refresh();

		mvprintw(10, 0, "Vous etes absent(e).");
	}
	
	/* L'utilisateur choisi l'option "Je suis present(e)." */
	if(strcmp(name, "Je suis present(e).") == 0){

		absent = false;
		
		pthread_join(th, &ret);		

		// mettre à jour le menu avec la nouvelle option
		my_items[1] = new_item("Je suis absent(e).", "");
		unpost_menu(my_menu);
		free_menu(my_menu);
		set_menu_items(my_menu, my_items);

		mvprintw(LINES - 3, 0, "Appuyez sur <ENTER> pour choisir une option.");
		mvprintw(LINES - 2, 0, "Utilisez les touches haut et bas pour navguer. (F1 pour quitter)");
		post_menu(my_menu);
		refresh();

		mvprintw(10, 0, "Vous etes present(e).");
	}

	/* L'utilisateur choisi l'option "Afficher la temperature et l'humidite." */
	if(strcmp(name, "Afficher la temperature et l'humidite.") == 0){
		afficher_temperature_humidite();
	}



}	

/**
 *\fn void afficher_temperature_humidite()
 *\brief Affiche la température et l'humidité relevées par les capteurs de la RPI.
 *\bug Dans certains cas, le capteur de température renvoie des données erronées/corrompues.
 *		La fonction affiche alors les dernières valeurs correctes enregistrées.
 */
void afficher_temperature_humidite() {
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ ) {
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate ) {
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 ) {
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )	{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 50 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) ) {
		mvprintw(10, 0, "La temperature ambiante est %d.%d C.", dht11_dat[2], dht11_dat[3]);
		mvprintw(11, 0, "L'humidite est de %d.%d %%.", dht11_dat[0], dht11_dat[1]);
		old[0] = dht11_dat[0];
		old[1] = dht11_dat[1];
		old[2] = dht11_dat[2];
		old[3] = dht11_dat[3];
	}
	else {
		mvprintw(10, 0, "La temperature ambiante est %d.%d C.", old[2], old[3]);
		mvprintw(11, 0, "L'humidite est de %d.%d %%.", old[0], old[1]);
	}
}

/**
 *\fn void * activer_alarme(void * arg)
 *Cette fonction est active tant que l'utilisateur est absent. 
 *Pendant cette durée, si le capteur de mouvement détecte un mouvement, le buzzer émettra un son.
 *\brief Active l'alarme.
 *\param arg NULL
 */
void * activer_alarme(void * arg) {
    uint8_t State = 0;
    int i = 0;
    pinMode(GPIOIN, INPUT);
    pinMode(1, OUTPUT);
    system("gpio write 1 off");
    /* Détection d'un mouvement et lecture des données. */
    while (absent == true){
        delay(998);
        State = digitalRead(GPIOIN);
        mvprintw(13, 0, "STATE = %d", State);
        if(State == 1) {
        	system("gpio write 1 on");   
        }
		else {
        	system("gpio write 1 off");
        }
        delay(2);
        
    }
    
    system("gpio write 1 off");

	pthread_exit(0);
}
