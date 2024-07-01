//------------------------------------------------------------------------------------------------//
// Fonctions en rapport avec le fichier des utilisateurs de la bibliotèques et leurs mot de passe //
//------------------------------------------------------------------------------------------------//

/* Fonction qui permet de rechercher l'existence d'un utilisateur dans le fichier texte et de stocker son mot de passe pour la vérification */
int rechercheUtilisateur(char* user, char* mdpUser);


/* Fonction qui permet d'ajouter un nouvel utilisateur */
void ajoutUtilisateur(char* Id, char* Mdp);


//------------------------------------------------------------------------------------------//
// Fonctions en rapport avec les différents fichiers contenant les informations des profils //
//------------------------------------------------------------------------------------------//

/* Fonction qui permet d'envoyer au client les noms des profils qu'il a enregistré */
void recupProfil(int canal, char* IdUser);


/* Fonction qui permet de récupérer les infos d'un profil */
void recupDataProfil(char* IdUser, char* nomProfil, char* tuning, char* nbCordes, char* nbFretes);


/* Fonction permettant de vérifier qu'un profil choisit existe bien toujours dans le serveur */
int verificationExistenceProfil(int canal, char* nomProfil, char* IdUser);


/* Fonction qui permet de supprimer un profil de la liste de l'utilisateur */
void supprProfil(char* nomProfil, char* IdUser);


//---------------------------------------------------------------------------//
// Fonctions en rapport avec la vérification des utilisateurs déjà connectés //
//---------------------------------------------------------------------------//

/* Fonction qui permet de rechercher si l'utilisateur est déjà connecté au serveur via un autre client */
int verifConnectedUser(char* user);


/* Fonction qui permet d'initialiser la variable globale contenant les utilisateurs connectés au serveur */
void initConnectedUser();


/* Fonction qui permet d'ajouter un utilisateur connecté à la liste */
void addConnectedUser(char* user);


/* Fonction qui permet de retirer un utilisateur connecté de la liste */
void subConnectedUser(char* user);
