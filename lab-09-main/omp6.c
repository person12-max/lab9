#include <stdio.h>
#include <omp.h>
int length = 0;
void swap(int arr[], int i, int j){
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
void qSort(int arr [], int left, int right) {
    if(left >=right) return;
    int pIndex = right; // using right end as pivot
    int pValue = arr[right];
    int i = left -1;
    int j = left;
    for(j = left; j < right;j++) {
        if(arr[j] < pValue) {
            i++;
            swap(arr,i,j);
        }
    }
    swap(arr,pIndex,i+1);
    #pragma omp task shared(arr) firstprivate(left,i)
    { qSort(arr, left, i);}
    #pragma omp task shared(arr) firstprivate(right,i)
    { qSort(arr,i+2, right);}
    #pragma omp taskwait
}
void qSortFinal(int arr []) {
   #pragma omp parallel default(none) shared(arr) firstprivate(length)
    {
	#pragma omp single nowait
	{
   	   qSort(arr,0,length-1);
	}
    }
}
void printArray(int arr[]){
    int i = 0;
    for(i = 0 ; i < length;i++){
        printf("%d\n", arr[i]);
    }
}
void getLength(char *argv[]){
    FILE * f = fopen(argv[1],"r");
//    FILE * f = fopen("input.txt", "r");
    int num = 0;
    while(fscanf(f, "%d", &num) > 0){
        length++;
    }
    fclose(f);
}
int main(int argc, char *argv[]){
    getLength(argv);
    int arr [length];
    FILE * f = fopen(argv[1],"r");
   // FILE * f = fopen("input.txt", "r");
    int num = 0;
    int i =0;
    while(fscanf(f, "%d", &num) > 0){
        arr[i] = num;
        i++;
    }
    fclose(f);
    qSortFinal(arr);
    printArray(arr);
    return 0;
}
