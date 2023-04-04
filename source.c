int a[5] = {1, 2, 3, 4, 5};

void quicksort(int m, int n)
/* recursively sorts a[m] through a[n] */
{
    int i, j;
    int v, x;
    if (n <= m) return;
/* fragment begins here */
    i = m - 1;
    j = n;
    v = a[n];
    while (1) {
        do i = i + 1; while (a[i] < v);
        do j = j - 1; while (a[j] > v);
        if (i >= j) break;
        x = a[i];
        a[i] = a[j];
        a[j] = x; /* swap a[i], a[j] */
    }
    x = a[i];
    a[i] = a[n];
    a[n] = x; /* swap a[i], a[n] */
/* fragment ends here */
    quicksort(m, j);
    quicksort(i + 1, n);
}