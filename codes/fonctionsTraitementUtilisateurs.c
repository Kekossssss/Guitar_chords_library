#include "pse.h"

#define   NB_WORKERS  5

int rechercheUtilisateur(char* user, char* mdpUser);
void recupProfil(int canal, char* IdUser);
void recupDataProfil(char* IdUser, char* nomProfil, char* tuning, char* nbCordes, char* nbFretes);
int verificationExistenceProfil(int canal, char* nomProfil, char* IdUser);
int verifConnectedUser(char* user);
void initConnectedUser();
void addConnectedUser(char* user);
void subConnectedUser(char* user);
void supprProfil(char* nomProfil, char* IdUser);
void ajoutUtilisateur(char* Id, char* Mdp);

extern char connectedUser[NB_WORKERS][LIGNE_MAX];

extern pthread_mutex_t monmutex;

int rechercheUtilisateur(char* user, char* mdpUser)
{
	FILE* fichier;
	char a[LIGNE_MAX];
	char b[LIGNE_MAX];
	fichier = fopen("utilisateurs.txt","r");
	while(fscanf(fichier,"%s",a)==1)
	{
		fscanf(fichier,"%s",b);
		if (strcmp(user,a)==0)
		{
			strcpy(mdpUser,b);
			fclose(fichier);
			return(1);
		}
	}
	fclose(fichier);
	return(0);
}

void recupProfil(int canal, char* IdUser)
{
	FILE* fichier;
	char NomFichierUtilisateur[LIGNE_MAX] = {};
	char a[LIGNE_MAX];
	int lg_written;
	//
	strcat(NomFichierUtilisateur,"../Users_profile/");
	strcat(NomFichierUtilisateur,IdUser);
	strcat(NomFichierUtilisateur,"_Profils.txt");
	//
	fichier = fopen(NomFichierUtilisateur,"a+");
	while(fscanf(fichier," %s",a)==1)
	{
		lg_written = ecrireLigne(canal, a); //Information pour le client, profil existants
		if (lg_written == -1)
			erreur_IO("ecriture serveur");
	}
	strcpy(a,"finProfils\n");
	lg_written = ecrireLigne(canal,a); //Information pour le client, profil existants
	if (lg_written == -1)
		erreur_IO("ecriture serveur");
	fclose(fichier);
}

void recupDataProfil(char* IdUser, char* nomProfil, char* tuning, char* nbCordes, char* nbFretes)
{
	FILE* fichier;
	char a[LIGNE_MAX];
	char NomFichierProfil[LIGNE_MAX] = {};
	strcat(NomFichierProfil,"../Users_profile/");
    	strcat(NomFichierProfil,IdUser);
    	strcat(NomFichierProfil,"_");
    	strcat(NomFichierProfil,nomProfil);
    	strcat(NomFichierProfil,".txt");
    	fichier = fopen(NomFichierProfil,"r");
    	fscanf(fichier," %s",a);
    	strcpy(nbCordes,a);
    	fscanf(fichier," %s",a);
    	strcpy(nbFretes,a);
    	fscanf(fichier," %s",a);
    	strcpy(tuning,a);
    	fclose(fichier);
}


int verificationExistenceProfil(int canal, char* nomProfil, char* IdUser)
{
	FILE* fichier;
	char NomFichierUtilisateur[LIGNE_MAX] = {};
	char a[LIGNE_MAX];
	int lg_written;
	//
	strcat(NomFichierUtilisateur,"../Users_profile/");
	strcat(NomFichierUtilisateur,IdUser);
	strcat(NomFichierUtilisateur,"_Profils.txt");
	//
	fichier = fopen(NomFichierUtilisateur,"a+");
	while(fscanf(fichier," %s",a)==1)
	{
		if (strcmp(a,nomProfil)==0)
		{
			lg_written = ecrireLigne(canal, "choixCorrect\n"); //Information pour le client, profil existants
			if (lg_written == -1)
				erreur_IO("ecriture serveur");
			fclose(fichier);
			return(1);
		}
	}
	strcpy(a,"choixIncorrect\n");
	lg_written = ecrireLigne(canal,a); //Information pour le client, profil existants
	if (lg_written == -1)
		erreur_IO("ecriture serveur");
	fclose(fichier);
	
	return(0);
}

int verifConnectedUser(char* user)
{
	pthread_mutex_lock(&monmutex);
	for (int i=0;i<NB_WORKERS;i++)
	{
		if (strcmp(user, connectedUser[i])==0)
		{
			pthread_mutex_unlock(&monmutex);
			return(0);
		}
	}
	pthread_mutex_unlock(&monmutex);
	return(1);
}

void initConnectedUser()
{
	pthread_mutex_lock(&monmutex);
	for (int i=0;i<NB_WORKERS;i++)
	{
		strcpy(connectedUser[i],"free");
	}
	pthread_mutex_unlock(&monmutex);
}


void addConnectedUser(char* user)
{
	pthread_mutex_lock(&monmutex);
	for (int i=0;i<NB_WORKERS;i++)
	{
		if (strcmp("free",connectedUser[i])==0)
		{
			strcpy(connectedUser[i],user);
		}
	}
	pthread_mutex_unlock(&monmutex);
}

void subConnectedUser(char* user)
{
	pthread_mutex_lock(&monmutex);
	for (int i=0;i<NB_WORKERS;i++)
	{
		if (strcmp(user,connectedUser[i])==0)
		{
			strcpy(connectedUser[i],"free");
		}
	}
	pthread_mutex_unlock(&monmutex);
}

void supprProfil(char* nomProfil, char* IdUser)
{
	FILE* fichier;
	char NomFichierUtilisateur[LIGNE_MAX] = {};
	char a[LIGNE_MAX];
	char enregProfil[100000] = "";
	//
	strcat(NomFichierUtilisateur,"../Users_profile/");
	strcat(NomFichierUtilisateur,IdUser);
	strcat(NomFichierUtilisateur,"_Profils.txt");
	//
	fichier = fopen(NomFichierUtilisateur,"r");
	while(fscanf(fichier," %s",a)==1)
	{
		if (strcmp(a,nomProfil)!=0)
		{
			strcat(enregProfil,a);
		}
		else
		{
			strcat(enregProfil,"\n");
		}
	}
	strcat(enregProfil,"\n");
	fclose(fichier);
	fichier = fopen(NomFichierUtilisateur,"w");
	fprintf(fichier," %s", enregProfil);
	fclose(fichier);
}

void ajoutUtilisateur(char* Id, char* Mdp)
{
	FILE* fichier;
	fichier = fopen("utilisateurs.txt","a");
	fprintf(fichier, " %s\t%s\n", Id, Mdp);
	fclose(fichier);
}
