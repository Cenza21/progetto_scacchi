#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIM_SCACCHIERA 8

typedef enum{FALSE,TRUE}boolean;
typedef enum{NERO,BIANCO}colore;
typedef enum{PEDONE,TORRE,CAVALLO,ALFIERE,REGINA,RE,VUOTO}tipo;
typedef enum{
PEDINA_NON_IN_CAMPO,
PEDINA_INESISTENTE,
COLORE_ERRATO,
ARRIVO_NON_IN_CAMPO,
RISPETTA_REGOLE,
NON_RISPETTA_REGOLE,
BLOCCATA_STESSO_COLORE,
BLOCCATA_ALTRO_COLORE,
MOSSA_EFFETTUABILE,
SCACCO_NERO,
SCACCO_BIANCO,
SCACCO_MATTO,
PEDONE_DIVENTA_REGINA,
MOSSA_NULLA
}stati_mossa;

/*
Per creare il gioco degli scacchi è necessario ottenere questi componenti:

 -Scacchiera
 -Singola pedina
 -Singola mossa
 -Giudice
 -Esecutore
 -Controllore
*/
typedef struct{

    tipo nome;
    colore col;
    boolean mossa;

}pedina;
typedef struct{

    pedina* campo[DIM_SCACCHIERA][DIM_SCACCHIERA];

}scacchiera;
typedef struct{

    int pos_inizio_x;
    int pos_inizio_y;
    int pos_arrivo_x;
    int pos_arrivo_y;
    colore giocatore;

}mossa_corrente;
typedef struct{

    pedina* info;
    int pos_x;
    int pos_y;

}re;

pedina* creaPedina(tipo nome, colore c);

scacchiera* creaScacchiera();

mossa_corrente* creaMossaCorrente(colore col);

stati_mossa giudice              (scacchiera* scacchi, mossa_corrente* mossa, colore giocatore_corrente);
stati_mossa rispettaRegolePedone (scacchiera* scacchi, pedina* pedina_corrente,  mossa_corrente* mossa, int distanza, int x, int y);
stati_mossa rispettaRegoleTorre  (pedina* pedina_corrente, int distanza, int x, int y);
stati_mossa rispettaRegoleCavallo(pedina* pedina_corrente, int distanza, int x, int y);
stati_mossa rispettaRegoleAlfiere(pedina* pedina_corrente, int distanza, int x, int y);
stati_mossa rispettaRegoleRegina (pedina* pedina_corrente, int distanza, int x, int y);
stati_mossa rispettaRegoleRe     (pedina* pedina_corrente, int distanza, int x, int y);
stati_mossa muoviPedinaVerso     (scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int* x_s, int* y_s);
stati_mossa muoviPedoneVerso     (scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int x_spostamento, int y_spostamento);
stati_mossa muoviCavalloVerso    (scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int x_spostamento, int y_spostamento);
int norma_quadrato(int x, int y);

stati_mossa esecutore(scacchiera* scacchi, mossa_corrente* mossa, stati_mossa stato, re* r);
scacchiera* invertiPedine(scacchiera* scacchi, int x1, int y1, int x2, int y2);
scacchiera*        mangia(scacchiera* scacchi, int x1, int y1, int x2, int y2);

re* aggiornaRE(re* r, int x, int y);
re* creaRE(pedina* p, int x, int y);
stati_mossa controllore(scacchiera* scacchi, re* re_nero, re* re_bianco);

void stampaScacchiera(scacchiera* scacchi);

/*

SINGLOLA PEDINA:

Ricordiamo che tutte le possibili pedine sono:

 -Pedone
 -Cavallo
 -Torre
 -Alfiere
 -Re
 -Regina
 -NULL(se non c'è nessuna pedina in quella posizione)

    Ogni pedina conterrà il nome, il colore che identificheranno univocamente la pedina per le mosse che può fare.
    La posizione corrente sarà memorizzata direttamente all'interno della scacchiera, creando così un accesso diretto
    più veloce alla singola pedina.
    Inoltre conterrà pure una variabile booleana che le permetterà di capire se è già stata mossa oppure no.
    Può risultare utile per i pedoni, che possono avanzare di due caselle come prima mossa, oppure per il re e la torre
    ed effettuare il "castle".

*/

pedina* creaPedina(tipo nome, colore c){

    pedina* p=NULL;

    p=(pedina*)malloc(sizeof(pedina));

    if(p==NULL){
        puts("Errore nella creazione di una pedina :(");
        return NULL;
    }

    p->nome=nome;
    p->col=c;
    p->mossa=FALSE;

    return p;
}

/*

SCACCHIERA:

    E' una matrice 8x8 dove in ogni posizione sarà presente un puntatore
    alla pedina che occupa la cella nella determinata posizione

*/

/*

La funzione creaScacchiera crea un nuovo campo di gioco e lo sistema per iniziare il gioco.

    Ipotizzando che il bianco sia rivolto verso chi gioca, prima verranno create tutte le pedine nere
    e successivamente tutte quelle bianche a specchio, tranne il re e la regina

*/
scacchiera* creaScacchiera(){

    scacchiera* scacchi=(scacchiera*)malloc(sizeof(scacchiera));

    if(scacchi==NULL){
        puts("Errore nella creazione della scacchiera :(");
        return NULL;
    }

    int x=0;
    int y=0;

    scacchi->campo[0][0] = creaPedina(TORRE   ,NERO);
    scacchi->campo[0][1] = creaPedina(CAVALLO ,NERO);
    scacchi->campo[0][2] = creaPedina(ALFIERE ,NERO);
    scacchi->campo[0][3] = creaPedina(REGINA  ,NERO);
    scacchi->campo[0][4] = creaPedina(RE      ,NERO);
    scacchi->campo[0][5] = creaPedina(ALFIERE ,NERO);
    scacchi->campo[0][6] = creaPedina(CAVALLO ,NERO);
    scacchi->campo[0][7] = creaPedina(TORRE   ,NERO);

    for(x=0,y=1;x<DIM_SCACCHIERA;x++){
        scacchi->campo[y][x] = creaPedina(PEDONE,NERO);
    }

    for(y=2;y<6;y++){
        for(x=0;x<DIM_SCACCHIERA;x++){
            scacchi->campo[y][x] = creaPedina(VUOTO,NERO);
        }
    }

    for(x=0;x<DIM_SCACCHIERA;x++){
        scacchi->campo[y][x] = creaPedina(PEDONE,BIANCO);
    }

    scacchi->campo[DIM_SCACCHIERA-1][0] = creaPedina(TORRE   ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][1] = creaPedina(CAVALLO ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][2] = creaPedina(ALFIERE ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][3] = creaPedina(RE      ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][4] = creaPedina(REGINA  ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][5] = creaPedina(ALFIERE ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][6] = creaPedina(CAVALLO ,BIANCO);
    scacchi->campo[DIM_SCACCHIERA-1][7] = creaPedina(TORRE   ,BIANCO);
    return scacchi;
}

/*

SINGOLA MOSSA:

La singola mossa è difficile da pensare in termini informatici, perchè l'idea è quella di chiedere all'utente
una pedina e la posizione dove la vuole muovere.
Di conseguenza sarà anche poi facile da implementare una lista che tiene traccia di tutte le mosse fatte.

Entrambe saranno rappresentate da una coppia (lettera, numero) RIGOROSAMENTE in questo ordine, la quale verrà presa dal cervello
e analizzata per decidere se essa è valida oppure no.

Sarà anche salvato il colore del giocatore che ha effettuato la mossa.

NB: l'utente inserirà la mossa grazie alla coppia lettera-numero, ma il programma la salverà come int-int per comodità

*/

/*
Per trasformare la mossa in un indice della matrice (es: A1 -> (0,7))

ho bisogno di prendere la prima lettera e trasformarla in indice da 0 a 7 secondo questo procedimento:

indice=(int)lettera-num;
indice+=(8-indice);]->

num=65 per le x e = 49 per le y;
sono i valori in codice ascii di 'A' ed '1' rispettivamente

NB: nel nostro sistema di riferimento le y vanno invertite => y=(8-y)
*/
mossa_corrente* creaMossaCorrente(colore col){

    int ini_x,ini_y,arr_x,arr_y;

    printf("Inserisci la pedina che vuoi muovere nell'ordine lettera maiuscola-numero \ncome negli scacchi originali (es. A1=torre bianca): ");
    ini_y=getchar()-65;
    ini_x=getchar()-49;
    ini_x=DIM_SCACCHIERA-ini_x-1;
    getchar();
    printf("Inserisci la posizione dove vuoi arrivare: ");
    arr_y=getchar()-65;
    arr_x=getchar()-49;
    arr_x=DIM_SCACCHIERA-arr_x-1;
    getchar();

    mossa_corrente* mossa=(mossa_corrente*)malloc(sizeof(mossa_corrente));

    if(mossa==NULL){
        puts("Errore nella costruzione della mossa corrente :(");
        return NULL;
    }

    mossa->giocatore=col;
    mossa->pos_arrivo_x=arr_x;
    mossa->pos_arrivo_y=arr_y;
    mossa->pos_inizio_x=ini_x;
    mossa->pos_inizio_y=ini_y;

    return mossa;
}

/*

GIUDICE:

Il compito del giudice è quello di prendere la mossa corrente e decidere se essa è lecita oppure no.
Esso è composto da un conglomerato di funzioni che comunicano tra di loro.

In una versione migliorata esso cercherà di capire quando sarà check, e dunque vieterà le mosse che mettono a rischio il re.

Il processo logico per decidere se la mossa è lecita è il seguente:

NB: ad ogni numero corrisponderà il relativo codice di errrore/successo

-0: si controlla che la pedina selezionata stia effettivamente nel campo di gioco
|
v
-1: si controlla che nella posizione ci sia effettivamente una pedina oppure VUOTO
|
v
-2: si controlla che il colore della pedina selezionata corrisponda al colore del giocatore
|
v
-3: si controlla che la posizione di arrivo cada effettivamente nel campo di gioco
|
v

Viene selezionata la funzione da chiamare in base al tipo di pedina, ovvero
il singolo sottoprocesso decide se la mossa sia lecita:

-4: la mossa rispetta le regole di movimento della singola pedina
|
v
-5: la pedina non sia bloccata da un'altra, nel caso è lo stesso colore non è lecita
|
v
-6: la pedina non sia bloccata da un'altra, nel caso non sia lo stesso colore la pedina mangia la pedina
|
v

Se la pedina è il re si aprono due strade possibili:

-6a: la mossa non è direttamente sul re ma nel controllare tutti gli stati del gioco la pedina scontra il re avversario è SCACCO
-6b: la mossa è direttamente sul re è SCACCO MATTO

|
v
-7: se non è bloccata da nessuna pedina, si muove in quella posizione con successo

*/

int norma_quadrato(int x, int y){return x*x + y*y;}

stati_mossa giudice(scacchiera* scacchi, mossa_corrente* mossa, colore giocatore_corrente){

    stati_mossa stato;

    //Caso 0
    if((mossa->pos_inizio_x<0||mossa->pos_inizio_x>DIM_SCACCHIERA-1) ||
       (mossa->pos_inizio_y<0||mossa->pos_inizio_y>DIM_SCACCHIERA-1)  )return PEDINA_NON_IN_CAMPO;
    pedina* pedina_corrente=scacchi->campo[mossa->pos_inizio_x][mossa->pos_inizio_y];
    //Caso 1
    if(pedina_corrente->nome==VUOTO) return PEDINA_INESISTENTE;
    //Caso 2
    if(pedina_corrente->col!=giocatore_corrente) return COLORE_ERRATO;
    //Caso 3
    if((mossa->pos_arrivo_x<0||mossa->pos_arrivo_x>DIM_SCACCHIERA-1) ||
       (mossa->pos_arrivo_y<0||mossa->pos_arrivo_y>DIM_SCACCHIERA-1)  )return ARRIVO_NON_IN_CAMPO;
    //Caso 4
    //NB Vanno invertite perchè il sistema di riferimento è invertito
    int x=mossa->pos_arrivo_x-mossa->pos_inizio_x;
    int y=mossa->pos_arrivo_y-mossa->pos_inizio_y;
    int distanza=norma_quadrato(x,y);
    if(x==0 && y==0) return MOSSA_NULLA;
    switch(pedina_corrente->nome){

    case PEDONE:
        stato=rispettaRegolePedone(scacchi, pedina_corrente, mossa, distanza, y, x);
        break;
    case TORRE:
        stato=rispettaRegoleTorre(pedina_corrente, distanza, y, x);
        break;
    case CAVALLO:
        stato=rispettaRegoleCavallo(pedina_corrente, distanza, y, x);
        break;
    case ALFIERE:
        stato=rispettaRegoleAlfiere(pedina_corrente, distanza, y, x);
        break;
    case REGINA:
        stato=rispettaRegoleRegina(pedina_corrente, distanza, y, x);
        break;
    case RE:
        stato=rispettaRegoleRe(pedina_corrente, distanza, y, x);
        break;
    default:
        //Impossibile che si arrivi qui
        break;
    }
    //In questo momento nello stato sarà contenuto o RISPETTA_REGOLE, oppure NON_RISPETTA_REGOLE.
    if(stato==RISPETTA_REGOLE){
        stato=muoviPedinaVerso(scacchi, pedina_corrente, mossa->pos_inizio_y, mossa->pos_inizio_x, &y, &x);
    }

    return stato;
}
/*
L'obbiettivo della pedina è quello di raggiungere una determinata casella, dunque
la domanda che bisogna farsi è la seguente:

tra tutte le mosse possibili, fermandosi ogni volta al primo ostacolo che incontra,
è presente la casella alla quale mi devo muovere?

Inoltre si può introdurre il concetto di "distanza" tra caselle, che aiuta nel chiedersi
se è possibile raggiungere una determinata casella e muoversi rispetto ad essa per vedere se ci sono ostacòli

La distanza sarà rappresentata da un vettore definito da:

distanza = posizione_inizio-posizione_arrivo; dove distanza = (x,y);

Ci possono essere diversi casi:

- x=0: vuol dire che lo spostamento sarà o avanti o indietro, in particolare:
        - y>0: spostamento verso il basso
        - y<0: spostamento verso l'alto

- y=0: vuol dire che lo spostamento sarà o a destra o a sinistra, in particolare:
        - x>0: spostamento verso destra
        - y<0: spostamento verso sinistra

- x=0,y=0 : non si muove

- |x|=|y| !=0 : lo spostamento è in diagonale

*/

/*
caso particolare del pedone:

- ||distanza||^2 ==1 e x=0: il pedone si può muovere o avanti o indietro a seconda del segno della y (<0 pedone bianco si può muovere)
- ||distanza||^2 ==2 e |x|=|y| => il pedone sta cercando di mangiare => y>0: il pedone bianco può mangiare se c'è una pedina nera,
                                                                             la direzione dipenderà da x
- ||distanza||^2 ==4 e x=0 => il pedone vuole muoversi di due caselle in avanti
*/
stati_mossa rispettaRegolePedone(scacchiera* scacchi, pedina* pedina_corrente, mossa_corrente* mossa, int distanza, int x, int y){

    if(distanza==1 && x==0){
        if(pedina_corrente->col==BIANCO && y<0) return RISPETTA_REGOLE;
        if(pedina_corrente->col==NERO   && y>0) return RISPETTA_REGOLE;
    }
    else if(distanza==2){
        if((pedina_corrente->col==BIANCO && y<0) || (pedina_corrente->col==NERO   && y>0)){
            if(scacchi->campo[mossa->pos_inizio_y+x][mossa->pos_inizio_x+y]->nome!=VUOTO
               && scacchi->campo[mossa->pos_inizio_y+x][mossa->pos_inizio_x+y]->col!=pedina_corrente->col){
                return RISPETTA_REGOLE;
            }
        }
    }
    else if(distanza==4){
        if(x==0){
            if(!pedina_corrente->mossa) return RISPETTA_REGOLE;
        }
    }

    return NON_RISPETTA_REGOLE;
}

stati_mossa rispettaRegoleTorre(pedina* pedina_corrente, int distanza, int x, int y){

    if(x==0 || y==0) return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}
/*
caso particolare del cavallo:

il cavallo si può muovere ad L, e analizzando tutti i possibili casi la conclusione è questa:

- ||distanza||^2 ==5 : la norma del vettore spostamento è uguale a 5 => la mossa del cavallo è lecita,
                       dunque mi basta controllare tutti gli otto punti raggiungibili dal cavallo e controllare quale è quello cercato
*/
stati_mossa rispettaRegoleCavallo(pedina* pedina_corrente, int distanza, int x, int y){

    if(distanza==5) return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}

stati_mossa rispettaRegoleAlfiere(pedina* pedina_corrente, int distanza, int x, int y){

    if(abs(x)==abs(y)) return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}

stati_mossa rispettaRegoleRegina(pedina* pedina_corrente, int distanza, int x, int y){

    if(abs(x)==abs(y)) return RISPETTA_REGOLE;
    if(x==0 || y==0)   return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}
/*
caso particolare del re:

- ||distanza||^2 ==1 oppure ||distanza||^2 ==2 : il re può muoversi solo se la norma della distanza al quadrato è uno o due
*/
stati_mossa rispettaRegoleRe(pedina* pedina_corrente, int distanza, int x, int y){

    if(distanza==1 || distanza==2) return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}
/*
L'obbiettivo della funzione muoviPedinaVerso è quello di prendere la pedina corrente e muoversi fino alla casella richiesta,
per vedere se incontra o no ostacoli nel suo percorso.

Il movimento è dato dal movimento lungo le componenti orizzontali e verticali del vettore distanza calcolato e verificato in precedenza.

Ipotizzando che io possa muovere qualsiasi pedina, l'algoritmo deve essere di conseguenza il più generale possibile.

Tutti gli spostamenti possibili sono: orizzontale,verticale, diagonale ed a L.

Invece di identificare ogni singolo spostamento e creare un'algoritmo diverso per ogni spostamento,
vorrei cercare di crearne uno il più generale possibile che mi permetta di muovermi secondo le coordinate dello spostamento.

L'idea sarebbe questa:

stepX=0;
stepY=0;

verso_x=sgn(x_spostamento);
verso_y=sgn(y_spostamento);

do{
    //eseguo uno step in entrambe le direzioni
    stepX++;
    stepY++;
    //controllo se in quella casella c'è un ostacolo e se c'è il rispettivo colore
    if(vero) trovato==TRUE, colore =colore_pedina;
    //decremento entrambe le variabili x_spostamento e y_spostamento
}while(!trovato e non(stepX==x_spostamento e stepY==y_spostamento);
*/
stati_mossa muoviPedinaVerso(scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int* x_s, int* y_s){

    int x_spostamento=*x_s;
    int y_spostamento=*y_s;

    if(pedina_corrente->nome==PEDONE){
        return muoviPedoneVerso(scacchi,pedina_corrente,x_inizio,y_inizio,x_spostamento,y_spostamento);
    }
    if(pedina_corrente->nome==CAVALLO){
        return muoviCavalloVerso(scacchi,pedina_corrente,x_inizio,y_inizio,x_spostamento,y_spostamento);
    }

    boolean ostacolo_trovato=FALSE;
    int step_x=0;
    int step_y=0;
    int verso_x;
    if(x_spostamento>0)      verso_x=1;
    else if(x_spostamento<0) verso_x=-1;
    else verso_x=0;
    int verso_y;
    if(y_spostamento>0)      verso_y=1;
    else if(y_spostamento<0) verso_y=-1;
    else verso_y=0;

    do{
        step_x+=verso_x;
        step_y+=verso_y;
        if(scacchi->campo[y_inizio+step_y][x_inizio+step_x]->nome!=VUOTO){
            ostacolo_trovato=TRUE;
        }
    }while(ostacolo_trovato==FALSE && !(step_x*verso_x==x_spostamento*verso_x && step_y*verso_y==y_spostamento*verso_y));

    if(ostacolo_trovato==TRUE){
        if(pedina_corrente->col==scacchi->campo[y_inizio+step_y][x_inizio+step_x]->col) return BLOCCATA_STESSO_COLORE;
        else if(pedina_corrente->col!=scacchi->campo[y_inizio+step_y][x_inizio+step_x]->col){
            if(scacchi->campo[y_inizio+step_y][x_inizio+step_x]->nome==RE) return SCACCO_MATTO;
            else return BLOCCATA_ALTRO_COLORE;
        }
    }
    *x_s=step_x;
    *y_s=step_y;
    return MOSSA_EFFETTUABILE;
}
/*
L'unico caso critico dello spostamento è il pedone, che se si muove in diagonale deve mangiare.

Dunque bisogna controllare se il pedone si muove in avanti e trova un ostacolo, oppure se si muove in diagonale
ed effettivamente in quella direzione può mangiare oppure no.
*/
stati_mossa muoviPedoneVerso(scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int x_spostamento, int y_spostamento){
    if(x_spostamento==0){
        if(scacchi->campo[y_inizio+y_spostamento][x_inizio]->nome==VUOTO) return MOSSA_EFFETTUABILE;
    }
    else{
        if(scacchi->campo[y_inizio+y_spostamento][x_inizio+x_spostamento]->col!=pedina_corrente->col){
            if(scacchi->campo[y_inizio+y_spostamento][x_inizio+x_spostamento]->nome==RE) return SCACCO_MATTO;
            else return BLOCCATA_ALTRO_COLORE;
        }
    }
    return BLOCCATA_STESSO_COLORE;
}
/*
Il cavallo è semplice da controllare perchè basta vedere se in quella posizione c'è o no una pedina e il colore della stessa
*/
stati_mossa muoviCavalloVerso(scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int x_spostamento, int y_spostamento){

    if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->nome==VUOTO) return MOSSA_EFFETTUABILE;

    if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->col!=pedina_corrente->col){
        if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->nome==RE) return SCACCO_MATTO;
        else return BLOCCATA_ALTRO_COLORE;
    }

    return BLOCCATA_STESSO_COLORE;
}
/*

ESECUTORE:

Il compito dell'esecutore è quello di prendere lo STATO ricevuto dal GIUDICE e di eseguire la corrispondente azione

*/

/*
Gli stati possibili con il quale si può uscire dal giudice sono:

PEDINA_NON_IN_CAMPO    -> non si muove
PEDINA_INESISTENTE     -> non si muove
COLORE_ERRATO          -> non si muove
ARRIVO_NON_IN_CAMPO    -> non si muove
MOSSA_NULLA            -> non si muove
NON_RISPETTA_REGOLE    -> non si muove
BLOCCATA_ALTRO_COLORE  -> la pedina mangia l'altra
BLOCCATA_STESSO_COLORE -> non si muove
MOSSA_EFFETTUABILE     -> invertire la pedina con il posto VUOTO
SCACCO_MATTO           -> finisce la partita
*/

scacchiera* invertiPedine(scacchiera* scacchi, int x1, int y1, int x2, int y2){
    pedina* temp=scacchi->campo[x1][y1];
    scacchi->campo[x1][y1]=scacchi->campo[x2][y2];
    scacchi->campo[x2][y2]=temp;
    return scacchi;
}

scacchiera* mangia(scacchiera* scacchi, int x1, int y1, int x2, int y2){
    scacchi->campo[x2][y2]->nome=VUOTO;
    return invertiPedine(scacchi,x1,y1,x2,y2);
}

stati_mossa esecutore(scacchiera* scacchi, mossa_corrente* mossa, stati_mossa stato, re* r){
    switch(stato){
    case MOSSA_EFFETTUABILE:
        scacchi=invertiPedine(scacchi,
                              mossa->pos_inizio_x, mossa->pos_inizio_y,
                              mossa->pos_arrivo_x, mossa->pos_arrivo_y);
        scacchi->campo[mossa->pos_arrivo_x][mossa->pos_arrivo_y]->mossa=TRUE;
        if(scacchi->campo[mossa->pos_arrivo_x][mossa->pos_arrivo_y]->nome==RE){
            r=aggiornaRE(r, mossa->pos_arrivo_x,mossa->pos_arrivo_y);
        }
        puts("Mosso :)");
        break;
    case BLOCCATA_ALTRO_COLORE:
        scacchi=mangia(scacchi,
                       mossa->pos_inizio_x, mossa->pos_inizio_y,
                       mossa->pos_arrivo_x, mossa->pos_arrivo_y);
        scacchi->campo[mossa->pos_arrivo_x][mossa->pos_arrivo_y]->mossa=TRUE;
        if(scacchi->campo[mossa->pos_arrivo_x][mossa->pos_arrivo_y]->nome==RE){
            r=aggiornaRE(r, mossa->pos_arrivo_x,mossa->pos_arrivo_y);
        }
        puts("Mangiato :)");
        break;
    case BLOCCATA_STESSO_COLORE:
        puts("La tua pedina e' bloccata da una del tuo stesso colore :/");
        break;
    case PEDINA_NON_IN_CAMPO:
        puts("Seleziona una pedina che sta nel un campo di gioco :/");
        break;
    case PEDINA_INESISTENTE:
        puts("Hai selezionato un punto vuoto nel campo di gioco :/");
        break;
    case COLORE_ERRATO:
        puts("Seleziona una pedina del tuo colore :/");
        break;
    case ARRIVO_NON_IN_CAMPO:
        puts("Seleziona una mossa che rimanga nel campo di gioco :/");
        break;
    case MOSSA_NULLA:
        puts("La tua pedina non si e' mossa");
        break;
    case NON_RISPETTA_REGOLE:
        puts("Mossa invalida :(");
        break;
    case SCACCO_MATTO:
        puts("SCACCO MATTO!");
        break;
    }
    return stato;
}

/*
CONTROLLORE:

Il compito del controllore è quello di analizzare dopo ogni mossa lo stato della partita, e dunque decidere una di queste situazioni:

-il re è in posizione di SCACCO
-il re è in posizione di SCACCO_MATTO
-un pedone è arrivato alla fine della scacchiera => pedone diventa regina

L'ultimo caso è facile da controllare, perchè basta controllare che sulla colonna 0 ci sia una pedone bianco e viceversa

Il concetto di scacco e scacco matto invece è più complicato, perchè in ogni momento vanno controllate la posizione di entrambi i re,
e bisogna chiedersi se è sotto attacco da una qualsiasi pedina avversaria, controllando ovvero la distanza da esse.

Il concetto è molto simile a quello di un pathfinding algorithm, dove ti puoi muovere solo in orizzontale e verticale
(il movimento in diagonale lo si ottiene grazie allo spostamento una volta in orizzontale e una volta in verticale)
e i muri sono rappresentati dalle tue pedine. Ogni volta che trovi un avversario nemico ti devi chiedere se con una mossa può raggiungere
direttamente il re, tramite le regole utilizzate nel giudice, perchè tu per ipotesi ti fermerai a controllare in una direzione
quando troverai una pedina del tuo stesso colore.

La cosa più comoda a lungo andare è quella di avere sempre un puntatore al re a portata di mano, così che quello sarà
sempre il tuo punto di partenza, dunque definirò una struct a tale proposito.
*/

re* aggiornaRE(re* r, int x, int y){
    r->pos_x=x;
    r->pos_y=y;

    return r;
}

re* creaRE(pedina* p, int x, int y){

    re* r=(re*)malloc(sizeof(re));

    if(r==NULL){
        puts("Errore nella creazione della struttura di controllo del RE");
    }

    r->info=p;
    r->pos_x=x;
    r->pos_y=y;

    return r;
}

/*
L'idea del cercare se incontro una pedina avversaria è dunque quella di muoversi una volta per direzione
utilizzando la funzione già implementata muoviPedinaVerso() in tutte le posizioni possibili della scacchiera.

L'unico problema è che ovviamente il processo andrebbe rieseguito DIM_SCACCHIERA*DIM_SCACCHIERA-1 = 63 volte, ma c'è un metodo
molto più veloce per sbrigarsela.

Questo metodo consiste non nel controllare ogni posizione possibile, ma bensì basta far muovere il re fino agli 8 angoli della scacchiera
e controllare se in quelle posizioni si scontra con una pedina.

NB: il movimento sarà sempre descritto da un vettore distanza=pos_bordo-pos_re

Da qui possono nascere i seguenti casi:
-non trova pedine    : si passa alla prossima direzione
-pedina STESSO colore: si passa alla prossima direzione
-pedina ALTRO  colore: deve rispettare queste condizioni per essere scacco:

PEDONE:
-||distanza||^2 =2

RE:
-||distanza||^2 =2 oppure =1

TORRE:
-x=0 oppure y=0

ALFIERE:
-x=y

REGINA:
condizioni torre oppure alfiere

Ho saltato il CAVALLO perchè per controllarlo basta fare un algoritmo a parte
che controlla tutte le 8 possibili posizioni a rischio per il RE, date dalle combinazioni con segno di (1,2) e (2,1)

L'unico problema è che la funzione muoviPedinaVerso ritorna se io vengo bloccato da una pedina e il colore oppure no,
senza nessuna altra possibilità di analizzare nello specifico la pedina.

La soluzione migliore che mi viene in mente è quella di ritoccare la funzione muoviPedinaVerso dove verrà passato l'indirizzo a
x e y_spostamento e lì dentro verranno salvati alla fine del processo gli effettivi valori di quanto si è spostato, così che
successivamente sarà possibile accederci tramite [pos_re_y+y_spostamento][pos_re_x+x_spostamento].
*/

/*
Tutte le possibili combinazioni di posizioni agli estremi sono, prendendo come riferimento y e x posizione nella matrice del re, sono:
(0,x)
(y,0)
(y,DIM-1)
(DIM-1,x)

Per le diagonali la formula per calcolarli è un po' più articolata, oerò la si scopre osservando che gli elementi su una stessa
diagonale hanno la stessa somma.

Per l'estremo in alto a destra si ha:
somma=x+y;
(somma%(DIM-1),DIM-1) se somma > DIM-1;
(0, somma)            se somma <=DIM-1;

Per l'estremo in basso a sinistra si ha: => è la riflessione
somma=x+y;
(DIM-1, somma%(DIM-1)) se somma > DIM-1;
(somma, 0)             se somma <=DIM-1;

Per l'estremo in alto a sinistra si ha:
(0,x-y) se x>y
(y-x,0) se x<y
(0  ,0) se x=y

Per l'estremo in basso a destra si ha:
(DIM-1-x+y,DIM-1) se x>y
(DIM-1,DIM-1-y+x) se x<y
(DIM-1,    DIM-1) se x=y

Dunque quello che bisogna fare è prendere i vettori che portano dal re a tutte quelle 8 posizioni e usare la funzione muoviPedinaVerso
per capire se effettivamente c'è un ostacolo, e se c'è per fare scacco bisogna che sia del colore opposto al re
*/
stati_mossa controllore(scacchiera* scacchi, re* re_nero, re* re_bianco){

    stati_mossa stato=MOSSA_EFFETTUABILE;
    int x,y;
    pedina* bloccante;
    //Cerco prima se un pedone diventa regina
    for(x=0,y=0;x<DIM_SCACCHIERA-1;x++){
        if(scacchi->campo[y][x]->nome==PEDONE && scacchi->campo[y][x]->col==BIANCO){
            scacchi->campo[y][x]->nome=REGINA;
            stato=PEDONE_DIVENTA_REGINA;
            puts("Un pedone bianco e' diventato regina");
        }
    }
    for(x=0,y=DIM_SCACCHIERA-1;x<DIM_SCACCHIERA-1;x++){
        if(scacchi->campo[y][x]->nome==PEDONE && scacchi->campo[y][x]->col==NERO){
            scacchi->campo[y][x]->nome=REGINA;
            stato=PEDONE_DIVENTA_REGINA;
            puts("Un pedone nero e' diventato regina");
        }
    }
    //Controllare ora lo SCACCO
    //Prima il re bianco
    int spostamento_x,spostamento_y;
    int arrivo_x,arrivo_y;
    //Direzione in alto
    arrivo_x=re_bianco->pos_x;
    arrivo_y=0;
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_NERO;
        }
    }
    //Direzione in basso
    arrivo_x=re_bianco->pos_x;
    arrivo_y=DIM_SCACCHIERA-1;
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_NERO;
        }
    }
    //Direzione a destra
    arrivo_x=DIM_SCACCHIERA-1;
    arrivo_y=re_bianco->pos_y;
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_NERO;
        }
    }
    //Direzione a sinistra
    arrivo_x=0;
    arrivo_y=re_bianco->pos_y;
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_NERO;
        }
    }
    //Direzione in alto a destra
    int somma=re_bianco->pos_x+re_bianco->pos_y;
    if(somma>DIM_SCACCHIERA-1){
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=somma%(DIM_SCACCHIERA-1);
    }
    else{
        arrivo_x=somma;
        arrivo_y=0;
    }
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_NERO;
        }
    }
    //Direzione in basso a destra
    if(re_bianco->pos_x > re_bianco->pos_y){
        arrivo_x=DIM_SCACCHIERA-1-re_bianco->pos_x+re_bianco->pos_y;
        arrivo_y=DIM_SCACCHIERA-1;
    }
    else if(re_bianco->pos_x < re_bianco->pos_y){
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=DIM_SCACCHIERA-1-re_bianco->pos_y+re_bianco->pos_x;
    }
    else {
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=DIM_SCACCHIERA-1;
    }
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_NERO;
        }
    }
    //Direzione in alto a sinistra
    if(re_bianco->pos_x > re_bianco->pos_y){
        arrivo_x=re_bianco->pos_x-re_bianco->pos_y;
        arrivo_y=0;
    }
    else if(re_bianco->pos_x < re_bianco->pos_y){
        arrivo_x=somma;
        arrivo_y=0;
    }
    else {
        arrivo_x=0;
        arrivo_y=0;
    }
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_NERO;
        }
    }
    //Direzione in basso a sinistra
    if(somma>DIM_SCACCHIERA-1){
        arrivo_x=somma%(DIM_SCACCHIERA-1);
        arrivo_y=DIM_SCACCHIERA-1;
    }
    else{
        arrivo_x=0;
        arrivo_y=somma;
    }
    spostamento_x=arrivo_x-re_bianco->pos_x;
    spostamento_y=arrivo_y-re_bianco->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_bianco->pos_y][re_bianco->pos_x], re_bianco->pos_x, re_bianco->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_bianco->pos_y+spostamento_y][re_bianco->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_NERO;
        }
    }
    //Controllare se ci sono cavalli avversari che possono attaccare il re
    spostamento_x=re_bianco->pos_x+1;
    spostamento_y=re_bianco->pos_y+2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x+1;
    spostamento_y=re_bianco->pos_y-2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x-1;
    spostamento_y=re_bianco->pos_y+2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x-1;
    spostamento_y=re_bianco->pos_y-2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x+2;
    spostamento_y=re_bianco->pos_y+1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x+2;
    spostamento_y=re_bianco->pos_y-1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x-2;
    spostamento_y=re_bianco->pos_y+1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;
    spostamento_x=re_bianco->pos_x-2;
    spostamento_y=re_bianco->pos_y-1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_NERO;

    //Ora il re nero
    //Direzione in alto
    arrivo_x=re_nero->pos_x;
    arrivo_y=0;
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione in basso
    arrivo_x=re_nero->pos_x;
    arrivo_y=DIM_SCACCHIERA-1;
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione a destra
    arrivo_x=DIM_SCACCHIERA-1;
    arrivo_y=re_nero->pos_y;
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione a sinistra
    arrivo_x=0;
    arrivo_y=re_nero->pos_y;
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
            || bloccante->nome==REGINA
            || bloccante->nome==TORRE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione in alto a destra
    somma=re_nero->pos_x+re_nero->pos_y;
    if(somma>DIM_SCACCHIERA-1){
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=somma%(DIM_SCACCHIERA-1);
    }
    else{
        arrivo_x=somma;
        arrivo_y=0;
    }
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione in basso a destra
    if(re_nero->pos_x > re_nero->pos_y){
        arrivo_x=DIM_SCACCHIERA-1-re_nero->pos_x+re_nero->pos_y;
        arrivo_y=DIM_SCACCHIERA-1;
    }
    else if(re_nero->pos_x < re_nero->pos_y){
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=DIM_SCACCHIERA-1-re_nero->pos_y+re_nero->pos_x;
    }
    else {
        arrivo_x=DIM_SCACCHIERA-1;
        arrivo_y=DIM_SCACCHIERA-1;
    }
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione in alto a sinistra
    if(re_nero->pos_x > re_nero->pos_y){
        arrivo_x=re_nero->pos_x-re_nero->pos_y;
        arrivo_y=0;
    }
    else if(re_nero->pos_x < re_nero->pos_y){
        arrivo_x=somma;
        arrivo_y=0;
    }
    else {
        arrivo_x=0;
        arrivo_y=0;
    }
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_BIANCO;
        }
    }
    //Direzione in basso a sinistra
    if(somma>DIM_SCACCHIERA-1){
        arrivo_x=somma%(DIM_SCACCHIERA-1);
        arrivo_y=DIM_SCACCHIERA-1;
    }
    else{
        arrivo_x=0;
        arrivo_y=somma;
    }
    spostamento_x=arrivo_x-re_nero->pos_x;
    spostamento_y=arrivo_y-re_nero->pos_y;
    if(spostamento_x!=0 && spostamento_y!=0){
        stato=muoviPedinaVerso(scacchi, scacchi->campo[re_nero->pos_y][re_nero->pos_x], re_nero->pos_x, re_nero->pos_y, &spostamento_x, &spostamento_y);
    }
    else stato=MOSSA_NULLA;
    if(stato==BLOCCATA_ALTRO_COLORE){
        bloccante=scacchi->campo[re_nero->pos_y+spostamento_y][re_nero->pos_x+spostamento_x];
        if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
            ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2)
            || bloccante->nome==REGINA
            || bloccante->nome==ALFIERE){
                return SCACCO_BIANCO;
        }
    }
    //Controllare se ci sono cavalli avversari che possono attaccare il re
    spostamento_x=re_nero->pos_x+1;
    spostamento_y=re_nero->pos_y+2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x+1;
    spostamento_y=re_nero->pos_y-2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x-1;
    spostamento_y=re_nero->pos_y+2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x-1;
    spostamento_y=re_nero->pos_y-2;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x+2;
    spostamento_y=re_nero->pos_y+1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x+2;
    spostamento_y=re_nero->pos_y-1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x-2;
    spostamento_y=re_nero->pos_y+1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;
    spostamento_x=re_nero->pos_x-2;
    spostamento_y=re_nero->pos_y-1;
    if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
        if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_nero->info->col &&
           scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO)return SCACCO_BIANCO;

    return MOSSA_EFFETTUABILE;
}

/*

*/

void stampaScacchiera(scacchiera* scacchi){

    //16
    puts("----------------");

    for(int x=0;x<DIM_SCACCHIERA;x++){
        for(int y=0;y<DIM_SCACCHIERA;y++){
            switch(scacchi->campo[x][y]->nome){
            case PEDONE:
                putchar('p');
                break;
            case TORRE:
                putchar('T');
                break;
            case CAVALLO:
                putchar('C');
                break;
            case ALFIERE:
                putchar('A');
                break;
            case REGINA:
                putchar('r');
                break;
            case RE:
                putchar('R');
                break;
            case VUOTO:
                putchar('+');
                break;
            }
        }
        puts("");
    }
    puts("----------------");
}





int main()
{
    scacchiera* scacchi=creaScacchiera();
    stampaScacchiera(scacchi);

    stati_mossa stato=MOSSA_EFFETTUABILE;
    colore giocatore=BIANCO;
    re* re_bianco=creaRE(scacchi->campo[7][3],3,7);
    re* re_nero  =creaRE(scacchi->campo[0][3],3,0);
    puts("INIZIO DEL GIOCO DEGLI SCACCHI!\nRicordati che il BIANCO muove per primo :)");

    while(stato!=SCACCO_MATTO){
        mossa_corrente* mossa=creaMossaCorrente(giocatore);
        if(giocatore){
            stato=esecutore(scacchi,
                  mossa,
                  giudice(scacchi,mossa,giocatore),
                  re_bianco);
        }
        else{
            stato=esecutore(scacchi,
                  mossa,
                  giudice(scacchi,mossa,giocatore),
                  re_nero);
        }
        //Il gioco continua se la mossa effettuata è valida
        if(stato==MOSSA_EFFETTUABILE || stato==BLOCCATA_ALTRO_COLORE) {
            system("cls");
            if(giocatore){
                giocatore=NERO;
                puts("Ora tocca al NERO");
            }
            else{
                giocatore=BIANCO;
                puts("Ora tocca al BIANCO");
            }
        }
        stato=controllore(scacchi, re_nero, re_bianco);
        if(stato==SCACCO_BIANCO){
            puts("SCACCO BIANCO!");
        }
        else if(stato==SCACCO_NERO){
            puts("SCACCO NERO!");
        }
        stampaScacchiera(scacchi);
    }
    return 0;
}
