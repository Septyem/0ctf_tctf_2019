#include<iostream>
#include<string>
#include<map>
#include<string.h>

#define ll long long
#define SZ 3


using namespace std;

map<ll, ll> stat;
map<ll, ll>::iterator it;

ll z[SZ][SZ],xx[SZ][SZ],cur[SZ][SZ],tar[SZ][SZ],a[SZ][SZ],b[SZ][SZ],c[SZ][SZ];
ll p = 25221961025508539;

ll schrage(ll a, ll b) {
    if (a == 0) { return 0; }
    ll q = p / a, r = p % a;
    ll result = a * (b % q) - r * (b / q);
    while (result < 0) { result += p; }
    return result;
}

ll mulmod(ll a, ll b) {
    //int rtsize = (64 - __builtin_clzll(p)) / 2;
    //cout<<rtsize<<endl;
    long long rt = 1LL << 28;
    return (schrage(rt, schrage(a >> 28, b)) + schrage(a & (rt - 1), b)) % p;
}

/*
ll mulmod(ll a, ll b) {
    if (a==0||b==0)
        return 0;
    ll q = p/a;
    ll r = p%a;
    return a*(b%q)-r*(b/q);
}
*/

void matmul(ll a[SZ][SZ], ll b[SZ][SZ], ll c[SZ][SZ], int n) {
    int i,j,k;
    for (i=0; i<n; i++)
        for (j=0; j<n; j++) {
            c[i][j] = 0;
            for (k=0; k<n; k++)
                c[i][j] = (c[i][j]+mulmod(a[i][k],b[k][j]))%p;
        }
}

// not good but just use gloal variable
void matpow(ll t) {
    memcpy(a,z,sizeof(z));
    memset(b,0,sizeof(b));
    b[0][0] = 1;
    b[1][1] = 1;
    b[2][2] = 1;
    while (t!=0) {
        if (t%2) {
            matmul(b,a,c,SZ);
            memcpy(b,c,sizeof(c));
        }
        matmul(a,a,c,SZ);
        memcpy(a,c,sizeof(c));
        t/=2;
    }
}

int main() {
    long i,j;
    /*
    ll aa=1514495313540557425;
    ll bb=1430224233130405120;
    cout<<mulmod(aa,bb)<<endl;
    return 0;
    */
    memset(a,0,sizeof(a));
    // some issues for this matrix construction
    // but still works
    for (i=0;i<2;i++)
        a[i][i+1] = 1;
    for (i=0;i<3;i++)
        a[2][i] = 1;
    memcpy(z,a,sizeof(a));
    memcpy(b, a, sizeof(a));
    /*
    for (i=1; i<(1<<30); i++) {
        if (i%1000000==0)
            cout<<i<<endl;
        matmul(b,a,c,SZ);
        memcpy(b,c,sizeof(c));
    }
    */
    /*
    ll val = 1807948441590189;
    matpow(val);
    memcpy(tar,b,sizeof(b));
    */
    tar[0][0] = 1914483117172565;
    tar[1][0] = 15349972598427081;
    tar[2][0] = 2510729161496127;
    tar[0][1] = (tar[2][0]-tar[1][0]+p)%p;
    tar[1][1] = (tar[0][0]+tar[1][0])%p;
    tar[2][1] = (tar[1][0]+tar[2][0])%p;
    tar[0][2] = tar[1][0];
    tar[1][2] = tar[2][0];
    tar[2][2] = ((tar[0][0]+tar[1][0])%p+tar[2][0])%p;
    for (i=0; i<SZ; i++) {
        for (j=0; j<SZ; j++)
            cout<<tar[i][j]<<' ';
        cout<<endl;
    }
    stat.clear();
    matpow(1<<30);
    memcpy(xx,b,sizeof(b));
    stat[b[0][0]] = 1;
    for (i=2; i<(1<<26);i++) {
        if (i%1000000==1)
            cout<<"1:"<<i<<endl;
        matmul(b,xx,c,SZ);
        memcpy(b,c,sizeof(c));
        stat[b[0][0]] = i;
    }
    memcpy(cur,tar,sizeof(tar));
    for (i=1; i<(1<<30);i++) {
        if (i%1000000==1)
            cout<<"2:"<<i<<endl;
        matmul(cur,z,c,SZ);
        memcpy(cur,c,sizeof(c));
        it = stat.find(cur[0][0]);
        if (it != stat.end()) {
            ll tmp = it->second;
            cout<<tmp<<' '<<i<<endl;
            cout<<((tmp<<30)-i)<<endl;
        }
    }

    return 0;
}
