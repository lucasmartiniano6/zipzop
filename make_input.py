import random
with open("test_input.txt",'w') as f:
    for i in range(1000000):
        f.write(str(random.randint(0,255)))
