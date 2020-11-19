#include <stdio.h>
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
    qSort(arr, left, i);
    qSort(arr,i+2, right);
}
void qSortFinal(int arr []) {
    qSort(arr,0,length-1);
}
void printArray(int arr[]){
    for(int i =0 ; i < length;i++){
        printf("%d\n", arr[i]);
    }
}
void getLength(){
    //FILE * in = fopen(argv[1],"r");
    FILE * f = fopen("input.txt", "r");
    int num = 0;
    while(fscanf(f, "%d", &num) > 0){
        length++;
    }
    fclose(f);
}
int main(){
    getLength();
    int arr [length];
    //FILE * in = fopen(argv[1],"r");
    FILE * f = fopen("input.txt", "r");
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
