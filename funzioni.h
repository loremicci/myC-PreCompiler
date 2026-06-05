#pragma once
/* #ifndef FUNZIONI_H
#define FUNZIONI_H
...
#endif */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

// ::::::::::::::::::::::::::::::::::::::: STATISTICHE ::::::::::::::::::::::::::::::::::::::::::::::

// informazioni della variabile di una file
typedef struct{
    char* name;     // identificatore
    int first_err;  // indice della prima occorrenza d'errore
    int row;        // riga in cui è stata trovata
} Var;

// informazioni della lista di variabili
typedef struct{
    Var *vars;      // puntatore al blocco di variabili
    int len;        // lunghezza della lista
} List_var;

// informazioni del file
typedef struct{
    char* name;     // nome del file
    unsigned size;  // peso in byte del file
    int num_row;    // numero righe
    int num_com;    // numero commenti
    Var** vars;     // puntatore ai puntatori delle variabili
    int num_var;    // numero delle variabili
} File;

// informazioni delle statistiche
typedef struct{
    File** files;   // puntatore ai puntatori dei file con le relative informazioni
    int num_files;  // numero file
} Stats;

// informazioni dei moduli
typedef struct{
    char* input;    // nome del file di input
    char* output;   // nome del file di output
    int verbose;   // flag per calcolare le statistiche
} Mods;

/**
 * @brief Inizializza la struct delle statistiche.
 * @return La struct delle statistiche.
 */
Stats* initialize_stats();

/**
 * @brief Inizializza la struct dei moduli dell'eseguibile.
 * @param argc Il numero degli argomenti dell'eseguibile.
 * @param args Gli argomenti dell'eseguibile.
 * @return Il puntatore alla struct dei moduli.
 */
Mods* initialize_mod(int argc, char* args[]);

/**
 * @brief Inizializza la struct per un file.
 * @return Il puntatore alla struct del file.
 */
File* initialize_file();

/**
 * @brief Aggiunge la variabile al file.
 * @param f Il file a cui aggiungere la variabile
 * @param v La variabile da aggiungere.
 */
void add_var(File* f, Var* v);

/**
 * @brief Aggiunge il file alle statistiche.
 * @param sts Le statistiche.
 * @param f Il file da aggiungere alle statistiche.
 */
void add_file(Stats* sts, File* f);

/**
 * @brief Stampa il testo alla destinazione se presente, altrimenti stampa il testo sullo stdout.
 * @param dest Il file di destinazione.
 * @param res Il testo da stampare.
 */
void print_output(const char* dest, const char* res);

/**
 * @brief Stampa le statistiche.
 * @param sts Le statistiche da stampare.
 * @param res Il testo dell'output.
 */
void print_statistics(Stats* sts, const char* res);


// ::::::::::::::::::::::::::::::::::::::: ERRORI :::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @brief Stampa il possibile errore ed interrompe l'esecuzione.
 */
void errors();

/**
 * @brief Stampa il possibile errore durante l'operazione su un file ed interrompe l'esecuzione.
 * @param f Il file in input.
 */
void file_errors(FILE* f);


// ::::::::::::::::::::::::::::::::::::::: UTILITA` :::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @brief Conta il numero di cifre del valore in input.
 * @param n Il valore in input.
 * @return Il numero di cifre del valore in input.
 */
unsigned num_digits(int n);

/**
 * @brief Confronta le due stringhe in input.
 * @param str1 La prima stringa.
 * @param str2 La seconda stringa.
 * @return 1 se le stringhe in input sono uguali, 0 altrimenti.
 */
int is_equal(const char *str1, const char *str2);

/**
 * @brief Confronta le due stringhe in input.
 * @param str1 La prima stringa.
 * @param str2 La seconda stringa.
 * @param n Il numero di caratteri da confrontare.
 * @return 1 se i primi n caretteri delle stringhe in input sono uguali, 0 altrimenti.
 */
int is_n_equal(const char* str1, const char* str2, int n);

/**
 * @brief Elimina in loco i caratteri vuoti iniziali e finali della stringa in input.
 * @param string La stringa in input.
 */
void strstrip(char **string);

/**
 * @brief Rimuove dal testo in input le righe senza contenuto.
 * @param text Il testo in input.
 */
void file_compacter(char **text);


// ::::::::::::::::::::::::::::::::::::::: BASE FILE ::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @brief Apre il file al percorso in input in modalità lettura.
 * @param path Il percorso del file da aprire.
 * @return Il puntatore al file aperto.
 */
FILE *file_open_read(const char *path);

/**
 * @brief Ritorna il numero di byte del file.
 * @param f Il file in input.
 * @return Il numero di byte del file.
 */
unsigned file_size(FILE *f);

/**
 * @brief Legge il testo del file in input.
 * @param f Il file in input.
 * @return Il testo del file in input.
 */
char *file_text(FILE *f);

/**
 * @brief Apre il file al percorso in input in modalità scrittura.
 * @param path Il percorso del file da aprire.
 * @return Il puntatore al file aperto.
 */
FILE *file_open_write(const char*path);

/**
 * @brief Scrive il testo in input nel file in input.
 * Se il file già conteneva del testo viene sovrascritto.
 * @param f Il file di destinazione.
 * @param text Il testo da inserire nel file.
 */
void file_write(FILE *f, const char *text);

/**
 * @brief Chiude il file al percorso in input.
 * @param path Il percorso del file da chiudere.
 */
void file_close(FILE *f);


// ::::::::::::::::::::::::::::::::::::::: FILE AVANZATI ::::::::::::::::::::::::::::::::::::::::::::
/**
 * @brief Elimina i commenti secondo lo standard di C dal testo in input.
 * @param text Il testo in input.
 * @param verbose Indica se aggiornare o meno le statistiche.
 * @return Il numero di righe commentate.
 */
unsigned file_del_comments(char **text, int verbose);

/**
 * @brief Rileva se il file da includere è già stato incluso in precedenza.
 * @param name nome del file.
 * @return 1 se il file era già stato incluso, 0 altrimenti. 
 */
int is_already_included(Stats *sts, const char *name);

/**
 * @brief Risolve gli include "file.h".
 * @param sts Le statistiche.
 * @param path Il percorso del file da includere.
 * @param verbose Indica se aggiornare o meno le statistiche.
 */
char *file_include(Stats *sts, const char* path, int verbose);


// ::::::::::::::::::::::::::::::::::::::: CHECK VARIABILI ::::::::::::::::::::::::::::::::::::::::::
/**
 * @brief Prende la prima istuzione del testo partendo dall'indice in input e lo aggiorna.
 * @param text Il testo in input.
 * @param index L'indice in input.
 * @return La prima istruzione del testo partendo dall'indice in input.
 */
char* get_instruction(const char* text, int* index);
/**
 * @brief Prende la prima parola minuscola del testo partendo dall'indice in input e lo aggiorna.
 * @param text Testo in input.
 * @param index L'indice in input.
 * @return La prima parola minuscola del testo partendo dall'indice in input.
 */
char* get_word(const char* text, int* index, int* row);
/**
 * @brief Prende l'indice dell'ultimo tipo dell'istruzione in input.
 * @param text L'istruzione in input.
 * @return L'indice dell'ultimo tipo dell'istruzione in input.
 */

int is_cast(const char* instr, int i, int len);

int index_type(const char* instr, int* row);
/**
 * @brief Crea la lista delle variabili dell'istuzione in input.
 * @param filename Il nome del file dell'istruzione.
 * @param instr L'istruzione dalla quale prendere le variabili.
 * @return La lista delle variabili dell'istruzione in input.
 */
List_var* get_variables(const char* instr, int* row);
/**
 * @brief Rileva se la parola in input è una keyword di C.
 * @param word Parola in input.
 * @return 1 se la parola in input è una keyword di C, 0 altrimenti. 
 */
int is_keyword(const char* word);
/**
 * @brief Controlla l'identificatore in input.
 * @param filename Il nome del file della variabile.
 * @param var La variabile da controllare.
 */
int check_variable(const char *name);
/**
 * @brief Analizza il testo del file in input ad aggiorna le statistiche.
 * @param filename Nome del file in input.
 * @param file_text Il testo in input.
 * @return Il numero di righe del file.
 */
int check_file(File* f, const char* file_text);