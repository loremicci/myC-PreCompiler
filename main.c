#include "funzioni.h"

int main(int argc, char* args[]){
    Stats* sts = initialize_stats();
    Mods* mds = initialize_mod(argc, args);

    if(!mds->input) return 1; // nessun file in input (parametro obbligatorio)
    
    errno = 0;
    
    char *text = file_include(sts, mds->input, mds->verbose);
    file_compacter(&text);
    print_output(mds->output, text);

    if(mds->verbose) print_statistics(sts, text); // altrimenti stampa statistiche vuote
    
    /* liberazione risorse */
    free(text);
    free(mds);
    
    for(int i = 0; i != sts->num_files; i++){
        File* f = sts->files[i];
        for(int j = 0; j != f->num_var; j++){
            Var* v = f->vars[j];
            free(v->name);  // libero il nome della var
        }
        free(f->name);      // libero il nome del file
        free(f->vars);      // libero il puntatore alle variabili
    }
    free(sts->files);       // libero il puntatore ai file
    free(sts);
    
    /* gcc main.c funzioni.c
    ./a.out --in=test1.c -x -v > o.txt*/
    return 0;
}