def gen_rand_words():
    import wonderwords
    s = wonderwords.RandomSentence()
    sentences = []
    max_chars = 1e2
    curr = 0
    while curr < max_chars:
        se = s.simple_sentence()
        curr += len(se)
        sentences.append(se)
    with open("test_input.txt",'w') as f:
        f.writelines(sentences)

"""
words->ratio, space_save
1e2 -> 1.3:1, 26%
1e3 -> 1.8:1, 45%
1e4 -> 2.2:1, 55%
1e5 -> 2.8:1, 64% 
1e6 -> 6.7:1, 85% (this is the 1e5 file 10x times)
"""

if __name__ == "__main__":
    gen_rand_words()
