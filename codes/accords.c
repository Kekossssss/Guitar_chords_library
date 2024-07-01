//
//  accords.c
//  
//
//  Created by Némo Kardassevitch on 29/05/2024.
//

#include "accords.h"
#include "pse.h"



//Initialisations


void instrument_Init (Instruments *I, char *tuning, int nbC, int nbF) {
    I->tuning = (char *)malloc(nbC*sizeof(char));
    strcpy(I->tuning, tuning);
    I->nbCordes = nbC;
    I->nbFrettes = nbF;
}

void accord_Init (Accords *A, char *nom, int pF, int taille) {
    A->nomAccord = (char *)malloc(strlen(nom)*sizeof(char));
    strcpy(A->nomAccord, nom);
    A->premFrette = pF;
    A->tailleAccord = taille;
}

void acces_Init (Acces *Ac, char *dossierProfil, char *dossierAccord) {
    Ac->dossierProfil = (char *)malloc((strlen(dossierProfil)+strlen("../Base_accords/"))*sizeof(char));
    strcpy(Ac->dossierProfil, "../Base_accords/");
    strcat(Ac->dossierProfil, dossierProfil);
    Ac->dossierAccord = (char *)malloc(strlen(dossierAccord)*sizeof(char));
    strcpy(Ac->dossierAccord, dossierAccord);
}

void representation_Init (Representation *R, Instruments *I, Accords *A, Acces *Ac) {
    R->instrument = I;
    R->accord = A;
    R->acces = Ac;
    R->T = (int **)malloc(R->instrument->nbCordes*sizeof(int *));
    for (int i = 0; i < R->instrument->nbCordes; i++) {
        int tAcc = R->accord->tailleAccord;
        if (R->accord->premFrette == 0) {
            tAcc++;
        }
        R->T[i] = (int *)malloc(tAcc*sizeof(int));
        for (int j = 0; j < tAcc; j++) {
            R->T[i][j] = 0;
        }
    }
}


void recherche_Init (Representation *R, char *tuning, int nbC, int nbF, char *nom, int pF, int taille) {
    //Initialisation de l'instrument
    Instruments *I;
    I = (Instruments *)malloc(sizeof(Instruments));
    instrument_Init(I,tuning,nbC,nbF);
    
    //Initialisation de l'accord
    Accords *A;
    A = (Accords *)malloc(sizeof(Accords));
    accord_Init(A,nom,pF,taille);
    
    //Initialisation du chemin d'accès
    Acces *Ac;
    Ac = (Acces *)malloc(sizeof(Acces));
    /*Format du dossier profil: nbCordes_tuning*/
    char dossierProfil [100] = "";
    if (I->nbCordes < 10) {
        strcat(dossierProfil,"0");
    }
    char mot_nbC [10];
    snprintf(mot_nbC, sizeof(I->nbCordes), "%d", I->nbCordes);
    strcat(dossierProfil,mot_nbC);
    strcat(dossierProfil,"_");
    strcat(dossierProfil,I->tuning);
    /*Format du dossier accord: accord*/
    acces_Init(Ac,dossierProfil,A->nomAccord);
    
    //Initialisation de la représentation
    representation_Init(R,I,A,Ac);
}



//Free


void free_Instrument (Instruments *I) {
    free(I->tuning);
    free(I);
}

void free_Accord (Accords *A) {
    free(A->nomAccord);
    free(A);
}

void free_Acces (Acces *Ac) {
    free(Ac->dossierProfil);
    free(Ac->dossierAccord);
    free(Ac);
}

void free_Representation (Representation *R) {
    for (int i  = 0; i < R->instrument->nbCordes; i++) {
        free(R->T[i]);
    }
    free(R->T);
}

void free_Tout (Representation *R) {
    free_Instrument(R->instrument);
    free_Accord(R->accord);
    free_Acces(R->acces);
    free_Representation(R);
}



//Affichages


void affiche_Instrument (Instruments *I, int canal) {
	char ligne[LIGNE_MAX];
	//nb corde
	snprintf(ligne, LIGNE_MAX, "%d\n", I->nbCordes);
	ecrireLigne(canal, ligne);
	
	//nbFrettes
	snprintf(ligne, LIGNE_MAX, "%d\n", I->nbFrettes);
	ecrireLigne(canal, ligne);
	
	//tuning
	snprintf(ligne, LIGNE_MAX, "%s\n", I->tuning);
	ecrireLigne(canal, ligne);
}

void affiche_Accord (Accords *A, int canal){
	char ligne[LIGNE_MAX];
	//nom accord
	snprintf(ligne, LIGNE_MAX, "%s\n", A->nomAccord);
	ecrireLigne(canal, ligne);
	
	//premiere frete
	snprintf(ligne, LIGNE_MAX, "%d\n", A->premFrette);
	ecrireLigne(canal, ligne);
}

void affiche_Representation (Representation *R, int canal) {
    char tuning [100];
    char ligne[LIGNE_MAX];
    strcpy(tuning, R->instrument->tuning);
    int tAcc = R->accord->tailleAccord;
    if (R->accord->premFrette == 0) {
        tAcc++;
        ecrireLigne(canal, "     Frette 0\n");
    }
    else {
    	snprintf(ligne, LIGNE_MAX, "     Frette %d\n", R->accord->premFrette);
        ecrireLigne(canal, ligne);
    }
    for (int i = 0; i < R->instrument->nbCordes; i++) {
        snprintf(ligne, LIGNE_MAX, "%c     \n", tuning[i]);
        ecrireLigne(canal, ligne);
        for (int j = 0; j < tAcc; j++) {
            if (j == 0 && R->accord->premFrette == 0) {
                if (R->T[i][j] == 0) {
                    ecrireLigne(canal, "O ||\n");
                }
                if (R->T[i][j] == 1) {
                    ecrireLigne(canal, "X ||\n");
                }
                if (R->T[i][j] == 2) {
                    ecrireLigne(canal, "? ||\n");
                }
            }
            else {
                if (R->T[i][j] == 0) {
                    ecrireLigne(canal, " -\n");
                }
                if (R->T[i][j] == 1) {
                    ecrireLigne(canal, " x\n");
                }
                if (R->T[i][j] == 2) {
                    ecrireLigne(canal, " ?\n");
                }
            }
        }
       	ecrireLigne(canal, "SautLigne\n");
    }
    ecrireLigne(canal, "FinRepresentation\n");
}

void affiche_Tout (Representation *R, int canal) {
    affiche_Instrument(R->instrument, canal);
    affiche_Accord(R->accord, canal);
    affiche_Representation(R, canal);
}

void affiche_Variantes (Representation *R, int **LV, int canal) {
	int nbV;
	char ligne[LIGNE_MAX];
	nbV = nb_Variantes(R);

	for (int i = 0; i < nbV; i++) 
	{
		snprintf(ligne, LIGNE_MAX, "Variante %d:\n", i);
		ecrireLigne(canal, ligne);
		
		snprintf(ligne, LIGNE_MAX, "     - première frette: %d\n", LV[i][1]);
		ecrireLigne(canal, ligne);
		
		snprintf(ligne, LIGNE_MAX, "     - taille accord: %d\n",LV[i][0]);
		ecrireLigne(canal, ligne);
	}    
	ecrireLigne(canal, "FinVariations\n");
}



//Organisation des dossiers


void dossier_Profil (Representation *R) {
    mkdir(R->acces->dossierProfil, 0755);
}

void dossier_Accord (Representation *R) {
    char chemin[100];
    snprintf(chemin, sizeof(chemin), "%s/%s", R->acces->dossierProfil, R->acces->dossierAccord);
    mkdir(chemin, 0755);
}

int fichier_Existe(Representation *R) {
    // Chemin d'accès
    int tAcc = R->accord->tailleAccord;
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "%s/%s/", R->acces->dossierProfil, R->acces->dossierAccord);
    if (tAcc < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d_", tAcc);
    if (R->accord->premFrette < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d.txt", R->accord->premFrette);
    
    //Verification d'existence
    struct stat st;
    return (stat(chemin, &st) == 0);
}

int nb_Variantes (Representation *R) {
    DIR *dossier;
    struct dirent *ent;
    int n = 0;
    char chemin[100];
    snprintf(chemin, sizeof(chemin), "%s/%s", R->acces->dossierProfil, R->acces->dossierAccord);
    
    if ((dossier = opendir(chemin)) != NULL) {
        while ((ent = readdir(dossier)) != NULL) {
            if (ent->d_name[0] != '.') {
                n++;
            }
        }
        closedir(dossier);
    }
    return n;
}



//Lecture et écriture des fichiers

void ecriture (Representation *R) {
    int tAcc = R->accord->tailleAccord;
    // Chemin d'accès
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "%s/%s/", R->acces->dossierProfil, R->acces->dossierAccord);
    if (tAcc < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d_", tAcc);
    if (R->accord->premFrette < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d.txt", R->accord->premFrette);
    // Écriture du fichier
    FILE *fichier = fopen(chemin, "w");
    int adjusted_tAcc = tAcc;
    if (R->accord->premFrette == 0) {
       adjusted_tAcc++;
    }
    for (int i = 0; i < R->instrument->nbCordes; i++) {
       for (int j = 0; j < adjusted_tAcc; j++) {
           fprintf(fichier, "%d\n", R->T[i][j]);
       }
    }
    fclose(fichier);
}

void lecture (Representation *R) {
    int tAcc = R->accord->tailleAccord;
    
    // Chemin d'accès
    char chemin[256];
    snprintf(chemin, sizeof(chemin), "%s/%s/", R->acces->dossierProfil, R->acces->dossierAccord);
    if (tAcc < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d_", tAcc);
    if (R->accord->premFrette < 10) {
       strcat(chemin, "0");
    }
    snprintf(chemin + strlen(chemin), sizeof(chemin) - strlen(chemin), "%d.txt", R->accord->premFrette);
    
    //Lecture du fichier
    FILE *fichier = fopen(chemin, "r");
    int adjusted_tAcc = tAcc;
    if (R->accord->premFrette == 0) {
       adjusted_tAcc++;
    }
    for (int i = 0; i < R->instrument->nbCordes; i++) {
       for (int j = 0; j < adjusted_tAcc; j++) {
           fscanf(fichier, "%d\n", &R->T[i][j]);
       }
    }
    fclose(fichier);
}



//Choix utilisateur

void choix_Variante(Representation *R, int canal) {
	//Récupération des données
	int nbV;
	int n = 0;
	char ligne[LIGNE_MAX];
	nbV = nb_Variantes(R);
	int **ListeVariantes;
	ListeVariantes = (int **)malloc(nbV*sizeof(int*));
	for (int i = 0; i < nbV; i++) {
	ListeVariantes[i] = (int *)malloc(2*sizeof(int));
	}

	if (nbV == 0) 
	{
		ecrireLigne(canal, "accordInexistant\n"); //Envoi de l'accord choisi
	}
	else 
	{
		ecrireLigne(canal, "accordExistant\n"); //Envoi de l'accord choisi
		DIR *dossier;
		struct dirent *ent;
		char chemin[100];
		char dP[100];
		char dA[100];
		strcpy(dP,R->acces->dossierProfil);
		strcpy(dA,R->acces->dossierAccord);
		snprintf(chemin, sizeof(chemin), "%s/%s", dP, dA);
		if ((dossier = opendir(chemin)) != NULL) {
		    while ((ent = readdir(dossier)) != NULL) {
			if (ent->d_name[0] != '.') {
			    char pF [10];
			    char tA [10];
			    strncpy(pF, ent->d_name+3, 2);
			    strncpy(tA, ent->d_name, 2);
			    ListeVariantes[n][0] = atoi(tA);
			    ListeVariantes[n][1] = atoi(pF);
			    n++;
			}
		    }
		    closedir(dossier);
		}
		//Choix
		int choix;
		affiche_Variantes(R,ListeVariantes, canal);
		lireLigne(canal, ligne);
		choix = atoi(ligne);
		accord_Init (R->accord, R->accord->nomAccord, ListeVariantes[choix][1], ListeVariantes[choix][0]);

		//Libération malloc
		for (int l = 0; l < nbV; l++) {
			free(ListeVariantes[l]);
		}
		
		//Affichage
		consult_Accord(R, canal);
	}
	free(ListeVariantes);
}

/*void choix_Action (Representation *R, int canal) {
    int choix;
    //printf("\nQuelle action souhaitez vous effectuer?\n");
    //printf("0 - Retour à la recherche\n");
    //printf("1 - Créer un nouvel accord\n");
    //printf("2 - Consulter les accords existants\n");
    //scanf("%d", &choix);
    switch (choix) {
        case 0:
            break;
        case 1:
            nouvel_Accord(R, canal);
            break;
        case 2:
            choix_Variante(R,canal);
            consult_Accord(R);
            break;
    }
}*/

void choix_Construction (Representation *R, int canal) {
	int pF;
	int taille;
	int lg_read;
	char ligne[LIGNE_MAX];
	
	lg_read = lireLigne(canal, ligne);
  	if (lg_read < 0)
		erreur_IO("lecture serveur");
	pF = atoi(ligne);
	
	lg_read = lireLigne(canal, ligne);
  	if (lg_read < 0)
		erreur_IO("lecture serveur");
	taille = atoi(ligne);
	accord_Init (R->accord, R->accord->nomAccord, pF, taille);
}



//Actions


void nouvel_Accord(Representation *R, int canal) {
    char ligne[LIGNE_MAX];
    //Données nécéssaire pour construir un nouvel accord
    choix_Construction(R, canal);
    
    //Vérification de la non existence de l'accord
    if (fichier_Existe(R) == 0) {
        
        ecrireLigne(canal, "fichierInexistant\n");
        //Création de l'accord
        modifie_Accord(R, canal);
        affiche_Tout(R, canal);
        
        //Confirmation
        int conf = 0;
	lireLigne(canal, ligne);
	conf = atoi(ligne);
        if (conf == 1) {
            ecriture(R);
        }
    }
    else {
    	ecrireLigne(canal, "fichierDejaExistant\n");
        consult_Accord(R, canal);
    }
    
}

void consult_Accord (Representation *R, int canal) {
    char ligne[LIGNE_MAX];
    //Récupération des données
    lecture(R);
    
    //Affichage des données
    affiche_Tout(R, canal);
    
    //Proposition de modification de l'accord
    int choix;
    lireLigne(canal, ligne);
    printf("Choix modification ou pas : %s\n",ligne);
    choix = atoi(ligne);
    if (choix == 1) {
        modifie_Accord(R, canal);
        affiche_Tout(R, canal);
        int conf = 0;
	lireLigne(canal, ligne);
	conf = atoi(ligne);
        if (conf == 1) {
            ecriture(R);
        }
    }
}

void modifie_Accord (Representation *R, int canal) 
{    
	//Construction de l'accord
	int tAcc = R->accord->tailleAccord;
	char ligne[LIGNE_MAX];
	if (R->accord->premFrette == 0) {
		tAcc++;
	}

	int Joue;
	for (int i = 0; i < R->instrument->nbCordes; i++) {
		for (int j = 0; j < tAcc; j++) {
	   		R->T[i][j] = 2;  // Initialiser la valeur à 2
	   		affiche_Representation(R, canal);  // Afficher l'état actuel
	    
	    		lireLigne(canal, ligne);
	    		Joue = atoi(ligne);
	    		R->T[i][j] = Joue;  // Mettre à jour la valeur selon l'entrée utilisateur
		}
	}
	ecrireLigne(canal, "FinModif\n"); //Information pour le client, fin de modification
}







/*int main () {
    
    Representation R;
    
    recherche_Init (&R,"EADGBE",6,24,"A",0,0);   //Pas toucher, tuning, nb cordes, nb fretes, accord recherché, pas toucher, pas toucher
    
    choix_Action(&R);
    
    free_Tout(&R);
    
    return 0;
}*/
