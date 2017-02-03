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
#include <regex>
#include <string>
#include <kikimora/diffs.hpp>

using namespace std;

namespace kikimora {
    class KmrJsonConf{
        private:
            Json::Value root;
            Json::Value string2node(const char* content);
            int upsert(const char* node_path, const char* node_content);
        public:
            KmrJsonConf(const char* content);
            int add(const char* node_path, const char* node_content);
            int update(const char* node_path, const char* node_content);
            int replace(const char* pattern, const char* content);
            int drop(const char* node_path);
            int save(const char* file_path);
    };

    Json::Value KmrJsonConf::string2node(const char* content){
        Json::Value ret;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(content, ret);
        if (!parsingSuccessful) {
            KK_LOG(KK_ERROR, \
                    "Could not parser the content from string to json: %s\n", \
                    reader.getFormattedErrorMessages().c_str());
            exit(1);
        }
        return ret;
    }

    KmrJsonConf::KmrJsonConf(const char* content){
        this->root = this->string2node(content);
    }


    int KmrJsonConf::add(const char* node_path, const char* node_content){
        return KmrJsonConf::upsert(node_path, node_content);
    }

    int KmrJsonConf::update(const char* node_path, const char* node_content){
        return KmrJsonConf::upsert(node_path, node_content);
    }

    int KmrJsonConf::upsert(const char* node_path, const char* node_content){
        Json::Path upsert_path(node_path);
        try{
            upsert_path.make(this->root) = this->string2node(node_content);
            KK_LOG(KK_NOTICE, \
                   "Set node %s with value %s.\n", \
                   node_path, node_content);
        } catch (exception& e) {
            KK_LOG(KK_ERROR, \
                   "Upsert node %s got exception: %s.\n", \
                   node_path, e.what());

        }
        return 0;
    }

    int KmrJsonConf::replace(const char* pattern, const char* content){
        Json::StyledWriter styled_writer;
        string dumped_json = styled_writer.write(this->root);
        regex ex_re(pattern);
        this->root = this->string2node(\
                regex_replace(dumped_json, ex_re, string(content)).c_str());
        return 0;
    }

    int KmrJsonConf::drop(const char* node_path){
        KxPath path = KxpathParse(node_path);
        KxNode last_node(-1);
        KxPath parent_path = KxParent(path, last_node);
        Json::Path parent(KxPathString(parent_path));
        try{
            Json::Value parent_cp = parent.resolve(this->root);
            cout << KxPathString(parent_path) << endl;
            cout << parent_cp << endl;
            if ( !last_node._name.empty() && \
               parent_cp.isObject()) {
               parent.make(this->root).removeMember(last_node.index_str());
            } else if ( parent_cp.isArray() ) {
                Json::Value new_arr(Json::arrayValue);
                for(auto i=0; i<parent_cp.size(); i++){
                    if ( i != last_node._index ) {
                        new_arr.append(parent_cp[i]);
                    }
                }
                parent.make(this->root) = new_arr;
            } else {
                KK_LOG(KK_ERROR, \
                        "Could not delete the node at %s", \
                        node_path);
                exit(1);
            }
        } catch (exception& e) {
            KK_LOG(KK_ERROR, \
                    "Could not delete the node at %s, error message: %s", \
                    node_path, e.what());
            exit(1);
        }
        return 0;
    }

    int KmrJsonConf::save(const char* file_path){
        Json::StyledWriter styled_writer;
        string dumped_json = styled_writer.write(this->root);
        cout << dumped_json << endl;
        return 0;
    }
} // end of namespace kikimora

