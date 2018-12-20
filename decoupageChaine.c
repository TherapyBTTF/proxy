#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* traiterChainePart1(char chaine[]);
char* traiterChainePart2(char chaine[]);

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
	printf("%s\n", traiterChainePart1(chaine));
	printf("%s\n", traiterChainePart2(chaine)); //si exécution en mm temps : 2ème affichage blanc
}

char* traiterChainePart1(char chaine[]) {
	char* chaineDecoupee = strtok(chaine, " @"); //la fonction strtok découpe la chaîne selon un délimiteur : @
	int i=0;
	char* login="";
	while (chaineDecoupee!=NULL) {
		chaineDecoupee=strtok(NULL, " @");
		i++;
		if (i==1) {
			login=chaineDecoupee;
		}
	}
	return login;
}

char* traiterChainePart2(char chaine[]) {
	char* chaineDecoupee = strtok(chaine, " @"); //la fonction strtok découpe la chaîne selon un délimiteur : @
	int i=0;
	char* serveur="";
	while (chaineDecoupee!=NULL) {
		chaineDecoupee=strtok(NULL, " @");
		i++;
		if (i==2) {
			serveur=chaineDecoupee;
		}
	}
	return serveur;
}
