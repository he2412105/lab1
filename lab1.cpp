#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
using namespace std;

// 配置参数 
#define TEST_TIMES 10

// 高精度计时函数
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// 矩阵列-向量内积：平凡算法（逐列访问）
void mat_vec_col_naive(int *mat, int *vec, int *res, int n) {
    for (int j = 0; j < n; j++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += mat[i * n + j] * vec[i];
        }
        res[j] = sum;
    }
}

// 矩阵列-向量内积：Cache 优化算法
void mat_vec_col_opt(int *mat, int *vec, int *res, int n) {
    memset(res, 0, n * sizeof(int));
    for (int i = 0; i < n; i++) {
        int v = vec[i];
        for (int j = 0; j < n; j++) {
            res[j] += mat[i * n + j] * v;
        }
    }
}

// 数组求和：平凡算法
long long sum_chain_naive(int *arr, int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

// 数组求和：两路链式（优化）
long long sum_chain_2way(int *arr, int n) {
    long long sum1 = 0, sum2 = 0;
    int i;
    for (i = 0; i < n - 1; i += 2) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
    }
    if (i < n) {
        sum1 += arr[i];
    }
    return sum1 + sum2;
}

// 数组求和：递归两路（优化）
long long sum_recursive_2way(int *arr, int l, int r) {
    if (l == r) return arr[l];
    if (r == l + 1) return arr[l] + arr[r];
    int mid = (l + r) / 2;
    return sum_recursive_2way(arr, l, mid) + sum_recursive_2way(arr, mid + 1, r);
}

long long sum_recursive_wrapper(int *arr, int n) {
    return sum_recursive_2way(arr, 0, n - 1);
}

int main() {
    // 矩阵测试规模
    int mat_sizes[] = {128, 256, 512, 1024, 2048, 4096};
    int num_mat_sizes = sizeof(mat_sizes) / sizeof(mat_sizes[0]);
    
    // 数组求和测试规模
    int arr_sizes[] = {1000, 10000, 100000, 500000, 1000000, 5000000};
    int num_arr_sizes = sizeof(arr_sizes) / sizeof(arr_sizes[0]);
    
    cout << "==================== 实验一 性能测试报告 ====================" << endl;
    cout << "重复测试次数：" << TEST_TIMES << endl << endl;
    
    // 矩阵列-向量内积测试
    cout << "================ 矩阵列-向量内积性能测试 ================" << endl;
    cout << left << setw(12) << "矩阵规模" 
         << setw(16) << "平凡算法(s)" 
         << setw(16) << "Cache优化(s)" 
         << setw(12) << "加速比" 
         << setw(10) << "正确性" << endl;
    cout << string(66, '-') << endl;
    
    for (int idx = 0; idx < num_mat_sizes; idx++) {
        int N = mat_sizes[idx];
        
        // 动态分配内存
        int *mat = (int*)malloc(N * N * sizeof(int));
        int *vec = (int*)malloc(N * sizeof(int));
        int *res_naive = (int*)malloc(N * sizeof(int));
        int *res_opt = (int*)malloc(N * sizeof(int));
        
        // 初始化数据
        for (int i = 0; i < N; i++) {
            vec[i] = 1;
            for (int j = 0; j < N; j++) {
                mat[i * N + j] = 1;
            }
        }
        
        // 测试两种算法.
        double t_naive = 0, t_opt = 0;
        for (int t = 0; t < TEST_TIMES; t++) {
            double start = get_time();
            mat_vec_col_naive(mat, vec, res_naive, N);
            double end = get_time();
            t_naive += end - start;
            
            start = get_time();
            mat_vec_col_opt(mat, vec, res_opt, N);
            end = get_time();
            t_opt += end - start;
        }
        t_naive /= TEST_TIMES;
        t_opt /= TEST_TIMES;
        
        bool correct = true;
        for (int i = 0; i < N; i++) {
            if (res_naive[i] != res_opt[i] || res_naive[i] != N) {
                correct = false;
                break;
            }
        }
        
        cout << left << setw(12) << N 
             << setw(16) << fixed << setprecision(6) << t_naive
             << setw(16) << fixed << setprecision(6) << t_opt
             << setw(12) << fixed << setprecision(2) << (t_naive / t_opt)
             << setw(10) << (correct ? "✓" : "✗") << endl;
        
        free(mat);
        free(vec);
        free(res_naive);
        free(res_opt);
    }
    
    //  第二部分：数组求和测试
    cout << endl << "================ 数组求和性能测试 ================" << endl;
    cout << left << setw(14) << "数组长度" 
         << setw(16) << "平凡累加(s)" 
         << setw(16) << "两路链式(s)" 
         << setw(16) << "递归两路(s)"
         << setw(12) << "加速比" << endl;
    cout << string(74, '-') << endl;
    
    for (int idx = 0; idx < num_arr_sizes; idx++) {
        int n = arr_sizes[idx];
        
        // 动态分配数组
        int *arr = (int*)malloc(n * sizeof(int));
        
        // 初始化
        for (int i = 0; i < n; i++) {
            arr[i] = i;
        }
        
        double t_chain = 0, t_2way = 0, t_rec = 0;
        long long sum_chain, sum_2way, sum_rec;
        
        for (int t = 0; t < TEST_TIMES; t++) {
            double start = get_time();
            sum_chain = sum_chain_naive(arr, n);
            t_chain += get_time() - start;
            
            start = get_time();
            sum_2way = sum_chain_2way(arr, n);
            t_2way += get_time() - start;
            
            start = get_time();
            sum_rec = sum_recursive_wrapper(arr, n);
            t_rec += get_time() - start;
        }
        
        t_chain /= TEST_TIMES;
        t_2way /= TEST_TIMES;
        t_rec /= TEST_TIMES;
        
        long long expect = (long long)n * (n - 1) / 2;
        bool correct = (sum_chain == expect && sum_2way == expect && sum_rec == expect);
        
        cout << left << setw(14) << n 
             << setw(16) << fixed << setprecision(6) << t_chain
             << setw(16) << fixed << setprecision(6) << t_2way
             << setw(16) << fixed << setprecision(6) << t_rec
             << setw(12) << fixed << setprecision(2) << (t_chain / t_2way)
             << (correct ? "✓" : "✗") << endl;
        
        free(arr);
    }
    
    cout << endl << "==================== 测试完成 ====================" << endl;
    return 0;
}