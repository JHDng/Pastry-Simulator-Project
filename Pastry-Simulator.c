#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define N 255
#define FACTOR 1804250316

typedef struct Lotto
{
    int peso;
    int scadenza;
    struct Lotto * prox;
} SingoloLotto;

typedef SingoloLotto * RifSingoloLotto;

typedef struct IngrMag
{
    char *nome;
    int pesoTot;
    int chiave;
    struct IngrMag * prox;
    RifSingoloLotto listaLotti;
} IngredienteMag;

typedef IngredienteMag * RiferIngredienteMag;

typedef struct IngrRic
{
    char *nome;
    int peso;
    struct IngrRic * prox;
    struct IngrMag * rifInMagazzino;
} IngredienteRic;

typedef IngredienteRic * RiferIngredienteRic;

typedef struct Ric
{
    char *nome;
    int pesoTot;
    int chiave;
    RiferIngredienteRic riferIngredienteRic;
} Ricetta;

typedef Ricetta * RiferRicetta;

typedef struct Ord
{
    RiferRicetta ricetta;
    int quantt;
    int tempoArrivo;
    struct Ord * prox;
} Ordine;

typedef Ordine * RifOrdine;

RiferRicetta * AggiungiRicetta();
RiferRicetta * RimuoviRicetta();
RiferRicetta * RiallocoHashRicette();
RifSingoloLotto InsInOrdScadenzaRicorsivo();
RiferIngredienteMag * AggiornaMagazzino();
RiferIngredienteMag * Rifornimento();
RiferIngredienteMag * ControllaScadenza();
RiferIngredienteMag * RiallocoHashMagazzino();
RiferIngredienteMag ControllaListaIngredienti();
RiferIngredienteRic InsIngrInListaRicetta();
RifOrdine ControllaOrdiniAttesa();
RifOrdine Corriere();
RifOrdine AggiornaCoda();
RifOrdine InsOrdineInOrdineRicorsivo();
RifOrdine * MergeSortDecrescente();
RifOrdine * MergeDecrescente();

int FunzioneHashRicette();
int FunzioneHashMagazzino();
int CalcolaPresenza();
int DimensioneRic();

void FaiUnOrdine();
void DistruggiListaIngredientiRicetta();
void DistruggiListaIngredientiMagazzino();
void DistruggiListaLottiMagazzino();

int main()
{
    RifOrdine ordiniAttesa = NULL;
    RifOrdine ordiniPronti = NULL;
    RifOrdine codaAttesa = NULL;
    RifOrdine codaPronti = NULL;
    
    int periodoCamion, capienzaCamion, tempo = 0, bucketNumRic = 1, bucketNumMag = 1, flag = 1;
    char stringaInput[N];

    RiferRicetta *hashRicette = calloc(bucketNumRic, sizeof(RiferRicetta));
    RiferIngredienteMag *hashMagazzino = calloc(bucketNumMag, sizeof(RiferIngredienteMag));

    /*specifiche del camion*/
    if(scanf("%d %d", &periodoCamion, &capienzaCamion))
    {
        while(flag && scanf("%s", stringaInput) == 1)
        {
            if(!strcmp(stringaInput, "aggiungi_ricetta"))
            {
                hashRicette = AggiungiRicetta(hashRicette, hashMagazzino,  &bucketNumRic, &bucketNumMag);
            }
            else if(!strcmp(stringaInput, "rimuovi_ricetta"))
            {
                hashRicette = RimuoviRicetta(hashRicette, ordiniAttesa, ordiniPronti, bucketNumRic);
            }
            else if(!strcmp(stringaInput, "rifornimento"))
            {
                hashMagazzino = Rifornimento(hashRicette, hashMagazzino, &bucketNumRic, &bucketNumMag, tempo);
                hashMagazzino = ControllaScadenza(hashMagazzino, bucketNumMag, tempo);
                ordiniAttesa = ControllaOrdiniAttesa(hashRicette, &hashMagazzino, &ordiniPronti, ordiniAttesa, &codaPronti, &codaAttesa, bucketNumRic, bucketNumMag);
            }
            else if(!strcmp(stringaInput, "ordine"))
            {
                hashMagazzino = ControllaScadenza(hashMagazzino, bucketNumMag, tempo);
                FaiUnOrdine(hashRicette, &hashMagazzino, &ordiniPronti, &ordiniAttesa, &codaPronti, &codaAttesa, bucketNumRic, bucketNumMag, tempo);
            }
            else
            {
                printf("Comando sconosciuto!");
                flag = 0;
            }

            if(flag && !(++tempo % periodoCamion))/*Passaggio corriere*/
            {
                if(ordiniPronti != NULL)
                {
                    ordiniPronti = Corriere(ordiniPronti, &codaPronti, capienzaCamion);
                }
                else
                {
                    printf("camioncino vuoto\n");
                }
            }
        }
    }
    else
    {
        printf("Errore acquisizione specifiche camion\n");
    }

    for(int i = 0; i < bucketNumMag; i++)
    {
        if(hashMagazzino[i])
        {
            DistruggiListaIngredientiMagazzino(hashMagazzino[i]);
        }
    }
    free(hashMagazzino);

    for(int i = 0; i < bucketNumRic; i++)
    {
        if(hashRicette[i])
        {
            DistruggiListaIngredientiRicetta(hashRicette[i]->riferIngredienteRic);
            free(hashRicette[i]->nome);
        }
        free(hashRicette[i]);
    }
    free(hashRicette);

    RifOrdine punt = ordiniAttesa;
    RifOrdine pross = ordiniAttesa;
    while(punt != NULL)
    {
        pross = punt->prox;
        free(punt);
        punt = pross;
    }

    punt = ordiniPronti;
    pross = ordiniPronti;
    while(punt != NULL)
    {
        pross = punt->prox;
        free(punt);
        punt = pross;
    }

    return 0;
}

int FunzioneHashRicette(RiferRicetta * hashRicette, char *nomeRicetta, int bucketNumRic, int ins1canc0)
{
    int tentativi = 0;
    int veraChiave = 0;
    uint32_t hash = FACTOR;

    for(int i = 0; i < strlen(nomeRicetta); i = i + 2)
    {
        hash ^= nomeRicetta[i];
        veraChiave += nomeRicetta[i];
    }

    while(tentativi < bucketNumRic)
    {
        uint32_t temp = (uint32_t)(hash + 0.5 * tentativi +  0.5 * tentativi * tentativi) % bucketNumRic;

        if(hashRicette[temp] == NULL)
        {
            if(ins1canc0)   return temp;
            return bucketNumRic + 1;
        }
        else if(veraChiave == hashRicette[temp]->chiave && !strcmp(hashRicette[temp]->nome, nomeRicetta))
        {
            if(ins1canc0)   return bucketNumRic + 2;
            return temp; /*indice ricetta da accedere*/
        }
        else
        {
            tentativi++;
        }
    }

    return bucketNumRic + 1;
}

int FunzioneHashMagazzino(RiferIngredienteMag * hashMagazzino, char *nomeIngrediente, int bucketNumMag, int ins1acc0)
{
    int veraChiave = 0;
    uint32_t hash = FACTOR;

    for(int i = 0; i < strlen(nomeIngrediente); i = i + 2)
    {
        hash ^= nomeIngrediente[i];
        veraChiave += nomeIngrediente[i];
    }

    hash = (uint32_t)(hash) % bucketNumMag;

    if(ins1acc0 && DimensioneRic(hashMagazzino[hash]) == bucketNumMag) return bucketNumMag + 1;
    return hash;
}

int DimensioneRic(RiferIngredienteMag lista) {
    if(lista) return 1 + DimensioneRic( lista->prox );
    return 0;
}

RiferIngredienteMag * ControllaScadenza(RiferIngredienteMag * hashMagazzino, int bucketNumMag, int tempo)
{
    for(int i = 0; i < bucketNumMag; i++)
    {
        if(hashMagazzino[i] != NULL)
        {
            hashMagazzino[i] = ControllaListaIngredienti(hashMagazzino[i], tempo);
        }
    }

    return hashMagazzino;
}

RiferIngredienteMag ControllaListaIngredienti(RiferIngredienteMag lista, int tempo)
{
    if(lista->prox)    lista->prox = ControllaListaIngredienti(lista->prox, tempo);

    RifSingoloLotto punt = lista->listaLotti;
    while(punt != NULL && punt->scadenza <= tempo)
    {
        if(punt->prox == NULL)
        {
            lista->pesoTot -= punt->peso;
            free(punt);
            lista->listaLotti = NULL;
            punt = NULL;
        }
        else
        {
            lista->pesoTot -= punt->peso;
            lista->listaLotti = punt->prox;
            free(punt);
            punt = lista->listaLotti;
        }
    }

	return lista;
}

RiferRicetta * AggiungiRicetta(RiferRicetta * hashRicette, RiferIngredienteMag * hashMagazzino, int * bucketNumRic, int * bucketNumMag)
{
    char nomeRicetta[N], nomeIngr[N];
    char chr = '%';
    int pesoIngrediente, index;

    if(scanf("%s", nomeRicetta))
    {
        index = FunzioneHashRicette(hashRicette, nomeRicetta, *bucketNumRic, 1);

        if(index == *bucketNumRic + 1)
        {
            *bucketNumRic = *bucketNumRic * 2;
            hashRicette = RiallocoHashRicette(hashRicette, *bucketNumRic);
            index = FunzioneHashRicette(hashRicette, nomeRicetta, *bucketNumRic, 1);
        }
        else if(index == *bucketNumRic + 2)
        {
            printf("ignorato\n");
            while(chr != '\n')
            {
                if(!scanf("%s %d%c", nomeIngr, &pesoIngrediente, &chr))
                {
                    return hashRicette;
                }
            }
            return hashRicette;
        }

        int chiaveTemp = 0;
        for(int i = 0; i < strlen(nomeRicetta); i = i + 2)
        {
            chiaveTemp += nomeRicetta[i];
        }

        RiferRicetta nuovaRicetta = (RiferRicetta) malloc(sizeof(Ricetta));
        nuovaRicetta->nome = malloc((strlen(nomeRicetta) + 1) * sizeof(char));
        nuovaRicetta->pesoTot = 0;
        nuovaRicetta->chiave = chiaveTemp;
        strcpy(nuovaRicetta->nome, nomeRicetta);
        nuovaRicetta->riferIngredienteRic = NULL;

        while(chr != '\n')
        {
            if(scanf("%s %d%c", nomeIngr, &pesoIngrediente, &chr))
            {
                nuovaRicetta->riferIngredienteRic = InsIngrInListaRicetta(nuovaRicetta->riferIngredienteRic, nomeIngr, pesoIngrediente);
                int index = FunzioneHashMagazzino(hashMagazzino, nomeIngr, *bucketNumMag, 0);
                if(index != *bucketNumMag + 1)
                {
                    RiferIngredienteMag punt = hashMagazzino[index];

                    int chiaveIngr = 0;
                    int flag = 1;
                    for(int i = 0; i < strlen(nomeIngr); i = i + 2)
                    {
                        chiaveIngr += nomeIngr[i];
                    }

                    while(punt != NULL && flag)
                    {
                        if(punt->chiave == chiaveIngr && !strcmp(punt->nome, nomeIngr))
                        {
                            flag = 0;
                        }
                        else
                        {
                            punt = punt->prox;
                        }
                    }
                    nuovaRicetta->riferIngredienteRic->rifInMagazzino = punt;
                }
                else
                {
                    nuovaRicetta->riferIngredienteRic->rifInMagazzino = NULL;
                }
                nuovaRicetta->pesoTot += pesoIngrediente;
            }
            else
            {
                DistruggiListaIngredientiRicetta(nuovaRicetta->riferIngredienteRic);
                free(nuovaRicetta->nome);
                free(nuovaRicetta);
                printf("Errore nell'acquisizione degli ingredienti");
                return hashRicette;
            }
        }

        hashRicette[index] = nuovaRicetta;

        printf("aggiunta\n");
        return hashRicette;
    }
    else
    {
        printf("Errore nell'aggiunta della ricetta");
        return hashRicette;
    }

    return hashRicette;
}

RiferRicetta * RimuoviRicetta(RiferRicetta * hashRicette, RifOrdine ordiniAttesa, RifOrdine ordiniPronti,int bucketNumRic)
{
    char nomeRicetta[N];
    int index;

    if(scanf("%s", nomeRicetta))
    {
        index = FunzioneHashRicette(hashRicette, nomeRicetta, bucketNumRic, 0);

        if(index == bucketNumRic + 1)/*non presente*/
        {
            printf("non presente\n");
            return hashRicette;
        }
        /*controllo che non sia in attesa*/
        int chiaveTemp = 0;
        for(int i = 0; i < strlen(nomeRicetta); i = i + 2)
        {
            chiaveTemp += nomeRicetta[i];
        }

        while(ordiniAttesa != NULL)
        {
            if(ordiniAttesa->ricetta->chiave == chiaveTemp && !strcmp(ordiniAttesa->ricetta->nome, nomeRicetta))
            {
                printf("ordini in sospeso\n");
                return hashRicette;
            }
            ordiniAttesa = ordiniAttesa->prox;
        }
        while(ordiniPronti != NULL)
        {
            if(ordiniPronti->ricetta->chiave == chiaveTemp && !strcmp(ordiniPronti->ricetta->nome, nomeRicetta))
            {
                printf("ordini in sospeso\n");
                return hashRicette;
            }
            ordiniPronti = ordiniPronti->prox;
        }

        free(hashRicette[index]->nome);
        hashRicette[index]->nome = malloc(sizeof(char) * 2);
        strcpy(hashRicette[index]->nome, "!"); /*lascia tombstone*/

        DistruggiListaIngredientiRicetta((hashRicette[index])->riferIngredienteRic);
        hashRicette[index]->riferIngredienteRic = NULL;

        printf("rimossa\n");

        return hashRicette;
    }
    else
    {
        printf("Errore nella rimozione della ricetta");
        return hashRicette;
    }

    return hashRicette;
}

RiferIngredienteMag * Rifornimento(RiferRicetta * hashRicette , RiferIngredienteMag * hashMagazzino, int * bucketNumRic, int * bucketNumMag, int tempo)
{
    char nomeIngr[N];
    char chr = '&';
    int pesoIngrediente, scadIngrediente, index;

    while(chr != '\n')
    {
        if(scanf("%s %d %d%c", nomeIngr, &pesoIngrediente, &scadIngrediente, &chr))
        {
            if(scadIngrediente > tempo)
            {
                index = FunzioneHashMagazzino(hashMagazzino, nomeIngr, *bucketNumMag, 1);

                if(index == *bucketNumMag + 1)
                {
                    *bucketNumMag = *bucketNumMag * 2;
                    hashMagazzino = RiallocoHashMagazzino(hashMagazzino, *bucketNumMag);
                    index = FunzioneHashMagazzino(hashMagazzino, nomeIngr, *bucketNumMag, 1);

                    for(int i = 0; i < *bucketNumRic; i++)
                    {
                        if(hashRicette[i] != NULL)
                        {
                            RiferIngredienteRic punt = hashRicette[i]->riferIngredienteRic;
                            while(punt != NULL)
                            {
                                punt->rifInMagazzino = NULL;
                                punt = punt->prox;
                            }
                        }
                    }
                }

                int chiaveTemp = 0;
                for(int i = 0; i < strlen(nomeIngr); i = i + 2)
                {
                    chiaveTemp += nomeIngr[i];
                }

                if(hashMagazzino[index] == NULL)
                {
                    hashMagazzino[index] = malloc(sizeof(IngredienteMag));
                    hashMagazzino[index]->nome = malloc((strlen(nomeIngr) + 1) * sizeof(char));
                    strcpy(hashMagazzino[index]->nome, nomeIngr);
                    hashMagazzino[index]->chiave = chiaveTemp;
                    hashMagazzino[index]->pesoTot = 0;
                    hashMagazzino[index]->pesoTot += pesoIngrediente;
                    hashMagazzino[index]->prox = NULL;
                    hashMagazzino[index]->listaLotti = NULL;
                    hashMagazzino[index]->listaLotti = InsInOrdScadenzaRicorsivo(hashMagazzino[index]->listaLotti, pesoIngrediente, scadIngrediente);
                }
                else
                {
                    RiferIngredienteMag punt = hashMagazzino[index];
                    int flag = 1;
                    while(flag && punt)
                    {
                        if(chiaveTemp == punt->chiave && !strcmp(nomeIngr, punt->nome))
                        {
                            flag = 0;
                        }
                        else
                        {
                            punt = punt->prox;
                        }
                    }
                    if(punt == NULL)
                    {
                        RiferIngredienteMag nuovoIngrediente = malloc(sizeof(IngredienteMag));
                        nuovoIngrediente->nome = malloc((strlen(nomeIngr) + 1) * sizeof(char));
                        strcpy(nuovoIngrediente->nome, nomeIngr);
                        nuovoIngrediente->chiave = chiaveTemp;
                        nuovoIngrediente->pesoTot = 0;
                        nuovoIngrediente->pesoTot += pesoIngrediente;
                        nuovoIngrediente->prox = hashMagazzino[index];
                        nuovoIngrediente->listaLotti = NULL;
                        nuovoIngrediente->listaLotti = InsInOrdScadenzaRicorsivo(nuovoIngrediente->listaLotti, pesoIngrediente, scadIngrediente);
                        hashMagazzino[index] = nuovoIngrediente;
                    }
                    else
                    {
                        punt->pesoTot += pesoIngrediente;
                        punt->listaLotti = InsInOrdScadenzaRicorsivo(punt->listaLotti, pesoIngrediente, scadIngrediente);
                    }
                }
            }
        }
        else
        {
            printf("Errore durante rifornimento");
            return hashMagazzino;
        }

    }

    printf("rifornito\n");
    return hashMagazzino;
}

RifOrdine ControllaOrdiniAttesa(RiferRicetta * hashRicette, RiferIngredienteMag ** hashMagazzino, RifOrdine * ordiniPronti, RifOrdine ordiniAttesa, RifOrdine * codaPronti, RifOrdine * codaAttesa, int bucketNumRic, int bucketNumMag)
{
    RifOrdine punt = ordiniAttesa;
    RifOrdine pre = ordiniAttesa;
    while(punt != NULL)
    {
        int qttRicetta = punt->quantt;

        if(CalcolaPresenza(punt->ricetta->riferIngredienteRic, *hashMagazzino, qttRicetta, bucketNumMag))
        {
            int temp = (*codaAttesa) == punt ? 1: 0;
            if(punt != ordiniAttesa)
            {
                pre->prox = punt->prox;
                *hashMagazzino = AggiornaMagazzino(punt->ricetta->riferIngredienteRic, *hashMagazzino, bucketNumMag, qttRicetta);
                punt->prox = NULL;
                *ordiniPronti = InsOrdineInOrdineRicorsivo(*ordiniPronti, punt, codaPronti);
                punt = pre->prox;
            }
            else
            {
                ordiniAttesa = punt->prox;
                pre = ordiniAttesa;
                *hashMagazzino = AggiornaMagazzino(punt->ricetta->riferIngredienteRic, *hashMagazzino, bucketNumMag, qttRicetta);
                punt->prox = NULL;
                *ordiniPronti = InsOrdineInOrdineRicorsivo(*ordiniPronti, punt, codaPronti);
                punt = pre;
            }
            if(temp)    (*codaAttesa) = AggiornaCoda(ordiniAttesa);
        }
        else
        {
            pre = punt;
            punt = punt->prox;
        }
    }

    return ordiniAttesa;
}

RifOrdine AggiornaCoda(RifOrdine lista)
{
    if(lista == NULL) return NULL;
    while(lista->prox != NULL)
    {
        lista = lista->prox;
    }
    return lista;
}

void FaiUnOrdine(RiferRicetta * hashRicette, RiferIngredienteMag ** hashMagazzino, RifOrdine *ordiniPronti, RifOrdine *ordiniAttesa, RifOrdine * codaPronti, RifOrdine * codaAttesa, int bucketNumRic, int bucketNumMag, int tempo)
{
    char nomeRicetta[N];
    int qttRicetta;


    if(scanf("%s %d", nomeRicetta, &qttRicetta))
    {
        int indexRic = FunzioneHashRicette(hashRicette, nomeRicetta, bucketNumRic, 0);

        if(indexRic == bucketNumRic + 1)/*non presente*/
        {
            printf("rifiutato\n");
            return;
        }
        else
        {
            RifOrdine nuovoOrdine = malloc(sizeof(Ordine));

            nuovoOrdine->ricetta = hashRicette[indexRic];
            nuovoOrdine->quantt = qttRicetta;
            nuovoOrdine->tempoArrivo = tempo;
            nuovoOrdine->prox = NULL;

            if(CalcolaPresenza(hashRicette[indexRic]->riferIngredienteRic, *hashMagazzino, qttRicetta, bucketNumMag))
            {
                *hashMagazzino = AggiornaMagazzino(hashRicette[indexRic]->riferIngredienteRic, *hashMagazzino, bucketNumMag, qttRicetta);
                if((*ordiniPronti) == NULL)
                {
                    (*ordiniPronti) = nuovoOrdine;
                }
                else
                {
                    (*codaPronti)->prox = nuovoOrdine;
                }
                (*codaPronti) = nuovoOrdine;
            }
            else
            {
                if((*ordiniAttesa) == NULL)
                {
                    (*ordiniAttesa) = nuovoOrdine;
                }
                else
                {
                    (*codaAttesa)->prox = nuovoOrdine;
                }
                (*codaAttesa) = nuovoOrdine;
            }

            printf("accettato\n");
        }
    }
    else
    {
        printf("Errore nell'ordine");
    }
}

RiferRicetta * RiallocoHashRicette(RiferRicetta *hashRicette, int bucketNumRic)
{
    RiferRicetta * temp = calloc(bucketNumRic, sizeof(RiferRicetta));

    for(int i = 0; i < bucketNumRic / 2; i++)
    {
        if(hashRicette[i] != NULL && hashRicette[i]->nome[0] == '!')
        {
            free(hashRicette[i]->nome);
            free(hashRicette[i]);
        }
        else if(hashRicette[i] != NULL && hashRicette[i]->nome[0] != '!')
        {
            int index = FunzioneHashRicette(temp, hashRicette[i]->nome, bucketNumRic, 1);
            temp[index] = hashRicette[i];
        }
    }

    free(hashRicette);

    return temp;
}

RiferIngredienteMag * RiallocoHashMagazzino(RiferIngredienteMag * hashMagazzino, int bucketNumMag)
{
    RiferIngredienteMag * temp = calloc(bucketNumMag, sizeof(RiferIngredienteMag));

    for(int i = 0; i < bucketNumMag / 2; i++)
    {
        if(hashMagazzino[i] != NULL)
        {
            RiferIngredienteMag punt = hashMagazzino[i];
            RiferIngredienteMag pross = hashMagazzino[i];
            while(punt)
            {
                pross = pross->prox;
                
                int index = FunzioneHashMagazzino(temp, punt->nome, bucketNumMag, 1);

                punt->prox = temp[index];
                temp[index] = punt;

                punt = pross;
            }
        }
    }

    free(hashMagazzino);

    return temp;
}

RiferIngredienteRic InsIngrInListaRicetta(RiferIngredienteRic listaIngr, char *nomeIngr, int pesoIngrediente)
{
    RiferIngredienteRic nuovoIngrediente = malloc(sizeof(IngredienteRic));

    nuovoIngrediente->nome = malloc((strlen(nomeIngr) + 1) * sizeof(char));
    strcpy(nuovoIngrediente->nome, nomeIngr);
    nuovoIngrediente->peso = pesoIngrediente;
    nuovoIngrediente->prox = listaIngr;

    return nuovoIngrediente;
}

RifSingoloLotto InsInOrdScadenzaRicorsivo(RifSingoloLotto listaIngr, int pesoIngrediente, int scadIngrediente)
{
    if(listaIngr == NULL || listaIngr->scadenza >= scadIngrediente)
    {
        RifSingoloLotto nuovoIngr = malloc(sizeof(SingoloLotto));

        nuovoIngr->peso = pesoIngrediente;
        nuovoIngr->scadenza = scadIngrediente;
        nuovoIngr->prox = listaIngr;

        return nuovoIngr;
    }
    listaIngr->prox = InsInOrdScadenzaRicorsivo(listaIngr->prox, pesoIngrediente, scadIngrediente);
    return listaIngr;
}

RifOrdine InsOrdineInOrdineRicorsivo(RifOrdine listaOrd, RifOrdine nuovoOrdine, RifOrdine * coda)
{
    if(listaOrd == NULL || listaOrd->tempoArrivo >= nuovoOrdine->tempoArrivo)
    {
        if(listaOrd == NULL)    *coda = nuovoOrdine;
        nuovoOrdine->prox = listaOrd;
        return nuovoOrdine;
    }
    listaOrd->prox = InsOrdineInOrdineRicorsivo(listaOrd->prox, nuovoOrdine, coda);
    return listaOrd;
}

void DistruggiListaIngredientiRicetta(RiferIngredienteRic listaIngredienti)
{
    if(!(listaIngredienti == NULL)) DistruggiListaIngredientiRicetta(listaIngredienti->prox);
    if(listaIngredienti != NULL)    free(listaIngredienti->nome);
    free(listaIngredienti);
}

void DistruggiListaIngredientiMagazzino(RiferIngredienteMag listaIngredienti)
{
    if(listaIngredienti != NULL) DistruggiListaIngredientiMagazzino(listaIngredienti->prox);
    if(listaIngredienti != NULL)
    {
        if(listaIngredienti->listaLotti != NULL)    DistruggiListaLottiMagazzino(listaIngredienti->listaLotti);
        free(listaIngredienti->nome);
    }
    free(listaIngredienti);
}

void DistruggiListaLottiMagazzino(RifSingoloLotto listaLotti)
{
    RifSingoloLotto punt = listaLotti;
    while(listaLotti != NULL)
    {
        punt = listaLotti->prox;
        free(listaLotti);
        listaLotti = punt;
    }
}

int CalcolaPresenza(RiferIngredienteRic listaIngr, RiferIngredienteMag * hashMagazzino, int qttRicetta, int bucketNumMag)
{
    int flag = 1;

    while(listaIngr != NULL && flag)
    {
        if(listaIngr->rifInMagazzino == NULL)
        {
            int indexIngr;
            int chiaveCercata = 0;

            for(int i = 0; i < strlen(listaIngr->nome); i = i + 2)
            {
                chiaveCercata += listaIngr->nome[i];
            }

            indexIngr = FunzioneHashMagazzino(hashMagazzino, listaIngr->nome, bucketNumMag, 0);

            RiferIngredienteMag punt = hashMagazzino[indexIngr];

            if(punt)
            {
                while(punt && flag)
                {
                    if(chiaveCercata == punt->chiave && !strcmp(punt->nome, listaIngr->nome))
                    {
                        listaIngr->rifInMagazzino = punt;
                        punt = NULL;
                    }
                    else
                    {
                        punt = punt->prox;
                        if(punt == NULL)    flag = 0;
                    }
                }
            }
            else
            {
                flag = 0;
            }
        }

        if(flag)
        {
            if(listaIngr->rifInMagazzino->pesoTot < listaIngr->peso * qttRicetta)
            {
                flag = 0;
            }
        }


        listaIngr = listaIngr->prox;
    }

    return flag;
}

RiferIngredienteMag * AggiornaMagazzino(RiferIngredienteRic listaIngr, RiferIngredienteMag * hashMagazzino, int bucketNumMag, int qttRicetta)
{
    while(listaIngr != NULL)
    {
        int qttNecessaria = qttRicetta * listaIngr->peso;
        RiferIngredienteMag puntMagazzino = listaIngr->rifInMagazzino;
        RifSingoloLotto puntSinLotto = puntMagazzino->listaLotti;

        while(qttNecessaria > 0)
        {
            if(puntSinLotto->peso > qttNecessaria)
            {
                puntMagazzino->pesoTot -= qttNecessaria;
                puntSinLotto->peso -= qttNecessaria;
                qttNecessaria = 0;
            }
            else if(puntSinLotto->prox == NULL && puntSinLotto->peso == qttNecessaria)
            {
                puntMagazzino->pesoTot = 0;
                qttNecessaria = 0;
                free(puntSinLotto);
                puntMagazzino->listaLotti = NULL;
            }
            else
            {
                puntMagazzino->pesoTot -= puntSinLotto->peso;
                qttNecessaria -= puntSinLotto->peso;
                puntMagazzino->listaLotti = puntSinLotto->prox;
                free(puntSinLotto);
                puntSinLotto = puntMagazzino->listaLotti;
            }
        }

        listaIngr = listaIngr->prox;
    }

    return hashMagazzino;
}

RifOrdine Corriere(RifOrdine ordiniPronti, RifOrdine * codaPronti, int capienzaCorriere)
{
    RifOrdine fine = ordiniPronti;
    int lunArray = 1;
    int pesoOrdini = ordiniPronti->quantt * ordiniPronti->ricetta->pesoTot;

    if(pesoOrdini > capienzaCorriere)
    {
        printf("camioncino vuoto\n");
        return ordiniPronti;
    }

    while(fine->prox != NULL && (pesoOrdini + fine->prox->quantt * fine->prox->ricetta->pesoTot) <= capienzaCorriere)
    {
        pesoOrdini = pesoOrdini + fine->prox->quantt * fine->prox->ricetta->pesoTot;
        fine = fine->prox;
        lunArray++;
    }

    RifOrdine * puntDaCaricare = calloc(lunArray, sizeof(RifOrdine));

    for(int i = 0; i < lunArray; i++)
    {
        puntDaCaricare[i] = ordiniPronti;
        ordiniPronti = ordiniPronti->prox;
    }
    fine->prox = NULL;

    if(lunArray > 1)
    {
        puntDaCaricare = MergeSortDecrescente(puntDaCaricare, 0, lunArray - 1);
    }

    for (int i = 0; i < lunArray; i++)
    {
        printf("%d %s %d\n", puntDaCaricare[i]->tempoArrivo, puntDaCaricare[i]->ricetta->nome, puntDaCaricare[i]->quantt);
        free(puntDaCaricare[i]);
    }
    
    (*codaPronti) = AggiornaCoda(ordiniPronti);

    free(puntDaCaricare);

    return ordiniPronti;
}

RifOrdine * MergeSortDecrescente(RifOrdine *array, int p, int r)
{
    if(p < r - 1)
    {
        int q = floor((p + r) / 2);
        array = MergeSortDecrescente(array, p, q);
        array = MergeSortDecrescente(array, q + 1, r);
        array = MergeDecrescente(array, p, q, r);
    }
    else
    {
        if(array[p]->ricetta->pesoTot * (array[p])->quantt < array[r]->ricetta->pesoTot * (array[r])->quantt)
        {
            RifOrdine tmp = array[r];
            array[r] = array[p];
            array[p] = tmp;
        }
    }

    return array;
}

RifOrdine * MergeDecrescente(RifOrdine *array, int p, int q, int r)
{
    int lun1 = q - p + 1;
    int lun2 = r - q;
    RifOrdine * left = malloc(lun1 * sizeof(RifOrdine));
    RifOrdine * right = malloc(lun2 * sizeof(RifOrdine));

    for(int i = 0; i < lun1; i++)
    {
        left[i] = array[p + i];
    }
    for(int i = 0; i < lun2; i++)
    {
        right[i] = array[q + i + 1];
    }

    int i = 0, j = 0, k = p;

    while (i < lun1 && j < lun2) {
        if (left[i]->quantt * left[i]->ricetta->pesoTot >= right[j]->quantt * right[j]->ricetta->pesoTot)
        {
            array[k] = left[i];
            i++;
        }
        else
        {
            array[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < lun1) {
        array[k] = left[i];
        i++;
        k++;
    }

    while (j < lun2) {
        array[k] = right[j];
        j++;
        k++;
    }

    free(left);
    free(right);

    return array;
}
