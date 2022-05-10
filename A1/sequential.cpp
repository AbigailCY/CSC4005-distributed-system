#include <iostream>
#include <cstdlib>
#include<ctime>
using namespace std;

void odd_sort(int *array, int size, bool &swapped);
void even_sort(int *array, int size, bool &swapped);
void swap(int* array, int posl, int posr);
void generate_rand_array(int * array, int size);

    
void odd_sort(int *array, int size, bool &swapped) {
    for (int i = 0; i < size-1; i+=2) {
        if (array[i] > array[i+1]){
            swap(array, i, i+1);
            swapped = true;
        }
    }
    return;
}

void even_sort(int *array, int size, bool &swapped) {
    for (int i = 1; i < size-1; i+=2) {
        if (array[i] > array[i+1]){
            swap(array, i, i+1);
            swapped = true;
        }
    }
    return;
}

void swap(int* array, int posl, int posr) {
    int temp = array[posl];
    array[posl] = array[posr];
    array[posr] = temp;
}

void generate_rand_array(int * array, int size) {
    srand((int)time(0));
    for (int i = 0; i < size; i++) {
        array[i] = rand() % size;
    }
    return;
}

int main(int argc, char *argv[]) {

    int size = atoi(argv[1]);
    int arr[size];
    generate_rand_array(arr, size);

    cout << "The first 20 elements in the unsorted array: "<< endl;
    for (int i = 0; i < 20; i++) {
        cout << arr[i] <<" " ;
    }
    cout<<endl;

    time_t start, end;
    start = clock();
    
    bool swapped = true;
    while (swapped) {
        swapped = false;
        odd_sort(arr, size, swapped);
        even_sort(arr, size, swapped);
    }

    end = clock();
	double Time = double(end - start)/CLOCKS_PER_SEC;

    cout << "The first 20 elements in the sorted array: "<< endl;
    for (int i = 0; i < 20; i++) {
        cout << arr[i] <<" " ;
    }
    cout<<endl;

    cout << "Array size: " << size<< "; Runtime: " << Time << endl;
    return -1;
}