#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIM_SCACCHIERA 8
#define COMBINAZIONI_CAVALLO 8
#define COMBINAZIONI_RE 8

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
stati_mossa controllore (scacchiera* scacchi, re* re_bianco, int combinazioni_re[COMBINAZIONI_RE][2],int combinazioni_cav[COMBINAZIONI_CAVALLO][2]);
stati_mossa muoviReVerso(scacchiera* scacchi, re* re_bianco, int direzione_x, int direzione_y, int* arrivo_x, int* arrivo_y);

int creaCombinazioniCavallo(int combinazioni[COMBINAZIONI_CAVALLO][2]);
int creaCombinazioniRe     (int combinazioni[COMBINAZIONI_RE     ][2]);

void stampaScacchiera(scacchiera* scacchi);



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
        break;
    }
    if(stato==RISPETTA_REGOLE){
        stato=muoviPedinaVerso(scacchi, pedina_corrente, mossa->pos_inizio_y, mossa->pos_inizio_x, &y, &x);
    }

    return stato;
}

stati_mossa rispettaRegolePedone(scacchiera* scacchi, pedina* pedina_corrente, mossa_corrente* mossa, int distanza, int x, int y){
    if(distanza==1 && x==0){
        if(pedina_corrente->col==BIANCO && y<0) return RISPETTA_REGOLE;
        if(pedina_corrente->col==NERO   && y>0) return RISPETTA_REGOLE;
    }
    else if(distanza==2){
        if((pedina_corrente->col==BIANCO && y<0) || (pedina_corrente->col==NERO   && y>0)){
            if(scacchi->campo[mossa->pos_inizio_x+y][mossa->pos_inizio_y+x]->nome!=VUOTO
               && scacchi->campo[mossa->pos_inizio_x+y][mossa->pos_inizio_y+x]->col!=pedina_corrente->col){
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

stati_mossa rispettaRegoleRe(pedina* pedina_corrente, int distanza, int x, int y){

    if(distanza==1 || distanza==2) return RISPETTA_REGOLE;

    return NON_RISPETTA_REGOLE;
}

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

stati_mossa muoviCavalloVerso(scacchiera* scacchi, pedina* pedina_corrente, int x_inizio, int y_inizio, int x_spostamento, int y_spostamento){

    if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->nome==VUOTO) return MOSSA_EFFETTUABILE;

    if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->col!=pedina_corrente->col){
        if(scacchi->campo[y_inizio+y_spostamento][x_inizio+y_spostamento]->nome==RE) return SCACCO_MATTO;
        else return BLOCCATA_ALTRO_COLORE;
    }

    return BLOCCATA_STESSO_COLORE;
}

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

stati_mossa controllore (scacchiera* scacchi, re* re_bianco, int combinazioni_re[COMBINAZIONI_RE][2],int combinazioni_cav[COMBINAZIONI_CAVALLO][2]){

    stati_mossa stato=MOSSA_EFFETTUABILE;
    int x,y;
    pedina* bloccante;
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
    int arrivo_x=0;
    int arrivo_y=0;
    int spostamento_x,spostamento_y;
    for(int i=0;i<COMBINAZIONI_RE;i++){
        stato=muoviReVerso(scacchi, re_bianco, combinazioni_re[i][1], combinazioni_re[i][0], &arrivo_y, &arrivo_x);
        if(stato==BLOCCATA_ALTRO_COLORE){
            bloccante=scacchi->campo[arrivo_y][arrivo_x];
            spostamento_x=arrivo_x-re_bianco->pos_x;
            spostamento_y=arrivo_y-re_bianco->pos_y;
            switch(i){
            case 0:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
                    || bloccante->nome==REGINA
                    || bloccante->nome==TORRE){
                        puts("Attaccato dal basso");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 1:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
                    || bloccante->nome==REGINA
                    || bloccante->nome==TORRE){
                        puts("Attaccato dall'alto");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 2:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
                    || bloccante->nome==REGINA
                    || bloccante->nome==TORRE){
                        puts("Attaccato da destra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 3:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==1)
                    || bloccante->nome==REGINA
                    || bloccante->nome==TORRE){
                        puts("Attaccato da sinistra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 4:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
                    ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2 && re_bianco->info->col==NERO)
                    || bloccante->nome==REGINA
                    || bloccante->nome==ALFIERE){
                        puts("Attaccato dal basso a destra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 5:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
                    ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2 && re_bianco->info->col==BIANCO)
                    || bloccante->nome==REGINA
                    || bloccante->nome==ALFIERE){
                        puts("Attaccato dall'alto a destra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 6:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
                    ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2 && re_bianco->info->col==NERO)
                    || bloccante->nome==REGINA
                    || bloccante->nome==ALFIERE){
                        puts("Attaccato dal basso a sinistra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            case 7:
                if((bloccante->nome==RE && norma_quadrato(spostamento_x,spostamento_y)==2)
                    ||(bloccante->nome==PEDONE && norma_quadrato(spostamento_x,spostamento_y)==2 && re_bianco->info->col==BIANCO)
                    || bloccante->nome==REGINA
                    || bloccante->nome==ALFIERE){
                        puts("Attaccato dall'alto a sinistra");
                        if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                        return SCACCO_BIANCO;
                }
                break;
            default:
                break;
            }
        }
    }
	for(int i=0;i<COMBINAZIONI_CAVALLO;i++){
        spostamento_x=re_bianco->pos_x+combinazioni_cav[i][0];
        spostamento_y=re_bianco->pos_y+combinazioni_cav[i][1];
        if(spostamento_x<DIM_SCACCHIERA && spostamento_x>=0 && spostamento_y<DIM_SCACCHIERA&&spostamento_y>=0)
            if(scacchi->campo[spostamento_y][spostamento_x]->col!=re_bianco->info->col &&
                scacchi->campo[spostamento_y][spostamento_x]->nome==CAVALLO){
                if(re_bianco->info->col==BIANCO) return SCACCO_NERO;
                return SCACCO_BIANCO;
                }
    }

    return MOSSA_EFFETTUABILE;
}

stati_mossa muoviReVerso(scacchiera* scacchi, re* re_bianco, int direzione_y, int direzione_x, int* arrivo_y, int* arrivo_x){

    boolean ostacolo_trovato=FALSE;

    int x=re_bianco->pos_x;
    int y=re_bianco->pos_y;
    while(x+direzione_x>=0 && x+direzione_x<DIM_SCACCHIERA &&
          y+direzione_y>=0 && y+direzione_y<DIM_SCACCHIERA &&
          !ostacolo_trovato){
        x+=direzione_x;
        y+=direzione_y;
        if(scacchi->campo[y][x]->nome!=VUOTO) ostacolo_trovato=TRUE;
    }
    *arrivo_x=x;
    *arrivo_y=y;
    if(x==re_bianco->pos_x && y==re_bianco->pos_y)      return MOSSA_NULLA;
    if(ostacolo_trovato){
        if(scacchi->campo[y][x]->col!=re_bianco->info->col) return BLOCCATA_ALTRO_COLORE;
    }
    return MOSSA_EFFETTUABILE;
 }

int creaCombinazioniCavallo(int combinazioni[COMBINAZIONI_CAVALLO][2]){

    combinazioni[0][0]= 1;combinazioni[0][1]= 2;
    combinazioni[1][0]= 1;combinazioni[1][1]=-2;
    combinazioni[2][0]=-1;combinazioni[2][1]= 2;
    combinazioni[3][0]=-1;combinazioni[3][1]=-2;
    combinazioni[4][0]= 2;combinazioni[4][1]= 1;
    combinazioni[5][0]= 2;combinazioni[5][1]=-1;
    combinazioni[6][0]=-2;combinazioni[6][1]= 1;
    combinazioni[7][0]=-2;combinazioni[7][1]=-1;

    return 1;
}

int creaCombinazioniRe(int combinazioni[COMBINAZIONI_RE][2]){

  
    combinazioni[0][0]= 0;combinazioni[0][1]= 1; 
    combinazioni[1][0]= 0;combinazioni[1][1]=-1; 
    combinazioni[2][0]= 1;combinazioni[2][1]= 0; 
    combinazioni[3][0]=-1;combinazioni[3][1]= 0; 
    combinazioni[4][0]= 1;combinazioni[4][1]= 1; 
    combinazioni[5][0]= 1;combinazioni[5][1]=-1; 
    combinazioni[6][0]=-1;combinazioni[6][1]= 1; 
    combinazioni[7][0]=-1;combinazioni[7][1]=-1; 

    return 1;
}

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
    re* re_nero  =creaRE(scacchi->campo[0][4],4,0);
    puts("INIZIO DEL GIOCO DEGLI SCACCHI!\nRicordati che il BIANCO muove per primo :)");

    int combinazioni_cav[COMBINAZIONI_CAVALLO][2];
    creaCombinazioniCavallo(combinazioni_cav);

    int combinazioni_re[COMBINAZIONI_RE][2];
    creaCombinazioniRe(combinazioni_re);

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
        stato=controllore(scacchi, re_bianco, combinazioni_re, combinazioni_cav);
        stato=controllore(scacchi, re_nero  , combinazioni_re, combinazioni_cav);
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
