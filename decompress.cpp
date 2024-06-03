#include "suffix.h"
#include <string>
#include <algorithm>
#include <bitset>
#include <chrono>

// Inverse Burrows–Wheeler Transform
std::string IBWT(std::string& t){
    std::vector<int> cnt(256), pref(256), rank(t.size());
    char sentinel = (char)35; // '#'
    for(int i=0;i<t.size();i++) {
        if(t[i] == sentinel) continue;
        rank[i] = cnt[t[i]];
        cnt[t[i]]++;
    }
    std::string s = "";
    for(int i=1;i<256;i++) pref[i] = cnt[i-1] + pref[i-1];
    int at = 0;
    while(t[at] != sentinel){
        s += t[at];
        at = 1+pref[t[at]]+rank[at]; // skip '#' and prev. letters
    }
    std::reverse(s.begin(), s.end());
    return s;
}

std::vector<int> common_dict(){
    std::vector<int> d(256);
    auto block32 = [](auto& v, auto a, auto b){for(int i=a;i<b;i++) v[i]=i;};
    block32(d, 0, 32);
    block32(d, 32, 64);
    block32(d, 64, 96);
    block32(d, 96, 128);
    block32(d, 128, 256);
    return d;
}

// Inverse Move-To-Front Transform
std::string IMTF(std::vector<int>& v){
    auto dict = common_dict();
    std::string ans = "";
    for(int i=0; i<dict.size(); i++) dict[i]=i;
    for(int rank : v){
        ans += (char)dict[rank];
        // update dict
        int c = dict[rank];
        for(int j=rank; j>0; j--) dict[j] = dict[j-1];
        dict[0] = c;
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
std::vector<Node> tree;
int root, sentinel = 255, alphabet = 256;

std::pair<int,Node> dfs(int idx, std::string& ref){
    if(ref[idx] == '1'){ // leaf
        std::bitset<8> number(ref.substr(idx+1,8));
        Node leaf(0, tree.size(), (int)number.to_ulong());
        tree.push_back(leaf);
        return std::make_pair(idx+8, leaf);
    }
    auto left = dfs(idx+1, ref);
    auto right = dfs(left.first+1, ref);
    Node par(0, tree.size());
    par.left = left.second.idx; par.right = right.second.idx;
    tree.push_back(par);
    return {right.first, par};
}

int build(std::string& s){
    // std::cout << s << '\n';
    // s = "00171819";
    auto p = dfs(0, s);
    root = p.second.idx;
    return p.first+1;
}

std::pair<int,int> dfs2(int v, int idx, std::string& ref){
    if(tree[v].left == -1 && tree[v].right == -1){
        return {tree[v].val, idx};
    }

    if(ref[idx] == '0') return dfs2(tree[v].left, idx+1, ref);
    else return dfs2(tree[v].right, idx+1, ref);
}

std::vector<int> decompress(std::vector<int>& v){
    std::string s = "";
    for(int el : v) for(int k=7;k>=0;k--) s += (((el>>k)&1)==0 ? "0" : "1");
    std::vector<int> og;
    int idx = build(s);
    while(idx < s.size()){
        auto p = dfs2(root, idx, s);
        if(p.first == sentinel) break;
        og.push_back(p.first);
        idx = p.second;
    }
    return og;
}
};

void write_file(std::string& s, std::string fileName){
    std::ofstream fout(fileName, std::ios::binary);
    
    size_t size=s.size();
    //fout.write(&size,sizeof(size));
    fout.write(&s[0],size);

    fout.close();
}

int main(){
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
    std::cout << "Decompressing file...\n";
    Skew sa; sa.string_from_file("output_comp.txt");
    std::vector<int> vs = *sa.string; vs.pop_back();

    auto t1 = high_resolution_clock::now();
    Huffman hf;
    auto v = hf.decompress(vs); 
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;    
    std::cout << "Huffman: " << ms_double.count() << "ms\n";

    t1 = high_resolution_clock::now();
    auto out_imtf = IMTF(v);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;    
    std::cout << "IMTF: " << ms_double.count() << "ms\n";


    t1 = high_resolution_clock::now();
    auto out_ibwt = IBWT(out_imtf);
    t2 = high_resolution_clock::now();
    ms_double = t2 - t1;    
    std::cout << "IBWT: " << ms_double.count() << "ms\n";


    write_file(out_ibwt, "output_decomp.txt");
}