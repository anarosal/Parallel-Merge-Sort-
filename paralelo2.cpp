#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <chrono>
#include <iostream>
#define NUM 1000000

void insertionSort(int array[], int tamanho);
void mergesort (int a[], int i, int j);
void merge (int a[], int i1, int j1, int i2, int j2);
void mergesortP (int a[], int i, int j);
void mergeDeC (int a[], int i1, int j1, int i2, int j2, int i3, int aux[]);
int buscaBinaria(int value, int a[], int left, int right);
void troca (int& a, int& b);
long maximo (int a, int b);


int main()
{
    int *a;

    a = (int *)malloc(sizeof(int) * NUM);

    srand((unsigned)time(0));
    for(unsigned long int i = 0; i < NUM; i++) {
        a[i] = ((rand()%1000) + 1);
    }

    auto inicio = std::chrono::high_resolution_clock::now();  
    mergesortP(a, 0, NUM-1);     
    auto resultado = std::chrono::high_resolution_clock::now() - inicio;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(resultado).count();
    std::cout << "Paralelo: " <<(double)microseconds/1000000 << "s" << std::endl;
    
    inicio = std::chrono::high_resolution_clock::now();  
    mergesort(a, 0, NUM-1);     
    resultado = std::chrono::high_resolution_clock::now() - inicio;
    microseconds = std::chrono::duration_cast<std::chrono::microseconds>(resultado).count();
    std::cout << "Sequencial: " << (double)microseconds/1000000 << "s" << std::endl;

    return 0;
}

void insertionSort(int array[], int tamanho) {
      long  i, j, tmp;
      for (i = 1; i < tamanho; i++) {
            j = i;
            while (j > 0 && array[j - 1] > array[j]) {
                  tmp = array[j];
                  array[j] = array[j - 1];
                  array[j - 1] = tmp;
                  j--;
            }
      }
}

void mergesortP(int a[], int i,int j){
    
    int mid;

    if(i < j)
    {   
        if(j - i >= 48){
            mid = (i + j) / 2;
            #pragma omp parallel sections num_threads(2)
            {
                #pragma omp section
                {
                    mergesort(a, i, mid);        
                }
    
                #pragma omp section
                {
                    mergesort(a, mid+1, j);    
                }
            }
            int aux[NUM];
        
            int i3 = 0;
      
            mergeDeC (a, i, mid, mid+1, j, i3, aux);   
        }
        else{
            insertionSort(a, j - i + 1);
        }
    }
}

void mergesort(int a[], int i, int j)
{
    int mid;

    if(i < j)
    {
        mid=(i+j)/2;
        //ramo esquerdo da árvore de recursão
        mergesort(a, i, mid);
        //ramo direito da árvore de recursão        
 		mergesort(a, mid+1, j);
        //mistura dos dois ramos 
        merge(a, i, mid, mid+1, j);    
    }
}

void merge (int a[], int i1, int j1, int i2, int j2){
    int temp[NUM];    
    int i, j, k;
    i = i1;    
    j = i2;    
    k = 0;

    while (i <= j1 && j <= j2)   
    {
        if (a[i] < a[j])
            temp[k++] = a[i++];
        else
            temp[k++] = a[j++];
    }

    while (i <= j1)    
        temp[k++] =a[i++];

    while (j <= j2)    
        temp[k++] = a[j++];

    for (i=i1, j=0; i<=j2; i++, j++)
        a[i] = temp[j];
}

void mergeDeC (int a[], int i1, int j1, int i2, int j2, int i3, int aux[]){
    //vetor 1: a[i1...j1]
    int tam1 = j1 - i1 + 1;
    //vetor 2: a[i2...j2]
    int tam2 = j2 - i2 + 1;

    if (tam1 < tam2)
    {
        troca(i1, i2);
        troca(j1, j2);
        troca(tam1, tam2);
    }

    if (tam1 == 0) return;
    if ((tam1 + tam2 <= 8192)){
        merge(a, i1, j1, i2, j2);
    }
    else{
        int q1 = (i1 + j1) / 2;
        int q2 = buscaBinaria( a[ q1 ], a, i2, j2 );
        int q3 = i3 + ( q1 - i1 ) + ( q2 - i2 );
        aux[ q3 ] = a[ q1 ];
        #pragma omp parallel sections num_threads(2)
        {
            #pragma omp section
            mergeDeC (a, i1, q1 - 1, i2, q2 - 1, i3, aux);
            #pragma omp section
            mergeDeC (a, q1 + 1, j1, q2, j2, q3 + 1, aux);
        }
    }
}

void troca(int& a, int& b){
    int t = a;
    a = b;
    b = t;
}

int buscaBinaria(int value, int a[], int left, int right)
{
    long low  = left;
    long high = maximo (left, right + 1);

    while (low < high)
    {
        long mid = ( low + high ) / 2;
        if (value <= a[mid]) high = mid;
        else low  = mid + 1; 
    }
    return high;
}

long maximo (int a, int b){
    if (a < b) return b;
    return a;
}