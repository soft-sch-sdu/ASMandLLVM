//int main(){
//    int k[3] = {19, 18, 17};
//
//    k[1] = k[2];
//    return k[1];
//}

//float a[10] = {52.1, 5.1, 34.1,6.1,23.1, 7.1,2.1, 38.1, 4.1,16.1 };
//void quicksort(int m, int n)
///* recursively sorts a[m] through a[n] */
//{
//    int i, j;
//    float v, x;
//    if (n <= m) return;
///* fragment begins here */
//    i = m-1; j = n; v = a[n];
//    while (1) {
//        do i = i+1; while (a[i] < v);
//        do j = j-1; while (a[j] > v);
//        if (i >= j) break;
//        x = a[i]; a[i] = a[j]; a[j] = x; /* swap a[i], a[j] */
//    }
//    x= a[i]; a[i] = a[n]; a[n] = x; /* swap a[i], a[n] */
///* fragment ends here */
//    quicksort(m,j);
//    quicksort(i+1,n);
//}
//
//int main() {
//    int n = 10;
//    int i;
//
//    quicksort(0,9);
//
//    for (i = 0; i < n; i=i+1) {
//        print(a[i]);
//    }
//
//    return 0;
//}
//


//float a[4] = {18.1, 15.3, 2.3, 21.1};
//int main() {
//    int a[4] = {18, 15, 2, 21}; //,76.7, 23.4, 7.9 };
//    int n=4;  //存放数组a中元素的个数
//    int i;  //比较的轮数
//    int j;  //每轮比较的次数
//    int k;
//    int buf;  //交换数据时用于存放中间数据
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
//
//    return a[0];
//}
int b[4] = {28, 25, 22, 17};
int main(){
    int a[4] = {18, 15, 2, 21};
    int n=4;
    int i;
    int j;
    int k;
    int buf;

    i = 0;
    while ( i < n - 1  )
    {
        j = 0;
        while ( j < n - 1 - i)
        {
            if (a[j] > a[j + 1]) {
                buf = a[j];
                a[j] = a[j + 1];
                a[j + 1] = buf;
            }
            j=j+1;
        }
        i= i+1;
    }

    if (a[0] < a[1]) return 5; else return 2;
//    return 3;
}