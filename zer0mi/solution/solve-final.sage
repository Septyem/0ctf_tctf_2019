# "Cryptanalysis of the Matsumoto and Imai Public Key Scheme"
# https://link.springer.com/content/pdf/10.1007/3-540-44750-4_20.pdf
from os import urandom

n = 63
q = 2^8
F = GF(q)
z8 = F.gen()
R.<x> = PolynomialRing(F)
g = x^63 + z8^5*x^62 + (z8^7 + z8)*x^61 + z8*x^60 + (z8^7 + z8^6 + z8^5 + z8^4 + z8)*x^59 + (z8^6 + z8^5 + z8^3 + z8 + 1)*x^58 + (z8^6 + z8^5 + z8^3 + z8 + 1)*x^57 + (z8^7 + z8^6 + z8^3)*x^56 + (z8^3 + z8 + 1)*x^55 + (z8^4 + z8)*x^54 + (z8^5 + z8^3)*x^53 + (z8^7 + z8^6 + z8^2 + z8)*x^52 + (z8^5 + 1)*x^51 + (z8^6 + z8^2 + z8)*x^50 + (z8^5 + z8 + 1)*x^49 + (z8^5 + z8^4 + z8^3 + 1)*x^48 + (z8^7 + z8^3 + z8^2 + z8 + 1)*x^47 + (z8^7 + z8^6 + z8^5 + z8^3 + 1)*x^46 + z8^4*x^45 + (z8^3 + 1)*x^44 + (z8^4 + z8^2 + z8)*x^43 + (z8^5 + 1)*x^42 + (z8^5 + z8^4 + z8)*x^41 + (z8^7 + z8^5 + z8^2)*x^40 + (z8^6 + z8^5 + z8^3 + z8 + 1)*x^39 + (z8^7 + z8^5 + z8^4 + z8^2)*x^38 + (z8^6 + z8^5 + z8^4 + z8^3 + z8^2)*x^37 + (z8^7 + z8^6 + z8^5 + z8^4 + z8^3 + z8^2 + z8)*x^36 + (z8^6 + z8^3 + 1)*x^35 + z8^3*x^34 + (z8^4 + z8^3 + z8^2)*x^33 + (z8^7 + z8^5 + z8)*x^32 + (z8^7 + z8^6 + z8^4 + z8^3 + z8^2)*x^31 + (z8^6 + z8^5 + 1)*x^30 + (z8^7 + z8^6 + z8^5 + z8^4 + z8^3 + z8^2 + z8 + 1)*x^29 + (z8^7 + z8^6 + z8^5 + z8^4 + z8^3 + z8^2 + z8)*x^28 + (z8^6 + z8^5 + z8^4 + z8^3)*x^27 + (z8^7 + z8^5 + z8^4 + 1)*x^26 + (z8^7 + z8^6 + z8^5 + z8^4 + z8^2)*x^25 + (z8^5 + z8^2 + z8 + 1)*x^24 + z8^7*x^23 + (z8^7 + z8^6 + z8)*x^22 + (z8^7 + 1)*x^21 + (z8^6 + z8^4 + z8^2)*x^20 + (z8^5 + 1)*x^19 + (z8^2 + 1)*x^18 + (z8^7 + z8^5 + z8^4 + z8^2 + z8 + 1)*x^17 + (z8^7 + z8^6 + z8^5 + z8^4)*x^16 + (z8^7 + z8^5 + z8^4 + z8^2 + z8)*x^15 + (z8^7 + z8^3 + z8)*x^14 + (z8^7 + z8^4 + z8^3 + z8)*x^13 + (z8^3 + z8^2 + 1)*x^12 + (z8^7 + z8^5 + z8^2)*x^11 + (z8^7 + z8^3 + z8 + 1)*x^10 + (z8^7 + z8^5 + z8^3 + 1)*x^9 + (z8^5 + z8^4 + z8^2 + z8)*x^8 + (z8^7 + z8^5 + z8^4 + z8^2 + z8 + 1)*x^7 + (z8^6 + z8^2 + 1)*x^6 + z8^6*x^5 + (z8^7 + z8)*x^4 + (z8^7 + z8^5 + z8^2)*x^3 + (z8^7 + z8^4)*x^2 + (z8^7 + z8^5 + z8^3)*x + z8^7
K = R.quotient(g)
with open('output') as f:
    pub = f.readline()
    ct = f.readline().strip()

ct = ct.decode('hex')
ct = map(F.fetch_int, map(ord, ct))

V.<x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16,x17,x18,x19,x20,x21,x22,x23,x24,x25,x26,x27,x28,x29,x30,x31,x32,x33,x34,x35,x36,x37,x38,x39,x40,x41,x42,x43,x44,x45,x46,x47,x48,x49,x50,x51,x52,x53,x54,x55,x56,x57,x58,x59,x60,x61,x62,x63> = GF(256)[]
sub = {'z8':z8,'x0':x0,'x1':x1,'x2':x2,'x3':x3,'x4':x4,'x5':x5,'x6':x6,'x7':x7,'x8':x8,'x9':x9,'x10':x10,'x11':x11,'x12':x12,'x13':x13,'x14':x14,'x15':x15,'x16':x16,'x17':x17,'x18':x18,'x19':x19,'x20':x20,'x21':x21,'x22':x22,'x23':x23,'x24':x24,'x25':x25,'x26':x26,'x27':x27,'x28':x28,'x29':x29,'x30':x30,'x31':x31,'x32':x32,'x33':x33,'x34':x34,'x35':x35,'x36':x36,'x37':x37,'x38':x38,'x39':x39,'x40':x40,'x41':x41,'x42':x42,'x43':x43,'x44':x44,'x45':x45,'x46':x46,'x47':x47,'x48':x48,'x49':x49,'x50':x50,'x51':x51,'x52':x52,'x53':x53,'x54':x54,'x55':x55,'x56':x56,'x57':x57,'x58':x58,'x59':x59,'x60':x60,'x61':x61,'x62':x62,'x63':x63}
pub = sage_eval(pub, locals=sub)

stats = []
SZ = 4096
fw = open('saved','w')
for cnt in range(SZ):
    print 'step 1',cnt
    a = vector(F, map(F.fetch_int, map(ord, urandom(n))))
    fw.write(repr(a)+'\n')
    b = map(lambda f:f.subs(x0=a[0],x1=a[1],x2=a[2],x3=a[3],x4=a[4],x5=a[5],x6=a[6],x7=a[7],x8=a[8],x9=a[9],x10=a[10],x11=a[11],x12=a[12],x13=a[13],x14=a[14],x15=a[15],x16=a[16],x17=a[17],x18=a[18],x19=a[19],x20=a[20],x21=a[21],x22=a[22],x23=a[23],x24=a[24],x25=a[25],x26=a[26],x27=a[27],x28=a[28],x29=a[29],x30=a[30],x31=a[31],x32=a[32],x33=a[33],x34=a[34],x35=a[35],x36=a[36],x37=a[37],x38=a[38],x39=a[39],x40=a[40],x41=a[41],x42=a[42],x43=a[43],x44=a[44],x45=a[45],x46=a[46],x47=a[47],x48=a[48],x49=a[49],x50=a[50],x51=a[51],x52=a[52],x53=a[53],x54=a[54],x55=a[55],x56=a[56],x57=a[57],x58=a[58],x59=a[59],x60=a[60],x61=a[61],x62=a[62]), pub)
    b = map(lambda x:F(x), b)
    fw.write(repr(b)+'\n')
    row = []
    for i in range(n):
        for j in range(n):
            row.append(a[i]*b[j])
    for i in range(n):
        row.append(a[i])
    for j in range(n):
        row.append(a[j])
    row.append(F.fetch_int(0))
    stats.append(row)
fw.close()

# Maybe using matrix kernel will be better. But this way still works.
for i in range(n):
    row = [F.fetch_int(0) for _ in range(SZ)]
    row[i*n+i] = F.fetch_int(1)
    stats.append(row)
mat = matrix(F, stats)
print mat.rank()
res = [F.fetch_int(0) for _ in range(SZ)]
coefs = []
r = 0
for i in range(256):
    print 'step 2',i
    print '---'
    v = vector(F, res+[F.random_element() for _ in range(n)])
    try:
        coef = mat.solve_right(v)
        #print coef
        newr = matrix(F, coefs+[coef]).rank()
        print 'current rank', newr
        if newr > r:
            r = newr
            coefs.append(coef)
    except:
        print 'not found'
    print '---'
    if r > n-4:
        break

print 'totally found %d equations' % len(coefs)

# save for later use
fw2 = open('saved2','w')
for coef in coefs:
    fw2.write(repr(coef)+'\n')
fw2.close()
# save end

eqs = []
for coef in coefs:
    dat = [F.fetch_int(0) for _ in range(n+1)]
    cur = 0
    for i in range(n):
        for j in range(n):
            dat[i] += coef[cur]*ct[j]
            cur += 1
    for i in range(n):
        dat[i] += coef[cur]
        cur += 1
    for j in range(n):
        dat[n] += coef[cur]*ct[j]
        cur += 1
    dat[n] += coef[cur]
    eqs.append(dat)

for i in range(4):
    row = [F.fetch_int(0) for _ in range(n+1)]
    row[i] = F.fetch_int(1)
    eqs.append(row)
mat = matrix(F, eqs)
print 'rank:',mat.rank()

res = [F.fetch_int(0) for _ in range(len(eqs)-4)]
v = vector(F, res+map(lambda x:F.fetch_int(ord(x)),'flag'))
ans = mat.solve_right(v)
print ans
print ''.join(map(lambda x:chr(x.integer_representation()), ans))
