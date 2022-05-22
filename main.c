#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "treemap.h"

typedef struct
{
    char title[100];
    int charCount;
    int wordCount;
    int uniqueWords;
    TreeMap * wordsFrecuency;
    char id[15];
} Book;

typedef struct
{
    char * word;
    int count;
    List * positions; //Data = char * position, works with a function to read the .txt
} NodeWF; //Node of Map * Words Frecuency

typedef struct{
    int position;
} Position;

//Función para comparar claves de tipo string, retorna 1 si son iguales
//Viene de Map.h
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}
long long stringHash(const void * key) {
    long long hash = 5381;
    const char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash = ((hash << 5) + hash) + tolower(*ptr); /* hash * 33 + c */
    }    
    return hash; 
}
int stringEqual(const void * key1, const void * key2) {
    const char * A = key1;
    const char * B = key2;
    return strcmp(A, B) == 0;
}
//-------------


//Viene de TreeMap.h
int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

void pressEnterToContinue()
{
    printf("\n(Presiona Enter para continuar.)\n");
    char enter = 0;
    while (enter != '\r' && enter != '\n')
    {
        enter = getchar();
    }
}

//Funcion que convierte las letras de un string a minusculas
void stringToLower(char * string)
{
    int i;
    for(i = 0 ; string[i] ; i++) string[i] = tolower(string[i]);
}

//Funcion que elimina los caracteres diferentes de letras en un string
void cleanWord(char * word)
{
    int i = 0;
    int k;
    int j = 0;
    printf("Checking [%s]\n", word);
    while(word[i] != '\0')
    {
        printf("Checking [%i] = %c\n", i, word[i]);
        if(isalpha(word[i]) == 0)
        {
            printf("Is not alpha\n");
            k = i;
            while(word[k+1] != '\0')
            {
                printf("Replacing %c with %c\n", word[k], word[k+1]);
                word[k] = word[k+1];
                k++;
            }
            word[k] = '\0';
            i--;
        }
        i++;
    }
}

void lowerAndClean(char * word)
{
    cleanWord(word);
    stringToLower(word);
}

//Lee la siguiente palabra
char * next_word (FILE *f) {
    char x[1024];
    /* assumes no word exceeds length of 1023 */
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
        return NULL;
}

char * assignFileName(char * id, char * fileName)
{
    printf("ID = %s\n", &id);
    strcat(fileName, id);
    strcat(fileName, ".txt");
    return fileName;
}

Book * createBook1(char * id, char * title)
{
    Book * newBook = (Book *) malloc (sizeof(Book));

    printf("Creando Map book->wordsFrecuency\n");
    newBook->wordsFrecuency = createTreeMap(lower_than_string);

    printf("Asignando id a book->id\n");
    strcpy(newBook->id, id);

    printf("Asignando Placeholder Title\n");
    strcpy(newBook->title, title);

    newBook->uniqueWords=0;

    return newBook;
}

Book * createBook2(FILE * file, int wordCount)
{
    Book * book = (Book *) malloc (sizeof(Book));
    book->charCount = ftell(file);
    book->wordCount = wordCount;
}

Position * getPos(FILE * file)
{
    Position * pos = (Position *) malloc (sizeof(Position));
    pos->position = ftell(file);
    printf("pos = [%i]\n", pos->position);
    return pos;
}

NodeWF * createNodeWF(char * word, Position * pos, Book * book)
{
    NodeWF * auxWord = (NodeWF *) malloc (sizeof(NodeWF));
    printf("auxWord creada\n");

    printf("Creando lista de posisiones\n");
    auxWord->positions = createList();

    if(auxWord->positions == NULL)
    {
        printf("No se pudo crear la Lista.\n");
    }
    else
    {
        printf("Lista creada\n");
        
        printf("Asignando auxWord->word\n");
        strcpy(auxWord->word, word);
        printf("auxWord->word = %s\n", auxWord->word);

        printf("Asignando cantidad = 1\n");
        auxWord->count = 1;

        printf("Position = %i\n", pos->position);
        printf("PositionPointer Value = %i\n", &pos->position);

        printf("Insertando a la lista\n");
        pushBack(auxWord->positions, pos);
        printf("Posicion insertada\n");

        printf("Insertando palabra al mapa\n");
        insertTreeMap(book->wordsFrecuency, word, auxWord);
        printf("Palabra insertada\n");

        printf("Palabra %s creada con cantidad %i\n", auxWord->word, auxWord->count);
    }
}

void * showCurrentWords(List * positions)
{
    printf("\nImprimiendo posiciones actuales en la lista\n");
    Position * auxPos = firstList(positions);
    while(auxPos!=NULL)
    {
        printf("Pos = %i\n", auxPos->position);
        auxPos = nextList(positions);
    }
}

NodeWF * foundWord(NodeWF * auxWord, Position * pos)
{
    printf("PALABRA ENCONTRADA, aumentando cantidad\n");
    auxWord->count++;
    printf("Cantidad = %i\n", auxWord->count);

    printf("Position = %i\n", pos->position);
    printf("PositionPointer Value = %i\n", &pos->position);

    printf("Insertando a la lista\n");
    pushBack(auxWord->positions, pos);
    printf("Posicion insertada\n");

    showCurrentWords(auxWord->positions);
    
}

void loadBooks(TreeMap * bookCase)
{
    printf("Ingrese el ID de los archivos que desea importar\nINGRESE 'end' PARA DETENERSE:\n");
    char fileName[] = "./books/";
    char id[15];
    char title[] = "Placeholder Title";
    getchar();
    gets(id);
    while(is_equal_string(id, "end") == 0)
    {
        //De aqui hasta el siguiente comentario es solo para adaptar el nombre del archivo y abrirlo
        strcpy(fileName, assignFileName(id, fileName));

        printf("Leyendo archivo: %s.txt\n", id);
        FILE * file = fopen(fileName, "r");
        if(file == NULL)
        {
            printf("Archivo no encontrado.\n");
            return;
        }
        //Hasta aqui

        printf("Creando Book * book\n");
        Book * book = createBook1(id, title);

        printf("Adquiriendo posicion actual\n");
        Position * pos = getPos(file);

        printf("Leyendo siguiente palabra\n");
        char * word = next_word(file);

        printf("Estableciendo contador de palabras en 0\n");
        int wordCount = 0;

        while(word)
        {
            wordCount++;
            printf("Palabra numero [%i]\n", wordCount);

            printf("Limpiando palabra y pasando a minuscula\n");
            lowerAndClean(word);

            NodeWF * auxWord;

            printf("\nBuscando si la palabra [%s] ya existe\n", word);
            PairTree * auxPair = searchTreeMap(book->wordsFrecuency, word);
            printf("Despues de PairTree *\n");

            if(auxPair == NULL)
            {
                printf("Palabra no encontrada, creando NodeWF *\n");

                auxWord = createNodeWF(word, pos, book);
                book->uniqueWords++;
            }
            else
            {
                auxWord = foundWord(auxPair->value, pos);
            }

            printf("Leyendo posicion de la siguiente palabra\n");
            pos = getPos(file);

            printf("\nLeyendo siguiente palabra\n");
            word = next_word(file);

            if(word == NULL)
            {
                printf("No hay mas palabras.\n");
                break;
            }
            printf("\n----------Siguiente palabra del archivo----------\n");
        }
        book = createBook2(file, wordCount);

        printf("Ultima posicion = [%i]\n", book->charCount);

        printf("Insertando en el TreeMap\n");
        insertTreeMap(bookCase, book->id, book);

        //fclose(file);

        printf("Ingrese el ID del siguiente archivo (o 'end' para detenerse).\n");
        char fileName[] = "./books/";
        gets(id);
    }
    printf("Finalizando de leer, volviendo al menu.\n");
}

void divideTitle(char * bookTitle, char ** title)
{
    int i = 0;
    char division[2] = " ";
    printf("Imprimiendo titulo por partes\n");
    while(1)
    {
        title[i] = strtok(bookTitle, division);
        if(title[i] == NULL) break;
        printf("%s\n",title[i]);
        i++;
    }
}

void searchBookByTitle(TreeMap * bookCase)
{
    PairTree * pairTree = firstTreeMap(bookCase);
    if(pairTree == NULL)
    {
        printf("No se ha encontrado ningun libro\n");
        return;
    }
    Book * book = pairTree->value;
    char bookTitle[100];
    printf("Ingrese el titulo del libro a buscar:\n");
    getchar();
    gets(bookTitle);
    //dividir titulo en un arreglo de palabras
    char * title[20];
    divideTitle(bookTitle, title);

    int required = 0;
    while(book != NULL)
    {
        //con un for, comparar cada palabra con cada palabra del titulo
        for(int i = 0; i < required ; i++)
        {

        }
        //se nececita un contador y la cantidad de palabras del titulo
        //si la palabra está en el titulo, se incrementa el contador
        //cuando el contador llegue a la cantidad del titulo, se muestra el libro
        if(strcmp(bookTitle, book->title) == 0)
        {
            printf("%s\n", bookTitle);
        }
        //se pasa al siguiente libro
        pairTree = firstTreeMap(bookCase);
        if(pairTree == NULL)
        {
            printf("No se ha encontrado ningun libro\n");
            return;
        }
        book = pairTree->value;
    }
}

void showSortedBooks(TreeMap * bookCase)
{
    printf("Searching for the first book\n");
    
    PairTree * pairTree = firstTreeMap(bookCase);
    if(pairTree == NULL)
    {
        printf("No se ha encontrado ningun libro\n");
        return;
    }
    Book * currentBook = pairTree->value;

    if(currentBook == NULL)
    {
        printf("No hay libros aún.\n");
        return;
    }

    printf("Mostrando todos los libros.");

    while(currentBook)
    {
        printf("%s", currentBook->title);
        printf("%s", currentBook->id);
        printf("%i", currentBook->charCount);
        printf("%i", currentBook->wordCount);
        currentBook = nextTreeMap(bookCase)->value;
    }
}

void showWordsWithMostFrecuency(TreeMap * bookCase)
{
    PairTree * auxP = firstTreeMap(bookCase);
    if(auxP == NULL)
    {
        printf("No hay libros aún.\n");
        return;
    }
    char id[15];
    printf("Ingrese el id del libro que desea revisar.\n");
    getchar();
    gets(id);

    printf("Buscando el libro de id = [%s]\n", id);
    auxP = searchTreeMap(bookCase, id);
    if(auxP == NULL)
    {
        printf("No se ha encontrado ese libro\n");
        return;
    }
    Book * book = auxP->value;
    printf("Revisando si hay palabras\n");
    auxP = firstTreeMap(book->wordsFrecuency);
    if(auxP == NULL)
    {
        printf("Ocurrio un error al intentar conseguir las palabras.\n");
        return;
    }
    NodeWF * actualWord;
    int i,j,k;
    printf("uniqueWords = [%i]\n", book->uniqueWords);
    printf("Mostrando las 10 palabras:\n");
    for(i = 0 ; i < 10 ; i++)
    {
        printf("Palabra [%i]\n", i+1);
        for(j = 0 ; j < book->uniqueWords ; j++)
        {
            printf("j = [%i]\n", j);
            k = book->uniqueWords-j;
            printf("k = [%i]\n", k);
            while(k > 0)
            {
                printf("Buscando la palabra en posicion [%i]\n", k);
                auxP = nextTreeMap(book->wordsFrecuency);
                if(auxP == NULL)
                {
                    printf("Ocurrio un error al intentar conseguir la palabra\n");
                    return;
                }
                k--;
            }
            actualWord = auxP->value;
            printf("Word = [%s]\nAmmount = [%i]\n", actualWord->word, actualWord->count);
            auxP = nextTreeMap(book->wordsFrecuency);
        }
        auxP = firstTreeMap(book->wordsFrecuency);
    }
}

int main()
{
    int op = 0;
    printf("Creando Map * bookCase\n");
    TreeMap * bookCase = createTreeMap(lower_than_string); //Key = id

    printf("INICIO\n");
    while(op!=8)
    {
        pressEnterToContinue();
        printf("\n--------------------MENU--------------------\n");
        printf("1.- Cargar documentos.\n");
        printf("2.- Mostrar documentos ordenados.\n");
        printf("3.- Buscar un libro por titulo.\n");
        printf("4.- Palabras con mayor frecuencia en un libro.\n");
        printf("5.- Palabras mas relevantes de un libro.\n");
        printf("6.- Buscar por palabra.\n");
        printf("7.- Mostrar palabra en su contexto dentro del libro.\n");
        printf("8.- SALIR\n");

        printf("--------------------------------------------\nIndica la opcion:\n");
        scanf("%i", &op);
        printf("Opcion escojida = %i\n\n", op);

        switch(op)
        {
            case 1:
            {
                /*Cargar documentos. El usuario coloca los nombres de una lista
                de archivos txt separados por espacio y la aplicación los carga.*/
                printf("Cargando documentos\n");
                loadBooks(bookCase);
                break;
            }
            case 2:
            {
                /*Mostrar documentos ordenados. La aplicación muestra los id y
                títulos de los libros en orden alfabético. Además se muestra la
                cantidad de palabras y caracteres que tiene cada uno de ellos.*/
                showSortedBooks(bookCase);
                break;
            }
            case 3:
            {
                searchBookByTitle(bookCase);
                /*Buscar un libro por título. El usuario coloca algunas palabras
                separadas por espacio y la aplicación muestra los títulos de
                libros que contienen todas las palabras.*/
                break;
            }
            case 4:
            {
                /*Palabras con mayor frecuencia. El usuario ingresa el ***ID*** de un
                libro y la aplicación muestra las 10 palabras que se repiten con
                mayor frecuencia (indicando la cantidad de veces que aparece
                cada una de ellas). (La frecuencia se calcula como la cantidad
                de veces que aparece una palabra dividida por el total de
                palabras en el libro.)*/
                showWordsWithMostFrecuency(bookCase);
                break;
            }
            case 5:
            {
                /*Palabras más relevantes.  El usuario ingresa el ***TÍTULO*** de un
                libro y la aplicación muestra las 10 palabras más relevantes
                de este.*/
                break;
            }
            case 6:
            {
                /*Buscar por palabra. El usuario ingresa una palabra y la
                aplicación muestra los libros (id y título) que tienen la
                palabra en su contenido. Los libros deben ser ordenados por
                la relevancia de la palabra buscada.
                Por ejemplo, si busco “Jesús”, la biblia debería aparecer en
                primer lugar. Si busco “mancha”, el Quijote debería salir en
                primer lugar.*/
                break;
            }
            case 7:
            {
                /*Mostrar palabra en su contexto dentro del libro. El usuario
                ingresa el título de un libro y el de una palabra a buscar. La
                aplicación muestra las distintas apariciones de la palabra en
                el contexto del documento, es decir, para cada aparición, se
                muestran algunas palabras hacia atrás y hacia adelante de la
                palabra buscada (por ejemplo, la línea completa en la que
                aparece la palabra, o 5 palabras hacia atrás y 5 hacia adelante).*/
                break;
            }
            case 8:
            {
                printf("Saliendo del programa...");
                break;
            }
        }
    }
    return 0;
}