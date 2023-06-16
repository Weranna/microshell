#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#define BUFFOR 1024
#define CZ "\033[1;31m"
#define Z "\033[1;32m"
#define ZO "\033[0;33m"
#define N "\033[1;34m"
#define B "\033[01;37m"
char katalog[BUFFOR];
int arg;
void znak()
{
    if(!(getcwd(katalog,sizeof(katalog))))
    {
        printf(CZ "Error number: %d\n",errno);
        printf(B "Nie udalo sie znalezc aktualnego katalogu roboczego\n");
    }
    char *login;
    login=getenv("USER");
    printf(B "[" Z "%s" B "][" N "%s" B "] $ ",login,katalog);
}

void help()
{
    printf(ZO "\nAUTOR:" B " Weronika Klimczak_s473564\n\n");
    printf( N "Dostepne funkcje: \n\n");
    printf( B "> " ZO "help" B "- wyswietlenie informacji o autorze oraz dostepnych funkcjach\n");
    printf(B "> " ZO "cd" B "- zmiana biezacego katalogu\n- " Z "[~][] " B "przejscie do katalogu domowego\n- " Z "[-]" B " powrocenie do poprzedniego katalogu\n- " Z "[.] " B "biezacy katalog\n");
    printf(B "> " ZO "exit" B "- zakonczenie dzialania programu\n");
    printf(B "> " ZO "mkdir" B "- utworzenie nowego katalogu w aktualnym katalogu roboczym\n- " Z "[-p] " B "utworzenie katalogu wraz z podkatalogami\n");
    printf(B "> " ZO "rm" B "- usuniecie pliku w biezacym katalogu\n- " Z "[-i] " B "bezpieczne usuniecie pliku\n- " Z "[-r] " B "usuniecie katalogu z zawartoscia\n\n");
}

void rozdzielenie(char *linia,char **polecenie)
{
    char *spacja;
    arg=0;
    spacja=strtok(linia," ");
    int i=0;
        while(spacja!=NULL)
        {
            polecenie[i]=spacja;
            spacja=strtok(NULL," ");
            i++;
            arg++;
        }
}

void cd(char **polecenie, char *prev)
{
    const char *homedir;
    homedir=getenv("HOME");
    if(strcmp(polecenie[1],"-")==0)
    {
        if(chdir(prev)==-1)
        {
            printf(CZ "Error number: %d\n",errno);
            printf(B "Nie udalo sie przejsc do katalogu\n");
        }
    }
    else
    {
        if(strcmp(polecenie[1],"~")==0)
        {
            if(chdir(homedir)==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie przejsc do katalogu\n");
            }
        }
        else
        {
            if(strcmp(polecenie[1],".")==0)
            {
                putchar('\n');
            }
            else
            {
               if(chdir(polecenie[1])==-1)
               {
                   printf(CZ "Error number: %d\n",errno);
                   printf(B "Nie udalo sie przejsc do katalogu\n");
               }
            }
        }
    }
}

void mk_dir(char **polecenie)
{
    char *katalogi[BUFFOR];
    char nazwa[BUFFOR];
    int dir=0;
    if(strcmp(polecenie[1],"-p")==0)
    {
        if(arg<3)
        {
            printf(CZ "Za mala liczba argumentow\n");
            return;
        }
        char *slash;
        slash=strtok(polecenie[2],"/");
        int i=0;
        while(slash!=NULL)
        {
            katalogi[i]=slash;
            slash=strtok(NULL,"/");
            i++;
            dir++;
        }
        int j;
        for(j=0;j<dir;j++)
        {
            if(mkdir(katalogi[j],0777)==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie utworzyc katalogu\n");
                return;
            }
            if(chdir(katalogi[j])==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie utworzyc katalogu\n");
                return;
            }
        }
        chdir(katalog);
    }
    else
    {
        if(arg>2)
        {
            strcpy(nazwa,polecenie[1]);
            strcat(nazwa," ");
            strcat(nazwa,polecenie[2]);
            if(mkdir(nazwa,0777)==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie utworzyc katalogu\n");
            }
        }
        else
        {
            if(mkdir(polecenie[1],0777)==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie utworzyc katalogu\n");
            }
        }
    }
}

void rm(char **polecenie)
{
    if(strcmp(polecenie[1],"-i")==0)
    {
        char wejscie[BUFFOR];
        char *yorn;
        if(arg<3)
        {
            printf(CZ "Za mala liczba argumentow\n");
            return;
        }
        printf(ZO "UWAGA: Plik zostanie usuniety bezpowrotnie, czy chcesz kontynuowac? (yes/no)\n" B);
        yorn=fgets(wejscie,BUFFOR,stdin);
        int len=strlen(wejscie);
        wejscie[len-1]='\0';
        if(strcmp(yorn,"no")==0)
        {
            return;
        }
        else
        {
            if(strcmp(yorn,"yes")==0)
            {
                if(remove(polecenie[2])==-1)
                {
                    printf(CZ "Error number: %d\n",errno);
                    printf(B "Nie udalo sie usunac pliku\n");
                }
            }
            else
            {
                printf(CZ "Podana zla odpowiedz\n");
            }
        }
    }
    else
    {
        if(strcmp(polecenie[1],"-r")==0)
        {
            if(arg<3)
            {
                printf(CZ "Za mala liczba argumentow\n");
                return;
            }
            if(chdir(polecenie[2])==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie usunac katalogu\n");
                return;
            }
            struct dirent *file;
            DIR *kat;
            if((kat=opendir("."))==NULL)
               {
                   printf(CZ "Error number: %d\n",errno);
                   printf(B "Nie udalo sie usunac katalogu\n");
                   return;
               }
            char *nazwa;
            while((file=readdir(kat))!=NULL)
            {
                nazwa=file->d_name;
                remove(nazwa);
            }
            if(closedir(kat)==-1)
                {
                    printf(CZ "Error number: %d\n",errno);
                    printf(B "Nie udalo sie usunac katalogu\n");
                    return;
                }
            chdir(katalog);
            if(rmdir(polecenie[2])==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie usunac katalogu\n");
                return;
            }
        }
        else
        {
            if(remove(polecenie[1])==-1)
            {
                printf(CZ "Error number: %d\n",errno);
                printf(B "Nie udalo sie usunac pliku\n");
            }
        }
    }
}
int main(int argc, char *argv[])
{
    char prevkat[BUFFOR];
    strcpy(prevkat,katalog);
    while(1)
    {
        char *linia;
        char *polecenie[BUFFOR];
        char tmp[BUFFOR];
        char wejscie[BUFFOR];
        znak();
        linia=fgets(wejscie,BUFFOR,stdin);
        int len=strlen(wejscie);
        wejscie[len-1]='\0';
        rozdzielenie(linia,polecenie);
        if(strcmp(polecenie[0],"help")==0)
        {
            help();
        }
        else
        {
            if(strcmp(polecenie[0],"exit")==0)
            {
                exit(0);
            }
            else
            {
                if(strcmp(polecenie[0],"cd")==0)
                {
                    if(arg==1)
                    {
                        polecenie[1]="~";
                        arg++;
                    }
                    strcpy(tmp,katalog);
                    cd(polecenie,prevkat);
                    strcpy(prevkat,tmp);
                }
                else
                {
                    if(strcmp(polecenie[0],"mkdir")==0)
                    {
                        if(arg<2)
                        {
                            printf(CZ "Za mala liczba argumentow\n");
                        }
                        else
                        {
                            mk_dir(polecenie);
                        }
                    }
                    else
                    {
                        if(strcmp(polecenie[0],"rm")==0)
                        {
                            if(arg<2)
                            {
                                printf(CZ "Za mala liczba argumentow\n");
                            }
                            else
                            {
                                rm(polecenie);
                            }
                        }
                        else
                        {
                            pid_t id=fork();
                            if(id==0)
                            {
                                execvp(polecenie[0],polecenie);
                                printf(CZ "Error number: %d\n",errno);
                                printf(B "Nie znalezieono polecenia\n");
                                exit(0);
                            }
                            else
                            {
                                wait(NULL);
                            }
                        }
                    }
                }
            }
        }
        int x;
        for(x=0;x<arg;x++)
        {
            polecenie[x]=NULL;
        }
    }
}
