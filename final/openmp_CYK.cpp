#pragma GCC optimize("Ofast")
#include <algorithm>
#include <bitset>
#include <cstdio>

using namespace std;

const int MAX_PRODUCTION2_NUM = 512;
const int MAX_PRODUCTION1_NUM = 128;
const int MAX_VN = 128;
const int MAX_VT = 128;
const int MAX_SLEN = 1024;

typedef struct Production1 {
  int parent;
  int child;
} Production1;
typedef struct Production2 {
  int parent;
  int child1;
  int child2;
} Production2;
typedef struct Interval {
  int end;
  int len;
} Interval;

int vn_num, p2_num, p1_num, slen;
Production2 p2[MAX_PRODUCTION2_NUM];
Production1 p1[MAX_PRODUCTION1_NUM];
char str[MAX_SLEN];
int dp[MAX_SLEN][MAX_SLEN][MAX_VN];  // 区间 [i, j] 由非终结符到情况总数的映射

// 解决旧版本GCC的OpenMP和optimize编译选项冲突的问题，详情见https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82374
void omp_for_1(int index) {
  // 枚举所有终结产生式
  for (int k = 0; k < p1_num; ++k) {
    if (str[index] == p1[k].child) {
      dp[index][index][p1[k].parent] = 1;
    }
  }
}
void omp_for_2(int len, int start) {
  int end = start + len - 1;
  for (int mid = start; mid < end; ++mid) {
    // 枚举所有非终结产生式
    for (int i = 0; i < p2_num; ++i) {
      dp[start][end][p2[i].parent] +=
          dp[start][mid][p2[i].child1] * dp[mid + 1][end][p2[i].child2];
    }
  }
}

// 对于结果总数较小的情况，进行按需计算
Interval c2i[MAX_VN][MAX_VN];  // 将产生式的两个child，映射到产生式的index范围
int vn_set[MAX_SLEN][MAX_SLEN][MAX_VN + 5];  // 区间 [i, j] 可以由哪些非终结符推导出来，这里用数组模拟vector

void omp_for_3(int index) {
  bitset<MAX_VN> bst;
  // 枚举所有终结产生式
  for (int k = 0; k < p1_num; ++k) {
    if (str[index] == p1[k].child) {
      dp[index][index][p1[k].parent] = 1;
      bst.set(p1[k].parent);
    }
  }
  for (int i = 0; i < vn_num; ++i) {
    if (bst[i]) vn_set[index][index][++vn_set[index][index][0]] = i;
  }
}
void omp_for_4(int len, int start) {
  bitset<MAX_VN> bst;
  int end = start + len - 1;
  for (int mid = start; mid < end; ++mid) {
    // 按需计算，组合出所有可能的结果
    for (int i = 1; i <= vn_set[start][mid][0]; ++i) {
      int vn1 = vn_set[start][mid][i];
      for (int j = 1; j <= vn_set[mid + 1][end][0]; ++j) {
        int vn2 = vn_set[mid + 1][end][j];
        int right = c2i[vn1][vn2].end;
        int left = right - c2i[vn1][vn2].len;
        for (int i = right; i > left; --i) {
          dp[start][end][p2[i].parent] +=
              dp[start][mid][p2[i].child1] * dp[mid + 1][end][p2[i].child2];
          bst.set(p2[i].parent);
        }
      }
    }
  }
  for (int i = 0; i < vn_num; ++i) {
    if (bst[i]) vn_set[start][end][++vn_set[start][end][0]] = i;
  }
}

int main() {
  freopen("input.txt", "r", stdin);
  scanf("%d\n", &vn_num);
  scanf("%d\n", &p2_num);
  for (int i = 0; i < p2_num; i++)
    scanf("<%d>::=<%d><%d>\n", &p2[i].parent, &p2[i].child1, &p2[i].child2);
  scanf("%d\n", &p1_num);
  for (int i = 0; i < p1_num; i++)
    scanf("<%d>::=%c\n", &p1[i].parent, &p1[i].child);
  scanf("%d\n", &slen);
  scanf("%s\n", str);

  if (p2_num < 300 || slen < 500) {
    // dp之前的必要初始化
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < slen; ++i) {
      omp_for_1(i);
    }

    // 区间 dp
    for (int len = 2; len <= slen; ++len) {
      #pragma omp parallel for schedule(dynamic)
      for (int start = 0; start <= slen - len; ++start) {
        omp_for_2(len, start);
      }
    }
  } else {
    // TODO 并行优化，对产生式进行排序
    sort(p2, p2 + p2_num, [](const Production2& a, const Production2& b) {
      if (a.child1 != b.child1)
        return a.child1 < b.child1;
      else if (a.child2 != b.child2)
        return a.child2 < b.child2;
      else
        return a.parent < b.parent;
    });

    // 预处理出chlid2index
    for (int i = 0; i < p2_num; ++i) {
      c2i[p2[i].child1][p2[i].child2].end = i;
      c2i[p2[i].child1][p2[i].child2].len++;
    }

    // dp之前的必要初始化
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < slen; ++i) {
      omp_for_3(i);
    }

    // 区间 dp
    for (int len = 2; len <= slen; ++len) {
      #pragma omp parallel for schedule(dynamic)
      for (int start = 0; start <= slen - len; ++start) {
        omp_for_4(len, start);
      }
    }
  }

  printf("%u\n", dp[0][slen - 1][0]);
  return 0;
}