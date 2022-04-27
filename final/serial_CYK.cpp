#include <iostream>
#include <algorithm>
#include <omp.h>
#include <time.h>
#include <memory.h>

using namespace std;

#define MAX_PRODUCTION2_NUM 512
#define MAX_PRODUCTION1_NUM 128
#define MAX_VN_NUM 128
#define MAX_VT_NUM 128
#define MAX_STRING_LENGTH 1024

struct BeginAndNum
{
    int begin;
    unsigned num;
};

struct Production2
{
    int parent;
    int child1;
    int child2;
} production2[MAX_PRODUCTION2_NUM];

struct Production1
{
    int parent;
    char child;
} production1[MAX_PRODUCTION1_NUM];

BeginAndNum vnIndex[MAX_VN_NUM][MAX_VN_NUM];
BeginAndNum vtIndex[MAX_VT_NUM];

char str[MAX_STRING_LENGTH];

struct SubTree
{
    int root;
    unsigned num;
} subTreeTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH][MAX_VN_NUM];

int subTreeNumTable[MAX_STRING_LENGTH][MAX_STRING_LENGTH];

int vn_num;
int production2_num;
int production1_num;
int string_length;

int main()
{
    freopen("input.txt", "r", stdin);
    scanf("%d\n", &vn_num);
    scanf("%d\n", &production2_num);
    for (int i = 0; i < production2_num; i++)
        scanf("<%d>::=<%d><%d>\n", &production2[i].parent, &production2[i].child1, &production2[i].child2);
    scanf("%d\n", &production1_num);
    for (int i = 0; i < production1_num; i++)
        scanf("<%d>::=%c\n", &production1[i].parent, &production1[i].child);
    scanf("%d\n", &string_length);
    scanf("%s\n", str);

    sort(production1, production1 + production1_num, [](const Production1& a, const Production1& b)
    {
        return a.child == b.child ? a.parent < b.parent : a.child < b.child;
    });
    for (int i = 0; i < MAX_VT_NUM; i++)
    {
        vtIndex[i].begin = -1;
        vtIndex[i].num = 0;
    }
    for (int i = 0; i < production1_num; i++)
    {
        int t = production1[i].child;
        if (vtIndex[t].begin == -1)
            vtIndex[t].begin = i;
        vtIndex[t].num++;
    }
    for (int i = 0; i < string_length; i++)
    {
        int t = str[i];
        int begin = vtIndex[t].begin;
        int end = begin + vtIndex[t].num;
        for (int j = begin; j < end; j++)
        {
            SubTree subTree;
            subTree.root = production1[j].parent;
            subTree.num = 1;
            subTreeTable[i][i][subTreeNumTable[i][i]++] = subTree;
        }
    }

    sort(production2, production2 + production2_num, [](const Production2& a, const Production2& b)
    {
        return a.child1 == b.child1 ?
            (a.child2 == b.child2 ? a.parent < b.parent : a.child2 < b.child2)
            : a.child1 < b.child1;
    });
    for (int i = 0; i < vn_num; i++)
    {
        for (int j = 0; j < vn_num; j++)
        {
            vnIndex[i][j].begin = -1;
            vnIndex[i][j].num = 0;
        }
    }
    for (int i = 0; i < production2_num; i++)
    {
        int n1 = production2[i].child1;
        int n2 = production2[i].child2;
        if (vnIndex[n1][n2].begin == -1)
            vnIndex[n1][n2].begin = i;
        vnIndex[n1][n2].num++;
    }
    for (int len = 2; len <= string_length; len++)
    {
        for (int left = 0; left <= string_length - len; left++)
        {
            SubTree subTreeBuf[2][MAX_STRING_LENGTH];
            //memset(subTreeBuf, 0, sizeof subTreeBuf);
            int curr = 0;
            int last = 1;
            int oldTreeNum = 0;
            for (int right = left + 1; right < left + len; right++)
            {
                //printf("[%d, %d] = [%d, %d] + [%d, %d]\n", left, left + len, left, right, right, left + len);
                for (int i1 = 0; i1 < subTreeNumTable[left][right - 1]; i1++)
                {
                    SubTree subTreeChild1 = subTreeTable[left][right - 1][i1];
                    for (int i2 = 0; i2 < subTreeNumTable[right][left + len - 1]; i2++)
                    {
                        SubTree subTreeChild2 = subTreeTable[right][left + len - 1][i2];
                        int begin = vnIndex[subTreeChild1.root][subTreeChild2.root].begin;
                        int end = begin + vnIndex[subTreeChild1.root][subTreeChild2.root].num;
                        if (begin == end)
                        {
                            continue;
                        }
                        swap(last, curr);
                        int newTreeNum = 0;
                        int k = 0;
                        for (int j = begin; j < end; j++)
                        {
                            SubTree subTreeParent;
                            subTreeParent.root = production2[j].parent;
                            subTreeParent.num = subTreeChild1.num * subTreeChild2.num;
                            while (k < oldTreeNum && subTreeParent.root > subTreeBuf[last][k].root)
                                subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];
                            if (k < oldTreeNum && subTreeParent.root == subTreeBuf[last][k].root)
                                subTreeParent.num += subTreeBuf[last][k++].num;
                            subTreeBuf[curr][newTreeNum++] = subTreeParent;
                        }
                        while (k < oldTreeNum)
                        {
                            subTreeBuf[curr][newTreeNum++] = subTreeBuf[last][k++];
                        }
                        oldTreeNum = newTreeNum;
                    }
                }
            }
            subTreeNumTable[left][left + len - 1] = oldTreeNum;
            if (subTreeNumTable[left][left + len - 1] > 0)
            {
                memcpy(subTreeTable[left][left + len - 1], subTreeBuf[curr], subTreeNumTable[left][left + len - 1] * sizeof(SubTree));
            }
        }
    }
    unsigned treeNum = 0;
    if (subTreeNumTable[0][string_length - 1] > 0)
    {
        if (subTreeTable[0][string_length - 1][0].root == 0)
        {
            treeNum = subTreeTable[0][string_length - 1][0].num;
        }
    }
    printf("%u\n", treeNum);
    return 0;
}