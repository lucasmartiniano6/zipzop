#include "suffix.h"
#include <queue>
#include <bitset>
#include <chrono>

// Burrows–Wheeler Transform
std::string BWT(std::vector<int>& t, std::vector<int>& sa){
    const char sentinel = '#';
    std::string ans = "";
    for(int i=0; i<sa.size(); i++){ 
        if(sa[i]==0) ans += sentinel;
        else ans += t[sa[i]-1];
    }
    return ans;
}

std::vector<int> common_dict(){
    std::vector<int> d(256);
    auto block32 = [](auto& v, auto a, auto b, auto off){for(int i=a;i<b;i++) v[i]=i-a+off;};
    block32(d, 0, 32, 96);
    block32(d, 32, 64, 64);
    block32(d, 64, 96, 32);
    block32(d, 96, 128, 0);
    block32(d, 128, 256, 128);
    return d;
}

// Move-To-Front Transformation
std::vector<int> MTF(std::string& s){
    auto dict = common_dict();
    std::vector<int> ans;
    for(char c : s){
        // find rank
        short int rank;
        for(int j=0; j<dict.size(); j++) if(dict[j]==(uint8_t)c) rank=j;
        ans.push_back(rank);
        // update dict
        for(int j=rank; j>0; j--) dict[j] = dict[j-1];
        dict[0] = (uint8_t)c;
    }    
    return ans;
}

// Run-Length Encoding
std::string RLE(std::vector<int>& v){
    std::string ans = "";
    for(int i=0; i<v.size(); i++){
        int cnt = 1;
        while(i+1 < v.size() && v[i] == v[i+1]) cnt++, i++;
        ans += std::to_string(cnt);
        ans += std::to_string(v[i]);
    }
    return ans;
}

class Huffman{
public:
struct Node{
    int idx, val, freq = 0, left=-1, right=-1;
    Node(int f, int i, int v=0) : freq(f), idx(i), val(v){}
    bool operator<(const Node b) const {
        return this->freq > b.freq; // inverted because we want MINIMUM-pq
    }
};
std::vector<std::string> table_code; // code
std::vector<Node> tree;
std::string tree_comp_form;
const int sentinel = 255, alphabet = 256;

void dfs(int v, std::string curr){
    if(v == -1) return;
    if(tree[v].left == -1 && tree[v].right == -1){
        table_code[tree[v].val] = curr; // leaf node
        std::bitset<8> number(tree[v].val); // i know
        tree_comp_form += '1' + number.to_string();
        return;
    }
    tree_comp_form += '0';
    dfs(tree[v].left, curr+'0');
    dfs(tree[v].right, curr+'1');
}

// Build Huffman tree
void build(std::vector<int>& v){
    std::priority_queue<Node> pq;
    std::vector<int> freq(alphabet);
    table_code.resize(alphabet);

    v.push_back(sentinel); // sentinel indicator (changes V)

    for(int e : v) freq[e]++;
    for(int i=0;i<freq.size();i++) {
        if(freq[i]==0) continue;
        Node n(freq[i], tree.size(), i);
        pq.push(n);
        tree.push_back(n);
    }

    while(pq.size() > 1){
        Node a = pq.top(); pq.pop();
        Node b = pq.top(); pq.pop();
        Node pai(a.freq + b.freq, tree.size());
        pai.left = a.idx; pai.right = b.idx;
        pq.push(pai);
        tree.push_back(pai);
    }
    if(tree.size() > 1) dfs(pq.top().idx, "");
    else dfs(pq.top().idx, "0");
}

std::string compress(std::vector<int>& v){
    build(v); // build huffman tree
    std::string ans = tree_comp_form; // huffman tree as header
    for(int el : v){
        ans += table_code[el];
    }
    return ans;
}

};

void write_file(std::string& s, std::string fileName){
    std::ofstream fout(fileName, std::ios::binary);
    while(s.size() % 8) s+="0";
    for(int i=0; i<s.size(); i+=8){
        char byte = 0;
        for(int j=i;j<i+8;j++) byte = (byte<<1) | (s[j]=='0'?0:1); 
        fout.write(&byte,1);
    }
    //size_t size=s.size();
    //fout.write(&s[0],size);

    fout.close();
}

int main(int argc, char* argv[]){
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    if(argc < 2){
        std::cout << "Please provide an input file.\n";
        return 0;
    }
    std::cout << "Zipzoping file...\n"; 
    Skew sa;
    sa.string_from_file(argv[1]);

    duration<double, std::milli> total;
    auto t1 = high_resolution_clock::now();
    sa.build();
    auto out_bwt = BWT(*sa.string, *sa.suffixArray);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    total += ms_double;
    // std::cout << "SA build + BWT: " << ms_double.count() << "ms\n";

    t1 = high_resolution_clock::now();
    auto out_mtf = MTF(out_bwt);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    total += ms_double;
    // std::cout << "MTF: " << ms_double.count() << "ms\n";

    t1 = high_resolution_clock::now();
    Huffman hf;
    auto out_hf = hf.compress(out_mtf);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;
    total += ms_double;
    // std::cout << "Huffman: " << ms_double.count() << "ms\n";

    write_file(out_hf, "out.compressed");
    std::cout << "Success in " << total.count() << "ms : created out.compressed file\n";
    return 0;
}