#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void traiterChaine(char chaine[]);

void main (int argc, char** argv) {
	/*char chaine[] = "tts2330@r-info-onyx";
	char chainefin[]="";
	for (int i=0;i<=strlen(chaine)-1; i++) {
		char* temp = chaine[i];
		if (temp!="@"){
		 strcat(chainefin, temp);
		}
	}*/
	char chaine[] = "USER tts2330a@r-info-onyx.fr";
	traiterChaine(chaine);
}

void traiterChaine(char chaine[]) {
	char* chaineDecoupee = strtok(chaine, "@"); //la fonction strtok découpe la chaîne selon un délimiteur : @
	while (chaineDecoupee!=NULL) {
		printf("%s\n", chaineDecoupee);
		chaineDecoupee=strtok(NULL, "@");
	}
}
