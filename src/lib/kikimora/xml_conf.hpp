/*
 * =====================================================================================
 *
 *       Filename:  xml_conf.hpp
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  12/05/2016 14:57:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <regex>
#include <string>
#include <kikimora/diffs.hpp>

using namespace std;

namespace kikimora {
    class KmrXmlConf{
        private:
            rapidxml::xml_document<> doc;
            rapidxml::xml_node<> string2node(const char* content);
            int upsert(const char* node_path, const char* node_content);
        public:
            KmrXmlConf(const char* content);
            int add(const char* node_path, const char* node_content);
            int update(const char* node_path, const char* node_content);
            int replace(const char* pattern, const char* content);
            int drop(const char* node_path);
            int save(const char* file_path);
    };

    KmrXmlConf::KmrXmlConf(const char* content){

    }

    int KmrXmlConf::upsert(const char* node_path, const char* node_content){
        return 0;
    }

    int KmrXmlConf::add(const char* node_path, const char* node_content){
        return 0;
    }

    int KmrXmlConf::update(const char* node_path, const char* node_content){
        return 0;
    }

    int KmrXmlConf::replace(const char* pattern, const char* content){
        return 0;
    }

    int KmrXmlConf::drop(const char* node_path){
        return 0;
    }

    int KmrXmlConf::save(const char* file_path){
        return 0;
    }

};
