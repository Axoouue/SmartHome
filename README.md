# SmartHome
Assistant textuel pour la maison.

## Installation

### Pré-requis
- télécharger le compilateur nécessaire pour recompiler en croisé pour la Raspberry Pi (github.com/raspberrypi/tools);
- recompiler les librairies ncurses et wiringpi avec ce compilateur.

### Makefile
- déclarer les variables TARGET_NCURSES et TARGET_WIRINGPI qui contiennent le chemin vers les librairies recompilées ;
- déclarer la variable CC_CROISE qui contient le chemin vers le compilateur utilisé ;
- exécuter la commande make, l'exécutable pour la rpi est généré.
