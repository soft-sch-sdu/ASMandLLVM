
int main() {
    int a[5] = {2, 3, 58, 34, 76};
    int n=5;  //存放数组a中元素的个数
    int i;  //比较的轮数
    int j;  //每轮比较的次数
    int buf;  //交换数据时用于存放中间数据
    for (i = 0; i < n - 1; i= i+1)  //比较n-1轮
    {
        for (j = 0; j < n - 1 - i; j=j+1)  //每轮比较n-1-i次,
        {
            if (a[j] < a[j + 1]) {
                buf = a[j];
                a[j] = a[j + 1];
                a[j + 1] = buf;
            }
        }
    }
    for (i = 0; i < n; i=i+1) {
        print(a[i]);
    }
    return 0;
}