#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <wait.h>
#include <stdlib.h>
#include <time.h>

/*
    Enio A. Hernández A. - 2017214037
	Estudiante Ingenieria de Sistemas
	Universidad del Magdalena
	2020 - 2
	Sistema Operativo usado -> Linux
	Taller Practico 1

    datos.txt
    -------------------------------
    5               //Número de filas del Vector;
    4               //Número de filas de la Matriz;
    5               //Número de Columnas de la Matriz;
    3               //Fila #1 del Vector; 
    4               //Fila #2 del Vector;
    6               //...; 
    3               //...;
    8               //n-ésima fila del Vector;
    11 12 13 14 15  //Fila #1 de la Matriz;
    21 22 23 24 25  //Fila #2 de la Matriz;
    31 32 33 34 35  //...;
    41 42 43 44 45  //n-ésima fila de la Matriz;
    -------------------------------
*/

typedef FILE * Fichero;
int i = 0;
int j = 0;
int k = 0;
int f = 0;
int c = 0;
int au = 0;

int aux[50];            //Vector auxiliar para guardar todos los tatos del archivo. 
int vector[50];
int matriz[50][50];

int validar(int num_f_vector, int num_f_matriz, int num_c_matriz); //Valido los numeros que ingresen;

int main(int argc, char const *argv[]){
    pid_t hijo1 = (pid_t)(0), hijo2 = (pid_t)(0), padre = getpid(); //Variables para validar Padre, hijo1 y hijo2;
    int num_fila_vector = 0;
    int num_fila_matriz = 0;
    int num_colum_matriz = 0;
    int aux2 = 0; //Variable auxiliar para guardar en los vectores o matrices.

    Fichero fichero;    //Fichero para leer datos.txt
    Fichero fichero2;   //Fichero para escribir y leer result_hijo1.txt
    Fichero fichero3;   //Fichero para escribir y leer result_hijo2.txt

    if(padre == getpid()) //Soy padre
    {
        printf("%s [%d]\n", "padre y mi id -> :", getpid()); //imprime el id del padre.
        fichero = fopen("datos.txt", "r");
        if(fichero == NULL){
            printf("error al abrir datos.txt");
        }else{


            while(!feof(fichero)){
                fscanf(fichero, "%d", &aux2);
                aux[i] = aux2; // Guardo todos los datos del fichero dato.txt en el vector auxiliar.
                i++;
            }
        }
        fclose(fichero);
        /* Los tres primeros elementos de aux[], equivalen
            Num de filas del verctor = aux[0]
            Num de filas de la matriz = aux[1]
            Num de columna de la matriz = aux[2]
        */
        num_fila_vector = aux[0];
        num_fila_matriz = aux[1];
        num_colum_matriz = aux[2];

        if(validar(num_fila_vector, num_fila_matriz, num_colum_matriz) == 0) // Validar si los datos son correctos
        {
            printf("error con los datos\n");
            exit(-1); // Si los datos son incorrectos, se cierra el programa.
        }

        for (k; k < num_fila_vector; k++){
            vector[k] = aux[k+3]; //Se guardan los datos del vector aux a el vector, desde la posición 3 hasta el Num_fila_vector
        }

        k = k + 3;//Las filas de la Matriz inician desde la posición num_fila_vector + 3, que es igual k + 3

        for (f; f < num_fila_matriz; f++){
            for (c; c < num_colum_matriz; c++){
                matriz[f][c] = aux[au+k]; //se guardar los datos del vector aux a la matriz.
                au++;
            }
            c = 0;
        }
    }
    
    hijo1 = fork(); //Creando el primer hijo.
    if(hijo1 == -1) //Validar si el primer hijo se creo correctamente.
    {
        printf("No se pudo crear el proceso hijo1");
    }else{ 
        if(hijo1 == 0)//Soy hijo1
        {
            printf("%s [%d]\n", "hijo 1 y mi id -> :", getpid()); //imprimo id del hijo 1
            hijo1 = getpid();
        }else{ /*Soy padre*/
            hijo2 = fork(); //Creo hijo 2
            if(hijo2 == -1){ /*Validar si el segundo hijo se creo correctamente.*/
                printf("No se pudo crear el proceso hijo2");
            }else{
                if(!hijo2) // Soy hijo 2
                {
                    printf("%s [%d]\n", "hijo 2 y mi id -> :", getpid()); //imprimo id del hijo 1
                    hijo2 = getpid();
                }
            } 
        }
    }


    if(hijo1 == getpid()) // soy hijo 1
    {
        fichero2 = fopen("result_hijo1.txt", "w");
        //sleep(10);
        aux2 = 0;
        for (int i = 0; i < num_fila_matriz/2; i++){ /*Multiplico la mitad de la matriz por el vactor*/
            for (int j = 0; j < num_colum_matriz; j++){
                aux2 = matriz[i][j] * vector[j] + aux2;   
            }
            fprintf(fichero2, "%d\n", aux2); //Escribo el resultado en result_hijo1.txt
            aux2 = 0;
        }
        fclose(fichero2);
    }

    if(hijo2 == getpid()) //Soy hijo 2
    {
        fichero3 = fopen("result_hijo2.txt", "w");
        //sleep(10);
        aux2 = 0;
        for (int i = num_fila_matriz/2; i < num_fila_matriz; i++){ /*Multiplico la segunda mitad de la matriz por el vactor*/
            for (int j = 0; j < num_colum_matriz; j++){
                aux2 = matriz[i][j] * vector[j] + aux2;   
            }
            fprintf(fichero3, "%d\n", aux2); //Escribo el resultado en result_hijo2.txt
            aux2 = 0;
        }
        fclose(fichero3);
    }

    if(padre == getpid()){
        /*Espero a que terminen los procesos de hijo 1 y hijo 2*/
        wait(NULL);
        wait(NULL);

        int resul; //Variable para imprimir Resultado

        printf("\n Matriz(%d, %d) = \n",num_fila_matriz, num_colum_matriz);//Imprimo matriz
        for (int r = 0; r < num_fila_matriz; r++){
            for (int col = 0; col < num_colum_matriz; col++){
                aux2 = matriz[r][col];
                printf("[%d],", aux2);
            }
            printf("\n");
        }

        printf("\nVector = \n"); // Imprimo Vector
        for (int p = 0; p < num_fila_vector; p++){
            printf("[%d]\n",vector[p]);
        }

        printf("\nResultado de Multiplicar la Matriz(%d, %d) x Vector = \n", num_fila_matriz, num_colum_matriz);
        //Imprimo resultados
        fichero2 = fopen("result_hijo1.txt", "r");
        if(fichero2 == NULL){
            printf("error al abrir result_hijo1.txt");
        }else{
            int cont = 0;
            while (!feof(fichero2))
            {
                while(cont < num_fila_matriz/2){
                    fscanf(fichero2, "%d", &resul);
                    printf("[%d],", resul);
                    cont++;
                }
                break;
            }
            
        }
        fichero3 = fopen("result_hijo2.txt", "r");
        if(fichero3 == NULL){
            printf("error al abrir result_hijo2.txt");
        }else{
            int cont = 0;
            while (!feof(fichero3))
            {
                while(cont < num_fila_matriz/2){
                    fscanf(fichero3, "%d", &resul);
                    printf("[%d],", resul);
                    cont++;
                }
                break;
            }
        }
        fclose(fichero2);
        fclose(fichero3);
        printf("\n");
    }   
    return 0;
}

int validar(int num_f_vector, int num_f_matriz, int num_c_matriz){
    if(num_c_matriz == num_f_vector){
        if(num_f_matriz % 2 == 0){
            return 1;
        }else{
            return 0;
        }
    }else{
        return 0;
    }
}