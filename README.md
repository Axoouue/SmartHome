# SmartHome
Assistant textuel pour la maison.

## Installation
- télécharger le compilateur nécessaire pour recompiler en croisé pour la Raspberry Pi ;
- recompiler les librairies ncurses et wiringpi avec ce compilateur.

### Makefile
- déclarer les variables TARGET_NCURSES et TARGET_WIRINGPI qui contiennent le chemin vers les librairies recompilées ;
- déclarer CC_CROISE qui contient le chemin vers le compilateur utilisé ;
- exécuter la commande make, l'exécutable pour la rpi est généré.
