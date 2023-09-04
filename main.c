#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Faccio un bst non bilanciato per contenere le stazioni di servizio

//Struttura per definire una stazione, che sarà un nodo dell'albero
struct Nodo_Stazione{
    int distanza;
    int num_veicoli;
    int autonomie[512];
    int max_autonomia;
    struct Nodo_Stazione* left; //Puntatore al sottoalbero sinistro
    struct Nodo_Stazione* right; //Puntatore al sottoalbero destro
    struct Nodo_Stazione* parent; //Puntatore al padre
};

typedef struct Nodo_Stazione Stazione; //Stazione è un tipo definito dalla struttura sopra
typedef Stazione* bst_node; //bst_node è un tipo che punta alla radice del bst

//PROTOTIPI DELLE FUNZIONI UTILIZZATE:

//FUNZIONI PRINCIPALI:
void insert_stazione(bst_node *radice, bst_node stazione);
void destroy_stazione(bst_node *radice, bst_node stazione);
void insert_auto(bst_node stazione, int autonomia, int posizione);
void destroy_auto(bst_node stazione, int autonomia, int posizione);
void plan_route_andata(bst_node array_stazioni[], int* index, bst_node arrivo, int* route, int* dim_route);
void plan_route_ritorno(bst_node array[], int dim_array, int* route, int* dim_route);

//FUNZIONI AUSILIARIE:
bst_node create_stazione(int distanza, int num_veicoli, const int* autonomie_veicoli);
bst_node search_stazione(bst_node root, int distanza_cercata);
bst_node search_minimum(bst_node root);
bst_node tree_successor(bst_node stazione);
int binary_search_inserimento(int array[], int sx, int dx, int veicolo);
int binary_search_rottamazione(int array[], int sx, int dx, int veicolo);
void inorder_tree_walk(bst_node radice, bst_node *array, int *indice, int distanza_minima, int distanza_massima);
void quick_sort(int A[], int sx, int dx);
int partition(int A[], int sx, int dx);
void swap(int *a, int *b);
void free_tree(bst_node* root);

int main() {
    char command[20];
    int distanza, num_veicoli, autonomia;
    int distanza1, distanza2;
    int contatore_stazioni = 0;
    int autonomie[512];
    bst_node radice = NULL;

    while (scanf("%s", command) != EOF) {
        if (strcmp(command, "aggiungi-stazione") == 0) {
            if (scanf("%d %d", &distanza, &num_veicoli) != EOF) {
                for (int i = 0; i < num_veicoli; i++) {
                    if (scanf("%d", &autonomie[i]) != EOF); //Per non avere warning
                }
                bst_node stazione_Cercata = search_stazione(radice,distanza); //Prima cerco se esiste già una stazione nella distanza indicata
                if (stazione_Cercata == NULL) { //Se NON esiste, la creo e la aggiungo
                    bst_node nuova_Stazione = create_stazione(distanza, num_veicoli, autonomie);
                    insert_stazione(&radice, nuova_Stazione);
                    contatore_stazioni++;
                } else //Altrimenti stampo direttamente e non alloco memoria inutilmente
                    printf("non aggiunta\n");
            }
        }

        if (strcmp(command, "demolisci-stazione") == 0) {
            if (scanf("%d", &distanza) != EOF) {
                bst_node stazione_Cercata = search_stazione(radice, distanza);
                if (stazione_Cercata == NULL)
                    printf("non demolita\n");
                else {
                    destroy_stazione(&radice, stazione_Cercata);
                    contatore_stazioni--;
                }
            }
        }

        if (strcmp(command, "aggiungi-auto") == 0) {
            if (scanf("%d %d", &distanza, &autonomia) != EOF) {
                bst_node stazione_Cercata = search_stazione(radice, distanza);
                if (stazione_Cercata == NULL)
                    printf("non aggiunta\n");
                else {
                    if (autonomia >= stazione_Cercata->max_autonomia || stazione_Cercata->num_veicoli == 0) { //Se l'autonomia da inserire è >= alla max aut corrente, posso inserirla direttamente alla fine senza cercare
                        stazione_Cercata->num_veicoli++; //Creo lo spazio per l'inserimento, aggiornando il numero di veicoli presenti nella stazione
                        stazione_Cercata->max_autonomia = autonomia; //Aggiorno la max aut
                        stazione_Cercata->autonomie[stazione_Cercata->num_veicoli-1] = autonomia; //Inserisco
                        printf("aggiunta\n");
                    }

                    else { //Se l'autonomia da inserire non è >= alla max aut corrente, devo cercare dove inserirla per mantenere l'ordinamento
                        int posizione = binary_search_inserimento(stazione_Cercata->autonomie, 0,stazione_Cercata->num_veicoli - 1, autonomia);
                        insert_auto(stazione_Cercata, autonomia, posizione);
                    }
                }
            }
        }

        if (strcmp(command, "rottama-auto") == 0) {
            if (scanf("%d %d", &distanza, &autonomia) != EOF) {
                bst_node stazione_Cercata = search_stazione(radice, distanza);
                if (stazione_Cercata == NULL) //Se non esiste la stazione a distanza indicata
                    printf("non rottamata\n");
                else {
                    if (autonomia == stazione_Cercata->max_autonomia) { //Se l'elemento da eliminare è il massimo nell'array di autonomie, non ho bisogno di cercare la sua posizione
                        stazione_Cercata->num_veicoli--; //Aggiorno il numero di veicoli presenti nella stazione, in questo modo elimino l'ultimo veicolo dal parco auto
                        stazione_Cercata->max_autonomia = stazione_Cercata->autonomie[(stazione_Cercata->num_veicoli) - 1]; //Aggiorno la max aut all'atuale ultimo elemento dell'array
                        printf("rottamata\n");
                    }
                    else {  //Se la stazione esiste e l'auto da rottamare non è quella con max aut, eseguo il binary search nel suo array di autonomie fino a trovare la sua posizione
                        //e chiamo la funzione destroy_auto
                        int posizione = binary_search_rottamazione(stazione_Cercata->autonomie, 0,stazione_Cercata->num_veicoli, autonomia);
                        if (posizione != -1) {
                            destroy_auto(stazione_Cercata, autonomia, posizione);
                        }
                        else //Caso in cui la stazione esiste ma non c'è nessun automobile con autonomia indicata nel parco auto di essa
                            printf("non rottamata\n");
                    }
                }
            }
        }

        if (strcmp(command, "pianifica-percorso") == 0) {
            if (scanf("%d %d", &distanza1, &distanza2) != EOF) {
                if (distanza1 == distanza2)
                    printf("%d\n", distanza1);
                else {
                    int indice = 0;
                    bst_node array_stazioni[contatore_stazioni]; //Dichiara un array per contenere tutte le stazioni fra partenza e arrivo
                    int dim_route = 0, index = 0;
                    if (distanza1 < distanza2) { //Andata
                        inorder_tree_walk(radice, array_stazioni, &indice, distanza1,distanza2); //riempie l'array dichiarato prima
                        int route[indice];
                        plan_route_andata(array_stazioni, &index, array_stazioni[indice - 1], route, &dim_route);

                    }
                    else { //Ritorno
                        inorder_tree_walk(radice, array_stazioni, &indice, distanza2,distanza1); //riempie l'array dichiarato prima
                        int route[indice];
                        plan_route_ritorno( array_stazioni, indice, route, &dim_route);
                    }
                }
            }
        }
    }

    free_tree(&radice);

}

//Funzione per inserire una nuova stazione nel bst, adattata dal pseudocodice visto a lezione
void insert_stazione(bst_node *radice, bst_node stazione) {
    bst_node y = NULL;
    bst_node x = *radice; //Variabile locale per contenere la radice e scendere nell'albero
    while(x != NULL){
        y = x; //Quando x = NULL, y non va aggiornato, quindi resta con il valore del parent della stazione da inserire
        if (stazione->distanza < x->distanza)
            x = x->left;
        else
            x = x->right;
    } //Esco dal while quando x=NULL, cioè, quando ho trovato il punto in cui inserire il nuovo nodo
    stazione->parent = y; //y è il parent di stazione
    if (y == NULL){
        *radice = stazione; //Se l'albero è vuoto, inserisco la stazione come radice dell'albero
    }
    else if (stazione->distanza < y->distanza){
        y->left = stazione; //Inserisco a sinistra del padre
    }
    else
        y->right = stazione; //Inserisco a destra del padre

    printf("aggiunta\n");
}

//Funzione per distruggere una stazione nel bst, adattata dal pseudocodice visto a lezione
void destroy_stazione(bst_node *radice, bst_node stazione){
    bst_node y = NULL; //Inizializzo y
    bst_node x = NULL; //Inizializzo x
    //Se stazione non ha figli o ne ha uno solo, allora stazione è il nodo effettivamente da eliminare
    if (stazione->left == NULL || stazione->right == NULL) {
        y = stazione;
    }
    //Se stazione ha due figli, non elimino il nodo stazione ma il suo successore (dopo aver copiato i suoi attributi nel nodo stazione)
    else {
        y = tree_successor(stazione);
    }
    //Sistemo i puntatori:
    //Se y ha figlio sx, x è il nodo che prende il posto di y
    if (y->left != NULL){
        x = y->left;
    }
    else {
        x = y->right; //NB: se y->right == NULL, x=NULL
    }
    //x prende il riferimento al padre di y
    if(x != NULL){
        x->parent = y->parent;
    }
    //Se y era radice, x diventa la nuova radice
    if (y->parent == NULL){
        *radice = x;
    }
    //Se y era figlio sx, x diventa figlio sx
    else if (y == y->parent->left){
        y->parent->left = x;
    }
    //Se y era figlio dx, x diventa figlio dx
    else {
        y->parent->right = x;
    }
    //Nel caso in cui sia da eliminare il successore di stazione, devo copiare i dati del successore (y) nel nodo stazione prima di deallocare y:
    if (y != stazione){
        stazione->distanza = y->distanza;
        stazione->num_veicoli = y->num_veicoli;
        stazione->max_autonomia = y->max_autonomia;
        memcpy(stazione->autonomie, y->autonomie, y->num_veicoli * sizeof(int)); //Copio in stazione->autonomie le autonomie di y
    }
    free(y);
    printf("demolita\n");
}

//Funzione per inserire un veicolo nel parco auto di una stazione, la inserisce nella posizione corretta per mantenere l'ordinamento dell'array
void insert_auto(bst_node stazione, int autonomia, int posizione){
    stazione->num_veicoli++; //Creo spazio per l'inserimento, aggiornando il numero di veicoli nella stazione
    //Sposto gli elementi dopo la posizione di inserimento a destra per fare spazio all'inserimento
    for (int i = (stazione->num_veicoli)-2; i >= posizione ; i--) {
        stazione->autonomie[i+1] = stazione->autonomie[i];
    }
    stazione->autonomie[posizione] = autonomia;
    printf("aggiunta\n");
}

//Funzione per rottamare un veicolo nel parco auto di una stazione, mantenendo l'ordinamento dell'array
void destroy_auto(bst_node stazione, int autonomia, int posizione){
    //Devo spostare tutti gli elementi dell'array di autonomie di una posizione a sx partendo dalla posizione che voglio eliminare
    //In pratica sovvrascrivo il valore dell'autonomia dell'auto da rottamare con il valore dell'autonomia dell'auto successivo nell'array,
    //vado avanti così fino a raggiungere la fine dell'array, cioè, fino a raggiungere la posizione num_veicoli-2
    int i;
    for (i = posizione; i < stazione->num_veicoli-1; i++) {
        stazione->autonomie[i] = stazione->autonomie[i+1]; //Prodecura eseguita fino al penultimo elemento dell'array,
        //Non è neccessario farla per l'ultimo perché non esiste nessun auto in posizione num_veicoli per sostituire l'auto rottamata (l'array va da 0 a num_veicoli-1)
    }
    stazione->num_veicoli--; //Aggiorno il numero di veicoli del parco auto della stazione
    printf("rottamata\n");
}

void plan_route_andata(bst_node array[], int* index, bst_node arrivo, int* route, int* dim_route){
    while(array[*index]->distanza != arrivo->distanza){
        int dove_arrivo_con_max_aut = array[*index]->distanza + array[*index]->max_autonomia; //Con la max aut arrivo a tutte le stazioni situate a distanza <= a questa
        if (dove_arrivo_con_max_aut >= arrivo->distanza){ //Se raggiungo l'arrivo, ho finito
            route[(*dim_route)++] = arrivo->distanza; //Aggiungo l'arrivo alla rotta
            int index2=0;
            plan_route_andata(array, &index2, array[*index], route, dim_route); //Faccio la stessa cosa ma ora considero l'arrivo come la stazione che ha trovato l'arrivo
            return;
        }
        else if (dove_arrivo_con_max_aut >= (array[*index+1])->distanza){
            (*index)++;
        }
        else {
            printf("nessun percorso\n");
            return;
        }
    }
    route[(*dim_route)++] = (*array)->distanza; //Aggiunge la partenza
    //Stampo il percorso:
    for (int j = *dim_route - 1; j > 0; j--) {
        printf("%d ", route[j]);
    }
    printf("%d", route[0]);
    printf("\n");
}

void plan_route_ritorno(bst_node array[], int dim_array, int* route, int* dim_route) {
    int p_array[dim_array];
    for (int i = 0; i < dim_array; i++) {
        p_array[i] = -1; //Inizializzo l'array che conterrà le posizioni dei padri
    }
    int num_hop[dim_array];
    for (int i = 0; i < dim_array; i++) {
        num_hop[i] = 0; //Numero di hop per arrivare ad ogni nodo
    }

    int current = dim_array-1;
    int dove_arrivo_con_max_aut;

    //Idea: penso ad ogni nodo corrente nell'array di stazioni come potenziale padre di tutti i nodi che raggiunge usando la sua massima autonomia
    while (current > 0) {
        dove_arrivo_con_max_aut = array[current]->distanza - array[current]->max_autonomia;
        for (int j = current-1; j >= 0; j--) { //Posizioni dei potenziali figli
            if (array[j]->distanza < dove_arrivo_con_max_aut) { //Non ho più potenziali figli da ispezionare
                break;
            }
            else { //Ispeziono i potenziali figli
                if (p_array[j] == -1){ //Se il potenziale figlio del nodo corrente non ha ancora un padre
                    p_array[j] = current; //Assegno la posizione del nodo corrente come padre di j
                    num_hop[j] = num_hop[current]+1; //Aggiorno il numero di hop necessari per arrivare a questo nodo partendo dal padre appena assegnato
                }
                else { //Se casco nell'else vuol dire che il potenziale figlio che sto analizzando ha già un padre, quindi devo capire se aggiornarlo o meno
                    if (num_hop[current] <= num_hop[p_array[j]]){ //Se il num_hop di questo potenziale padre è <= al num_hop del padre corrente, aggiorno il padre a current
                        p_array[j] = current;
                        //Non aggiorno il num_hop di questo nodo perché è già stato aggiornato dal vecchio padre
                    }
                }
            }
        }
        current--;
    }

    current = 0;

    route[(*dim_route)++] = array[0]->distanza; //Aggiungo l'arrivo all'array route
    while(current < dim_array-1){
        if (p_array[current] == -1){ //Se uno dei padri durante il ritorno è irraggiungibile, non riesco a ricostruire il percorso
            printf("nessun percorso\n");
            return;
        }
        else { //Altrimenti ricostruisco il percorso partendo dall'arrivo e guardando chi è il suo padre, poi chi è il padre del padre... e così via
            route[(*dim_route)++] = array[p_array[current]]->distanza;
            current = p_array[current];
        }
    }
    //Stampo il percorso:
    for (int j = *dim_route - 1 ; j > 0 ; j--) {
        printf("%d ", route[j]);
    }
    printf("%d", route[0]);
    printf("\n");
}

//FUNZIONI AUSILIARIE:

//Funzione per creare una nuova stazione
bst_node create_stazione(int distanza, int num_veicoli, const int *autonomie_veicoli) {
    bst_node nuova_Stazione = (bst_node) malloc(sizeof(Stazione));
    nuova_Stazione->distanza = distanza;
    nuova_Stazione->num_veicoli = num_veicoli;
    memcpy(nuova_Stazione->autonomie, autonomie_veicoli, num_veicoli * sizeof(int)); //Inizializzo l'array di autonomie con le autonomie indicate
    quick_sort(nuova_Stazione->autonomie, 0, (num_veicoli) - 1); //Faccio il quick-sort delle autonomie sempre che inserisco una nuova stazione nell'albero
    nuova_Stazione->max_autonomia = nuova_Stazione->autonomie[(num_veicoli) - 1];

    //Inizializzo tutti i puntatori della stazione a NULL, verrano risolti con le successive chiamate della funzione insert_stazione
    nuova_Stazione->left = NULL;
    nuova_Stazione->right = NULL;
    nuova_Stazione->parent = NULL;

    return nuova_Stazione;
}

//Funzione per cercare l'elemento minimo nell'albero, adattata dal pseudocodice visto a lezione
bst_node search_minimum(bst_node root) {
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

//Funzione per cercare il successore di una stazione nell'albero, adattata dal pseudocodice visto a lezione
bst_node tree_successor(bst_node stazione) {
    //Se il sottoalbero dx non è vuoto, il successore è il figlio più piccolo del sottoalbero dx
    if (stazione->right != NULL) {
        return search_minimum(stazione->right);
    }
    //Se il sottoalbero dx è vuoto, il successore è il primo elemento y che si incontra risalendo nell'albero dal nodo stazione tale che stazione è nel sottoalbero sx di y
    bst_node y = stazione->parent;
    while (y != NULL && stazione == y->right) {
        stazione = y;
        y = y->parent;
    }
    return y;
}

//Funzione per cercare un nodo dell'albero, adattata dal pseudocodice visto a lezione
bst_node search_stazione(bst_node root, int distanza_cercata) {
    if (root == NULL || distanza_cercata == root->distanza)
        return root;
    if (distanza_cercata < root->distanza)
        return search_stazione(root->left, distanza_cercata);
    if (distanza_cercata > root->distanza)
        return search_stazione(root->right, distanza_cercata);
    return root;
}

//Funzione per creare l'array ordinato di stazioni che userò per pianificare il percorso, adattata dal pseudocodice visto a lezione
void inorder_tree_walk(bst_node radice, bst_node *array, int *indice, int distanza_minima, int distanza_massima) {
    if (radice != NULL) {
        //Scende nell'albero fino alla stazione più piccola
        inorder_tree_walk(radice->left, array, indice, distanza_minima, distanza_massima);

        //Controlla se la distanza della stazione è compresa nell'intervallo desiderato
        if (radice->distanza >= distanza_minima && radice->distanza <= distanza_massima) {
            array[(*indice)++] = radice;
        }
        if (radice->distanza > distanza_massima) { //Se la stazione ha una distanza maggiore della distanza massima, non è necessario esplorare il sottoalbero destro
            return;
        }
        inorder_tree_walk(radice->right, array, indice, distanza_minima, distanza_massima);
    }
}

//Funzione di ricerca binaria per trovare la posizione di inserimento
int binary_search_inserimento(int array[], int sx, int dx, int veicolo) {
    while (sx <= dx) {
        int middle = sx + (dx-sx)/2;

        if (array[middle] == veicolo)
            return middle;

        if (array[middle] < veicolo)
            sx = middle + 1;
        else
            dx = middle - 1;
    }

    return sx; //Restituisce la posizione corretta per l'inserimento
}

//Funzione di ricerca binaria per trovare la posizione dell'auto da rottamare
int binary_search_rottamazione(int array[], int sx, int dx, int veicolo) {
    if (sx <= dx) {
        int middle = (sx+dx)/2;

        if (array[middle] == veicolo) {
            return middle;
        } else if (array[middle] < veicolo) {
            return binary_search_rottamazione(array, middle + 1, dx, veicolo);
        } else {
            return binary_search_rottamazione(array, sx, middle - 1, veicolo);
        }
    }
    return -1; //Elemento non trovato
}

//Funzione di quicksort e rispettive funzioni di supporto, adattate dai pseudocodici visti a lezione
void quick_sort(int A[], int sx, int dx) {
    if (sx < dx) {
        int pivot = partition(A, sx, dx);

        quick_sort(A, sx, pivot-1);
        quick_sort(A, pivot+1, dx);
    }
}

int partition(int A[], int sx, int dx) {
    int pivot = A[dx];
    int i = sx-1;

    for (int j = sx; j <= dx-1; j++) {
        if (A[j] <= pivot) {
            i++;
            swap(&A[i], &A[j]);
        }
    }

    swap(&A[i+1], &A[dx]);
    return i+1;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

//Funzione per deallocare la memoria dell'intero bst
void free_tree(bst_node *root) {
    if (*root != NULL) {
        free_tree(&((*root)->left)); //Deallocazione del sottoalbero sinistro
        free_tree(&((*root)->right)); //Deallocazione del sottoalbero destro
        free(*root); //Deallocazione del nodo corrente
        *root = NULL; //Imposta il puntatore della radice a NULL dopo la deallocazione
    }
}
