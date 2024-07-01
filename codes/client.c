#include "pse.h"

#define CMD   "client"

void affichageToutClient(int sock);
void affichageCreationAccord(int sock);


int main(int argc, char *argv[]) 
{
  	int sock, ret;
  	struct sockaddr_in *adrServ;
  	int boucleConnectUser=1;
  	int boucleRecherche;
  	int boucleSuppresion;
  	int boucleNewUser;
  	int existenceProfil;
  	int connected;
  	int modifProfil;
  	char ligne[LIGNE_MAX];
  	char nomProfil[LIGNE_MAX];
  	int lg_read,lg_written;

  	if (argc != 3)
    	erreur("usage: %s machine port\n", argv[0]);
	
	printf("########################################################\n");
  	printf("%s: creating a socket\n", CMD);
  	sock = socket (AF_INET, SOCK_STREAM, 0);
  	if (sock < 0)
    		erreur_IO("socket");

	//Obtention de l'adresse du serveur
  	printf("%s: DNS resolving for %s, port %s\n", CMD, argv[1], argv[2]);
  	adrServ = resolv(argv[1], argv[2]);
	if (adrServ == NULL)
		erreur("adresse %s port %s inconnus\n", argv[1], argv[2]);

	//Connexion au serveur
  	printf("%s: adr %s, port %hu\n", CMD,
	stringIP(ntohl(adrServ->sin_addr.s_addr)),
	ntohs(adrServ->sin_port));

  	printf("%s: connecting the socket\n", CMD);
  	printf("########################################################\n");
  	ret = connect(sock, (struct sockaddr *)adrServ, sizeof(struct sockaddr_in));
  	if (ret < 0)
    		erreur_IO("connect");

	//Message de bienvenue
	printf("\n\n");
  	printf("~ Bienvenue dans la bibliothèque d'accords participatifs du campus Provence de l'école des Mines de Saint-Etienne ~\n\n\n");
  	//Connexion au serveur
  	////Identifiant utilisateur
  	while(boucleConnectUser)
  	{
  		printf("---------------------\n");
	  	printf("Veuillez-vous connecter :\n");
  		printf("ID Utilisateur : ");
  		if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
			erreur("saisie ID Utilisateur\n");
			
		lg_written = ecrireLigne(sock, ligne); // Envoi de l'ID Utilisateur au serveur pour vérification de son existence
		if (lg_written == -1)
			erreur_IO("ecriture serveur");
  	
  		lg_read = lireLigne(sock, ligne);
		if (lg_read < 0)
		    	erreur_IO("lecture serveur");
		
		printf("---------------------\n");
		printf("%s \n",ligne);
		if (strcmp(ligne,"ID Utilisateur correct")==0)
		{
			boucleConnectUser = 0;
		}
	}
	////Mot de passe utilisateur
	printf("---------------------\n");
	boucleConnectUser = 1;
	while(boucleConnectUser)
	{
		printf("Mot de passe : ");
  		if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
			erreur("saisie mot de passe\n");
			
		lg_written = ecrireLigne(sock, ligne); // Envoi du mot de passe au serveur pour vérification
		if (lg_written == -1)
			erreur_IO("ecriture serveur");
  	
  		lg_read = lireLigne(sock, ligne);
		if (lg_read < 0)
		    	erreur_IO("lecture serveur");
		
		printf("%s \n",ligne);
		if (strcmp(ligne,"Mot de passe correct")==0)
		{
			boucleConnectUser = 0;
			connected = 1;
		}
		else if (strcmp(ligne, "Nombre de tentatives maximal atteint")==0)
		{
			boucleConnectUser = 0;
			connected = 0;
		}
	}
	//Arborescences des menus
	////Menu de sélections, création et suppresion des profils
	while(connected==1)
	{
		printf("---------------------\n");
		lireLigne(sock, ligne);
		if (strcmp(ligne,"finProfils")!=0)
		{
			printf("Choisisez un des profils suivant : \n");
			while(strcmp(ligne,"finProfils")!=0)
			{
				printf(" * %s \n",ligne);
				lireLigne(sock, ligne);
			}
			printf("Ou tapez 'c' pour ouvrir le menu de création d'un profil.\n");
			printf("Vous pouvez également taper 's' pour supprimer un profil.\n");
			printf("Il est également possible de taper 'u' pour ajouter un nouvel utilisateur.\n");
			printf("Enfin, tapez 'r' pour vous deconnecter de la bibliothèque d'accords.\n");
		}
		else
		{
			printf("Attention, il n'existe pas de profil pour cet utilisateur.\n");
			printf("Tapez 'c' pour ouvrir le menu de création d'un profil.\n");
			printf("Il est également possible de taper 'u' pour ajouter un nouvel utilisateur.\n");
			printf("Ou tapez 'r' pour vous deconnecter de la bibliothèque d'accords.\n");
		}
		printf(">> ");
		if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
			erreur("saisie menu selection profil\n");
			
		lg_written = ecrireLigne(sock, ligne); //Envoi du choix de l'utilisateur
		if (lg_written == -1)
			erreur_IO("ecriture serveur");
		
		//////Menu de création d'un profil
		if (strcmp(ligne,"c\n")==0)
		{
			existenceProfil = 1;
			while(existenceProfil)
			{
				printf("---------------------\n");
				printf("Entrée dans le menu de création d'un profil\n");
				printf("Entrez le nom à donner au profil : ");
				if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
					erreur("saisie nom profil\n");
				
				lg_written = ecrireLigne(sock, ligne); //Envoi du nom du profil
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
			
				lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
			    		erreur_IO("lecture serveur");
			    	
			    	if (strcmp(ligne,"choixIncorrect")==0)
			    	{
			    		existenceProfil=0;
			    	}
			    	else
			    	{
			    		printf("---------\n");
			    		printf("Ce nom de profil est déjà utilisé, veuillez en choisir un autre.\n");
			    	}
			}
		    	
			modifProfil = 1;
			while(modifProfil)
			{
				printf("---------\n");
				printf("Selectionnez un parametre à modifier :\n");
		    		lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
		    			erreur_IO("lecture serveur");
		    		printf("%s \n", ligne);
		    		lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
		    			erreur_IO("lecture serveur");
		    		printf("%s \n", ligne);
		    		lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
		    			erreur_IO("lecture serveur");
		    		printf("%s \n", ligne);
		    		printf("Ou, tapez 'r' pour revenir au menu de sélection des profils\n");
		    		printf(">> ");
		    		
		    		if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
					erreur("saisie menu creation profil\n");
					
				printf("---------\n");
			
				lg_written = ecrireLigne(sock, ligne); //Envoi du choix de l'utilisateur
				if (lg_written == -1)
					erreur_IO("ecriture serveur");

				else if (strcmp(ligne,"1\n")==0)
				{
					printf("Entrez le nombre de cordes de l'instrument : ");
					if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
						erreur("saisie menu creation profil\n");
			
					lg_written = ecrireLigne(sock, ligne); //Envoi du nom de l'instrument au serveur					
				}
				else if (strcmp(ligne,"2\n")==0)
				{
					printf("Entrez le nombre de fretes de l'instrument : ");
					if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
						erreur("saisie menu creation profil\n");
			
					lg_written = ecrireLigne(sock, ligne); //Envoi du nom de l'instrument au serveur					
				}	
				else if (strcmp(ligne,"3\n")==0)
				{
					printf("Entrez le tuning de l'instrument : ");
					if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
						erreur("saisie menu creation profil\n");
			
					lg_written = ecrireLigne(sock, ligne); //Envoi du nom de l'instrument au serveur					
				}			
				else if (strcmp(ligne, "r\n")==0)
				{
					modifProfil = 0;
				}	
			}
			printf("Profil correctement créer\n");			
		}
		//////Deconnection de l'utilisateur
		else if (strcmp(ligne,"r\n")==0)
		{
			connected = 0;
		}
		else if (strcmp(ligne,"s\n")==0)
		{
			boucleSuppresion = 1;
			while(boucleSuppresion)
			{
				printf("---------------------\n");
				printf("Veuillez entrer le nom du profil à supprimer : ");
				if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
					erreur("saisie menu suppresion profil\n");
				
				lg_written = ecrireLigne(sock, ligne); //Envoi du choix de l'utilisateur
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
				
				lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
			    		erreur_IO("lecture serveur");
			    	
			    	if (strcmp(ligne,"choixCorrect")==0)
			    	{
			    		printf("Le profil a été correctement supprimé.\n");
			    		boucleSuppresion = 0;
			    	}
			    	else
			    	{
			    		printf("Ce profil n'existe pas\n");
			    	}
		    	}			
		}
		else if (strcmp(ligne,"u\n")==0)
		{
			boucleNewUser = 1;
			while(boucleNewUser)
			{
				printf("---------------------\n");
				printf("Veuillez entrer l'identifiant du nouvel utilisateur : ");
				if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
					erreur("saisie ID nouvel utilisateur\n");
				
				lg_written = ecrireLigne(sock, ligne); //Envoi du nouvel ID utilisateur
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
					
				lg_read = lireLigne(sock, ligne);
				if (lg_read < 0)
			    		erreur_IO("lecture serveur");
			    	
			    	if (strcmp(ligne, "IdUser libre")==0)
			    	{
			    		boucleNewUser = 0;
			    	}
			    	else
			    	{
			    		printf("Ce nom d'utilisateur est déjà utilisé.\n");
			    	}
			}
			printf("Veuillez entrer le mot de passe du nouvel utilisateur : ");
			if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
				erreur("saisie MDP nouvel utilisateur\n");
			
			lg_written = ecrireLigne(sock, ligne); //Envoi du mot de passe du nouvel utilisateur
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
		}
		else
		{
			lg_read = lireLigne(sock, ligne);
			if (lg_read < 0)
		    		erreur_IO("lecture serveur");
		    		
		    	if (strcmp(ligne,"choixCorrect")==0)
		    	{
		    		strcpy(nomProfil,ligne);
		    		boucleRecherche = 1;
		    		while(boucleRecherche)
		    		{
		    			printf("---------------------\n");
		    			printf("Tapez l'accord que vous souhaitez rechercher.\n");
		    			printf("Ou tapez 'r' pour revenir au menu de choix des profils.\n");
		    			printf(">> ");
		    			if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
						erreur("saisie recherche accord\n");
		    			
		    			lg_written = ecrireLigne(sock, ligne); //Envoi de l'accord choisi
					if (lg_written == -1)
						erreur_IO("ecriture serveur");
		    			
		    			if (strcmp(ligne, "r\n")==0)
		    			{
		    				boucleRecherche = 0;
		    			}
		    			else
		    			{
		    				printf("---------------------\n");
						printf("Quelle action souhaitez vous effectuer?\n");
						printf("0 - Créer une nouvelle variante\n");
						printf("1 - Consulter les variantes existantes\n");
						printf("2 - Retour à la recherche\n");
						printf(">> ");
						if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
							erreur("saisie menu variante\n");
		    			
		    				lg_written = ecrireLigne(sock, ligne); //Envoi de l'accord choisi
						if (lg_written == -1)
							erreur_IO("ecriture serveur");
						
						if (strcmp(ligne, "0\n")==0)
						{
							printf("---------\n");
							printf("En quelle frete l'accord sera-t-il joué ?\n");
							printf(">> ");
							if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
								erreur("saisie premiere frete\n");
		    			
		    					lg_written = ecrireLigne(sock, ligne); //Envoi de la premiere frete
							if (lg_written == -1)
								erreur_IO("ecriture serveur");
							
							printf("---------\n");
							printf("Combien de fretes sont nécessaires pour jouer l'accord ?\n");
							printf(">> ");
							if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
								erreur("saisie nb frete\n");
		    			
		    					lg_written = ecrireLigne(sock, ligne); //Envoi du nombre de fretes choisi
							if (lg_written == -1)
								erreur_IO("ecriture serveur");
							
							//Verification existence fichier
							lg_read = lireLigne(sock, ligne);
							if (lg_read < 0)
		    						erreur_IO("lecture serveur");
		    						
		    					if (strcmp(ligne, "fichierInexistant")==0)
		    					{
								//affichage du menu de creation d'un accord
								affichageCreationAccord(sock);
								
								//affichage des caractéristiques d'un accord
								affichageToutClient(sock);
								
								//Suite
								printf("---------\n");
								printf("\nSouhaitez-vous enregistrer cet accord?\n");
								printf("0 : Non\t\t");
								printf("1 : Oui\n");
								printf(">> ");
								
								if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
									erreur("saisie enregistrement variante accord\n");
			    			
			    					lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
								if (lg_written == -1)
									erreur_IO("ecriture serveur");
							}
							else
							{
								printf("Cet accord existe déjà, le voici:\n");
								//affichage des caractéristiques d'un accord
								affichageToutClient(sock);
								
		    						//Modification accord
		    						printf("---------\n");
		    						printf("Souhaitez-vous modifier cet accord?\n");
		    						printf("0 : Non\t\t");
								printf("1 : Oui\n");
		    						printf(">> ");
		    						
		    						if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
									erreur("saisie modification accord existant\n");
			    			
			    					lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
								if (lg_written == -1)
									erreur_IO("ecriture serveur");
								
								if (strcmp(ligne, "1\n")==0)
								{
									//affichage du menu de creation d'un accord
									affichageCreationAccord(sock);
									
									//affichage des caractéristiques d'un accord
									affichageToutClient(sock);
									
									printf("---------\n");
									printf("\nSouhaitez-vous enregistrer cet accord?\n");
									printf("0 : Non\t\t");
									printf("1 : Oui\n");
									printf(">> ");
								
									if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
										erreur("saisie enregistrement variante accord\n");
			    			
			    						lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
									if (lg_written == -1)
										erreur_IO("ecriture serveur");
								}
							}
						}
						else if (strcmp(ligne, "1\n")==0)
						{
							lg_read = lireLigne(sock, ligne);
							if (lg_read < 0)
		    						erreur_IO("lecture serveur");
		    						
		    					if (strcmp(ligne, "accordExistant")==0)
		    					{	
		    						printf("Choisisez une des variations suivante :\n");
		    						lg_read = lireLigne(sock, ligne);
								if (lg_read < 0)
		    							erreur_IO("lecture serveur");
		    							
		    						while(strcmp(ligne, "FinVariations")!=0)
		    						{
		    							printf("%s\n",ligne);
		    							
		    							lg_read = lireLigne(sock, ligne);
									if (lg_read < 0)
		    								erreur_IO("lecture serveur");
		    							printf("%s\n",ligne);
		    							
		    							lg_read = lireLigne(sock, ligne);
									if (lg_read < 0)
		    								erreur_IO("lecture serveur");
		    							printf("%s\n",ligne);
		    							
		    							lg_read = lireLigne(sock, ligne);
									if (lg_read < 0)
		    								erreur_IO("lecture serveur");
		    							printf("\n");
		    						}
		    						printf(">> ");
		    						if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
									erreur("saisie enregistrement variante accord\n");
		    			
		    						lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
								if (lg_written == -1)
									erreur_IO("ecriture serveur");
								
								affichageToutClient(sock);
								//Modification accord
		    						printf("---------\n");
		    						printf("Souhaitez-vous modifier cet accord?\n");
		    						printf("0 : Non\t\t");
								printf("1 : Oui\n");
		    						printf(">> ");
		    						
		    						if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
									erreur("saisie modification accord existant\n");
			    			
			    					lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
								if (lg_written == -1)
									erreur_IO("ecriture serveur");
								
								if (strcmp(ligne, "1\n")==0)
								{
									//affichage du menu de creation d'un accord
									affichageCreationAccord(sock);
									
									//affichage des caractéristiques d'un accord
									affichageToutClient(sock);
									
									printf("---------\n");
									printf("\nSouhaitez-vous enregistrer cet accord?\n");
									printf("0 : Non\t\t");
									printf("1 : Oui\n");
									printf(">> ");
								
									if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
										erreur("saisie enregistrement variante accord\n");
			    			
			    						lg_written = ecrireLigne(sock, ligne); //Envoi de la decision accord
									if (lg_written == -1)
										erreur_IO("ecriture serveur");
								}
		    					}
		    					else
		    					{
		    						printf("Il n'existe aucune variation de cet accord.\n");
		    					}
						}
						else if(strcmp(ligne, "2\n")==0)
						{
						
						}
						else
						{
							printf("Erreur, veuillez recommencer.\n");
						}
		    			}
		    		}
		    	}
		    	else
		    	{
		    		printf("---------------------\n");
		    		printf("Le profil choisi n'existe pas.\n");
		    	}
		}
	}
	
	//Fermeture du client
	printf("---------------------\n");
	printf("Au revoir\n");
	
  	if (close(sock) == -1)
    		erreur_IO("close socket");

  	exit(EXIT_SUCCESS);
}

void affichageToutClient(int sock)
{
	char ligne[LIGNE_MAX];
	int lg_read;
	//Affichage instrument
	printf("---------\n");
	printf("L'instrument possède :\n");
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("     - %s cordes;\n", ligne);
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("     - %s frettes.\n",  ligne);
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("Il est accordé en %s.\n", ligne);
	
	//affichage accord
	printf("---------\n");
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("L'accord joué est un %s.\n", ligne);
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("Il est joué en frette %s\n", ligne);
	
	//affichage representation finale
	printf("---------\n");
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	printf("%s\n",ligne);
	
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
	while(strcmp(ligne,"FinRepresentation")!=0)
	{
		if (strcmp(ligne,"SautLigne")==0)
		{
			printf("\n");
		}
		else
		{
			printf("%s",ligne);
		}
		lg_read = lireLigne(sock, ligne);
		if (lg_read < 0)
			erreur_IO("lecture serveur");
	}
}

void affichageCreationAccord(int sock)
{
	char ligne[LIGNE_MAX];
	int lg_read, lg_written;
	int boucleNote;
	lg_read = lireLigne(sock, ligne);
	if (lg_read < 0)
		erreur_IO("lecture serveur");
		
	printf("---------\n");
	
	while(strcmp(ligne,"FinModif")!=0)
	{
		printf("%s\n",ligne);
		lg_read = lireLigne(sock, ligne);
		if (lg_read < 0)
			erreur_IO("lecture serveur");
		while(strcmp(ligne,"FinRepresentation")!=0)
		{
			if (strcmp(ligne,"SautLigne")==0)
			{
				printf("\n");
			}
			else
			{
				printf("%s",ligne);
			}
			lg_read = lireLigne(sock, ligne);
			if (lg_read < 0)
				erreur_IO("lecture serveur");
		}
		boucleNote = 1;
		while(boucleNote)
		{
			printf("Souhaitez-vous que cette note (représentée par le caractère ?) soit jouée? (0 = non, 1 = oui)\n");
			printf(">> ");
			if (fgets(ligne, LIGNE_MAX, stdin) == NULL)
				erreur("saisie nb frete\n");
				
			if (strcmp(ligne,"0\n")==0 || strcmp(ligne,"1\n")==0)
			{
				boucleNote = 0;
				lg_written = ecrireLigne(sock, ligne); //Envoi du nombre de fretes choisi
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
			}
			else
			{
				printf("Erreur dans la selection, veuillez répondre par 0 ou 1.\n");
			}
		}
		lg_read = lireLigne(sock, ligne);
		if (lg_read < 0)
			erreur_IO("lecture serveur");
	}
}

