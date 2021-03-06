#ifndef FUNC_H
#define FUNC_H

#include <fstream>
#include <string>
#include <iostream>
#include <regex>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

// #include <io.h>
// #include <direct.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::ofstream;
using std::regex;
using std::set;
using std::string;
using std::vector;
using std::ios;

bool is_folder_exist(string folder) {
    return access(folder.c_str(), 0) == 0;
}

bool make_dir(string path) {
    string cur = "";
    for(auto c : path) {
        cur += c;
        if(c == '\\' || c == '/') {
            if(!is_folder_exist(cur)) {
                int ret = mkdir(cur.c_str(), S_IRWXU);
                if(ret == -1) {
                    return false;
                }
            }
        }
    }

    if(!is_folder_exist(cur)) {
        int ret = mkdir(cur.c_str(), S_IRWXU);
        return ret != -1;
    }

    return true;
}

string get_filename(string path) {
    string::size_type pos = path.find_last_of('\\');
    
    if(pos == string::npos) {
        pos = path.find_last_of('/');
        if(pos == string::npos) {
            return path;
        }
    }
    return path.substr(pos+1);
}

vector<string> get_all_files(string base_dir, string extend) {
    vector<string> files;

    DIR *pDir;
    dirent* ptr;
    if(!(pDir = opendir(base_dir.c_str()))) {
        return files;
    }
    while((ptr = readdir(pDir))!=0) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0 && 
            std::regex_search(ptr->d_name, std::regex("\\w+\\." + extend + "$"))) {
            files.push_back(base_dir + "/" + ptr->d_name);
        }
    }
    closedir(pDir);

    return files;
}

bool cp_file(string file, string destination) {
    string filename = get_filename(file);
    if (!is_folder_exist(destination)) {
        make_dir(destination);
    }

    ifstream in(file, ios::in);
    string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    
    ofstream out(destination + filename, ios::ate|ios::out);
    out << str;

    in.close();
    out.close();
}

string get_pname(string filename) {
    string name;
    for(auto &c : filename) {
        if((c >= '0' && c <= '9') || c == '.') {
            break;
        }
        name += c;
    }
    return name;
}

// trim string
string trim(const string &str) {
    string::size_type pos = str.find_first_not_of(" \r\n\t");
    if (pos == string::npos) {
        return str;
    }
    string::size_type pos2 = str.find_last_not_of(" \r\n\t");
    if (pos2 != string::npos) {
        return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
}

double frac2double(const string &str) {
    if(str.find("/") == string::npos) {
        return 0.0;
    }

    int coff = 1;
    int pos = str.find("/"), i = pos-1, j = pos+1;
    double numerator = 0, denominator = 0;
    while(i >= 0 && str[i] >= '0' && str[i] <= '9') {
        numerator += (str[i] - '0') * pow(10, abs(i - pos) - 1);
        --i;
    }

    if(i >= 0 && str[i] == '-') {
        coff = -1;
    }

    while(j < str.size() && str[j] >= '0' && str[j] <= '9') {
        denominator = denominator * 10 + (str[j] - '0');
        ++j;
    }
    
    return coff * numerator / denominator;
}

// remove \' and \"
string clean_str(const string &str) {
    int start = 0, end = str.size() - 1;
    while(str[start] == '\'' || str[start] == '\"') ++start;
    while(str[end] == '\'' || str[end] == '\"') --end;
    return str.substr(start, end - start + 1);
}

// divide strings according to delimiters
vector<string> del_split(const string &str, char delimiter) {
    vector<string> result;
    string cur = str;
    while (!cur.empty()) {
        int index = cur.find_first_of(delimiter);
        if (index == -1) {
            result.push_back(cur);
            cur.clear();
        }
        else {
            if(trim(cur.substr(0, index)) != "") {
                result.push_back(cur.substr(0, index));
            }
            cur = cur.substr(index + 1, cur.size() - index - 1);
        }
    }
    return result;
}

// divide strings according to regular matches
vector<string> re_split(const string &str, string pattern) {
    string cur = str;
    std::regex rgx(pattern);
    vector<string> result;
    for (std::sregex_token_iterator iter(cur.begin(), cur.end(), rgx, -1), end; iter != end; ++iter) {
        // cerr << iter->str() << endl;
        string s = iter->str();
        if(!s.empty()) {
            result.push_back(iter->str());
        }
    }
    return result;
}

// get real num from string
double get_num(string value) {
    try {
        return atof(value.c_str());
    }
    catch(...) {
        vector<string> nums = del_split(value, '(');

        if(!nums.empty()) {
            return atof(nums[0].c_str());
        }

        return -1;
    }
}

void cross(double a[3], double b[3], double res[3]) {
    // a = (a1, a2, a3), b = (b1, b2, b3)
    // a x b = (a2b3-a3b2, a3b1-a1b3, a1b2-a2b1)

    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

double round(double number, unsigned int bits) {
    double integer = floor(number);
    number -= integer;
    for (unsigned int i = 0; i < bits; ++i)
        number *= 10;
    number = floor(number + 0.5);
    for (unsigned int i = 0; i < bits; ++i)
        number /= 10;
    return integer + number;
}


// print vector for debugging
template <typename T> 
void printVec(vector<set<T>> &vec) {
    for(auto item : vec) {
        for(auto it = item.begin(); it != item.end(); it++) {
            cerr << *it << " ";
        }
        cerr << endl << endl;
    }
}

// print vector for debugging
template <typename T>
void printVec(vector<T> &vec) {
    for(auto &item : vec) {
        cerr << item << " ";
    }
    // cerr << endl;
}


// print set for debugging
template <typename T>
void printSet(set<T> &set) {
    for(auto iter = set.begin(); iter != set.end(); iter++) {
        cerr << *iter << " ";
    }
    cerr << endl;
}

// print map for debugging
template <typename key, typename member>
void printMap(map<key, vector<member>> &m) {
    for(auto iter = m.begin(); iter != m.end(); iter++) {
        cerr << iter->first << " : ";
        for(auto it = iter->second.begin(); it != iter->second.end(); it++) {
            cerr << *it << " ";
        }
        cerr << endl;
    }
}

// print map for debugging
template <typename key, typename member>
void printMap(map<key, vector<vector<member>>> &m) {
    for(auto iter = m.begin(); iter != m.end(); iter++) {
        cerr << iter->first << " : ";
        for(auto it = iter->second.begin(); it != iter->second.end(); it++) {
            printVec(*it);
            cerr << "\t";
        }
        cerr << endl;
    }
}

// print map for debugging
template <typename key, typename value>
void printMap(map<key, value> &m) {
    for(auto iter = m.begin(); iter != m.end(); iter++) {
        cerr << iter->first << " : " << iter->second << endl;
    }
}

#endif