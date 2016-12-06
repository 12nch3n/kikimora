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

#include <json.h>
#include <string>
using namespace std;

namespace kikimora {
    class KmrJsonConf{
        private:
            const char* src_fname;
            string content;
        public:
            int add(const char* node_path, const char* node_content);
            int update(const char* node_path, const char* node_content);
            int replace(const char* pattern, const char* content);
            int drop(const char* node_path);
    }

    int KmrJsonConf::add(const char* node_path, const char* node_content){
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
