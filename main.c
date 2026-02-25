#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CIDADES   200
#define MAX_NOME      21
#define MAX_CONEXOES  200
#define INF           0x3f3f3f3f

typedef struct {
    int to;
    int w;
} Edge;

typedef struct {
    Edge e[MAX_CONEXOES];
    int deg;
} Adj;

static Adj g[MAX_CIDADES];

static int cityIndex(char cities[][MAX_NOME], int n, const char *name) {
    for (int i = 0; i < n; i++) if (strcmp(cities[i], name) == 0) return i;
    return -1;
}

static int pathLess(const int *pathA, int lenA, const int *pathB, int lenB, char cities[][MAX_NOME]) {
    int m = (lenA < lenB) ? lenA : lenB;
    for (int i = 0; i < m; i++) {
        int cmp = strcmp(cities[pathA[i]], cities[pathB[i]]);
        if (cmp < 0) return 1;
        if (cmp > 0) return 0;
    }
    return (lenA < lenB);
}

static int buildPath(int v, int parent[], int outPath[]) {
    int tmp[MAX_CIDADES];
    int len = 0;
    while (v != -1) {
        tmp[len++] = v;
        v = parent[v];
    }
    for (int i = 0; i < len; i++) outPath[i] = tmp[len - 1 - i];
    return len;
}

static int dijkstra(int n, char cities[][MAX_NOME], int src, int dst, int parent_out[], int *dist_out, int *hops_out) {
    int dist[MAX_CIDADES];
    int hops[MAX_CIDADES];
    int used[MAX_CIDADES];
    int parent[MAX_CIDADES];

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        hops[i] = INF;
        used[i] = 0;
        parent[i] = -1;
    }

    dist[src] = 0;
    hops[src] = 1;

    for (int it = 0; it < n; it++) {
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (used[i]) continue;
            if (u == -1) { u = i; continue; }
            if (dist[i] < dist[u]) u = i;
            else if (dist[i] == dist[u]) {
                if (hops[i] < hops[u]) u = i;
                else if (hops[i] == hops[u]) {
                    int pA[MAX_CIDADES], pB[MAX_CIDADES];
                    int lA = buildPath(i, parent, pA);
                    int lB = buildPath(u, parent, pB);
                    if (pathLess(pA, lA, pB, lB, cities)) u = i;
                }
            }
        }

        if (u == -1 || dist[u] == INF) break;
        used[u] = 1;

        for (int k = 0; k < g[u].deg; k++) {
            int v = g[u].e[k].to;
            int w = g[u].e[k].w;

            int nd = dist[u] + w;
            int nh = hops[u] + 1;

            int improve = 0;
            if (nd < dist[v]) improve = 1;
            else if (nd == dist[v]) {
                if (nh < hops[v]) improve = 1;
                else if (nh == hops[v]) {
                    int curP[MAX_CIDADES], candP[MAX_CIDADES];
                    int curLen = buildPath(v, parent, curP);

                    int up[MAX_CIDADES];
                    int upLen = buildPath(u, parent, up);
                    for (int t = 0; t < upLen; t++) candP[t] = up[t];
                    candP[upLen] = v;
                    int candLen = upLen + 1;

                    if (pathLess(candP, candLen, curP, curLen, cities)) improve = 1;
                }
            }

            if (improve) {
                dist[v] = nd;
                hops[v] = nh;
                parent[v] = u;
            }
        }
    }

    if (dist[dst] == INF) return 0;

    for (int i = 0; i < n; i++) parent_out[i] = parent[i];
    *dist_out = dist[dst];
    *hops_out = hops[dst];
    return 1;
}

static void printSchedule(char cities[][MAX_NOME], int path[], int pathLen, int showHours, int totalFlightHours) {
    long long t = 0;
    int lastPrintedDay = 0;
    int currentDay = 0;
    int firstInDay = 1;

    for (int i = 0; i < pathLen; i++) {
        int arrival = (int)t;
        int day = arrival / 24 + 1;

        if (currentDay != 0 && day != currentDay) {
            printf("\n");
            lastPrintedDay = currentDay;
            firstInDay = 1;
        }

        if (currentDay == 0) {
            for (int d = 1; d < day; d++) {
                printf("%d:\n", d);
                lastPrintedDay = d;
            }
        } else {
            for (int d = lastPrintedDay + 1; d < day; d++) {
                printf("%d:\n", d);
                lastPrintedDay = d;
            }
        }

        if (day != currentDay) {
            printf("%d:", day);
            currentDay = day;
            firstInDay = 1;
        }

        printf("%s%s", firstInDay ? " " : " ", cities[path[i]]);
        firstInDay = 0;

        t += showHours;
        if (i + 1 < pathLen) {
            int u = path[i], v = path[i + 1];
            int w = 0;
            for (int k = 0; k < g[u].deg; k++) if (g[u].e[k].to == v) { w = g[u].e[k].w; break; }
            t += w;
        }
    }

    printf("\n%d\n", totalFlightHours);
}

int main(void) {
    int conjunto = 1;

    while (1) {
        int n;
        if (scanf("%d", &n) != 1) break;

        char cities[MAX_CIDADES][MAX_NOME];
        for (int i = 0; i < n; i++) if (scanf("%20s", cities[i]) != 1) return 0;

        for (int i = 0; i < n; i++) g[i].deg = 0;

        for (int i = 0; i < n; i++) {
            int q;
            if (scanf("%d", &q) != 1) return 0;
            for (int j = 0; j < q; j++) {
                char destName[MAX_NOME];
                int w;
                if (scanf("%20s %d", destName, &w) != 2) return 0;
                int v = cityIndex(cities, n, destName);
                if (v >= 0 && g[i].deg < MAX_CONEXOES) {
                    g[i].e[g[i].deg].to = v;
                    g[i].e[g[i].deg].w = w;
                    g[i].deg++;
                }
            }
        }

        int m;
        if (scanf("%d", &m) != 1) return 0;

        printf("Conjunto #%d\n", conjunto);

        for (int a = 0; a < m; a++) {
            char artist[MAX_NOME];
            int x;
            char origem[MAX_NOME], destino[MAX_NOME];

            if (scanf("%20s %d %20s %20s", artist, &x, origem, destino) != 4) return 0;

            printf("%s\n", artist);

            int src = cityIndex(cities, n, origem);
            int dst = cityIndex(cities, n, destino);

            if (src < 0 || dst < 0) {
                printf("turne cancelada\n");
                if (a != m - 1) printf("\n");
                continue;
            }

            int parent[MAX_CIDADES];
            int dist, hops;

            int ok = dijkstra(n, cities, src, dst, parent, &dist, &hops);

            if (!ok) {
                printf("turne cancelada\n");
                if (a != m - 1) printf("\n");
                continue;
            }

            int path[MAX_CIDADES];
            int pathLen = buildPath(dst, parent, path);

            printSchedule(cities, path, pathLen, x, dist);

            if (a != m - 1) printf("\n");
        }

        printf("\n");
        conjunto++;
    }

    return 0;
}