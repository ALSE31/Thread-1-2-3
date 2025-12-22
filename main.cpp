#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

const int SIZE = 100000;

class timeChart {
public:
    void operator()(void (*f)(int* mas), int* mas)
    {
        auto start = std::chrono::steady_clock::now();
        f(mas);
        auto finish = std::chrono::steady_clock::now();
        std::cout << "time=" << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "mcs\n";
    }
};

void init(int* mas) {
    for (int i = 0; i < SIZE; i++)
        mas[i] = SIZE - i;
}

int* GetStandart() {
    int* mas = new int[SIZE];
    for (int i = 0; i < SIZE; i++)
        mas[i] = i + 1;
    return mas;
}

bool isEqual(int* mas, int* mas2) {
    for (int i = 0; i < SIZE; i++) {
        if (mas[i] != mas2[i]) {
            return false;
        }
    }
    return true;
}

bool isSorted(int* mas, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (mas[i] > mas[i + 1]) {
            return false;
        }
    }
    return true;
}

void PrintAll(int* mas) {
    for (int i = 0; i < SIZE; i++) {
        std::cout << mas[i] << "\n";
    }
}

void quickSort(int* mas, int start, int finish)
{
    int i = start;
    int j = finish - 1;
    int  midVal = mas[(j + i) / 2];
    do {

        while (mas[i] < midVal) i++;
        while (mas[j] > midVal) j--;
        if (i <= j) {
            int tmp = mas[i];
            mas[i] = mas[j];
            mas[j] = tmp;
            i++;
            j--;
        }
    } while (i <= j);
    if (j > start)
    {
        quickSort(mas, start, j);
    }
    if (i < finish)
    {
        quickSort(mas, i, finish);
    }
}

void quickThreadSort(int* mas, int start, int finish)
{
    int i = start;
    int j = finish - 1;
    int  midVal = mas[(j + i) / 2];
    do {

        while (mas[i] < midVal) i++;
        while (mas[j] > midVal) j--;
        if (i <= j) {
            int tmp = mas[i];
            mas[i] = mas[j];
            mas[j] = tmp;
            i++;
            j--;
        }
    } while (i <= j);
    if (i < j)
    {
        std::thread* pool = new std::thread[2];
        pool[0] = std::thread(quickThreadSort, mas, start, j + 1);
        pool[1] = std::thread(quickThreadSort, mas, i - 1, finish);
        if (j > start)
        {
            pool[0].join();
        }
        if (i < finish)
        {
            pool[1].join();
        }
    }
}

void merge(int* mas, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* leftArr = new int[n1];
    int* rightArr = new int[n2];

    for (int i = 0; i < n1; i++)
        leftArr[i] = mas[left + i];
    for (int i = 0; i < n2; i++)
        rightArr[i] = mas[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            mas[k] = leftArr[i];
            i++;
        }
        else {
            mas[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        mas[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        mas[k] = rightArr[j];
        j++;
        k++;
    }

    delete[] leftArr;
    delete[] rightArr;
}

void mergeSort(int* mas, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(mas, left, mid);
        mergeSort(mas, mid + 1, right);
        merge(mas, left, mid, right);
    }
}

void mergeSortParallel(int* mas, int left, int right, int depth = 0) {
    const int MAX_DEPTH = 3;
    const int MIN_SIZE_FOR_THREADS = 1000;

    if (left >= right) return;

    bool use_threads = (depth < MAX_DEPTH) && ((right - left + 1) > MIN_SIZE_FOR_THREADS);
    int mid = left + (right - left) / 2;

    if (use_threads) {
        std::thread left_thread(mergeSortParallel, mas, left, mid, depth + 1);
        std::thread right_thread(mergeSortParallel, mas, mid + 1, right, depth + 1);
        left_thread.join();
        right_thread.join();
    }
    else {
        mergeSort(mas, left, mid);
        mergeSort(mas, mid + 1, right);
    }

    merge(mas, left, mid, right);
}

void stdSortWrapper(int* mas) {
    std::sort(mas, mas + SIZE);
}

int main()
{
    std::cout << "Sorting " << SIZE << " elements\n\n";

    {
        int* mas = new int[SIZE];
        auto start = std::chrono::steady_clock::now();
        init(mas);
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of init mas = " << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "mcs\n";
        delete[] mas;
        std::cout << "\n";
    }

    {
        int* mas = new int[SIZE];
        init(mas);
        auto start = std::chrono::steady_clock::now();
        quickSort(mas, 0, SIZE);
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of quick sort (single thread) = "
            << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
            << "mcs\n";
        int* standard = GetStandart();
        if (isEqual(mas, standard)) {
            std::cout << "Correctly sorted\n";
        }
        else {
            std::cout << "Not sorted correctly\n";
        }
        delete[] standard;
        delete[] mas;
        std::cout << "\n";
    }

    {
        int* mas = new int[SIZE];
        init(mas);
        auto start = std::chrono::steady_clock::now();
        quickThreadSort(mas, 0, SIZE);
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of quick thread sort = "
            << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
            << "mcs\n";
        int* standard = GetStandart();
        if (isEqual(mas, standard)) {
            std::cout << "Correctly sorted\n";
        }
        else {
            std::cout << "Not sorted correctly\n";
        }
        delete[] standard;
        delete[] mas;
        std::cout << "\n";
    }

    {
        int* mas = new int[SIZE];
        init(mas);
        auto start = std::chrono::steady_clock::now();
        mergeSort(mas, 0, SIZE - 1); // Исправлено: SIZE-1 вместо SIZE
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of merge sort (single thread) = "
            << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
            << "mcs\n";
        int* standard = GetStandart();
        if (isEqual(mas, standard)) {
            std::cout << "Correctly sorted\n";
        }
        else {
            std::cout << "Not sorted correctly\n";
        }
        delete[] standard;
        delete[] mas;
        std::cout << "\n";
    }

    {
        int* mas = new int[SIZE];
        init(mas);
        auto start = std::chrono::steady_clock::now();
        mergeSortParallel(mas, 0, SIZE - 1); // Исправлено: SIZE-1 вместо SIZE
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of parallel merge sort = "
            << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
            << "mcs\n";
        int* standard = GetStandart();
        if (isEqual(mas, standard)) {
            std::cout << "Correctly sorted\n";
        }
        else {
            std::cout << "Not sorted correctly\n";
        }
        delete[] standard;
        delete[] mas;
        std::cout << "\n";
    }

    {
        int* mas = new int[SIZE];
        init(mas);
        auto start = std::chrono::steady_clock::now();
        stdSortWrapper(mas);
        auto finish = std::chrono::steady_clock::now();
        std::cout << "Time of std::sort = "
            << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count()
            << "mcs\n";
        int* standard = GetStandart();
        if (isEqual(mas, standard)) {
            std::cout << "Correctly sorted\n";
        }
        else {
            std::cout << "Not sorted correctly\n";
        }
        delete[] standard;
        delete[] mas;
        std::cout << "\n";
    }

    return 0;
}
