#define KK_NOTICE 0
#define KK_WARNING 1
#define KK_ERROR 2
#define KK_LOG(LEVEL, fmt, ...)\
        do {\
                if (KK_NOTICE == LEVEL)\
                    fprintf(stdout, "NOTICE:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
                else if (KK_WARNING == LEVEL)\
                    fprintf(stdout, "WARNING:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
                else if (KK_ERROR == LEVEL)\
                    fprintf(stdout, "ERROR:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
            } while(0)

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
