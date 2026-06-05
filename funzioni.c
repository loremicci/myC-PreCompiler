#include "funzioni.h"

// ::::::::::::::::::::::::::::::::::::::: STATISTICHE :::::::::::::::::::::::::::::::::::::::::::::::
/*  - numero di variabili controllate
    - numero di errori rilevati
    - per ogni errore rilevato, nome del file in cui e' stato rilevato e numero di riga nel file. 
    - numero di righe di commento eliminate
    - numero di file inclusi
    - per il file di input la dimensione in byte ed il numero di righe (pre processamento)
    - per ogni file incluso dimensione in byte e numero di righe (pre processamento)
    - numero di righe e dimensione dell'output */

Stats* initialize_stats(){
    Stats* sts = (Stats*) malloc(sizeof(Stats));
    errors();
    *sts = (Stats) {NULL, 0};
    return sts;
}

Mods* initialize_mod(int argc, char* args[]){
    Mods* mds = (Mods*) malloc(sizeof(Mods));
    errors();
    *mds = (Mods) {NULL, NULL, 0};

    if(argc < 1 || !args) return mds;
    
    /*  -i, --in (notazione doppio trattino) per specificare il file di input
        -o --out (notazione doppio trattino) per specificare il file di output
        -v, --verbose (notazione doppio trattino) per produrre o meno come output le statistiche di elaborazione */
    for(unsigned short i = 1; i < argc; i++){
        char *arg = args[i];
        
        if(is_equal(arg, "-v") || is_equal(arg, "--verbose")) mds->verbose = 1;
        else if(is_n_equal(arg, "--in=", 5)) mds->input = arg + 5;
        else if(is_n_equal(arg, "--out=", 6)) mds->output = arg + 6;
        else if(i < argc - 1 && is_equal(arg, "-i")) mds->input = args[++i];
        else if(i < argc - 1 && is_equal(arg, "-o")) mds->output = args[++i]; 
        else{
            printf("Invalid option: %s\n", args[i]);
            exit(EXIT_FAILURE);
        }   
    }
    return mds;
}

File* initialize_file(){
    File* f = (File*) malloc(sizeof(File));
    errors();
    *f = (File) {NULL, 0, 0, 0, NULL, 0};
    return f;
}

void add_var(File *f, Var *v){
    f->vars = (Var**) realloc(f->vars, (f->num_var + 1) * sizeof(Var*));
    errors();
    f->vars[f->num_var++] = v;
}

void add_file(Stats *sts, File *f){
    sts->files = (File**) realloc(sts->files, (sts->num_files + 1) * sizeof(File*));
    errors();
    sts->files[sts->num_files++] = f;
}

void print_output(const char* dest, const char* res){
    if(dest){
        FILE* f = file_open_write(dest);
        file_write(f, res); // se non ho risultato crea solo il file
        file_close(f);
    }
    else if(res) printf("%s", res); // stampa il risultato solo se non è NULL
}

void print_statistics(Stats* sts, const char* result){
    // stampo sempre le statistiche sullo stderr dato di default viene stampato lo stesso a schermo
    char *T = "├── ", *L = "└── ", *I = "│", *N = "";
    char *tree_brench, *tree_vert;

    fprintf(stderr, "STATISTICHE:\n");
    fprintf(stderr, "%d file inclusi:\n", sts->num_files);
    for(int i = 0; i < sts->num_files; i++){
        File* f = sts->files[i];
        
        tree_brench = i != sts->num_files - 1 ? T : L;
        tree_vert = i != sts->num_files - 1 ? I : N;

        fprintf(stderr, "\t%s", tree_brench);
        fprintf(stderr, "%s:\n", f->name);
        fprintf(stderr, "\t%s", tree_vert);
        fprintf(stderr, "\t├── %u byte occupati\n", f->size);
        fprintf(stderr, "\t%s", tree_vert);
        fprintf(stderr, "\t├── %d righe mantenute\n", f->num_row);
        fprintf(stderr, "\t%s", tree_vert);
        fprintf(stderr, "\t└── %d righe di commento eliminate\n", f->num_com);
    }
        
    fprintf(stderr, "\nOutput: ");
    if(!result) fprintf(stderr, "0 byte, 0 righe\n");
    else{
        int out_size = strlen(result), out_row = 0;
        for(int i = 0; i < out_size; i++) if(result[i] == '\n') out_row++;
        fprintf(stderr, "%d byte, %d righe\n", out_size, out_row);
    }
    
    int num_ok_tot = 0, num_err_tot = 0;
    for(int i = 0; i != sts->num_files; i++){
        File* f = sts->files[i];
        for(int j = 0; j != f->num_var; j++){
            Var* v = f->vars[j];
            if(v->first_err == -1) num_ok_tot++;
            else num_err_tot++;
        }
    }
    fprintf(stderr, "\n%d variabili controllate\n", num_ok_tot + num_err_tot);
    
    fprintf(stderr, "\n%d variabili corrette:\n", num_ok_tot);
    for(int i = 0; i < sts->num_files; i++){
        File* f = sts->files[i]; // i-esimo file

        tree_brench = i != sts->num_files - 1 ? T : L;
        fprintf(stderr, "\t%s", tree_brench);
        fprintf(stderr, "%s:\n", f->name);
        int num_ok = 0;
        for(int j = 0; j < f->num_var; j++){
            Var* v = f->vars[j]; // j-esima variabile dell'i-esimo file
            if(v->first_err == -1) num_ok++;
        }
        int c = 0;
        for(int j = 0; j < f->num_var; j++){
            Var* v = f->vars[j]; // j-esima variabile dell'i-esimo file
            if(v->first_err == -1){ // stampa le variabili giuste dell'i-esimo file incluso
                c++;
                tree_vert = i != sts->num_files - 1 ? I : N;
                fprintf(stderr, "\t%s", tree_vert);
                tree_brench = c < num_ok ? T : L;
                fprintf(stderr, "\t%s", tree_brench);
                fprintf(stderr, "riga %d, %s\n", v->row, v->name);
            }
        }
    }

    fprintf(stderr, "\n%d errori rilevati:\n", num_err_tot);
    for(int i = 0; i < sts->num_files; i++){
        File* f = sts->files[i];

        tree_brench = i != sts->num_files - 1 ? T : L;
        fprintf(stderr, "\t%s", tree_brench);
        fprintf(stderr, "%s:\n", f->name);
        
        int num_err = 0;
        for(int j = 0; j < f->num_var; j++){
            Var* v = f->vars[j]; // j-esima variabile dell'i-esimo file
            if(v->first_err != -1) num_err++;
        }

        int c = 0;
        for(int j = 0; j < f->num_var; j++){
            Var* v = f->vars[j]; // j-esima variabile dell'i-esimo file
            
            if(v->first_err != -1){ // stampa le variabili giuste dell'i-esimo file incluso
                c++;
                tree_vert = i != sts->num_files - 1 ? I : N;
                fprintf(stderr, "\t%s", tree_vert);
                tree_brench = c < num_err ? T : L;
                fprintf(stderr, "\t%s", tree_brench);
                fprintf(stderr, "riga %d, %s\n", v->row, v->name);

                // freccina
                tree_vert = i != sts->num_files - 1 ? I : N;
                fprintf(stderr, "\t%s", tree_vert);
                tree_vert = c < num_err ? I : N;
                fprintf(stderr, "\t%s\t ", tree_vert);
                int d_row = num_digits(v->row);
                for(int _ = 0; _ != d_row; _++) fprintf(stderr, " "); //%d
                fprintf(stderr, "  "); //, 
                for(int _ = 0; _ != v->first_err; _++) fprintf(stderr, " "); // %s
                fprintf(stderr, "^");
                int len_var = strlen(v->name) - 1;
                for(int _ = v->first_err; _ != len_var; _++) fprintf(stderr, "~");
                fprintf(stderr, "\n");
            }
        }
    }
}


// ::::::::::::::::::::::::::::::::::::::: ERRORI :::::::::::::::::::::::::::::::::::::::::::::::
void errors(){
    if(errno != 0){
        perror("");
        exit(EXIT_FAILURE);
    }
}

void file_errors(FILE *f){
    if(!f) return;

    if(ferror(f)){
        perror("");
        exit(EXIT_FAILURE);
    }
}


// ::::::::::::::::::::::::::::::::::::::: UTILITA' :::::::::::::::::::::::::::::::::::::::::::::::
unsigned num_digits(int n){
    unsigned count = 0;
    n = abs(n);
    while((n /= 10)) count++;
    return ++count;
}

inline int is_equal(const char *str1, const char *str2){
    if(!str1 || !str2) return 0; // se uno dei 2 argomenti è null strcmp da un errore di segmentazione
    return strcmp(str1, str2) == 0;
}

inline int is_n_equal(const char *str1, const char *str2, int n){
    if(!str1 || !str2) return 0; // se uno dei 2 argomenti è null strcmp da un errore di segmentazione
    return strncmp(str1, str2, n) == 0;
}

void strstrip(char **string){
    if(!(*string)){
        *string = (char*) malloc(1 * sizeof(char)); // inizializzato di default \0
        errors();
        return;
    }
    unsigned len = strlen(*string); // va fatto qui altrimenti è inconsistente
    if(len == 0) return;
    
    int start = 0, end = len - 1;
    while(end >= start && isspace((*string)[end])) end--;
    while(start <= end && (isspace((*string)[start]) || (*string)[start] == '*' || (*string)[start] == '&')) start++;
    if(end < start){
        free(*string);
        *string = (char*) malloc(sizeof(char));
        errors();
        *string = '\0';
        return;
    }
    
    unsigned read = end - start + 1;
    char *out = (char*) malloc((read + 1) * sizeof(char));
    errors();
    memcpy(out, (*string) + start, read);
    out[read] = '\0';
    free(*string);
    *string = out;
}

void file_compacter(char **text){
    if(!text || !*text) return;

    unsigned len = strlen(*text);
    char* out = (char*)malloc(len + 1);
    errors();

    unsigned start = 0, j = 0;
    int empty = 1;
    for(unsigned i = 0; i <= len; i++){
        char cur = (*text)[i];
        if(empty && (cur != '\0' && !isspace(cur))) empty = 0;
        else if(cur == '\n' || cur == '\0'){
            if(!empty){
                unsigned read = (i - start) + 1; // numero di caratteri della riga
                memcpy(out + j, (*text) + start, read * sizeof(char)); // appende a out i caratteri della riga
                j += read;
                empty = 1;
            }
            start = i + 1;
        }
    }
    if(empty) out[j++] = '\0';
    *text = (char*) realloc(out, j * sizeof(char));
    errors();
}


// ::::::::::::::::::::::::::::::::::::::: BASE FILE :::::::::::::::::::::::::::::::::::::::::::::::
FILE* file_open_read(const char *path){
    if(!path) return NULL;

    FILE* f = fopen(path, "r");
    errors();
    return f;
}

unsigned file_size(FILE* f){
    if(!f) return 0;

    fseek(f, 0, SEEK_END);
    unsigned size = ftell(f); // sposta il puntatore all'ultimo carattere del file
    file_errors(f);
    rewind(f); // sposta il puntatore al primo carattere del file
    return size;
}

char* file_text(FILE* f){
    if(!f) return NULL;

    int size = file_size(f);
    if(size == 0) return NULL;
    char* out = (char*) malloc(size + 1);
    errors();

    fread(out, sizeof(char), size / sizeof(char), f);
    file_errors(f);
    out[size] = '\0';
    return out;
}

FILE* file_open_write(const char *path){
    if(!path) return NULL;

    FILE* f = fopen(path, "w");
    errors();
    return f;
}

void file_write(FILE* f, const char *text){
    if(!f || !text) return;

    fprintf(f, "%s", text);
    file_errors(f);
}

void file_close(FILE *f){
    if(!f) return;

    fclose(f);
    errors();
}


// ::::::::::::::::::::::::::::::::::::::: FILE AVANZATI ::::::::::::::::::::::::::::::::::::::::::::
unsigned file_del_comments(char** text, int verbose){
    if(!text || !(*text)) return 0;

    unsigned len = strlen((*text));
    char* out = (char*) malloc(len + 1);
    errors();

    unsigned i = 0, j = 0, num_com = 0;
    int skip1 = 0, skip2 = 0;
    while(i < len){
        char cur = (*text)[i], nxt = (*text)[i+1];

        // salta le righe di commenti
        if(!skip1 && cur == '/' && nxt == '/'){
            skip1 = 1;
            i++;
        }
        else if(!skip2 && cur == '/' && nxt == '*'){
            skip2 = 1;
            i++;
        }
        
        // smette di saltare
        else if(skip1 && (cur == '\n' || cur == '\0')){
            skip1 = 0;
            if(verbose) num_com++;
            out[j++] = cur;
        }
        else if(skip2){
            if(cur == '*' && nxt == '/'){
                skip2 = 0;
                if(verbose) num_com++;
                i++;
            }
            else if(cur == '\n' || cur == '\0'){
                if(verbose) num_com++;
                out[j++] = cur;
            }
        }
        
        else if(!skip1 && !skip2) out[j++] = cur;

        i++;
    }
    out[j++] = '\0';
    (*text) = (char*) realloc(out, j * sizeof(char));
    errors();
    return num_com;
}

int is_already_included(Stats *sts, const char *name){
    if(!name) return 0;

    for(int i = 0; i < sts->num_files; i++) 
        if(is_equal(name, sts->files[i]->name)) return 1;

    return 0;
}

char* file_include(Stats* sts, const char* path, int verbose){
    if(!path) return NULL;

    File* f = initialize_file();

    FILE* file = file_open_read(path);
    char* text = file_text(file);
    unsigned size = file_size(file); // dimensione del file in byte
    file_close(file);

    f->name = strdup(path);
    f->size = size;
    f->num_com = file_del_comments(&text, verbose); // eliminiamo i commenti in ogni file e assegno il conteggio al file corrente
    f->num_row = verbose ? check_file(f, text) : 0; // controlla le variabili del file
    add_file(sts, f);

    unsigned len = size / sizeof(char); // numero di caratteri del file
    char* out = (char*) malloc(len + 1); // per lo \0 incluso
    errors();
    
    const char include[] = "#include";
    unsigned start = 0, i = 0, k = 0, len_include = sizeof(include) - 1; // non considero lo \0
    while(i < len){
        if((i + len_include) >= len || !is_n_equal(text + i, include, len_include)){
            out[k++] = text[i++];
            continue;
        }
        
        // se trova "#include", i = indice di '#'
        start = i;
        i += len_include; // salta #include
        while(i < len && text[i] == ' ') i++; // salta spazi
        if(text[i++] != '\"'){ // salta gli include delle librerie standard
            memcpy(out + k, text + start, i - start);
            k += i - start;
            continue; 
        }
        
        // trova un include valido
        char name[255]; // dimensione massima nome di un file in Linux
        unsigned j = 0;
        while(i < len && text[i] != '\"') name[j++] = text[i++]; // legge il nome fino a "
        name[j] = '\0';
        i++; // salta le "
        
        if(is_already_included(sts, name)) continue; // salto se ho già incluso il file

        // vado in ricorsione sui file da includere che mi ritornano il loro testo, da aggiungere al padre
        char *child_text = file_include(sts, name, verbose);
        if(child_text){ // se ho del testo lo aggiungo alla stringa
            unsigned new_size = strlen(child_text);
            out = (char*)realloc(out, len + new_size + 1);
            errors();
            memcpy(out + k, child_text, new_size);
            k += new_size;
        }
        free(child_text);
    }
    free(text);
    out[k] = '\0';
    return out;
}


// ::::::::::::::::::::::::::::::::::::::: CHECK VARIABILI ::::::::::::::::::::::::::::::::::::::::::
const char* types[] = {"int", "void", "long", "char", "short", "float", "double", "signed", "unsigned"};
const char* keywords[] = {
    "do", "if", "for", "auto", "case", "else", "goto", "break", "const", "while",
    "extern", "inline", "return", "sizeof", "static", "switch", "default", "typedef",
    "continue", "register", "volatile", "restrict"
};
const unsigned len_types = sizeof(types) / sizeof(types[0]), len_keys = sizeof(keywords) / sizeof(keywords[0]);

char* get_instruction(const char *text, int *index){
    if(!text || !index) return NULL;
    
    int start = (*index), len = strlen(text);
    while((*index) < len && text[*index] != ';') (*index)++; // scorre fino a ; o fine file
    // if(text[*index] == '\0') return NULL;

    int read = ++(*index) - start;
    if(read == 0) return NULL;
    
    // copia i caratteri letti
    char* out = strndup(text + start, read);
    errors();
    return out;
}

char* get_word(const char* text, int* index, int* row){
    if(!text) return NULL;
    if(!index) *index = 0;
    
    int len = strlen(text);
    while((*index) < len && !islower(text[*index])){
        if(text[(*index)] == '\n') (*row)++;
        (*index)++; // salta fino al primo carattere minuscolo
    }
    int start = (*index);
    while((*index) < len && islower(text[*index])) (*index)++; // scorre finchè trova caratteri minuscoli
    int read = (*index) - start;
    if(read == 0) return NULL;
    
    // copia i caratteri letti
    char* out = strndup(text + start, read);
    errors();
    return out;
}

int is_cast(const char *instr, int index, int len){
    if(!instr || index == 0 || len < 1) return 0;

    unsigned int start = (index - len) - 1, end = index;

    index = start;
    while(index >= 0 && isspace(instr[index])) index--; // mi sposto a sinistra
    if(instr[start] != '(') return 0; // se non ho nessuna parentesi a sx del tipo allora non è un cast

    index = end;
    while(isspace(instr[index])) index++; // mi sposto a destra
    if(instr[index] != ')') return 0; // se non ho nessuna parentesi a dx del tipo allora non è un cast
    
    return 1;
}

int index_type(const char* instr, int* row){
    if(!instr) return -1;
    
    int len = strlen(instr), i = 0;
    int last = -1;
    char* word;
    while((word = get_word(instr, &i, row))){ // un'istruzione può non iniziare con un tipo, ma una volta iniziata una lista di tipi basta 1 parola non tipo per interrompere
        for(int j = 0; types[j]; j++){ // controllo tipo
            if(is_equal(word, types[j])){
                if(!is_cast(instr, i, strlen(word))) last = i; // se il tipo non è un cast allora aggiorno last
                break;
            }
        }
    }
    return last;
}

List_var* get_variables(const char* instr, int* row){
    List_var* l_vars = (List_var*) malloc(sizeof(List_var));
    errors();
    *l_vars = (List_var) {NULL, 0};

    if(!instr) return l_vars;
    
    int i = index_type(instr, row);
    if(i == -1) return l_vars;

    int len = strlen(instr), start = i;
    int skip = 0;
    while(i < len){
        if(instr[i] == '\n') (*row)++;

        else if(!skip && (instr[i] == ',' || instr[i] == '=' || instr[i] == ';')){ // prende una variabile
            char* str_var = strndup(instr + start, i - start);
            errors();
            strstrip(&str_var);

            if(*str_var != '\0') {
                l_vars->vars = (Var*) realloc(l_vars->vars, (l_vars->len + 1) * sizeof(Var));
                errors();
                l_vars->vars[l_vars->len] = (Var) {strdup(str_var), -1, *row};
                errors();
                l_vars->len++;
            }
            free(str_var);
            start = i + 1;
        }
        
        if(instr[i] == '=') skip = 1;
        else if(instr[i] == ','){
            skip = 0;
            start = i + 1;
        }
        
        i++;
    }
    return l_vars;
}

int is_keyword(const char* word){
    if(!word) return 0;
    for(int i = 0; i < len_keys; i++) 
        if(is_equal(word, keywords[i])) return 1;
    return 0;
}

int check_variable(const char *name){
    /* - Il primo carattere in un identificatore deve essere una lettera o underscore ( _ ) e può essere seguito solo da qualsiasi lettera, numero o underscore;
       - Gli identificatori fanno distinzione tra maiuscole e minuscole;
       - Le virgole o gli spazi vuoti non sono consentiti all'interno di un identificatore;
       - Le parole chiave non possono essere utilizzate come identificatore;
       - Gli identificatori non devono avere una lunghezza superiore a 31 caratteri (facendo dei test invece sembra di si); */
    if(!name) return 0; // non ha nome
    
    int len_var = strlen(name);
    if(!isalpha(name[0]) && name[0] != '_' || len_var > 31 || is_keyword(name)) return 0; // errore all'inizio di nome
    for(int i = 1; i < len_var; i++) // prima lettera già controllata
        if(!isalnum(name[i]) && name[i] != '_') return i; // caratteri non ammessi all'interno del nome

    return -1; // variabile giusta
}

int check_file(File* f, const char* file_text){
    if(!f || !file_text) return 0;

    int i = 0, len = strlen(file_text), row = 1 ; // righe controllate
    while(i < len){
        char* instr = get_instruction(file_text, &i);
        if(!instr) continue;
        
        List_var* l_vars = get_variables(instr, &row);
        free(instr);
        
        if(!l_vars->vars) continue;

        for(int j = 0; j < l_vars->len; j++){
            Var* v = &l_vars->vars[j];
            v->first_err = check_variable(v->name);
            add_var(f, v);
        }
        free(l_vars);
    }
    return row;
}