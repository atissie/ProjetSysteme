#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <utime.h>


void  lireCommande(char * ligne);
//char commande[50][100];
//char historique[50][100];
void execCommande();
int remplirArg(char *ligne,char **com, char *car);
//char ligne1[4096];
void execCd();
int nbCommande;
void history();
void nouvExit();
char repCour[500];
void nouvTouch();
void nouvCat();
void copie(char ** com);
//int nbArgu; // nombre d'argument de la commande actuelle
int analyseChaine(char * chaine, char car);
char  PATH[2000];
int connectionPipe(int entree, int sortie, char ** com, int nbArg);
void monExec(char ** com, int nbArg);
void libereEspace(char ** com,int nbArg);


int main(int argc, char * argv[])
{
    strcpy(PATH,getenv("PATH"));
    //nbCommande=0;
    //char **com=(char**)malloc(sizeof(char*));;
    //historique=(char**)malloc(sizeof(char*));
    do
    {

        //char ligne[4096];
        // Saisie de la commande
        //lireCommande(ligne);
        //int nbArg=remplirArg(ligne,com);
        execCommande();



        /*int i=0;
        for(i;i<nbArg;i++)
        	free(commande[i]);
        free(commande);*/
    }
    while(1);


}

int analyseChaine(char * chaine,char car) //retourne le nombre de caractère c dans la chaine et remplit un tableau de position
{
    int i=0,nb=0;
    //tab=(int*)malloc(sizeof(int));
    for(i=0;i<strlen(chaine);i++)
    {
        if(chaine[i]==car )
        {
            nb++;
            //tab=realloc(tab,sizeof(int)*(nb));
            //tab[nb]=i;
        }
        //i++;
    }
     return nb;
}

void lireCommande(char * ligne)
{
    char nom[500];
    if ( gethostname(nom, 500) )
    {
        printf("Erreur nom");
    }


    if ( getcwd (repCour, 500) == NULL )
    {
        printf("Erreur repertoire courant");
    }

    //*strchr(nom, '.') = '\0';

    printf("[%s@%s : %s]$ ", getenv("USER"), nom, repCour);

    fgets(ligne,200,stdin);

    //scanf("%s",ligne);
    //fflush(stdout);
    //fflush(stdin);
    /*if(ligne1[0]=='\0')
    	strcpy(ligne1,"");*/
    //historique[nbCommande]=(char*)malloc(sizeof(char)*strlen(ligne1));
    //strcpy(historique[nbCommande],ligne1);
    nbCommande++;
    int fic;
    fic=open("historique",O_CREAT |O_APPEND |  O_WRONLY,0666);
    if(fic==-1)
        printf("problème open\n");
    write(fic,ligne,strlen(ligne));
    close(fic);


}



int decoupeChaine(char *init,char * dest, int depla)
{

    int i=0;
    while(init[depla]!=':' && init[depla]!='\0')
    {

        dest[i]=init[depla];
        i++;
        depla++;
    }
    depla++;
    dest[i]='\0';
    //printf("dest : %s\n",dest);
    return depla;
}

void monExec(char ** com, int nbArg)
{
    // Création d'un tableau de type char ** pour executer execv
        char ** nouvCom;
        nouvCom=(char**)malloc(sizeof(char*)*nbArg+1);
        int i=1;
        //printf("nouvCom[%d] : \t%s\n",0,nouvCom[0]);


        for(i=0; i<nbArg; i++)
        {
            nouvCom[i]=(char*)malloc(strlen(com[i]));
            strcpy(nouvCom[i],com[i]);
        }
        //Mettre null dans la derniere case du tableau
        nouvCom[i]=NULL;
        // on récupere le contenu du path
        PATH[strlen(PATH)]='\0';
        int depla=0;
        while(depla<strlen(PATH))	// on essai d'executer la commande avec tous les chemins du path
        {
            char *dest =(char*)malloc(strlen(nouvCom[0]));
            depla=decoupeChaine(PATH,dest,depla);
            strcat(dest,"/");
            strcat(dest,nouvCom[0]);
            int tmp;
            execv(dest,nouvCom);
            strcpy(nouvCom[0],com[0]);
        }
        printf("Commande inconnue\n");
        exit(1);
}

/*void monExec( char ** com, int nbArg)
{
    int idproc=fork();
    if(idproc==0)
    {



    }
		else
		{
			int cptRendu;
			wait(&cptRendu);
			//printf("fils : %d\n",WEXITSTATUS(cptRendu));
		}
}*/

/*void recupPosCar(char * chaine, char car) // recupere la position  de la premiere occurence du caractère
{
    int i=0,trouve=0,pos=-1;
    while(i<strlen(chaine)&& trouve==0)
    {
        if(chaine[i]==car)
        {
            trouve=1;
            pos=i;
        }
        i++;
    }
    return pos;
}*/

void execCommande()
{
    char ligne[200];
    lireCommande(ligne); // on lit la ligne saisie par l'utilisateur
    //int *tabPipe,*tabChev;
    int nbPipe=analyseChaine(ligne,'|');
    int nbChev=analyseChaine(ligne,'>');
    int nbArg;
    if(nbPipe==0 && nbChev==0)
    {
        char ** com=(char**)malloc(sizeof(char *));
        nbArg=remplirArg(ligne,com," ");

    if(!strcmp(com[0],"cd"))
        {
            execCd(com,nbArg);
        }
        else if (!strcmp(com[0],"history"))
        {
            history(com,nbArg);
        }
        else if (!strcmp(com[0],"cat"))
        {
            nouvCat(com,nbArg);
        }
        else if (!strcmp(com[0],"exit")||!strcmp(com[0],"quit"))
        {
            nouvExit();
        }
        else if (!strcmp(com[0],"touch"))
        {
            nouvTouch(com,nbArg);
        }
        else if (!strcmp(com[0],"cp"))
        {
            copie(com);
        }
        else
        {
            int idproc=fork();
            if(idproc==0)
            {
                monExec(com,nbArg);
            }

            else
            {
                int cptRendu;
                wait(&cptRendu);
            }
        }
    }
    else if(nbPipe==0 )
    {
        if(analyseChaine(ligne,'>')==1)
        {

            int idproc=fork();
            if(idproc==0)
            {
                char ** decoupe=(char**)malloc(sizeof(char*));
                remplirArg(ligne,decoupe,">");
                decoupe[1]=strtok(decoupe[1]," ");
                while(strcmp(decoupe[1]," ")==0)
                    decoupe[1]=strtok(NULL," ");
                char ** tabCommande=(char**)malloc(sizeof(char*));
                int red=remplirArg(decoupe[0],tabCommande," ");


                int desc=open(decoupe[1],O_CREAT | O_APPEND | O_WRONLY,0666);
                if(desc<0)
                {
                    printf("erreur de fichier cible dans la redirection. Fin du prog\n");
                    exit(3);
                }
                dup2(desc,1); // la chaine contient au maximumm un > donc le premiere argument contient une commande et le deuxième le nom du fichier cible


                monExec(tabCommande,red);
            }
            else{
                int cptRendu;
                wait(&cptRendu);
            }
        }
    }
    else
    {                                                          //PIPE A FINIR
        /*int i;
        char ** tabCommande=(char**)malloc(sizeof(char*)); // tableau qui contient les différentes commandes séparées par des | ou >
        nbArg=remplirArg(ligne,tabCommande,"|"); // on remplit tabCommande

        for(i=0;i<nbArg;i++) // pour chaque commande de tabCommande, on crée un sous tableau pour exécuter la commande
        {
            char ** sousTabCom=(char **)malloc(sizeof(char *));
            int nbArgSousTab=remplirArg(tabCommande[i],sousTabCom," ");
            int tab[2];
            pipe(tab);
            int idproc=fork();
            if(idproc==0)
            {

                if(i!=nbArg-1) // le dernier fils doit ecrire le resultat dans la sortie standard, les autres dans le pipe
                {
                    dup2(tab[1],1);
                    close(tab[1]);
                }
                else
                {
                    if(analyseChaine(ligne,'>')==1) // ou il écrit dans un fichier s'il y a eu redirection
                    {
                        int red=remplirArg(ligne,tabCommande,">");
                        if(red!=1)
                        {
                            int desc=open(tabCommande[1],O_APPEND | O_WRONLY);
                            if(desc<0)
                            {
                                printf("erreur de fichier cible dans la redirection. Fin du prog\n");
                                exit(3);
                            }
                            dup2(desc,1); // la chaine contient au maximumm un > donc le premiere argument contient une commande et le deuxième le nom du fichier cible

                        }
                        for(i=0;i<red;i++)
                            free(tabCommande[i]);
                        free(tabCommande);
                    }
                }


                /*if(i!=0) // le premier fils doit lire dans l'entrée standard
                {
                    dup2(tab[0],0);
                    close(tab[0]);
                }
                monExec(sousTabCom,nbArgSousTab);
            }
            else
            {
                int cptRendu;
                wait(&cptRendu);

            }
            libereEspace(sousTabCom,nbArgSousTab);
        }

        libereEspace(tabCommande,nbArg);
    */}
    /*if(verifRedir!=-1)
    {
        if(ligne[verifRedir]=='|')
        {
            char * partie1=(char*)malloc(verifRedir+1); // la premiere partie de la commande va du début à |
            char * partie2=(char*)malloc(strlen(ligne)-verifRedir); // la 2e partie va de | à la fin
            int i;
            for(i=0;i<verifRedir;i++)
            {
                partie1[i]=ligne[i];
            }
            partie1[verifRedir]='\0';
            for(i=verifRedir+1;i<strlen(ligne);i++)
            {
                partie2[i]=ligne[i];
            }

        }
    }*/


    //
}
void libereEspace(char ** com,int nbArg)
{
int i;
    for(i=0;i<nbArg;i++)
        free(com[i]);
    free(com);
}

void copie(char ** com)
{
	struct stat source;
		stat(com[1],&source);
		//Test si la source est un répertoire
		if(S_ISDIR(source.st_mode))
		{
			// Cible contient les infos du répertoire cible
			struct stat cible;
			stat(com[2],&cible);
			//Test si la cible est un répertoire
			if(!S_ISDIR(cible.st_mode))
			{
				printf("Erreur, le 2e arg n'est pas un répertoire. Fin de prog\n");
				exit(1);
			}
			//On ouvre le repertoire 1
			DIR *rep1= opendir(com[1]);
			//On crée un fichier
			struct dirent * fic;
			int i=0;
			//On compile le fichier de exo3 partie 1 pour pouvoir l'executer plus tard dans le prog
			int id=fork();
			if(id==0)
			{
				execlp("gcc","gcc","exo3Partie1et2.c","-o","exo3Partie1et2",NULL);
				printf("erreur exec gcc\n");
			}
			sleep(1);
			//Pour chaque fichier du repertoire 1 qui ne soit pas . ou ..
			while(fic=readdir(rep1))
			{
				char nouvNom[100], ancNom[100];
				strcpy(nouvNom,com[2]);
				strcpy(ancNom,com[1]);
				strcat(nouvNom,"/");
				strcat(ancNom,"/");
				strcat(nouvNom,fic->d_name);
				strcat(ancNom,fic->d_name);
				struct stat source;
				stat(ancNom,&source);
				if(strcmp(fic->d_name,".")!=0 && strcmp(fic->d_name,"..")!=0)
				{
					if(S_ISDIR(source.st_mode))
					{
						mkdir(nouvNom,source.st_mode);
						char ** tmp=(char **)malloc(sizeof(char*)*2);
						strcpy(tmp[0],ancNom);
						strcpy(tmp[1],nouvNom);
						copie(tmp);
					}
					int idproc=fork();
					if(idproc==0)
					{
						//On crée une variable avec le nom du dossier et du fichier source
						//et une autre pour le fichier cible

						//printf("nouvNom : %s\n",nouvNom);

						//On execute exo3 Partie 1 et 2 qu'on a compilé
						execl("exo3Partie1et2","exo3Partie1et2",ancNom,nouvNom,NULL);
						printf("Erreur exec fils %d\n",i);
						exit(2);
					}
				i++;
				}

			}


		}
		else
		{
			printf("L'argument 1 n'est pas un repertoire ou n'existe pas\n");
			exit(2);
		}
}



int remplirArg(char * ligne,char **com, char *car) //lit une ligne, la divise selon le caractère car et écrit dans com chaque partie de la phrase. Retourne le nombre d'arguments
{

        int i=0;
        int j=0;
        char * pointeur,*phrase;
        if(strcmp(car,">")!=0) // Si car est différent de >
        {
            char *tmp=(char *)malloc(strlen(ligne));
            strcpy(tmp,ligne);
            pointeur=strtok(tmp,car);
        }
        else
        {
            pointeur=strtok(ligne,car);     // Si car vaut >, on modifie la ligne pour tout le programme, sinon on travail sur une copie pour ne pas la modifier dans le programme principal
        }



        //char commande[50][100];
        while(pointeur!=NULL)
        {
            /*if((com=realloc(com,sizeof(char*)*(i+1)))==NULL)
                {
                    printf("erreur realloc, plus assez de place. Fin fu prog\n");
                    exit(2);
                }
            */

            if(strcmp(pointeur,car)!=0)
            {
                com[i]=(char *)malloc(strlen(pointeur));
                if(com[i]==NULL)
                    printf("erreur malloc\n");
                strcpy(com[i],pointeur);
                pointeur=strtok(NULL,car);

                i++;
            }
        }
        //com=realloc(com,sizeof(char*)*(i+1));
        //enlever le \n
        char * tmp=(char*)malloc(strlen(com[i-1]));
        strcpy(tmp,com[i-1]);
        phrase=strtok(tmp,"\n");
        if(phrase!=NULL)
            strcpy(tmp,phrase);
        strcpy(com[i-1],tmp);
        // mettre null dans le dernier argument
        //commande=realloc(commande,sizeof(char*)*(i+1));

        /*printf("i=%d\n",i);
        for(j;j<=i;j++)
            printf("Commane[%d] : %s\n",j,commande[j]);*/
        return i;


}


void execCd(char ** com)
{
    if(com[1]!=NULL && strcmp(com[1],"~")!=0)
    {
        if(chdir(com[1])!=0)
        {
            printf("Repertoire incorrecte\n");
        }
    }
    else
       if(chdir("/~")!=0)
        {
            printf("Repertoire mauvais\n");
        }
}

void history(char **com, int nbArg)
{
    char *tab[100]= {"cat","-n","historique"};
    nouvCat(tab);

    /*if(com[1]==NULL){
    	int i;
    	for(i=0;i<nbCommande;i++){
    		printf(" %d  %s",i,historique[i]);
    	}
    }
    else {
    	int i;
    	printf("%d\n",(int)com[1][0]);
    	for(i=(int)com[0][1];i>0;i--){
    		printf(" %d  %s",nbCommande-i,historique[nbCommande-i]);
    	}
    }*/

}

void nouvExit()
{
    exit(0);
}

// --------------------------------------A finir-----------------------------------------
void nouvTouch(char ** com, int nbArg)
{
    if(com[2]==NULL)
    {
        char * nomFic;
        //strcpy(nomFic,repCour);
        //strcat(nomFic,commande[1]);c
        int fd=open(com[1],O_RDONLY | O_CREAT,0666);
        struct utimbuf ubuf;
        ubuf.modtime = time(NULL);
        ubuf.actime = time(NULL);
        utime(com[1], &ubuf);
        close(fd);
    }
    /*else if(!strcmp(com[3],"-t"))
    {
        if(com[4][0]!='"' || com[4][strlen(com[4])]!='"' || strlen(com[4])!=8)
            printf("format de date invalide");
        else
        {

        }

    }*/
    /*else
    {

    }*/
    libereEspace(com,nbArg);
}





void nouvQuit()
{
    exit(0);
}

//cat : écrit le contenu du fichier dans la console
//cat nomfichier -n : écrit le contenu du fichier dans la console avec le numéro de ligne avant chaque ligne
//cat nomfichier1 -m nomfichier2 : écrit le contenu du fichier2 aprés celui du fichier1
void nouvCat( char  ** com, int nbArg)
{
    //affiche le contenu d'un fichier
    if(com[2]==NULL) /*if(strlen(commande[2])==0)*/	//s'il n'y a pas d'arguments
    {
        int fd=open(com[1],O_RDONLY); //on ouvre le fichier
        if(fd==-1)
            printf("fichier inexistant\n");
        else
        {
            char tmp;   //on lit et on affiche  caractère par caractère
            int nb=read(fd,&tmp,1);
            while(nb!=0)
            {
                write(1,&tmp,1);
                nb=read(fd,&tmp,1);
            }
        }
        close(fd);
    }
    else if(strcmp(com[1],"-n")==0)	//affiche le contenu du fichier avec le numéro de ligne avant
    {
        int fd=open(com[2],O_RDONLY);
        if(fd==-1)
            printf("fichier inexistant\n");
        else
        {
            char tmp;
            int ligne=1;
            int nb=read(fd,&tmp,1);
            //write(1,&ligne,sizeof(int));
            printf("%d\t",ligne);
            ligne++;
            fflush(stdout);
            while(nb!=0)
            {
                if(/*!strcmp(tmp,"\n")*/ tmp=='\n')
                {
                    fflush(stdout);
                    printf("\n");
                    printf("%d\t",ligne);
                    fflush(stdout);
                    ligne++;
                    nb=read(fd,&tmp,1);

                }
                else
                {
                    fflush(stdout);
                    write(1,&tmp,1);
                    nb=read(fd,&tmp,1);
                    fflush(stdout);
                }
            }
            printf("\n");
        }
        close(fd);
    }
    else if(strcmp(com[1],"-m")==0)	//affiche le contenu de deux fichier a la suite
    {
        int fd=open(com[2],O_RDONLY);
        if(fd==-1)
            printf("fichier 1 inexistant\n");
        else
        {
            int fd2=open(com[3],O_RDONLY);
            if(fd2==-1)
            {
                printf("Problème avec le 2e fichier\n");
            }
            else
            {
                char tmp, tmp2;
                int nb=read(fd,&tmp,1);
                while(nb!=0)
                {
                    write(1,&tmp,1);
                    nb=read(fd,&tmp,1);
                }
                fflush(stdout);
                int nb2=read(fd2,&tmp2,1);
                while(nb2!=0)
                {
                    write(1,&tmp2,1);
                    nb2=read(fd2,&tmp2,1);
                }
            }
            close(fd2);
        }
        close(fd);

    }
    //libereEspace(com,nbArg);
}

