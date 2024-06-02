g++ compress.cpp suffix.cpp -o a && ./a
g++ decompress.cpp suffix.cpp -o a && ./b
diff input.txt output_decomp.txt
