import random, string
from requests import Session
from colorama import Fore, init

try:
    main()
except Exception as e:
    print(e)
    pass

init(convert=True)

print('%sTry not to skid this :) how many codes?%s ' % (Fore.CYAN, Fore.WHITE), end='')
amount = int(input())
for i in range(amount):
    code = "https://discordapp.com/gifts/%s" % (('').join(random.choices(string.ascii_letters + string.digits, k=16)))
    print('Code: %s' % (code))
    with open('codes.txt', 'a') as f:
        f.write('%s\n' % (code))
