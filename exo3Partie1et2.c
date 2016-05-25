#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define N 100000



//Copie le contenu du fichier 1 dans le fichier 2
void lectureEcriture(int fdRead, int fdWrite)
{
	char buf[100];
	int nbLu;
	while((nbLu=read(fdRead,buf,sizeof(buf)))!=0)
	{
		write(fdWrite,buf,(ssize_t)nbLu);
	}
	
}

//exo3 
int main(int argc, char * argv[])
{
	int fdWrite;
	// Test s'il y a bien les 2 noms de fichier
	if(argc !=3)
	{
		printf("Problème d'arguments\n");
		exit(1);
	}
	else
	{
		// On ouvre le fichier cible en écriture
		fdWrite=open(argv[2],O_WRONLY |  O_CREAT,0666);
		if(fdWrite==-1)
		{
			printf("erreur avec la création du fichier d'écriture \n");
			exit(2);
		}
	}
	// On ouvre le fichier source en lecture
	//printf("argv[1] : %s\n",argv[1]);
	int fdRead=open(argv[1],O_RDONLY);
	if(fdRead==-1)
	{
		printf("erreur avec l'ouverture du fichier de lecture\n");
		exit(2);
	}

	// On crée une variable qui contient les infos du fichier source
	struct stat source;
	stat(argv[1],&source);
	// On donne des droits au fichier cible identique au fichier source
	fchmod(fdWrite,source.st_mode);

	// On copie le contenu du fichier source dans le fichier cible
	lectureEcriture(fdRead,fdWrite);

	//On ferme les fichiers
	close(fdWrite);
	close(fdRead);
	
	return 0;
}

