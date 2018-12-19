#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <stdbool.h>
#include  <unistd.h>

#define SERVADDR "localhost"        // Définition de l'adresse IP d'écoute
#define SERVPORT "12345"            // Définition du port d'écoute, 
                                    // si 0 port choisi dynamiquement
#define LISTENLEN 1                 // Taille du tampon de demande de connexion
#define MAXBUFFERLEN 1024
#define MAXHOSTLEN 64
#define MAXPORTLEN 6

void socketClient();
void socketServeur();


//Variables globales 
int descSockCOMServer;                 // Descripteur de socket de communication Cliente
int descSockCOMClient;                 // Descripteur de la socket de communication Serveur
char buffer[MAXBUFFERLEN];       // Tampon de communication entre le client et le serveur

int main(int argc, char* argv[]){
	socketServeur();
	int ecode;
	
	//Demander le login au client ftp à l'aide de 220
	strcpy(buffer, "220\n");
	write(descSockCOMServer, buffer, strlen(buffer));  //buffer = 220
	
	ecode = read(descSockCOMServer, buffer, MAXBUFFERLEN); //buffer = USER xxxxx
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0'; 
	write(descSockCOMClient, buffer, strlen(buffer));//buffer = USER xxxx
	
	
	ecode = read(descSockCOMClient, buffer, MAXBUFFERLEN); //buffer = 331
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0'; 
	
	write(descSockCOMServer, buffer, strlen(buffer)); //buffer = 331 
	
	ecode = read(descSockCOMServer, buffer, MAXBUFFERLEN); //buffer = PASS xxxxx
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0';
	
	write(descSockCOMClient, buffer, strlen(buffer)); //buffer = PASS xxxx
	
	
	ecode = read(descSockCOMClient, buffer, MAXBUFFERLEN); //buffer = 230 User logged in.
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0'; 
	
	write(descSockCOMServer, buffer, strlen(buffer)); //buffer = 230
	
	ecode = read(descSockCOMServer, buffer, MAXBUFFERLEN); //buffer = SYST
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0';  
	write(descSockCOMClient, buffer, strlen(buffer)); //buffer = SYST
	
	ecode = read(descSockCOMClient, buffer, MAXBUFFERLEN); //buffer = 215
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0'; 
	
	write(descSockCOMServer, buffer, strlen(buffer)); //buffer = 215
	
	ecode = read(descSockCOMServer, buffer, MAXBUFFERLEN); //buffer = UNIX TYPE
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0';  
	write(descSockCOMClient, buffer, strlen(buffer)); //buffer = UNIX TYPE
	
	
	
	while (1) {
	
		strcpy(buffer, "227\n");
		write(descSockCOMClient, buffer, strlen(buffer));  //Mode passive vers le serveur ????
	}
}

void socketServeur(){
	int ecode;                       // Code retour des fonctions
	char serverAddr[MAXHOSTLEN];     // Adresse du serveur
	char serverPort[MAXPORTLEN];     // Port du server
	int descSockRDV;                 // Descripteur de socket de rendez-vous

	struct addrinfo hints;           // Contrôle la fonction getaddrinfo
	struct addrinfo *res;            // Contient le résultat de la fonction getaddrinfo
	struct sockaddr_storage myinfo;  // Informations sur la connexion de RDV
	struct sockaddr_storage from;    // Informations sur le client connecté
	socklen_t len;                   // Variable utilisée pour stocker les 
				                        // longueurs des structures de socket
	
    
	// Publication de la socket au niveau du système
	// Assignation d'une adresse IP et un numéro de port
	// Mise à zéro de hints
	memset(&hints, 0, sizeof(hints));
	// Initailisation de hints
	hints.ai_flags = AI_PASSIVE;      // mode serveur, nous allons utiliser la fonction bind
	hints.ai_socktype = SOCK_STREAM;  // TCP
	hints.ai_family = AF_UNSPEC;      // les adresses IPv4 et IPv6 seront présentées par 
				                         // la fonction getaddrinfo

	// Récupération des informations du serveur
	ecode = getaddrinfo(SERVADDR, SERVPORT, &hints, &res);
	if (ecode) {
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
		exit(1);
	}

	//Création de la socket IPv4/TCP
	descSockRDV = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (descSockRDV == -1) {
		perror("Erreur creation socket");
		exit(4);
	}

	// Publication de la socket
	ecode = bind(descSockRDV, res->ai_addr, res->ai_addrlen);
	if (ecode == -1) {
		perror("Erreur liaison de la socket de RDV");
		exit(3);
	}
	// Nous n'avons plus besoin de cette liste chainée addrinfo
	freeaddrinfo(res);
	// Récuppération du nom de la machine et du numéro de port pour affichage à l'écran

	len=sizeof(struct sockaddr_storage);
	ecode=getsockname(descSockRDV, (struct sockaddr *) &myinfo, &len);
	if (ecode == -1)
	{
		perror("SERVEUR: getsockname");
		exit(4);
	}
	ecode = getnameinfo((struct sockaddr*)&myinfo, sizeof(myinfo), serverAddr,MAXHOSTLEN, 
                         serverPort, MAXPORTLEN, NI_NUMERICHOST | NI_NUMERICSERV);
	if (ecode != 0) {
		fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(ecode));
		exit(4);
	}
	printf("L'adresse d'ecoute est: %s\n", serverAddr);
	printf("Le port d'ecoute est: %s\n", serverPort);
	// Definition de la taille du tampon contenant les demandes de connexion
	ecode = listen(descSockRDV, LISTENLEN);
	if (ecode == -1) {
		perror("Erreur initialisation buffer d'écoute");
		exit(5);
	}

	len = sizeof(struct sockaddr_storage);
	// Attente connexion du client
	// Lorsque demande de connexion, creation d'une socket de communication avec le client
	descSockCOMServer = accept(descSockRDV, (struct sockaddr *) &from, &len);
	if (descSockCOMServer == -1){
		perror("Erreur accept\n");
		exit(6);
	}

	
	//Lancement du client proxy
	socketClient();
	printf("Serveur lancé \"%s\".\n",buffer);
	
	//reception et transmission de la réponse au proxy-serveur, qui lui la transmet au client (ftp)
	//transmission du password à r-info-onyx
	//reception et transmission de la réponse au proxy-serveur, qui lui la transmet au client (ftp)


	//boucle : on attend une commande du client (read)
	//dés la réception, transmission de la commande au proxy-client qui la transmettra à son tour au serveur r-info-onyx
	//on récupére la réponse du serveur r-info-unix et la transmet au proxy-serveur qui lui la transmettra à son tour au client ftp
	//On vide le buffer et on refait la boucle.

	
	//Fermeture de la connexion
	//close(descSockCOMServer);
	//close(descSockRDV);
}

void socketClient() {
	int ecode;                     // Retour des fonctions
	struct addrinfo *res,*resPtr;  // Résultat de la fonction getaddrinfo
	struct addrinfo hints;
	char serverName[MAXHOSTLEN] = "r-info-onyx";   // Nom de la machine serveur
	char serverPort[MAXPORTLEN] = "21";   // Numéro de port
	char buffer[MAXBUFFERLEN];     // Buffer stockant les messages entre le client et le serveur
	bool isConnected = false;      // booléen indiquant que l'on est bien connecté

   	/*//On teste les valeurs rentrées par l'utilisateur
    	if (argc != 3){ perror("Mauvaise utilisation de la commande: <nom serveur> <numero de port>\n"); exit(1);}
    	if (strlen(argv[1]) >= MAXHOSTLEN){ perror("Le nom de la machine serveur est trop long\n"); exit(2);}
   	if (strlen(argv[2]) >= MAXPORTLEN){ perror("Le numero de port du serveur est trop long\n"); exit(2);}
    	strncpy(serverName, argv[1], MAXHOSTLEN);
    	serverName[MAXHOSTLEN-1] = '\0';
    	strncpy(serverPort, argv[2], MAXPORTLEN);
   	serverPort[MAXPORTLEN-1] = '\0';*/

    
    	// Initailisation de hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;  // TCP
	hints.ai_family = AF_UNSPEC;      // les adresses IPv4 et IPv6 seront présentées par 
				                          // la fonction getaddrinfo

	//Récupération des informations sur le serveur
	ecode = getaddrinfo(serverName,serverPort,&hints,&res);
	if (ecode){
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
		exit(1);
	}

	resPtr = res;

	while(!isConnected && resPtr!=NULL){

		//Création de la socket IPv4/TCP
		descSockCOMClient = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);
		if (descSockCOMClient == -1) {
			perror("Erreur creation socket");
			exit(2);
		}
  
  		//Connexion au serveur
		ecode = connect(descSockCOMClient, resPtr->ai_addr, resPtr->ai_addrlen);
		if (ecode == -1) {
			resPtr = resPtr->ai_next;    		
			close(descSockCOMClient);	
		}
		// On a pu se connecter
		else isConnected = true;
	}
	freeaddrinfo(res);
	if (!isConnected){
		perror("Connexion impossible");
		exit(2);
	}
	
	//Echange de donneés avec le serveur
	ecode = read(descSockCOMClient, buffer, MAXBUFFERLEN);
	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
	buffer[ecode] = '\0';
	
}



