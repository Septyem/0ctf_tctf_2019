// g++ -O3 solve.cpp -fpermissive -o util
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdint.h>
#include<string.h>

using namespace std;

const int ROUNDS = 8;

const int IP[64] = {58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

const int IP_1[64] = {40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31, 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29, 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27, 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};

const int E[48] = {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

const int SBOX[8][4][16] = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7}, {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8}, {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0}, {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}}, {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10}, {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5}, {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15}, {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}}, {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8}, {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1}, {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7}, {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}}, {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15}, {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9}, {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4}, {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}}, {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9}, {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6}, {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14}, {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}}, {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11}, {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8}, {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6}, {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}}, {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1}, {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6}, {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2}, {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}}, {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7}, {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2}, {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8}, {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

const int P[32] = {16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25};

const int PC_1[56] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};

const int PC_2[48] = {14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

const int R[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

const int Rev4[16] = {0 , 8 , 4 , 12 , 2 , 10 , 6 , 14 , 1 , 9 , 5 , 13 , 3 , 11 , 7 , 15};

const uint64_t B[64] = {1 , 2 , 4 , 8 , 16 , 32 , 64 , 128 , 256 , 512 , 1024 , 2048 , 4096 , 8192 , 16384 , 32768 , 65536 , 131072 , 262144 , 524288 , 1048576 , 2097152 , 4194304 , 8388608 , 16777216 , 33554432 , 67108864 , 134217728 , 268435456 , 536870912 , 1073741824 , 2147483648 , 4294967296 , 8589934592 , 17179869184 , 34359738368 , 68719476736 , 137438953472 , 274877906944 , 549755813888 , 1099511627776 , 2199023255552 , 4398046511104 , 8796093022208 , 17592186044416 , 35184372088832 , 70368744177664 , 140737488355328 , 281474976710656 , 562949953421312 , 1125899906842624 , 2251799813685248 , 4503599627370496 , 9007199254740992 , 18014398509481984 , 36028797018963968 , 72057594037927936 , 144115188075855872 , 288230376151711744 , 576460752303423488 , 1152921504606846976 , 2305843009213693952 , 4611686018427387904 , 9223372036854775808};

uint64_t permute(uint64_t val, const int *A, int sz) {
    uint64_t res = 0;
    for (int i=0; i<sz; i++) {
        if (val&B[A[i]-1])
            res += B[i];
    }
    return res;
}

uint64_t F(uint64_t hblk, uint64_t subkey) {
    uint64_t bits = permute(hblk, E, 48);
    bits ^= subkey;
    uint64_t res = 0;
    for (int i=0; i<8; i++) {
        int row=0,col=0;
        if (bits&B[i*6])
            row += 2;
        if (bits&B[i*6+5])
            row += 1;
        if (bits&B[i*6+1])
            col += 8;
        if (bits&B[i*6+2])
            col += 4;
        if (bits&B[i*6+3])
            col += 2;
        if (bits&B[i*6+4])
            col += 1;
        uint64_t val = SBOX[i][row][col];
        res += Rev4[val]<<(i*4);
    }
    res = permute(res, P, 32);
    return res;
}

uint64_t str_to_int(unsigned char *s, int sz) {
    uint64_t res=0;
    for (int i=0; i<sz; i++) {
        uint64_t tmp = Rev4[s[i]>>4];
        res += tmp<<(i*8);
        tmp = Rev4[s[i]&15];
        res += tmp<<(i*8+4);
    }
    return res;
}

int int_to_str(uint64_t val, int sz, unsigned char *out) {
    uint64_t tmp = val;
    int i=0;
    for (i=0; i<sz; i++) {
        int cur = tmp%256;
        tmp >>= 8;
        uint8_t ch = (Rev4[cur&15]<<4) + Rev4[cur>>4];
        out[i] = ch;
    }
    return 0;
}

int gen_subkey(unsigned char *key, uint64_t *subkeys) {
    uint64_t left, right, cur;
    uint64_t kbits = str_to_int(key, 8);
    kbits = permute(kbits, PC_1, 56);
    left = kbits>>28;
    right = kbits&(B[28]-1);
    for (int i=0; i<ROUNDS; i++) {
        int tmp = left&(B[R[i]]-1);
        left = (left>>R[i])+(tmp<<(28-R[i]));
        tmp = right&(B[R[i]]-1);
        right = (right>>R[i])+(tmp<<(28-R[i]));
        cur = (left<<28)+right;
        subkeys[i] = permute(cur, PC_2, 48);
    }
    return 0;
}

uint64_t encrypt_block(uint64_t blk, uint64_t *subkeys) {
    uint64_t left, right;
    uint64_t bits = permute(blk, IP, 64);
    for (int i=0; i<ROUNDS; i++) {
        left = bits&(B[32]-1);
        right = bits>>32;
        left ^= F(right, subkeys[i]);
        bits = right + (left<<32);
    }
    bits = left + (right<<32);
    bits = permute(bits, IP_1, 64);
    return bits;
}

int encrypt(unsigned char *pt, int sz, unsigned char *key, unsigned char *out) {
    uint64_t subkeys[ROUNDS];
    if (sz%8!=0)
        return -1;
    gen_subkey(key, subkeys);
    for (int i=0; i<sz; i+=8) {
        uint64_t ct = encrypt_block(str_to_int(pt+i,8), subkeys);
        unsigned char tmp[8];
        int_to_str(ct, 8, tmp);
        for (int j=0; j<8; j++)
            out[i+j] = tmp[j];
    }
    return 0;
}

uint64_t decrypt_block(uint64_t blk, uint64_t *subkeys) {
    uint64_t left, right;
    uint64_t bits = permute(blk, IP, 64);
    for (int i=0; i<ROUNDS; i++) {
        left = bits&(B[32]-1);
        right = bits>>32;
        left ^= F(right, subkeys[ROUNDS-1-i]);
        bits = right + (left<<32);
    }
    bits = left + (right<<32);
    bits = permute(bits, IP_1, 64);
    return bits;
}

int decrypt(unsigned char *pt, int sz, unsigned char *key, unsigned char *out) {
    uint64_t subkeys[ROUNDS];
    if (sz%8!=0)
        return -1;
    gen_subkey(key, subkeys);
    for (int i=0; i<sz; i+=8) {
        uint64_t ct = decrypt_block(str_to_int(pt+i,8), subkeys);
        unsigned char tmp[8];
        int_to_str(ct, 8, tmp);
        for (int j=0; j<8; j++)
            out[i+j] = tmp[j];
    }
    return 0;
}

void hexdump(unsigned char *ptr, unsigned long size) {
    for (int i=0; i<size; i++)
        printf("%02x", ptr[i]);
    puts("");
}

int unhex(char c) {
    if (c>='0'&&c<='9')
        return c-'0';
    else if (c>='a'&&c<='f')
        return c-'a'+10;
    else if (c>='A'&&c<='F')
        return c-'A'+10;
}

void decodehex(char *in, int sz, unsigned char *out) {
    int i,j;
    j=0;
    for (i=0; i<sz; i+=2) {
        out[j] = (unhex(in[i])<<4)+unhex(in[i+1]);
        j++;
    }
    out[j] = '\x00';
}

int ndiff,num,npos,nknown;
uint64_t sdiff0[10000];
int pos[10], known[100][2];
char buf[1100000];
unsigned char ct[1000000];
unsigned char buf2[10];
uint64_t rr[1000];
int stat[10000];

unsigned char ct0[10];
int ncands[10],cands[10][10];
int kbits[64];

int getbit(int idx, int pos) {
    uint8_t ch = (uint8_t)ct[idx*8+pos/8];
    if (B[7-pos%8]&ch)
        return 1;
    else
        return 0;
}

uint64_t revint(uint64_t val, int sz) {
    uint64_t tmp = val;
    uint64_t res = 0;
    int i=0;
    for (i=0; i<sz; i++) {
        int cur = tmp%256;
        tmp >>= 8;
        uint8_t ch = (Rev4[cur&15]<<4) + Rev4[cur>>4];
        res = res*256+ch;
    }
    return res;
}

// two subkeys unknown
int ana2() {
    int i,i0,i1,j,cnt,max,max2;
    uint64_t subkey,dd,msk,upper,lower;
    max = 0;
    msk = (0xf<<((pos[0]-1)*4))|(0xf<<((pos[1]-1)*4));
    msk = permute(msk, P, 32);
    //cout<<revint(msk,4)<<endl;
    for (j=0; j<B[12]; j++) {
        cnt = 0;
        upper = j>>6;
        lower = j&0x3f;
        subkey = (upper<<((pos[0]-1)*6))|(lower<<((pos[1]-1)*6));
        for (i=0; i<num; i++) {
            uint64_t res0=F(rr[2*i], subkey);
            uint64_t res1=F(rr[2*i+1], subkey);
            dd = msk&(res0^res1);
            dd = revint(dd, 4);
            for (i0=0; i0<ndiff; i0++)
                for (i1=0; i1<ndiff; i1++)
                    if ((sdiff0[i0]^sdiff0[i1]) == dd) {
                        int idx0 = i*ndiff*2+i0;
                        int idx1 = i*ndiff*2+ndiff+i1;
                        int p0=getbit(idx0,41)^getbit(idx0,1)^getbit(idx0,19)^getbit(idx0,63)^getbit(idx0,60);
                        int p1=getbit(idx1,41)^getbit(idx1,1)^getbit(idx1,19)^getbit(idx1,63)^getbit(idx1,60);
                        if (p0==p1)
                            cnt++;
                    }
        }
        stat[j] = cnt;
        //cout<<j<<' '<<cnt<<endl;
        if (cnt > max) {
            max2 = max;
            max = cnt;
        } else if (cnt != max && cnt > max2) {
            max2 = cnt;
        }
    }
    for (j=0; j<B[12]; j++) {
        if (stat[j]>=max2) {
            upper = j>>6;
            lower = j&0x3f;
            cout<<upper<<' '<<lower<<endl;
        }
    }
}

// first subkey unknown with two subkeys known
int ana3() {
    int i,i0,i1,j,j0,cnt,max;
    uint64_t subkey,dd,msk,k0,k1,k2;
    max = 0;
    msk = (0xf<<((pos[0]-1)*4))|(0xf<<((pos[1]-1)*4))|(0xf<<((pos[2]-1)*4));
    msk = permute(msk, P, 32);
    //cout<<revint(msk,4)<<endl;
    for (j0=0;j0<nknown;j0++) {
    for (j=0; j<B[6]; j++) {
        cnt = 0;
        k0 = j;
        k1 = known[j0][0];
        k2 = known[j0][1];
        subkey = (k0<<((pos[0]-1)*6))|(k1<<((pos[1]-1)*6))|(k2<<((pos[2]-1)*6));
        for (i=0; i<num; i++) {
            uint64_t res0=F(rr[2*i], subkey);
            uint64_t res1=F(rr[2*i+1], subkey);
            dd = msk&(res0^res1);
            dd = revint(dd, 4);
            for (i0=0; i0<ndiff; i0++)
                for (i1=0; i1<ndiff; i1++) 
                    if ((sdiff0[i0]^sdiff0[i1]) == dd) {
                        int idx0 = i*ndiff*2+i0;
                        int idx1 = i*ndiff*2+ndiff+i1;
                        int p0=getbit(idx0,41)^getbit(idx0,1)^getbit(idx0,19)^getbit(idx0,63)^getbit(idx0,60);
                        int p1=getbit(idx1,41)^getbit(idx1,1)^getbit(idx1,19)^getbit(idx1,63)^getbit(idx1,60);
                        if (p0==p1)
                            cnt++;
                    }
        }
        stat[j] = cnt;
        //cout<<j0<<' '<<j<<' '<<cnt<<endl;
        if (cnt > max)
            max = cnt;
    }
    for (j=0; j<B[6]; j++) {
        if (stat[j]==max) {
            cout<<j<<' '<<known[j0][0]<<' '<<known[j0][1]<<endl;
        }
    }
    }
}

int subkeypos[9][6] = {{},{},{32, 56, 1, 8, 18, 41} , {2, 34, 25, 24, 43, 57} , {58, 0, 35, 26, 17, 40} , {21, 27, 38, 53, 36, 3} , {46, 29, 4, 52, 22, 28} , {60, 20, 37, 62, 14, 19} , {44, 13, 12, 61, 54, 30}};
int others[14] = {5 , 6 , 9 , 10 , 11 , 16 , 33 , 42 , 45 , 48 , 49 , 50 , 51 , 59};

void setkbits(int num, int val) {
    for (int i=0;i<6;i++) {
        int pos = subkeypos[num][i];
        if (val&B[i])
            kbits[pos] = 1;
        else
            kbits[pos] = 0;
    }
}

// bruteforce the remaining 14 bits
void brute() {
    int i,j,j0,j1,k2,k3,k4,k5,k6,k7,k8;
    bool ok;
    unsigned char ct[10],pt[10],key[10];
    for (i=0; i<8; i++)
        pt[i] = 0;
    for (i=0; i<8; i++)
        kbits[i*8+7]=0;
    for (int i2=0;i2<ncands[2];i2++) {
        setkbits(2,cands[2][i2]);
    for (int i3=0;i3<ncands[3];i3++) {
        setkbits(3,cands[3][i3]);
    for (int i4=0;i4<ncands[4];i4++) {
        setkbits(4,cands[4][i4]);
    for (int i5=0;i5<ncands[5];i5++) {
        setkbits(5,cands[5][i5]);
    for (int i6=0;i6<ncands[6];i6++) {
        setkbits(6,cands[6][i6]);
    for (int i7=0;i7<ncands[7];i7++) {
        setkbits(7,cands[7][i7]);
    for (int i8=0;i8<ncands[8];i8++) {
        setkbits(8,cands[8][i8]);
    for (j=0;j<B[14];j++) {
        for (j0=0;j0<14;j0++) {
            int pos = others[j0];
            if (j&B[j0])
                kbits[pos] = 1;
            else
                kbits[pos] = 0;
        }
        for (j0=0;j0<8;j0++) {
            int tmp = 0;
            for (j1=0;j1<8;j1++)
                tmp = tmp*2+kbits[j0*8+j1];
            key[j0] = tmp;
        }
        encrypt(pt,8,key,ct);
        ok = true;
        for (i=0;i<8;i++)
            if (ct[i]!=ct0[i]) {
                ok=false;
                break;
            }
        if (ok) {
            for (i=0;i<8;i++)
                printf("%02x",key[i]);
            printf("\n");
            return;
        }
    }}}}}}}}
}

int main(int argc, char **argv) {
    int i,j;
    if (argc<2) {
        cout<<"Usage: "<<argv[0]<<" [r|b]"<<endl;
        return 0;
    }
    if (argv[1][0] == 'r') {
        fstream fin("dat");
        fin>>ndiff;
        for (i=0; i<ndiff; i++)
            fin>>sdiff0[i];
        fin>>num;
        for (i=0; i<num; i++) {
            fin>>buf;
            decodehex(buf,8,buf2);
            rr[2*i] = str_to_int(buf2,4);
            fin>>buf;
            decodehex(buf,8,buf2);
            rr[2*i+1] = str_to_int(buf2,4);
        }
        fin>>buf;
        decodehex(buf,strlen(buf),ct);
        fin>>npos;
        for (i=0; i<npos; i++)
            fin>>pos[i];
        fin>>nknown;
        for (i=0; i<nknown; i++)
            fin>>known[i][0]>>known[i][1];
        if (npos==2) {
            ana2();
        } else if (npos==3&&nknown>0) {
            ana3();
        } else {
            cout<<"unimplemented!"<<endl;
        }
    } else if (argv[1][0] == 'b') {
        fstream fin("subkeys");
        fin>>buf;
        decodehex(buf,16,ct0);
        for (i=2;i<=8;i++) {
            fin>>ncands[i];
            for (j=0;j<ncands[i];j++)
                fin>>cands[i][j];
        }
        brute();
    }
    /*
    unsigned char ct[100];
    unsigned char pt[100];
    char *k = "12345678";
    char *t = "12345678";
    encrypt(t, 8, k, ct);
    hexdump(ct, 8);
    decrypt(ct, 8, k, pt);
    hexdump(pt, 8);
    */
    /*
    uint64_t subkeys[16];
    gen_subkey("12345678", subkeys);
    for (int i=0;i<ROUNDS;i++)
        cout<<subkeys[i]<<endl;
    */
    //cout<<F(1711406848LL, 74031985669130LL)<<endl;
    return 0;
}
