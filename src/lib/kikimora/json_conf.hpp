/*
 * =====================================================================================
 *
 *       Filename:  json_conf.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/05/2016 14:57:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <json/json.h>
#include <string>
#include <kikimora/diffs.hpp>

using namespace std;

namespace kikimora {
    class KmrJsonConf{
        private:
            string content;
        public:
            KmrJsonConf(const char* content);
            int add(const char* node_path, const char* node_content);
            int update(const char* node_path, const char* node_content);
            int replace(const char* pattern, const char* content);
            int drop(const char* node_path, const char* content);
            int save(const char* node_path);
    };
    KmrJsonConf::KmrJsonConf(const char* content){}


    int KmrJsonConf::add(const char* node_path, const char* node_content){
        KxPath xpath = KxpathParse(node_path);
        return 0;
    }
    int KmrJsonConf::update(const char* node_path, const char* node_content){
        return 0;
    }
    int KmrJsonConf::replace(const char* node_path, const char* node_content){
        return 0;
    }
    int KmrJsonConf::drop(const char* node_path, const char* node_content){
        return 0;
    }
    int KmrJsonConf::save(const char* node_content){
        return 0;
    }
} // end of namespace kikimora

