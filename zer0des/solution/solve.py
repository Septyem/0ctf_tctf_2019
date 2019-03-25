# The solution is from 9-round attack at paper "Enhancing Differential-Linear Cryptanalysis", which suitable for 8-round as well, with several new differential paths (see comment below) and linear path unchanged. 
# https://pdfs.semanticscholar.org/988f/c67a5bc729929c3afa934029dad9e5ea56f3.pdf

from des import xor_bits,str_to_bits,bits_to_str,IP_1,IP,E,P,SBOX

from pwn import *
import random
from itertools import product
import string
from hashlib import sha256

#context.log_level='debug'
def dopow():
    chal = c.recvline()
    post = chal[12:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.printable.strip(), 4)
    c.sendline(found)

def bits_to_int(bits):
    val = 0
    for b in bits:
        val = val*2+b
    return val

def int_to_bits(x, sz):
    res = []
    for i in range(sz):
        res.append(x%2)
        x/=2
    return res[::-1]

def gen_subsets(bits, i):
    if i>=len(bits):
        bits2 = [bits[x-1] for x in IP_1]
        return [[bits_to_int(bits[:32])],[bits_to_int(bits2)]]
    res = [[],[]]
    if bits[i]==1:
        bits[i]=0
        tmp = gen_subsets(bits, i+1)
        res[0].extend(tmp[0])
        res[1].extend(tmp[1])
        bits[i]=1
        tmp = gen_subsets(bits, i+1)
        res[0].extend(tmp[0])
        res[1].extend(tmp[1])
    else:
        tmp = gen_subsets(bits, i+1)
        res[0].extend(tmp[0])
        res[1].extend(tmp[1])
    return res

def subsets(s):
    bits = str_to_bits(s)
    return gen_subsets(bits, 0)

'''
differential paths by intuitively guessing
3. 00600000 -> 00010004 0.1896 S4,S5,S8 ok
2. 00400000 -> 04000004 0.1547 S2,S8 ok
0. 40000000 -> 00000202 0.1787 S6,S8 ok
1. 00000600 -> 00202000 0.1926 S3,S5 ok
4. 00200000 -> 04000100 0.1898 S2,S6,S7 ok
00004000 -> 01000080 0.1552 S2,S3,S6,S7 not used
00000060 -> 00100001 0.2118 S1,S3,S4,S8 not used
'''
struc0 = subsets('1822282800000000'.decode('hex')) # possible influenced bits after F, for diff 00000202
struc1 = subsets('2505018400000000'.decode('hex')) # 00202000
struc2 = subsets('480a483000000000'.decode('hex')) # 04000004
struc3 = subsets('a94618e000000000'.decode('hex')) # 00010004
struc4 = subsets('5238641900000000'.decode('hex')) # 04000100

#c = remote('127.0.0.1', 10001)
c = remote('111.186.63.15', 10001)
#c = remote('111.186.56.54', 10001)
dopow()

def recover(diff, sdiff0, sdiff1, num, pos, known):
    diff = str_to_bits(diff)
    diff2 = [diff[x-1] for x in IP_1]
    diff2 = bits_to_str(diff2)
    diff2 = int(diff2.encode('hex'),16)
    #print hex(diff2)
    
    p = ''
    ct = ''
    for i in range(num):
        pt = random.randrange(0, 2**64)
        for x in sdiff1:
            p += p64(pt^x, endian='big')
        for x in sdiff1:
            p += p64(pt^x^diff2, endian='big')
    
    for i in range((len(p)-1)/10000+1):
        c.sendlineafter('(hex):',p[i*10000:(i+1)*10000].encode('hex'))
        tmp = c.recvline().strip()
        ct += tmp
    
    ss = len(sdiff0)
    rr = []
    for i in range(num):
        bits = str_to_bits(p[i*ss*16:i*ss*16+8])
        bits = [bits[x-1] for x in IP]
        rr.append(bits[32:64])
        bits = str_to_bits(p[i*ss*16+ss*8:i*ss*16+ss*8+8])
        bits = [bits[x-1] for x in IP]
        rr.append(bits[32:64])
    
    '''
    file format:
    len(sdiff0)
    sdiff0 (array)
    num = len(rr)
    rr (array)
    ct
    pos
    known kbits
    '''
    fw = open('dat','w')
    print >>fw, len(sdiff0)
    for x in sdiff0:
        fw.write(str(x)+' ')
    fw.write('\n')
    print >>fw, num
    for i in range(2*num):
        print >>fw, bits_to_str(rr[i]).encode('hex')
    print >>fw, ct
    print >>fw, len(pos)-1
    for i in range(len(pos)-1):
        fw.write(str(pos[i])+' ')
    fw.write('\n')
    print >>fw, len(known)
    for i in range(len(known)):
        fw.write(str(known[i][0])+' '+str(known[i][1])+' ')
    fw.write('\n')
    fw.close()
    
    os.system("./util r > tmpres")
    f = open('tmpres')
    res = []
    for line in f:
        l = line.strip()
        if l!='':
            res.append(map(int,l.split(' ')))
    f.close()
    return res

'''
# too slow. transfer to C.

def partial_F(hblk, kbits, pos):
    allbits = [hblk[x-1] for x in E]
    res = []
    cur = 0
    for i in range(8):
        if pos[cur]==i+1:
            bits = xor_bits(allbits[i*6:i*6+6],kbits[cur*6:cur*6+6])
            row = bits[0]*2+bits[5]
            col = bits[1]*8+bits[2]*4+bits[3]*2+bits[4]
            val = bin(SBOX[i][row][col])[2:]
            res.extend(map(int, list(val.rjust(4,'0'))))
            cur += 1
        else:
            res.extend([0,0,0,0])
    res = [res[x-1] for x in P]
    return res

stat = []
for j in range(2**12):
    print j,
    kbits = int_to_bits(j,12)
    cnt = 0
    for i in range(num):
        res0 = partial_F(rr[2*i],kbits,pos)
        res1 = partial_F(rr[2*i+1],kbits,pos)
        dd = bits_to_int(xor_bits(res0, res1))
        for i0,i1 in product(range(ss),range(ss)):
            if sdiff0[i0]^sdiff0[i1] == dd:
                idx0 = i*ss*2+i0
                idx1 = i*ss*2+ss+i1
                ct0 = str_to_bits(ct[idx0*16:idx0*16+16].decode('hex'))
                ct1 = str_to_bits(ct[idx1*16:idx1*16+16].decode('hex'))
                p0 = ct0[41]^ct0[1]^ct0[19]^ct0[63]^ct0[60]
                p1 = ct1[41]^ct1[1]^ct1[19]^ct1[63]^ct1[60]
                if p0==p1:
                    cnt += 1
    stat.append(cnt)
    print cnt

cand = stat.index(max(stat))
print int_to_bits(cand,12)
'''

ks = ['']
for i in range(8):
    ks.append(set([]))

r0 = recover('4000000000000202'.decode('hex'), struc0[0], struc0[1], 2**7, [6,8,-1], [])
print r0
for k0,k1 in r0:
    ks[6].add(k0)
    ks[8].add(k1)
r1 = recover('0000060000202000'.decode('hex'), struc1[0], struc1[1], 2**7, [3,5,-1], [])
print r1
for k0,k1 in r1:
    ks[3].add(k0)
    ks[5].add(k1)
r2 = recover('0040000004000004'.decode('hex'), struc2[0], struc2[1], 2**7, [2,8,-1], [])
print r2
tmps = set([])
for k0,k1 in r2:
    ks[2].add(k0)
    tmps.add(k1)
ks[8] = ks[8].intersection(tmps)
if len(ks[8])==0:
    print 'failed'
    exit()
r3 = recover('0060000000010004'.decode('hex'), struc3[0], struc3[1], 2**3, [4,5,8,-1], list(product(list(ks[5]),list(ks[8]))))
print r3
ks[5] = set([])
ks[8] = set([])
for k0,k1,k2 in r3:
    ks[4].add(k0)
    ks[5].add(k1)
    ks[8].add(k2)
r4 = recover('0020000004000100'.decode('hex'), struc4[0], struc4[1], 2**3, [7,2,6,-1], list(product(list(ks[2]),list(ks[6]))))
print r4
ks[2] = set([])
ks[6] = set([])
for k0,k1,k2 in r4:
    ks[7].add(k0)
    ks[2].add(k1)
    ks[6].add(k2)

print ks

c.sendlineafter('(hex):','0'*16)
ct0 = c.recvline().strip()
'''
file format:
ct of '\x00'*8
num of key candidates
key candidates (array)
etc.
'''
fw = open('subkeys','w')
print >>fw, ct0
for i in range(2,9):
    kk = list(ks[i])
    print >>fw, len(kk)
    print >>fw, ' '.join(map(str,kk))
fw.close()

os.system("./util b > tmpres")
f = open('tmpres')
key = f.read().strip()
f.close()
c.sendlineafter('(hex):','-1')
c.sendlineafter('key(hex):',key)
print c.recv()
c.interactive()

# for testing. not used.
def distinguishing(ct, tot):
    cnt = 0
    for i in range(tot):
        ct0 = str_to_bits(ct[32*i:32*i+16].decode('hex'))
        ct1 = str_to_bits(ct[32*i+16:32*i+32].decode('hex'))
        p0 = ct0[41]^ct0[1]^ct0[19]^ct0[63]^ct0[60] # the bits after permuting 2104008000008000
        p1 = ct1[41]^ct1[1]^ct1[19]^ct1[63]^ct1[60]
        if p0==p1:
            cnt += 1
    ratio = float(cnt)/tot
    print ratio
    if ratio > 0.508:
        return True
    else:
        return False

#print distinguishing(ct, num)
