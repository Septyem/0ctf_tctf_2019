from pwn import *
import string
from hashlib import sha256

context.log_level='debug'

def dopow():
    chal = c.recvline()
    post = chal[12:28]
    tar = chal[33:-1]
    c.recvuntil(':')
    found = iters.bruteforce(lambda x:sha256(x+post).hexdigest()==tar, string.ascii_letters+string.digits, 4)
    c.sendline(found)

#c = remote('172.17.0.2',10001)
c = remote('111.186.63.13',10001)
dopow()
c.recvuntil('OK\n')
with open('exp.txt','rb') as f:
    cont = f.read()
c.sendline(str(len(cont)))
c.send(cont)
c.interactive()
