#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
using namespace std;

// 配置参数 
#define TEST_TIMES 10
#define N 1024

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
    int *mat = (int*)malloc(N * N * sizeof(int));
    int *vec = (int*)malloc(N * sizeof(int));
    int *res_naive = (int*)malloc(N * sizeof(int));
    int *res_opt = (int*)malloc(N * sizeof(int));
    int *arr = (int*)malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        vec[i] = 1;
        arr[i] = i;
        for (int j = 0; j < N; j++) {
            mat[i * N + j] = 1;
        }
    }

    cout << "==================== 实验一 测试结果 ====================" << endl;
    cout << "测试规模:N = " << N << "，重复测试次数：" << TEST_TIMES << endl << endl;

    // 矩阵
    cout << "---------------- 矩阵列-向量内积 性能测试 ----------------" << endl;
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

    int correct = 1;
    for (int i = 0; i < N; i++) {
        if (res_naive[i] != res_opt[i] || res_naive[i] != N) {
            correct = 0;
            break;
        }
    }

    cout << "平凡算法平均耗时：" << fixed << setprecision(6) << t_naive << "s" << endl;
    cout << "Cache优化平均耗时:" << fixed << setprecision(6) << t_opt << "s" << endl;
    cout << "性能提升" << fixed << setprecision(2) << t_naive / t_opt << " 倍" << endl;
    cout << "结果" << (correct ? "正确" : "错误") << endl << endl;

    // 数组求和
    cout << "---------------- 数组求和 性能测试 ----------------" << endl;
    double t_chain = 0, t_2way = 0, t_rec = 0;
    long long sum_chain, sum_2way, sum_rec;

    for (int t = 0; t < TEST_TIMES; t++) {
        double start = get_time();
        sum_chain = sum_chain_naive(arr, N);
        t_chain += get_time() - start;

        start = get_time();
        sum_2way = sum_chain_2way(arr, N);
        t_2way += get_time() - start;

        start = get_time();
        sum_rec = sum_recursive_wrapper(arr, N);
        t_rec += get_time() - start;
    }

    t_chain /= TEST_TIMES;
    t_2way /= TEST_TIMES;
    t_rec /= TEST_TIMES;

    long long expect = (long long)N * (N-1) / 2;
    correct = (sum_chain == expect && sum_2way == expect && sum_rec == expect);

    cout << "平凡累加耗时：" << fixed << setprecision(6) << t_chain << "s 结果：" << sum_chain << endl;
    cout << "两路链式耗时：" << fixed << setprecision(6) << t_2way << "s 结果：" << sum_2way << endl;
    cout << "递归两路耗时：" << fixed << setprecision(6) << t_rec << "s 结果：" << sum_rec << endl;
    cout << "性能提升" << fixed << setprecision(2) << t_chain / t_2way << " 倍" << endl;
    cout << "结果" << (correct ? "正确" : "错误") << endl << endl;

    free(mat);
    free(vec);
    free(res_naive);
    free(res_opt);
    free(arr);

    cout << "==================== 测试完成 ====================" << endl;
    return 0;
}