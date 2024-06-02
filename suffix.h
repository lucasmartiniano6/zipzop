// https://github.com/Tascate/Suffix-Arrays-in-CPP/blob/master/Skew.cpp
#include <iostream>
#include <fstream>
#include <vector>

class Skew {
public:
    Skew();
    ~Skew();
    void build();
    void print_suffix_array();
    void string_from_file(std::string);
	std::vector<int>* suffixArray;
	std::vector<int>* string;
    	
private:
	const int maxAlphabetSize = 256;
	int numStrings;
    int sentinelCharacter = -1;
	void skew_suffix_array(const std::vector<int>&, std::vector<int>&, int, int, int);
	void radixsort(std::vector<int>&, std::vector<int>&, const std::vector<int>&, int, int, int, int);
};