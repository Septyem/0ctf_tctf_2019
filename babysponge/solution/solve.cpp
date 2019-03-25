// g++ -O3 solve.cpp Keccak-readable-and-compact.c 
#include<iostream>
#include<string>
#include<map>
#include<stdio.h>
#include<string.h>
#include<stdint.h>

using namespace std;

typedef basic_string<unsigned char> ustring;

void KeccakF1600_StatePermute(void *state);

void hexdump(unsigned char *ptr, unsigned long size) {
    for (int i=0; i<size; i++)
        printf("%02x", ptr[i]);
    puts("");
}

map<uint64_t, ustring> stat;
map<uint64_t, ustring>::iterator it;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
void Keccak_bruteforce(unsigned int rate, unsigned int capacity, unsigned char *output0, unsigned char *output1)
{
    unsigned char state[200], state0[200], state1[200];
    unsigned char input[200];
    uint64_t key;
    unsigned int rateInBytes = rate/8;
    unsigned int blockSize = rateInBytes;
    unsigned int i;

    if (((rate + capacity) != 1600) || ((rate % 8) != 0))
        return;

    stat.clear();
    FILE *f = fopen("/dev/urandom","rb");
    ustring s;

    long cnt=0;
    while(true) {
        cnt++;
        if (cnt%10000==0) {
            cout<<cnt<<endl;
        }
        fread(input, blockSize, 1, f);
        memset(state, 0, sizeof(state));
        memcpy(state, input, blockSize);
        KeccakF1600_StatePermute(state);
        key = 0;
        for(i=blockSize; i<200; i++)
            key = key*256+state[i];
        it = stat.find(key);
        if (it != stat.end()) {
            memset(state0, 0, sizeof(state0));
            memcpy(state0, input, blockSize);
            KeccakF1600_StatePermute(state0);
            memset(state1, 0, sizeof(state1));
            for (i=0; i<blockSize; i++)
                state1[i] = it->second[i];
            KeccakF1600_StatePermute(state1);
            for (i=0; i<blockSize; i++) {
                output0[i] = input[i];
            }
            for (i=0; i<blockSize; i++) {
                output0[blockSize+i] = 0;
            }
            // maybe not necessary but adding one more block does not matter
            for (i=0; i<blockSize; i++) {
                output0[blockSize*2+i] = 0x61;
            }
            for (i=0; i<blockSize; i++) {
                output1[i] = it->second[i];
            }
            for (i=0; i<blockSize; i++) {
                output1[blockSize+i] = state0[i]^state1[i];
            }
            for (i=0; i<blockSize; i++) {
                output1[blockSize*2+i] = 0x61;
            }
            return;
        } else {
            if (cnt<(1<<24)) {
                s.clear();
                for (i=0;i<blockSize;i++)
                    s.push_back(input[i]);
                stat.insert(map<uint64_t,ustring>::value_type(key, s));
            }
        }
    }
}

int main() {
    unsigned char output0[1000],output1[1000];
    Keccak_bruteforce(1552, 48, output0, output1);
    hexdump(output0,582);
    hexdump(output1,582);
    return 0;
}
