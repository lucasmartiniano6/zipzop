import random
with open("input.txt",'w') as f:
    for i in range(100000):
        f.write(str(random.randint(0,255)))
