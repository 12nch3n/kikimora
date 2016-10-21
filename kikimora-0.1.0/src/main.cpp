#include <json/json.h>
#include <iostream>
#include <kikimora/conf_package.hpp>
#include <vector>
using namespace std;
using namespace kikimora;

int main()
{
    cout << "Hello world!" << endl;
    Json::Value vec(Json::arrayValue);
    vec.append(Json::Value(true));
    vec.append(Json::Value(2));
    vec.append(Json::Value(3));
    cout << vec[0] << endl;
    string src_path="/Users/yinchen/Projects/kikimora";
    string dst_path="./test";
    string diff_path="./test";
    vector<string> file_list;
    ConfPackage cp = ConfPackage(src_path.c_str(), dst_path.c_str(), diff_path.c_str());
    cp.ScanSrcFiles(&file_list);
    vector<string>::iterator v = file_list.begin();
    while (v != file_list.end()) {
        cout << *v << endl; 
        v++;
    }
    return 0;
}
