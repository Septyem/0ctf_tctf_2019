import time
from hashlib import sha256

from pwn import *

# sig example for pubkey, etc.
with open('dat0','rb') as f:
    dat = f.read()
with open('sig0','rb') as f:
    sig = f.read()
with open('dat1','rb') as f:
    dat1 = f.read()
with open('sig1','rb') as f:
    sig1 = f.read()

def to_byte(t, n):
    res = ''
    tmp = t
    for i in range(n):
        res = chr(tmp&0xff)+res
        tmp /= 256
    return res

def addr_to_byte(addr):
    res = ''
    for i in range(8):
        res += to_byte(addr[i],4)
    return res

def core_hash_SHA2(t, key, inp):
    res = to_byte(t, 32)
    res += key+inp
    return sha256(res).digest()
    
def prf(inp, key):
    assert len(inp)==32
    return core_hash_SHA2(3, key, inp);

def h_msg(inp, key):
    assert len(key)==3*32
    return core_hash_SHA2(2, key, inp)

def hash_f(inp, pub, addr):
    addr[7] = 0
    tmp = addr_to_byte(addr)
    key = prf(tmp, pub)
    addr[7] = 1
    tmp = addr_to_byte(addr)
    msk = prf(tmp, pub)
    buf = ''
    for i in range(32):
        buf += chr(ord(inp[i])^ord(msk[i]))
    return core_hash_SHA2(0, key, buf)

root = dat[0x9e:0xbe]
pub = dat[0xbe:0xde]
R = sig[0x20:0x40]
idx = to_byte(0,32)


hashkey = R+root+idx
def do_hash(dat):
    msgh = h_msg(dat, hashkey)
    msgh = msgh.encode('hex')
    csum = 0
    for i in range(len(msgh)):
        csum += 15-int(msgh[i],16)
    csum = hex(csum)[2:].strip('L')
    return msgh+csum

# should start patched sshd for collecting dat/sig file
def dopow():
    chal = c.recvline()
    post = chal[11:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.ascii_letters+string.digits, 3)
    c.sendline(found)
    c.recvuntil('OK\n')

SZ = 67
cnt = 0
sec = ['']*SZ
while cnt<SZ:
    #c = process('/home/septyem/naglfar/ssh/client/service.py')
    #c = remote('127.0.0.1', 3333)
    c = remote('192.168.201.12', 3333)
    dopow()
    #c.sendline('172.17.0.1')
    c.sendline('192.168.151.6')
    time.sleep(1)
    c.close()
    with open('/tmp/dat0','rb') as f:
        dat = f.read()
    with open('/tmp/sig0','rb') as f:
        sig = f.read()
    msgh = do_hash(dat)
    for i in range(SZ):
        if sec[i] == '':
            if msgh[i]=='0' or (i==SZ-3 and msgh[i]=='1'):
                sec[i] = sig[0x40+0x20*i:0x60+0x20*i]
                cnt += 1
    print cnt
    
print sec
'''
msgh0 = do_hash(dat)
msgh1 = do_hash(dat1)
print msgh0
print msgh1

idx = 65
h0 = sig[0x40+0x20*idx:0x60+0x20*idx]
h1 = sig1[0x40+0x20*idx:0x60+0x20*idx]
print h0.encode('hex')
print h1.encode('hex')
print '---'

def gen(inp,ic,i):
    addr = [0,0,0,0,0,0,0,0]
    addr[5] = ic
    addr[6] = i
    return hash_f(inp, pub, addr)

tmp = h0
for i in range(0xd,0xe):
    tmp = gen(tmp, idx, i)
print tmp.encode('hex')
'''
