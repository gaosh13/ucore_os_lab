#include <stdio.h>
#include <string.h>

#define MAXM 100
#define MAXN 100
int rs[MAXM];
int rsp[MAXM];
int tot[MAXN][MAXM];
int req[MAXN][MAXM];
int temp[MAXN][MAXM];
int flagn[MAXN];

int n, m;

int check_safe()
{
    int flag = 1;
    bzero(&flagn, n*sizeof(int));
    for (;flag;)
    {
        flag = 0;
        for (int i = 0; i < n; ++i)
        {
            if (flagn[i])
                continue;
            int j = 0;
            for (; j < m && tot[i][j]-temp[i][j] <= rsp[j]; ++j);
            if (j == m)
            {
                flag = 1;
                flagn[i] = 1;
                for (j = 0; j < m; ++j)
                    rsp[j] += temp[i][j];
            }
        }
    }
    for (int i = 0; i < n; ++i)
        if (!flagn[i]) return 0;
    return 1;
}

int show()
{
    printf("resources remain:\n");
    for (int i = 0; i < m; ++i)
        printf("%d\t", rs[i]);
    puts("");
    printf("total, request and remain matrix:\n");
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < m; ++j)
            printf("%d\t", tot[i][j]);
        printf("\t\t");
        for (int j = 0; j < m; ++j)
            printf("%d\t", req[i][j]);
        printf("\t\t");
        for (int j = 0; j < m; ++j)
            printf("%d\t", tot[i][j]-req[i][j]);
        puts("");
    }
    return 0;
}

int main()
{

    printf("please input n and m\n");
    scanf("%d%d", &n, &m);
    printf("resources of m integers\n");
    for (int i = 0; i < m; ++i)
        scanf("%d", rs+i);
    printf("resouces max n*m matrix\n");
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
        {
            scanf("%d", &tot[i][j]);
            req[i][j] = 0;
            if (tot[i][j] > rs[j])
            {
                printf("proccess %d resource %d error\n", i, j);
                return 0;
            }
        }

    for (;;)
    {
        memcpy(temp, req, sizeof temp);
        memcpy(rsp, rs, sizeof rs);
        printf("process and m resouces, process = -1 to quit, process = n to show\n");
        int flag = 1;
        int x;
        scanf("%d", &x);
        if (x == -1)
            break;
        if (x == n)
        {
            show();
            continue;
        }
        int rq[MAXM];
        for (int j = 0; j < m; ++j)
        {
            scanf("%d", rq+j);
            temp[x][j] += rq[j];
            rsp[j] -= rq[j];
            if (temp[x][j] > tot[x][j])
            {
                flag = 0;
                printf("process %d total resource %d error\n", x, j);
            }
            if (rsp[j] < 0)
            {
                flag = 0;
                printf("lack of resource %d\n", j);
            }
        }
        if (flag)
        {
            flag = check_safe();
            if (!flag) printf("unsafe and ");
        }
        if (!flag)
        {
            printf("reject\n");
            continue;
        } else
        {
            printf("safe and accept\n");
        }
        for (int j = 0; j < m; ++j)
        {
            req[x][j] += rq[j];
            rs[j] -= rq[j];
        }
    }
    return 0;
}
