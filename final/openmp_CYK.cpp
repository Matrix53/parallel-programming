#include <cstdio>

using namespace std;

const int MAX_VN = 128;
const int MAX_VT = 256;
const int MAX_SLEN = 1024;

int vn_num, p2_num, p1_num, slen;
int p2[MAX_VN][3];
int p1[MAX_VT][2];
char str[MAX_SLEN];
int dp[MAX_SLEN][MAX_SLEN][MAX_VN];  // 区间 [i, j] 由非终结符到情况总数的映射

int main() {
  freopen("input.txt", "r", stdin);
  scanf("%d\n", &vn_num);
  scanf("%d\n", &p2_num);
  for (int i = 0; i < p2_num; i++)
    scanf("<%d>::=<%d><%d>\n", &p2[i][0], &p2[i][1], &p2[i][2]);
  scanf("%d\n", &p1_num);
  for (int i = 0; i < p1_num; i++) scanf("<%d>::=%c\n", &p1[i][0], &p1[i][1]);
  scanf("%d\n", &slen);
  scanf("%s\n", str);

  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < slen; ++i) {
    // 枚举所有终结产生式
    for (int k = 0; k < p1_num; ++k) {
      if (str[i] == p1[k][1]) dp[i][i][p1[k][0]] = 1;
    }
  }

  // 区间 dp
  for (int len = 2; len <= slen; ++len) {
    #pragma omp parallel for schedule(dynamic)
    for (int start = 0; start <= slen - len; ++start) {
      int end = start + len - 1;
      for (int mid = start; mid < end; ++mid) {
        // 枚举所有非终结产生式
        for (int i = 0; i < p2_num; ++i) {
          dp[start][end][p2[i][0]] +=
              dp[start][mid][p2[i][1]] * dp[mid + 1][end][p2[i][2]];
        }
      }
    }
  }

  printf("%u\n", dp[0][slen - 1][0]);
  return 0;
}