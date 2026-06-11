<div align="center">
  <h1>⚙️ C-PreCompiler</h1>
  
  <p>
    <img src="https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white" alt="C" />
    <img src="https://img.shields.io/badge/GCC-A100FF?style=for-the-badge&logo=gnu&logoColor=white" alt="GCC" />
    <img src="https://img.shields.io/badge/CLI_Tool-4D4D4D?style=for-the-badge&logo=gnubash&logoColor=white" alt="CLI" />
  </p>

  <p><i>Un tool a riga di comando scritto in C che effettua il parsing, il pre-processamento e l'analisi di base di file sorgente C.</i></p>
</div>

Il programma funge da pre-compilatore in grado di risolvere ricorsivamente le dipendenze, rimuovere i commenti, compattare il codice e validare la sintassi degli identificatori delle variabili.

Il programma legge un file di input, risolve le direttive `#include` locali (ignorando le librerie standard), elimina i commenti (singola e multi-riga), rimuove le righe vuote e produce un output pulito, stampandolo a schermo o salvandolo in un file. Include inoltre un sistema di reportistica visiva molto dettagliato per l'analisi del codice.

---

## 🎯 Panoramica

Basato sull'analisi del codice sorgente fornito, questo tool offre le seguenti funzionalità:

* **Risoluzione delle Dipendenze:** Cerca e analizza ricorsivamente le direttive `#include "..."`. Unisce il contenuto dei file inclusi all'interno del flusso principale, ignorando deliberatamente le inclusioni di librerie standard (quelle con parentesi angolari `<...>`). È dotato di una protezione integrata per evitare inclusioni multiple dello stesso file.
* **Rimozione dei Commenti:** Riconosce e rimuove automaticamente tutti i commenti a singola riga (`//`) e i blocchi multi-riga (`/* ... */`).
* **Compattamento del Codice:** Elimina le righe vuote o prive di contenuto testuale generate dopo la rimozione dei commenti o l'inclusione dei file.
* **Validazione della Sintassi delle Variabili:** Isola le dichiarazioni di variabili, ignorando costrutti come i cast espliciti, e verifica che gli identificatori rispettino gli standard del C (iniziare con lettera o underscore, contenere solo caratteri alfanumerici, non superare i 31 caratteri e non essere *keyword* riservate).
* **Output Flessibile:** Genera il file pre-processato in un file di destinazione (tramite `-o`/`--out=`) oppure stampa direttamente su `stdout`.
* **Statistiche Verbose (ad Albero):** Tramite il flag `-v`/`--verbose`, il programma stampa su `stderr` una struttura ad albero molto dettagliata che mostra le dimensioni, le righe e i commenti rimossi per ogni file analizzato, oltre a un resoconto preciso delle variabili corrette e degli errori rilevati (puntando all'errore specifico con una grafica a freccia `^~~~`).

---

## 📂 Struttura del Progetto

```text
C-PreCompiler/
├── funzioni.c      # Logica core (lettura, stringhe, parser, statistiche)
├── funzioni.h      # Strutture dati, prototipi e costanti
├── main.c          # Entry point e coordinamento
├── README.md       # Documentazione
├── test1.c         # File sorgente di test
├── test1.h         # Header di test
├── test2.c         # File sorgente di test
├── test2.h         # Header di test
└── test2.1.h       # Header di test (inclusioni multiple)
```

---

## 🧩 Componenti Principali

Il progetto è suddiviso in un file principale e una libreria dedicata per una gestione modulare della logica.

* **`main.c`**: Il punto d'ingresso dell'applicazione. Si occupa dell'inizializzazione delle strutture dati, del coordinamento delle funzioni principali (lettura, compattamento, stampa) e della liberazione finale della memoria allocata dinamicamente.
* **`funzioni.c`**: Contiene tutta la logica "core" del programma. Implementa la lettura/scrittura dei file su disco, l'elaborazione del testo (taglio spazi, rimozione commenti), l'algoritmo ricorsivo per la risoluzione degli `#include`, l'analisi grammaticale per estrarre le istruzioni e la generazione delle statistiche a schermo.
* **`funzioni.h`**: L'header file che definisce le `struct` fondamentali (`Stats`, `Var`, `List_var`, `File`, `Mods`), i prototipi delle funzioni e le costanti (come i tipi di dato e le keyword di sistema del linguaggio C).

---

## ⚙️ Funzionalità Principali

La logica del tool ruota attorno a diverse funzioni chiave situate in `funzioni.c`:

* **`file_include(Stats* sts, const char* path, int verbose)`**: È la funzione principale per l'analisi e l'assemblaggio del codice. Apre i file, richiama la pulizia dei commenti, invia il file al controllo delle variabili e cerca la parola chiave `#include`. Se trova un file locale (racchiuso tra virgolette doppie), controlla che non sia già stato incluso (tramite `is_already_included`) e si richiama ricorsivamente.
* **`file_del_comments(char** text, int verbose)`**: Attraversa il testo carattere per carattere, utilizzando un sistema a "stati" (`skip1`, `skip2`) per scartare in sicurezza tutto il testo racchiuso all'interno di definizioni di commento.
* **`file_compacter(char** text)`**: Ripassa il testo ripulito per accorparlo, identificando ed eliminando i ritorni a capo (`\n`) appartenenti a righe divenute vuote o contenenti solo spazi.

**Validazione:**

* **`check_file(...)`** e **`get_variables(...)`**: Leggono il file istruzione per istruzione (fino al `;`). Isolano i tipi di dato (assicurandosi che non siano all'interno di espressioni di cast come `(int)`) ed estraggono i nomi delle variabili gestendo anche dichiarazioni multiple divise da virgola.
* **`check_variable(...)`**: Esegue l'effettiva validazione sintattica dell'identificatore restituendo `-1` se corretto o l'indice del carattere errato se viola le regole del linguaggio C.

**Reportistica:**

* **`print_statistics(...)`**: Formatta in modo visuale i dati raccolti, creando alberi testuali per illustrare chiaramente la dipendenza dei file inclusi e tracciando l'esatta riga e l'offset degli errori di sintassi scoperti.

---

## 💻 Come Eseguire il Progetto

Compila il progetto utilizzando `gcc`:

```bash
gcc -o myPreCompiler main.c funzioni.c -Wall -Wextra

```

Esegui il programma utilizzando gli argomenti supportati (sia in forma breve che a doppio trattino).

* Per processare un file e stampare l'output su `stdout`:

```bash
./myPreCompiler -i nome_file.c
# oppure
./myPreCompiler --in=nome_file.c

```

* Per processare un file e salvare il risultato in un altro file:

```bash
./myPreCompiler -i nome_file.c -o output.c
# oppure
./myPreCompiler --in=nome_file.c --out=output.c

```

* Per generare il file di output e visualizzare l'albero delle statistiche e degli errori:

```bash
./myPreCompiler -i nome_file.c -o output.c -v
# oppure
./myPreCompiler --in=nome_file.c --out=output.c --verbose

```

---

## 🧰 Requisiti

* Compilatore C (es. `gcc` o `clang`)
* Librerie standard C (in particolare `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<errno.h>`, `<ctype.h>`)
* Sistema operativo compatibile con terminale e file system standard (Unix-like consigliato per via dei font utilizzati nell'albero testuale).

---

## 👥 Autori

* Lorenzo Micci
* Filippo Pierbattisti
* Lorenzo Mercuri