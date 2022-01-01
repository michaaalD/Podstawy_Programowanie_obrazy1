#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define MAX 512            /* Maksymalny rozmiar wczytywanego obrazu */
#define DL_LINII 1024      /* Dlugosc buforow pomocniczych */
#define WCZYTANO 1

/************************************************************************************
 * Funkcja wczytuje obraz PGM z pliku do tablicy       	       	       	       	    *
 *										    *
 * \param[in] plik_we uchwyt do pliku z obrazem w formacie PGM			    *
 * \param[out] obraz_pgm tablica, do ktorej zostanie zapisany obraz		    *
 * \param[out] wymx szerokosc obrazka						    *
 * \param[out] wymy wysokosc obrazka						    *
 * \param[out] szarosci liczba odcieni szarosci					    *
 * \return liczba wczytanych pikseli						    *
 ************************************************************************************/

int czytaj(FILE *plik_we,int obraz_pgm[][MAX],int *wymx,int *wymy, int *szarosci) {
  char buf[DL_LINII];      /* bufor pomocniczy do czytania naglowka i komentarzy */
  int znak;                /* zmienna pomocnicza do czytania komentarzy */
  int koniec=0;            /* czy napotkano koniec danych w pliku */
  int i,j;

  /*Sprawdzenie czy podano prawid³owy uchwyt pliku */
  if (plik_we==NULL) {
    fprintf(stderr,"Blad: Nie podano uchwytu do pliku\n");
    return(0);
    }
  /* Sprawdzenie "numeru magicznego" - powinien byæ P2 */
  if (fgets(buf,DL_LINII,plik_we)==NULL)   /* Wczytanie pierwszej linii pliku do bufora */
    koniec=1;                              /* Nie udalo sie? Koniec danych! */

  if ( (buf[0]!='P') || (buf[1]!='2') || koniec) {  /* Czy jest magiczne "P2"? */
    fprintf(stderr,"Blad: To nie jest plik PGM\n");
    return(0);
    }

  /* Pominiecie komentarzy */
  do {
    if ((znak=fgetc(plik_we))=='#') {         /* Czy linia rozpoczyna sie od znaku '#' */
      if (fgets(buf,DL_LINII,plik_we)==NULL)  /* Przeczytaj ja do bufora */

	  koniec=1;                             /* Zapamietaj ewentualny koniec danych */
    }  else {
      ungetc(znak,plik_we);                   /* Gdy przeczytany znak z poczatku linii */
    }                                         /* nie jest '#' zwroc go                 */
  } while (znak=='#' && !koniec);             /* Powtarzaj dopoki sa linie komentarza */
                                              /* i nie nastapil koniec danych         */

  /* Pobranie wymiarow obrazu i liczby odcieni szarosci */
  if (fscanf(plik_we,"%d %d %d",wymx,wymy,szarosci)!=3) {
    fprintf(stderr,"Blad: Brak wymiarow obrazu lub liczby stopni szarosci\n");
    return(0);
  }
  /* Pobranie obrazu i zapisanie w tablicy obraz_pgm*/
  for (i=0;i<*wymy;i++) {
    for (j=0;j<*wymx;j++) {
      if (fscanf(plik_we,"%d",&(obraz_pgm[i][j]))!=1) {
	fprintf(stderr,"Blad: Niewlasciwe wymiary obrazu\n");
	return(0);
      }
    }
  }
  return *wymx**wymy;   /* Czytanie zakonczone sukcesem    */
}                       /* Zwroc liczbe wczytanych pikseli */


void rozmycie_pionowe(int obraz_pgm[][MAX], int wymx, int wymy)
{
  int i,j,rozmycie;

  printf("Jaki promien rozmycia?:\n");
  scanf("%d",&rozmycie);

  if(rozmycie<=0)
	  printf("ERROR - promien rozmycia mniejszy lub rowny zero");
  else
    {
	    for(i=0; i<wymy; i++){
		    for(j=0; j<wymx; j++){
		    obraz_pgm[i][j]=(obraz_pgm[i][j-rozmycie] + obraz_pgm[i][j] + obraz_pgm[i][j + rozmycie])/3;
        }
  	}
  }
}
	

void progowanie(int obraz_pgm[][MAX], int wymx, int wymy,float szarosci, float prog)
{
    int i,j;

    prog = (prog/szarosci) * 100;

    for(i=0; i<wymy; i++){
        for(j=0; j<wymx; j++){
            if(obraz_pgm[i][j] <= prog)
                obraz_pgm[i][j]=0;
            else
                obraz_pgm[i][j]=szarosci;
        }
    }
}

void konturowanie(int obraz_pgm[][MAX], int wymx, int wymy)
  {
      int i,j;
      for(i=0; i<wymy; i++){
	      for(j=0; j<wymx; j++){
	        obraz_pgm[i][j]=abs(obraz_pgm[i+1][j] - obraz_pgm[i][j]) + abs(obraz_pgm[i][j+1] - obraz_pgm[i][j]);
        }
    }
  }

void negatyw(int obraz_pgm[][MAX], int wymx, int wymy, int szarosci)
  {
      int i,j;
      
      for(i=0; i<wymy; i++){
	        for(j=0; j<wymx; j++){
	          obraz_pgm[i][j]=szarosci-obraz_pgm[i][j];
	          }
	      }
  }
	  
int zapisz(int obraz_pgm[][MAX], int wymx, int wymy, int szarosci,char *zapis_nazwa)
{
  char polecenie[DL_LINII];
  int i,j;
  FILE *zapis_plik;
  
  strcpy(polecenie,"touch ");
  strcat(polecenie,zapis_nazwa);
  strcat(polecenie, " &");
  system(polecenie);

  zapis_plik=fopen(zapis_nazwa, "w");

  fprintf(zapis_plik, "P2\n");
  fprintf(zapis_plik , "%d %d\n",wymx,wymy);
  fprintf(zapis_plik , "%d\n", szarosci);

  for(i=0; i<wymy; i++){
    fprintf(zapis_plik, "\n");
      for(j=0; j<wymx; j++){
         fprintf(zapis_plik,"%d ", obraz_pgm[i][j]);
	}
    }
  fclose(zapis_plik);
  return 0;
  
}

/* Wyswietlenie obrazu o zadanej nazwie za pomoca programu "display"   */

 void wyswietl(char *zapis_nazwa) {
  char polecenie[DL_LINII];      /* bufor pomocniczy do zestawienia polecenia */

  strcpy(polecenie,"display ");  /* konstrukcja polecenia postaci */
  strcat(polecenie,zapis_nazwa);     /* display "nazwa_pliku" &       */
  strcat(polecenie," &");
  printf("%s\n",polecenie);      /* wydruk kontrolny polecenia */
  system(polecenie);             /* wykonanie polecenia        */
}

int main() 
{
  int wczytano_plik=0;
  int wybor;
  int obraz_pgm[MAX][MAX] ;
  int wymx,wymy,szarosci;
  int odczytano = 0;
  int prog;

  char zapis_nazwa[80];
  FILE *zapis_plik;
 
  FILE *plik;
  char nazwa_pliku[50];

  while(wybor!=8)
 {
  printf("Menu\n");
  printf("1.\tWczytaj\n");
  printf("2.\tZapisz\n");
  printf("3.\tWyswietl\n");
  printf("4.\tNegatyw\n");
  printf("5.\tProgowanie\n");
  printf("6.\tKonturowanie\n");
  printf("7.\tRozmycie Pionowe\n");
  printf("8.\tKoniec Programu\n");
  
  


  scanf("%d",&wybor);
  
  switch(wybor)
  {
    case 1:
        {
	    /* Wczytanie zawartosci wskazanego pliku do pamieci */
	    printf("Podaj nazwe pliku:\n");
  	    scanf("%s",nazwa_pliku);
  	    plik=fopen(nazwa_pliku,"r");

	    if (plik != NULL) {       
	     odczytano = czytaj(plik,obraz_pgm,&wymx,&wymy,&szarosci);
	     fclose(plik);
       wczytano_plik=WCZYTANO; /*umozliwia wlaczenie nastepnych funkcji*/
       printf("Wczytano plik\n");
	      }
	    else
		      printf("Nie wcztano pliku o nazwie %s\n",nazwa_pliku);
  	  break;
         }
    case 2:
      {
      if(wczytano_plik==WCZYTANO){
	      printf("Nazwa pliku do zapisu:\n");
	      scanf("%s",zapis_nazwa);
	      zapis_plik=fopen(zapis_nazwa,"w");
      	if(zapis_plik!=NULL){
	  zapisz(obraz_pgm, wymx, wymy, szarosci, zapis_nazwa);
        	printf("Plik zapisany\n");
      		}
	fclose(zapis_plik);
      }
      else
        printf("Nie wczytano obrazu\n");
	    break;
      }
    case 3:
      {
        if(wczytano_plik==WCZYTANO){
        	plik=fopen(zapis_nazwa,"r");
        	if(odczytano!=0)
        	  wyswietl(zapis_nazwa);
		        fclose(plik);
		 return odczytano;
	      }
	      else
        	printf("Nie wczytano obrazu\n");
      break;
      }
    case 4:
      {	
	    if(wczytano_plik==WCZYTANO){
	    	negatyw(obraz_pgm,wymx,wymy,szarosci);
	    	printf("Wykonano negatyw obrazka\n");
	    }
	    else
        	printf("Nie wczytano obrazu\n");
	    break;
      }


   case 5:
      {
         if(wczytano_plik==WCZYTANO){
              printf("Podaj wartosc progu:\n");
              scanf("%d",&prog);

                  progowanie(obraz_pgm,wymx,wymy,szarosci, prog);

       }
        else
          printf("Nie wczytano obrazu\n");
      }
    case 6:
      {
	    if(wczytano_plik==WCZYTANO){
	    	konturowanie(obraz_pgm,wymx,wymy);
	    	printf("Wykonano konturowanie obrazka\n");
	    }
	    else
        	printf("Nie wczytano obrazu\n");
	    break;
      }
    case 7:
      {
	    if(wczytano_plik==WCZYTANO){
		    rozmycie_pionowe(obraz_pgm,wymx,wymy);
	      }
	    else
        	printf("Nie wczytano obrazu\n");
	    break;
      }

    default:
      if(wybor!=8)
	printf("Niepoprawny wybor opcji\n");
      break;
    
}
}
printf("Koniec Programu\n");
fclose(plik);
return 0;
  
}

/*
Sprawozdanie

Michal Dos
263498
Temat: Przetwarzanie obrazow
17.12.2021r.

TESTY:

Testy programu zaczalem od sprawdzenia czy menu programu porawnie reaguje na poszczegolne argumenty.
Sprawdzilem czy program poprawnie czyta opcje dla :
1)liczb od 1 do 8
2)liczb ujemnych np. -5
3)liter np. a
4)liczb zmienno przecinkowych np. 3.5
5)liczb spoza zakresu od 1 do 8 np. 110

Program poprawnie wykrywal liczby nie pasujace do zakresy 1-8. Dla liter i liczb zmiennoprzecinkowych  programw zapetleniu wyswietla menu programu.


Nastepneie testowalem poszczegolne opcje programu np. dla pliku "kubus.pgm", przez:
- wybranie pliku 1)
- nastepnie wybranie opcji edycji 4),5),6),7)
-zapis nowego pliku zedytowanego 2)
-wyswietlenie zedytowanego pliku 3)
-zakonczenie dzialania programu 8)

WNIOSKI:

Program wydaje sie poprwanie przetwarzac pliki pgm.

*/ 


