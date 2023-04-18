//void foo() {
//    int i = 3;
//    while (i>=2 && i <6) {
//        i = i+1;
//    }
//    if (1) print(i);
//    return;
//}
////float k = 233.6;
//int main(){
//    int i = 3; int j = 4;
//    print(i);  print(j);
//    if (i>j) print(j+1);// else print(j);
//    if (i<j) print(j+222);
//    return 0;
//}


//float a[10] = {52.1, 5.1, 34.1,6.1,23.1, 7.1,2.1, 38.1, 4.1,16.1 };


//float a[6] = {18.1, 15.3, 2.3, 21.1, 33.4, 2.1};
//int a[6] = {18, 15, 43, 32, 21, 92};
//int main() {
////    int a[6] = {18, 15, 43, 32, 21, 92};
////    float a[6] = {18.1, 15.3, 2.3, 21.1, 33.4, 2.1};
//    int n=6;  //存放数组a中元素的个数
//    int i;  //比较的轮数
//    int j;  //每轮比较的次数
//    int k;
////    int buf;
//    float buf;  //交换数据时用于存放中间数据
//
//    for (i = 0; i < n - 1; i= i+1)  //比较n-1轮
//    {
//        for (j = 0; j < n - 1 - i; j=j+1)  //每轮比较n-1-i次,
//        {
//            if (a[j] < a[j + 1]) {
//                buf = a[j];
//                a[j] = a[j + 1];
//                a[j + 1] = buf;
//            }
//        }
//    }
//
////    print(buf);
//
//    for (i = 0; i < n; i= i+1)  //比较n-1轮
//        print(a[i]);
//
//    return 0;
//}

//float a[10] = {52.1, 5.1, 34.1,6.1,23.1, 7.1,2.1, 38.1, 4.1,16.1 };
//int a[10] = {52, 5, 77, 6, 23, 7, 2, 38, 4, 16};
//void quicksort(int m, int n)
///* recursively sorts a[m] through a[n] */
//{
//    int i, j;
//    int v, x;
////    float v, x;
//    if (n <= m) return;
///* fragment begins here */
//    i = m-1; j = n; v = a[n];
////    print(9999);
//    while (true) {
//        do i = i+1; while (a[i] < v);
//        do j = j-1; while (a[j] > v);
//        if (i >= j) break;
//        x = a[i]; a[i] = a[j]; a[j] = x; /* swap a[i], a[j] */
//    }
//    x= a[i]; a[i] = a[n]; a[n] = x; /* swap a[i], a[n] */
///* fragment ends here */
//    quicksort(m,j);
//    quicksort(i+1,n);
//    return;
//}
//
//int main() {
//    int n = 10;
//    int i;
//
//    quicksort(0,n-1);
//
//    for (i = 0; i < n; i=i+1) {
//        print(a[i]);
//    }
//
//    return 0;
//}


//
////int a[7] = {18, 15, 2, 21, 25, 22, 17};
//int main(){
//    int a[7] = {18, 15, 2, 21, 25, 22, 17};
//    int n=7;
//    int i;
//    int j;
//    int k;
//    int buf;
//
//    i = 0;
//    while ( i < n - 1  )
//    {
//        j = 0;
//        while ( j < n - 1 - i)
//        {
//            if (a[j] > a[j + 1]) {
//                buf = a[j];
//                a[j] = a[j + 1];
//                a[j + 1] = buf;
//            }
//            j=j+1;
//        }
//        i= i+1;
//    }
//
//    for (i = 0; i < n; i= i+1)
//    print(a[i]);
//
//    if (a[0] < a[1]) return a[0]; else return a[3];
////    b[1] = 99;
////    return b[1];
//}


//int main() {
////    int i = 0;
//    float a[5] = {1.1, 5.3, 3.4, 6.2, 2.9};
//    if (a[0] < a[1]) print(a[0]); //else print(a[1]);
////    int sum = 0;
////    do {
////        if (a[i] == 2) {
////            break;
////        }
////        sum  = sum + a[i];
////        i = i+1;
////    } while(i<5);
////
////    print(sum);
//
//    return 0;
//}

float arr[10] = {52.1, 5.1, 34.1, 6.1, 23.1, 7.1, 2.1, 38.1, 4.1, 16.1};
//int arr[10] = {18, 15, 43, 32, 21, 92, 35, 23, 12, 22};
void SelectionSort(int size) {
    int i, j, k;
//    int tmp;
    float tmp;
    for (i = 0; i < size - 1; i=i+1) {
        k = i;
        for (j = i + 1; j < size; j=j+1) {
            if (arr[j] < arr[k]) {
                k = j;
            }
        }
        tmp = arr[k];
        arr[k] = arr[i];
        arr[i] = tmp;
    }
    return ;
}


void InsertionSort(int size)
{
    int i, j;
//    int tmp;
    float tmp;
    for (i = 1; i < size; i=i+1) {
        if (arr[i] < arr[i-1]) {
            tmp = arr[i];
            for (j = i - 1; j >= 0 && arr[j] > tmp; j=j-1) {
                arr[j+1] = arr[j];
            }
            arr[j+1] = tmp;
        }
    }
    return;
}


void BubbleSort(int size)
{
    int i, j;
    //    int tmp;
    float tmp;
    for (i = 0; i < size - 1; i=i+1) {
        for (j = 0; j < size - i - 1; j=j+1) {
            if (arr[j] > arr[j+1]) {
                tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
    return;
}

//void QuickSort(int begin, int end)
//{
//    int i, j;
//    //    int tmp;
//    float temp;
//    if (begin < end) {
//        i = begin + 1;
//        j = end;
//        while (i < j) {
//            if(arr[i] > arr[begin]) {
//                temp = arr[i];
//                arr[i] = arr[j];
//                arr[j] = temp;
//                j = j-1;
//            } else {
//                i=i+1;
//            }
//        }
//        if (arr[i] >= arr[begin]) {
//            i=i-1;
//        }
//
//        temp = arr[begin];
//        arr[begin] = arr[i];
//        arr[i] = temp;
//        QuickSort(begin, i);
//        QuickSort(j, end);
//    }
//    return;
//}
//



//float a[3] = {52.1, 5.1, 34.1};
//float a[3] = {52.1, 65.6, 34.1};
////float a[3] = {52.1, 75.1, 89.1};
////float a[2] = {52.1, 75.1};
//float a[2] = {75.1, 52.1};
//float a[1] = {52.1};
//float a[2] = {52.1, 75.1};
//void quicksort(int first,int last){
//    int i, j, pivot;
//    int temp;
////    float temp;
//
//    if(first<last){
//        pivot=first;
//        i=first;
//        j=last;
//
//        while(i<j){
//            while(a[i]<=a[pivot] && i<last)
//                i = i+1;
//            while(a[j]>a[pivot])
//                j = j-1;
//            if(i<j){
//                temp=a[i];
//                a[i]=a[j];
//                a[j]=temp;
//            }
//        }
//
//        temp=a[pivot];
//        a[pivot]=a[j];
//        a[j]=temp;
//
//        quicksort(first,j-1);
//        quicksort(j+1,last);
//    }
//    return;
//}
//
int main(){
    int i, count = 10;

    BubbleSort(count);
//    QuickSort(10,0,9);

    for(i=0;i<count;i=i+1)
        print(arr[i]);

    return 0;
}