#include<iostream>
#include<stdlib.h>

using namespace std;

long long p = 25221961025508539;
long long num0 = 1914483117172565;
long long num1 = 15349972598427081;
long long num2 = 2510729161496127;
/*
long long p = 65537;
long long num0 = 19513;
long long num1 = 35890;
long long num2 = 475;
*/

int main() {
    void *head, *cur, *tmp, *p0, *p1, *p2;
    void *it;
    void *tar, *tari;
    void *f0,*f1,*f2;
    long long i;
    long long inp;
    head = malloc(sizeof(void *));
    cur = head;
    for (i=1; i<p ;i++) {
        tmp = malloc(sizeof(void *));
        *(void **)cur = tmp;
        cur = tmp;
    }
    *(void **)cur = head;
    i = 0;
    f0 = head;
    while (i<num0) {
        i++;
        f0 = *(void **)f0;
    }
    i = 0;
    f1 = head;
    while (i<num1) {
        i++;
        f1 = *(void **)f1;
    }
    i = 0;
    f2 = head;
    while (i<num2) {
        i++;
        f2 = *(void **)f2;
    }
    cin>>inp;
    i = 0;
    tar = head;
    while (i<inp) {
        i++;
        tar = *(void **)tar;
    }
    tari = head;
    p0 = head;
    p1 = head;
    p2 = *(void **)head;
    long long ans;
    ans = 0;
    while (tari != tar) {
        it = head;
        while (it != p1) {
            it = *(void **)it;
            p0 = *(void **)p0;
        }
        it = head;
        while (it != p2) {
            it = *(void **)it;
            p0 = *(void **)p0;
        }
        tmp = p0;
        p0 = p1;
        p1 = p2;
        p2 = tmp;
        tari = *(void **)tari;
        ans++;
    }
    
    if (p0==f0&&p1==f1&&p2==f2) {
        cout<<"flag{"<<ans<<"}"<<endl;
    }
}
