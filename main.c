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
    char word[1024];
    int count;
    List * positions; //Data = char * position, works with a function to read the .txt
} NodeWF; //Node of Map * Words Frecuency

typedef struct{
    int position;
} Position;

//Función para comparar claves de tipo string, retorna 1 si son iguales
int is_equal_string(void * key1, void * key2) {
    if(strcmp((char*)key1, (char*)key2)==0) return 1;
    return 0;
}

//Viene de TreeMap.h
int lower_than_string(void* key1, void* key2){
    char* k1=(char*) key1;
    char* k2=(char*) key2;
    if(strcmp(k1,k2)<0) return 1;
    return 0;
}

int lower_than_float(void* key1, void* key2)
{
    float * k1 = (float*) key1;
    float * k2 = (float*) key2;
    if(k1 < k2) return 1;
    return 0;
}

//Funcion que requiere presionar enter para proseguir, sirve para generar una pausa.
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
    //printf("Checking [%s]\n", word);
    while(word[i] != '\0')
    {
        //printf("Checking [%i] = %c\n", i, word[i]);
        if(isalpha(word[i]) == 0)
        {
            //printf("Is not alpha\n");
            k = i;
            while(word[k+1] != '\0')
            {
                //printf("Replacing %c with %c\n", word[k], word[k+1]);
                word[k] = word[k+1];
                k++;
            }
            word[k] = '\0';
            i--;
        }
        i++;
    }
}

//Funcion que limipia y convierte a minuscula una palabra
void lowerAndClean(char * word)
{
    cleanWord(word);
    stringToLower(word);
}

//Asigna el id al nombre del archivo para incluirle la ruta de la carpeta y el txt
char * assignFileName(char * id, char * fileName)
{
    strcat(fileName, id);
    strcat(id, ".txt");
    return fileName;
}

//Se salta la informacion del projecto y extrae el titulo del archivo (primera linea hasta \n), y retorna un string
char * getTitle(FILE * file)
{
    //Reservar memoria para title
    char * title = malloc(300);
    char c;
    int j;
    size_t n = 0;

    j = 0;
    while ((c = fgetc(file)) != '\n')
    {
        j++;
        if(j > 7)
        {
            //printf("c = [%c]\n", c);
            title[n] = c;
            n++;
        }
    }

    title[n] = '\0';
    //printf("title = [%s]\n", title);

    return title;
}

char * skipToContent(FILE * file)
{
    int i;
    char s[1024];
    //Saltarse las lineas inecesarias
    for(i = 0 ; i < 10 ; i++)
    {
        fgets(s, 1024, file);
    }

    //Obtener el Titulo
    char * title = getTitle(file);

    //Saltarse el resto hasta el contenido importante
    for(i = 0 ; i < 13 ; i++)
    {
        fgets(s, 1024, file);
    }
    return title;
}

//Crea un tipo Book y le asigna los valores de ID y el título, e inicializa el TreeMap
Book * createBook1(char * id, char * title)
{
    Book * newBook = (Book *) malloc (sizeof(Book));

    //printf("Creando Map book->wordsFrecuency\n");
    newBook->wordsFrecuency = createTreeMap(lower_than_string);

    //printf("Asignando id a book->id\n");
    strcpy(newBook->id, id);

    //printf("Asignando Title\n");
    strcpy(newBook->title, title);

    newBook->uniqueWords=0;

    return newBook;
}

//Asigna la ultima posicion como el numero de characteres y el valor del numero de palabras
void createBook2(FILE * file, Book * book, int wordCount)
{
    book->charCount = ftell(file);
    book->wordCount = wordCount;
}

//Inicializa un tipo Position * y le asigna el valor de la posición de la palabra en el archivo
Position * getPos(FILE * file)
{
    Position * pos = (Position *) malloc (sizeof(Position));
    pos->position = ftell(file);
    //printf("pos = [%i]\n", pos->position);
    return pos;
}

//Inicializa un NodeWF, inicializa su lista, le copia el string palabra e inicializa el cont a 1
//Ademas inserta la posicion en la lista del nodo
NodeWF * createNodeWF(char * word, Position * pos, Book * book)
{
    NodeWF * auxWord = (NodeWF *) malloc (sizeof(NodeWF));
    //printf("auxWord creada\n");

    //printf("Creando lista de posisiones\n");
    auxWord->positions = createList();

    if(auxWord->positions == NULL)
    {
        printf("No se pudo crear la Lista.\n");
        return NULL;
    }
    else
    {
        //printf("Lista creada\n");
        
        //printf("Asignando auxWord->word\n");
        strcpy(auxWord->word, word);
        //printf("auxWord->word = %s\n", auxWord->word);

        //printf("Asignando cantidad = 1\n");
        auxWord->count = 1;

        //printf("Position = %i\n", pos->position);
        //printf("PositionPointer Value = %i\n", &pos->position);

        //printf("Insertando a la lista\n");
        pushBack(auxWord->positions, pos);
        //printf("Posicion insertada en la lista.\n");

        //printf("Insertando palabra al mapa\n");
        insertTreeMap(book->wordsFrecuency, word, auxWord);
        //printf("Palabra [%s] insertada en el mapa\n", auxWord->word);

        //printf("\nPalabra %s creada\n", auxWord->word);
        
        return auxWord;
    }
}

//Muestra las posiciones actuales desde la lista de una palabra (NodeWF)
void * showCurrentPositions(List * positions)
{
    printf("\nImprimiendo posiciones actuales en la lista\n");
    Position * auxPos = firstList(positions);
    while(auxPos!=NULL)
    {
        printf("Pos = %i\n", auxPos->position);
        auxPos = nextList(positions);
    }
}

//En caso de que la palabra ya exista en el mapa, le aumenta el count, le asigna la nueva posicion y la guarda en la lista
void foundWord(NodeWF * auxWord, Position * pos)
{
    //printf("Palabra encontrada, aumentando cantidad.\n");
    auxWord->count++;
    //printf("Cantidad = %i\n", auxWord->count);

    //printf("Position = %i\n", pos->position);
    //printf("PositionPointer Value = %i\n", &pos->position);

    //printf("Insertando a la lista\n");
    pushBack(auxWord->positions, pos);
    //printf("Posicion insertada en la lista.\n");

    //showCurrentPositions(auxWord->positions);
}

//Funcion principal de cargar los .txt
int loadBooks(TreeMap * bookCase, int ammountOfDocs)
{
    printf("-----<Ingrese el ID del archivo que desea importar o 'end' para detenerse>-----\n");
    char auxID[15];
    char id[15] = "";
    getchar();
    do
    {
        char fileName[] = "./books/";
        gets(id);
        strcpy(auxID, id);
        //printf("ID = %s\n", auxID);
        //De aqui hasta el siguiente comentario es solo para adaptar el nombre del archivo y abrirlo
        strcpy(fileName, assignFileName(id, fileName));
        //printf("fileName = %s\n", fileName);

        //printf("ID = %s\n", auxID);
        
        printf("Leyendo archivo: %s.txt\n", auxID);
        FILE * file = fopen(fileName, "r");
        if(file == NULL)
        {
            printf("Archivo no encontrado. Ingrese el ID nuevamente.\n");
            continue;
        }
        //----------------Hasta aqui-----------------
        ammountOfDocs++;

        char * title = skipToContent(file);
        printf("Titulo del libro = [%s]\n", title);

        //printf("Creando Book * book\n");
        Book * book = createBook1(auxID, title);

        //printf("Adquiriendo posicion actual\n");
        Position * pos = getPos(file);
    
        //printf("Estableciendo contador de palabras en 0\n");
        int wordCount = 0;

        char * word;
        char scannedWord[300]="";
        printf("Porfavor espere a que carguen las palabras...\n");

        while(fscanf(file, " %1023s", scannedWord) != EOF)
        {
            wordCount++;
            //printf("Palabra numero [%i]\n", wordCount);

            word = strdup(scannedWord);
            //printf("Limpiando palabra y pasando a minuscula\n");
            lowerAndClean(word);

            //printf("Word = [%s]\n", word);

            int bool = 0;
            NodeWF * auxWord;

            //printf("\nBuscando si la palabra [%s] ya existe\n", word);
            PairTree * auxPair = searchTreeMap(book->wordsFrecuency, word);
            //printf("Despues de PairTree *\n");
            if(auxPair != NULL)
            {
                auxWord = auxPair->value;
                lowerAndClean(auxWord->word);
                //printf("Comparing [%s] with [%s]\n", auxWord->word, word);
                if(strcmp(auxWord->word, word) == 0) bool = 1;
            }
            //printf("Bool = [%i]\n", bool);
            if(bool == 0)
            {
                //printf("Palabra no encontrada, creando NodeWF *\n");

                auxWord = createNodeWF(word, pos, book);

                book->uniqueWords++;
                //printf("book->uniqueWords = [%i]\n", book->uniqueWords);

                //printf("Palabra recien creada = [%s]\n", auxWord->word);
            }
            else
            {
                //printf("Palabra encontrada\n");
                //printf("auxWord = [%s]\n", auxWord->word);
                foundWord(auxPair->value, pos);
            }
            
            //printf("\n----------Siguiente palabra del archivo----------\n");

            //printf("Leyendo posicion de la siguiente palabra\n");
            pos = getPos(file);
        }
        createBook2(file, book, wordCount);

        printf("Cantidad total de Caracteres = [%i]\n", book->charCount);
        printf("Cantidad de palabras en el contenido = [%i]\n", book->wordCount);

        //printf("Insertando en el TreeMap\n");
        insertTreeMap(bookCase, book->id, book);

        printf("Cerrando archivo\n");
        if(fclose(file) == 0)
        {
        printf("Archivo cerrado\n");
        }
        else
        {
            printf("El archivo no se pudo cerrar\n");
        }

        printf("-----<Ingrese el ID del siguiente archivo o 'end' para detenerse>-----\n");
    }while(is_equal_string(auxID, "end") == 0);
    printf("Finalizando de leer, volviendo al menu.\n");
    return ammountOfDocs;
}

void searchBookByTitle(TreeMap * bookCase)
{
    PairTree * pairTree = firstTreeMap(bookCase);
    if(pairTree == NULL)
    {
        printf("No hay ningun libro aun.\n");
        return;
    }
    char bookTitle[100];
    printf("Ingrese el titulo del libro a buscar:\n");
    getchar();
    gets(bookTitle);
    stringToLower(bookTitle);

    printf("Mostrando los libros que coinciden con la busqueda:\n");

    //dividir titulo en un arreglo de palabras
    char division[2] = " ";

    char wantedTitle[100][1024];
    int nWordsRequired = 0;
    char * auxString1;
    auxString1 = strtok(bookTitle, division);
    //printf("Imprimiendo titulo buscado por partes\n");
    while(auxString1 != NULL)
    {
        //printf("%s\n" ,auxString1);
        strcpy(wantedTitle[nWordsRequired], auxString1);
        nWordsRequired++;
        auxString1 = strtok(NULL, division);
    }

    //printf("nwords of the title = [%i]\n", nWordsRequired);
    while(pairTree != NULL)
    {
        //printf("Revisando libro\n");
        Book * book = pairTree->value;

        //Dividiendo titulo del libro actual
        char currentTitle[100][1024];
        int nWordsTitle = 0;
        char * auxString2 = malloc(100);
        strcpy(auxString2, book->title);
        auxString2 = strtok(auxString2, division);
        //printf("Imprimiendo titulo actual por partes\n");
        while(auxString2 != NULL)
        {
            //printf("%s\n" ,auxString2);
            strcpy(currentTitle[nWordsTitle], auxString2);
            nWordsTitle++;
            auxString2 = strtok(NULL, division);
        }
        //se nececita un contador y la cantidad de palabras del titulo
        //printf("Num of words of title = [%i]\n", nWordsTitle);

        //con un for, comparar cada palabra con cada palabra del titulo
        int matches = 0;
        int x,y;
        for(x = 0; x < nWordsRequired ; x++)
        {
            for(y = 0 ; y < nWordsTitle ; y++)
            {
                //printf("\nComparing [%s]\nwith      [%s]\n", wantedTitle[x], currentTitle[y]);
                //si la palabra está en el titulo, se incrementa el contador
                if(strcmp(wantedTitle[x], currentTitle[y]) == 0)
                {
                    //printf("Match!\n");
                    matches++;
                }
            }
        }
        
        //Si el contador llegua a la cantidad del titulo, se muestra el libro
        if(matches >= nWordsRequired)
        {
            printf("\n[%s]\n", book->title);
        }

        //se pasa al siguiente libro
        pairTree = nextTreeMap(bookCase);
    }
}

void showSortedBooks(TreeMap * bookCase)
{
    printf("Searching for the first book\n");
    
    PairTree * auxP = firstTreeMap(bookCase);
    if(auxP == NULL)
    {
        printf("No hay libros aun.\n");
        return;
    }

    Book * currentBook;

    printf("Mostrando todos los libros.");
    while(auxP)
    {
        currentBook = auxP->value;
        printf("Titulo: %s\n", currentBook->title);
        printf("Id: %s\n", currentBook->id);
        printf("Cantidad total de Caracteres: %i\n", currentBook->charCount);
        printf("Cantidad de palabras en el contenido: %i\n", currentBook->wordCount);
        auxP = nextTreeMap(bookCase);
    }
    printf("fin.\n");
}

Book * searchOneBookByTitle(TreeMap * bookCase, char * title)
{
    PairTree * auxP = firstTreeMap(bookCase);
    while(auxP)
    {
        Book * book = auxP->value;
        printf("Comparing [%s] with [%s]\n", title, book->title);
        if(strcmp(book->title, title) == 0)
        {
            printf("\nSe ha encontrado el libro.\n");
            return book;
        }
        auxP = nextTreeMap(bookCase);
    }
    printf("\nNo se encontro el libro (Verifique las mayusculas)\n");
    return NULL;
}

void showTop10Words(TreeMap * bookCase)
{
    PairTree * auxP = firstTreeMap(bookCase);
    if(auxP == NULL)
    {
        printf("No hay libros aun.\n");
        return;
    }
    char title[150];

    Book * book = NULL;
    getchar();
    while(book == NULL)
    {
        printf("Ingrese el titulo del libro que desea revisar.\n");
        gets(title);

        printf("Buscando el libro [%s]\n", title);
        book = searchOneBookByTitle(bookCase, title);
    }
    //printf("Revisando si hay palabras\n");
    auxP = firstTreeMap(book->wordsFrecuency);
    NodeWF * actualWord;
    if(auxP == NULL)
    {
        printf("No hay palabras en el arbol.\n");
        return;
    }
    int h,i,k;
    //printf("uniqueWords = [%i]\n", book->uniqueWords);
    if(book->uniqueWords < 10)
    {
        h = book->uniqueWords;
    }
    else
    {
        h = 10;
    }
    printf("Mostrando las %i palabras:\n", h);
    //Para la posicion [i]
    for(i = 0 ; i < h ; i++)
    {
        printf("Palabra en %i lugar.\n", i+1);
        
        //printf("j = [%i]\n", j);
        //k = book->uniqueWords-i-1;

        //printf("Buscando la palabra en posicion [%i]\n", k);
        
        //Recorrer las palabras
        auxP = firstTreeMap(book->wordsFrecuency);
        actualWord = auxP->value;
        //printf("First = [%s]\n", actualWord->word);

        for(k = 0 ; k < i ; k++)
        {
            //printf("k = [%i]\n", k);
            //printf("En la palabra del mapa [%i]\n",k);
            auxP = nextTreeMap(book->wordsFrecuency);
            if(auxP == NULL)
            {
                printf("Ocurrio un error al intentar conseguir la palabra\n");
                return;
            }
            actualWord = auxP->value;
            //printf("Word = [%s]\n", actualWord->word);
        }

        actualWord = auxP->value;
        float veces = actualWord->count;
        float total = book->wordCount;
        float frecuencia = veces/total;
        printf("> Palabra = [%s]\n  Veces = [%i]\n  Frecuencia = [%f]\n\n", actualWord->word, actualWord->count, frecuencia);
    }
}

void showRelevantWords(TreeMap * bookCase, int ammountOfDocs)
{/*
    PairTree * auxP = firstTreeMap(bookCase);
    if(auxP == NULL)
    {
        printf("No hay libros aun.\n");
        return;
    }
    char title[150];

    Book * book = NULL;
    getchar();
    while(book == NULL)
    {
        printf("Ingrese el titulo del libro que desea revisar.\n");
        gets(title);

        printf("Buscando el libro [%s]\n", title);
        book = searchOneBookByTitle(bookCase, title);
    }
    //printf("Revisando si hay palabras\n");
    auxP = firstTreeMap(book->wordsFrecuency);
    NodeWF * actualWord;
    if(auxP == NULL)
    {
        printf("No hay palabras en el arbol.\n");
        return;
    }
    int h,i,k;
    //printf("uniqueWords = [%i]\n", book->uniqueWords);
    if(book->uniqueWords < 10)
    {
        h = book->uniqueWords;
    }
    else
    {
        h = 10;
    }
    printf("Mostrando las %i palabras:\n", h);
    //Para la posicion [i]
    for(i = 0 ; i < h ; i++)
    {
        printf("Palabra en %i lugar.\n", i+1);
        
        //Recorrer las palabras
        auxP = firstTreeMap(book->wordsFrecuency);
        actualWord = auxP->value;
        //printf("First = [%s]\n", actualWord->word);
        //TreeMap * relevancyMap = createTreeMap(lower_than_float);
        for(k = 0 ; k < i ; k++)
        {
            //printf("k = [%i]\n", k);
            //printf("En la palabra del mapa [%i]\n",k);
            auxP = nextTreeMap(book->wordsFrecuency);
            if(auxP == NULL)
            {
                printf("Ocurrio un error al intentar conseguir la palabra\n");
                return;
            }
            actualWord = auxP->value;
            //printf("Word = [%s]\n", actualWord->word);
        }

        actualWord = auxP->value;
        float veces = actualWord->count;
        float total = book->wordCount;
        float frecuencia = veces/total;
        float docs = ammountOfDocs;
        float relevancy = frecuencia*log(1);
        printf("> Palabra = [%s]\n  Relevancia = [%f]\n\n", actualWord->word, relevancy);
    }*/
}

int main()
{
    int op = 0;
    printf("Creando Map * bookCase\n");
    TreeMap * bookCase = createTreeMap(lower_than_string); //Key = id

    int ammountOfDocs = 0;

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
                ammountOfDocs = loadBooks(bookCase, ammountOfDocs);
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
                /*Buscar un libro por título. El usuario coloca algunas palabras
                separadas por espacio y la aplicación muestra los títulos de
                libros que contienen todas las palabras.*/
                searchBookByTitle(bookCase);
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
                showTop10Words(bookCase);
                break;
            }
            case 5:
            {
                /*Palabras más relevantes.  El usuario ingresa el ***TÍTULO*** de un
                libro y la aplicación muestra las 10 palabras más relevantes
                de este.*/
                showRelevantWords(bookCase, ammountOfDocs);
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
                ingresa el título de un libro y una palabra a buscar. La
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