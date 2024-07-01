#include "pse.h"
#include "fonctionsTraitementUtilisateurs.h"
#include "accords.h"

#define   CMD         "serveur"
#define   NB_WORKERS  5
#define MAX_CONNECTION_USER 3


void *sessionClient(void *arg);

sem_t monsem;

pthread_mutex_t monmutex = PTHREAD_MUTEX_INITIALIZER;

DataSpec dataSpec[NB_WORKERS];

char connectedUser[NB_WORKERS][LIGNE_MAX];

int main(int argc, char *argv[]) {
	short port;
	int semaph, ecoute, canal, ret;
	int N = NB_WORKERS;
	struct sockaddr_in adrEcoute, adrClient;
	unsigned int lgAdrClient;
	DataThread *dataThread;

	semaph = sem_init (&monsem, 0, N); 

	initConnectedUser();

	if (semaph == -1)
		erreur_IO("initialisation semaphore");

	if (argc != 2)
		erreur("usage: %s port\n", argv[0]);

	port = (short)atoi(argv[1]);

	printf("########################################################\n");
	printf("%s: creating a socket\n", CMD);
	
	ecoute = socket (AF_INET, SOCK_STREAM, 0);
	if (ecoute < 0)
		erreur_IO("socket");

	adrEcoute.sin_family = AF_INET;
	adrEcoute.sin_addr.s_addr = INADDR_ANY;
	adrEcoute.sin_port = htons(port);
	printf("%s: binding to INADDR_ANY address on port %d\n", CMD, port);
	
	ret = bind (ecoute,  (struct sockaddr *)&adrEcoute, sizeof(adrEcoute));
	if (ret < 0)
		erreur_IO("bind");

	printf("%s: listening to socket\n", CMD);
	printf("########################################################\n");
	
	ret = listen (ecoute, 5);
	if (ret < 0)
		erreur_IO("listen");

	while (VRAI) 
	{
		printf("\n\n");
		printf("########################################################\n");
		printf("%s: accepting a connection\n", CMD);
		printf("########################################################\n");
		
		canal = accept(ecoute, (struct sockaddr *)&adrClient, &lgAdrClient);
		if (canal < 0)
			erreur_IO("accept");

		printf("\n\n");
		printf("########################################################\n");
		printf("%s: adr %s, port %hu\n", CMD, stringIP(ntohl(adrClient.sin_addr.s_addr)), ntohs(adrClient.sin_port));
		printf("########################################################\n");

		if (sem_wait(&monsem)==-1)
			erreur_IO("Ouverture Semaphore");

		dataThread = ajouterDataThread();
		if (dataThread == NULL)
			erreur_IO("ajouter data thread");
		dataThread->spec.canal = canal;
		
		ret = pthread_create(&dataThread->spec.id, NULL, sessionClient, &dataThread->spec);
		if (ret != 0)
			erreur_IO("creation thread");
	}

	if (close(ecoute) == -1)
		erreur_IO("fermeture ecoute");


	exit(EXIT_SUCCESS);
}

void *sessionClient(void *arg) 
{
	DataSpec *dataTh = (DataSpec *)arg;
  	int canal;
  	int deconnection = FAUX;
  	int boucleUtilisateur = 1;
  	int boucleRecherche;
  	int boucleSuppresion;
  	int boucleNewUser;
  	int existenceProfil;
  	int modifProfil;
  	char IdUser[LIGNE_MAX];
  	char mdpUser[LIGNE_MAX];
  	char IdNewUser[LIGNE_MAX];
  	char mdpNewUser[LIGNE_MAX];
  	char listeElementProfil[LIGNE_MAX];
    	char nbCordes[LIGNE_MAX];
    	char nbFretes[LIGNE_MAX];
    	char tuning[LIGNE_MAX];
  	int nbConnectionUtilisateur = 1;
  	char ligne[LIGNE_MAX];
  	char nomProfil[LIGNE_MAX];
  	int lg_read, lg_written;
  	Representation R;
  
  	canal = dataTh->canal;
  	
  	//Connexion du client
  	////Verification de l'ID utilisateur
  	while(boucleUtilisateur)
  	{
  		lg_read = lireLigne(canal, ligne);
  		if (lg_read < 0)
		    	erreur_IO("lecture serveur");
		
		if (rechercheUtilisateur(ligne, mdpUser) && verifConnectedUser(ligne))
		{
			printf("\n\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("%s : Utilisateur existant en tentative de connexion.\n",ligne);
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			strcpy(IdUser,ligne);
			
			lg_written = ecrireLigne(canal, "ID Utilisateur correct\n"); //Confirmation de l'existence de l'utilisateur
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
			
			boucleUtilisateur = 0;
		}
		else if(verifConnectedUser(ligne)==0)
		{
			lg_written = ecrireLigne(canal, "Utilisateur déjà connecté via un autre terminal, veuillez vous déconnecter de l'autre terminal ou utiliser un autre compte.\n"); //Confirmation de la non existence de l'utilisateur
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
		}
		else
		{
			lg_written = ecrireLigne(canal, "ID utilisateur inexistant dans la base de données, veuillez vérifier l'écriture, ou vous rapprocher d'un utilisateur pour créer un compte\n"); //Confirmation de la non existence de l'utilisateur
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
		}
  	}
  	addConnectedUser(IdUser);
  	////Verification du mot de passe
  	boucleUtilisateur = 1;
  	while(boucleUtilisateur)
  	{
  		lg_read = lireLigne(canal, ligne);
  		if (lg_read < 0)
		    	erreur_IO("lecture serveur");
		
		//////Cas 1 : Mot de passe correct
		if (strcmp(ligne, mdpUser)==0)
		{
			printf("\n\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("%s : Mot de passe correct reçu : %s \n", IdUser, ligne);
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			
			lg_written = ecrireLigne(canal, "Mot de passe correct\n"); //Confirmation du mot de passe et de la demande de connexion
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
			
			boucleUtilisateur = 0;
		}
		//////Cas 2 : Nombre de tentatives maximales dépassées
		else if (nbConnectionUtilisateur==MAX_CONNECTION_USER)
		{
			printf("\n\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("%s : L'utilisateur a dépassée le maximum de tentatives autorisées\n", IdUser);
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

			lg_written = ecrireLigne(canal, "Nombre de tentatives maximal atteint\n"); //Information pour le client, deconnection de ce dernier
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
			
			boucleUtilisateur = 0;
			deconnection = 1;
		}
		//////Cas 3 : Mot de passe eronnée, mais tentatives maximales non atteint
		else
		{
			lg_written = ecrireLigne(canal, "Mot de passe incorrect, veuillez réessayer\n"); //Information pour le client, mot de passe eronnée
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
			nbConnectionUtilisateur++;
		}
  	}
  	//Arborescence des menus
  	while(!deconnection)
  	{
  		recupProfil(canal,IdUser);
		
		lg_read = lireLigne(canal, ligne);
  		if (lg_read < 0)
		    	erreur_IO("lecture serveur");
		
		if (strcmp(ligne,"c")==0)
		{
			existenceProfil = 1;
			while(existenceProfil)
			{
				lg_read = lireLigne(canal, ligne);
				
	  			if (lg_read < 0)
			    		erreur_IO("lecture serveur");
			    
			    	
			    	if(verificationExistenceProfil(canal, ligne, IdUser)==0)
			    	{
					existenceProfil = 0;
			    	}
		    	}
		    	FILE* fichierProfil;  //Fichier qui contient les parametres du profil creer
		    	FILE* fichierUtilisateur;   //Fichier qui contient les noms des différents profils de l'utilisateur connecté
		    	char NomFichierProfil[LIGNE_MAX] = {};
		    	char NomFichierUtilisateur[LIGNE_MAX] = {};
		    	strcpy(nbCordes,"6");
		    	strcpy(nbFretes,"12");
		    	strcpy(tuning,"EADGBE");
		    	//
		    	strcat(NomFichierProfil,"../Users_profile/");
		    	strcat(NomFichierProfil,IdUser);
		    	strcat(NomFichierProfil,"_");
		    	strcat(NomFichierProfil,ligne);
		    	strcat(NomFichierProfil,".txt");
		    	//
		    	strcat(NomFichierUtilisateur,"../Users_profile/");
		    	strcat(NomFichierUtilisateur,IdUser);
		    	strcat(NomFichierUtilisateur,"_Profils.txt");
		    	//
		    	fichierProfil = fopen(NomFichierProfil,"w");
		    	fichierUtilisateur = fopen(NomFichierUtilisateur,"a");
		    	//
		    	fprintf(fichierUtilisateur,"%s\n",ligne);
		    	fclose(fichierUtilisateur);
		    	//
		    	fprintf(fichierProfil,"%s\n%s\n%s",nbCordes,nbFretes,tuning);
		    	fclose(fichierProfil);
		    	modifProfil = 1;
		    	while(modifProfil)
		    	{					
		    		strcpy(listeElementProfil,"1 : Nombre de cordes = ");
		    		strcat(listeElementProfil,nbCordes);
		    		lg_written = ecrireLigne(canal, listeElementProfil); //Information pour le client, profil existants
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
					
		    		strcpy(listeElementProfil,"2 : Nombre de fretes = ");
		    		strcat(listeElementProfil,nbFretes);
		    		lg_written = ecrireLigne(canal, listeElementProfil); //Information pour le client, profil existants
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
					
		    		strcpy(listeElementProfil,"3 : Tuning = ");
		    		strcat(listeElementProfil,tuning);
		    		lg_written = ecrireLigne(canal, listeElementProfil); //Information pour le client, profil existants
				if (lg_written == -1)
					erreur_IO("ecriture serveur");
				
				lg_read = lireLigne(canal, ligne);
  				if (lg_read < 0)
		    			erreur_IO("lecture serveur");
		    		
		    		else if (strcmp(ligne,"1")==0)
		    		{
		    			lg_read = lireLigne(canal, ligne);
  					if (lg_read < 0)
		    				erreur_IO("lecture serveur");
		    			
		    			strcpy(nbCordes,ligne);		    		
		    		}
		    		else if (strcmp(ligne,"2")==0)
		    		{
		    			lg_read = lireLigne(canal, ligne);
  					if (lg_read < 0)
		    				erreur_IO("lecture serveur");
		    			
		    			strcpy(nbFretes,ligne);		    		
		    		}
		    		else if (strcmp(ligne,"3")==0)
		    		{
		    			lg_read = lireLigne(canal, ligne);
  					if (lg_read < 0)
		    				erreur_IO("lecture serveur");
		    			
		    			strcpy(tuning,ligne);		    		
		    		}
		    		else if (strcmp(ligne,"r")==0)
		    		{
		    			modifProfil = 0;
		    		}
		    		fichierProfil = fopen(NomFichierProfil,"w");
		    		fprintf(fichierProfil,"%s\n%s\n%s",nbCordes,nbFretes,tuning);
		    		fclose(fichierProfil);
		    	}
		    	printf("\n\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("%s : Nouveau profil créer par l'utilisateur", IdUser);
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		}
		else if (strcmp(ligne,"r")==0)
		{
			deconnection = 1;
		}
		else if (strcmp(ligne, "s")==0)
		{
			boucleSuppresion = 1;
			while(boucleSuppresion)
			{
				lg_read = lireLigne(canal, ligne);
  				if (lg_read < 0)
		    			erreur_IO("lecture serveur");
		    		
		    		if (verificationExistenceProfil(canal, ligne, IdUser)==1)
				{
					supprProfil(ligne, IdUser);
					boucleSuppresion = 0;
				}
			}
		}
		else if (strcmp(ligne, "u")==0)
		{
			boucleNewUser = 1;
			while(boucleNewUser)
			{
				lg_read = lireLigne(canal, IdNewUser);
	  			if (lg_read < 0)
			    		erreur_IO("lecture serveur");
			    	
			    	if (rechercheUtilisateur(IdNewUser, mdpNewUser)==0)
			    	{
			    		boucleNewUser = 0;
			    		lg_written = ecrireLigne(canal, "IdUser libre\n"); //Envoi du résultat de a vérification de l'exitence de l'utilisateur
					if (lg_written == -1)
						erreur_IO("ecriture serveur");
			    	}
			    	else
			    	{
			    		lg_written = ecrireLigne(canal, "IdUser occupé\n"); //Envoi du résultat de a vérification de l'exitence de l'utilisateur
					if (lg_written == -1)
						erreur_IO("ecriture serveur");
			    	}
		    	}	
		    	lg_read = lireLigne(canal, mdpNewUser);
  			if (lg_read < 0)
		    		erreur_IO("lecture serveur");
		    	
		    	ajoutUtilisateur(IdNewUser, mdpNewUser);
		    	
			printf("\n\n");
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			printf("%s : Nouvel utilisateur ajouté par l'utilisateur.\n", IdUser);
			printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		}
		else
		{
			strcpy(nomProfil,ligne);
			if (verificationExistenceProfil(canal, nomProfil, IdUser)==1)
			{
				recupDataProfil(IdUser, nomProfil, tuning, nbCordes, nbFretes);
				boucleRecherche = 1;
		    		while(boucleRecherche)
		    		{
		    			lg_read = lireLigne(canal, ligne);
  					if (lg_read < 0)
		    				erreur_IO("lecture serveur");
		    			
		    			printf("%s\n",ligne);
		    			
		    			if (strcmp(ligne, "r")==0)
		    			{
		    				boucleRecherche = 0;
		    			}
		    			else
		    			{
		    				recherche_Init(&R, tuning, atoi(nbCordes), atoi(nbFretes), ligne,0,0);
		    				dossier_Profil(&R);
		    				dossier_Accord(&R);
			    			
			    			lg_read = lireLigne(canal, ligne);
	  					if (lg_read < 0)
			    				erreur_IO("lecture serveur");
			    				
			    			if (strcmp(ligne, "0")==0)
						{
							nouvel_Accord(&R, canal);
						}
						else if (strcmp(ligne, "1")==0)
						{
							choix_Variante(&R,canal);
            						printf("%s\n",ligne);
						}
		    			}
		    		}
			}
		}
  	}
  	//Procédure de fermeture du thread
  	if (sem_post(&monsem) == -1)
  	 	erreur_IO("fermeture semaphore");
  
  	if (close(canal) == -1)
    		erreur_IO("fermeture canal");
    	
    	subConnectedUser(IdUser);
    	
  	dataTh->libre = VRAI;

	printf("\n\n");
    	printf("########################################################\n");
    	printf("%s : Deconnection de l'utilisateur\n", IdUser);
    	printf("########################################################\n");

  	pthread_exit(NULL);
}
