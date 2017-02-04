/*
 * =====================================================================================
 *
 *       Filename:  diffs.hpp
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  10/30/2016 11:27:57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization: 
 *
 * =====================================================================================
 */

#ifndef DIFFS_H
#define DIFFS_H
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>
#include <assert.h>

#ifndef MAX_STR_LEN
#define MAX_STR_LEN 1024
#endif
#ifndef FEA_LEN
#define FEA_LEN 25
#endif

namespace kikimora{
    using namespace std;
    enum class OpType{ \
        ACHIEVE=0, /* Achieve file from a speicified source. */ \
        ADD=1, /* ADD a node in the conf file with specified contents. */ \
        UPDATE=2, /*  Update a node in the conf file with specified contents. */ \
        REPLACE=3, /* Replace the conf file content with the specified contents.*/ \
        DELETE=4 /* Delete the specified contents. */
    };
    typedef struct Version{ //reference to http://semver.org/
        uint16_t MAJOR;
        uint16_t MINOR;
        uint16_t PATCH;
        char* FEATURE; // 8 charactor length;
        uint32_t Weight() const {
            return ((uint32_t)MAJOR*1000+(uint32_t)MINOR)*1000+(uint32_t)PATCH;
        }
    } Version;

    Version* NewVersion(const char* version_str){
        Version* ret=(Version*)malloc(sizeof(Version));
        ret->FEATURE = (char*)malloc(FEA_LEN*sizeof(char));
        regex version_pattern("(\\d+)\\.(\\d+)\\.(\\d+)\\-(.{1,8})|(\\d+)\\.(\\d+)\\.(\\d+)|(\\d+)\\.(\\d+)");
        cmatch match;
        if ( regex_match(version_str, match, version_pattern) ) {
            int index=1;
            for ( ; index<match.size();index++ ){
                if ( !match[index].str().empty()) break;
            }
            ret->MAJOR = (uint16_t)atoi(match[index].str().c_str());
            ret->MINOR = (uint16_t)atoi(match[index+1].str().c_str());
            if ( index+2 < match.size() && \
                    ! match[index+2].str().empty() ) {
                ret->PATCH = (uint16_t)atoi(match[index+2].str().c_str());
            }else{
                ret->PATCH = 0;
            }
            if ( index+3 < match.size() && \
                    ! match[index+3].str().empty() ) {
                strcpy(ret->FEATURE, match[index+3].str().c_str());
            }else{
                memcpy(ret->FEATURE, "", FEA_LEN*sizeof(char));
            }
        }
        assert(ret->MAJOR<(uint16_t)1000);
        assert(ret->MINOR<(uint16_t)1000);
        assert(ret->PATCH<(uint16_t)1000);
        return ret;
    }

    typedef struct Diff{
        uint32_t rcd_no;
        OpType operation;
        char* conf_file;
        Version* version;
        char* category;
        char* diff_node;
        /* diff_node: If operation is REPLACE, diff_node would be the keyword to replace. */
        char* node_content;
        uint32_t NodeHash() const {
            hash<string> hash_fn;
            return hash_fn(string(conf_file)+":"+string(diff_node));
        }
        uint64_t Weight() const {
            uint64_t ret = 0;
            ret = (uint64_t)version->Weight()<<34;
            ret += (uint64_t)(0 != strcmp(category, "common"))<<33;
            ret += (uint64_t)(0 != strcmp(version->FEATURE, ""))<<32;
            ret += (uint64_t)rcd_no;
            return ret;
        }
    }Diff;

    Diff* NewDiff(){
        Diff* ret=(Diff*)malloc(sizeof(Diff));
        ret->conf_file = (char*)malloc(MAX_STR_LEN*sizeof(char));
        ret->version = NewVersion("0.0");
        ret->category = (char*)malloc(MAX_STR_LEN*sizeof(char));
        ret->diff_node = (char*)malloc(MAX_STR_LEN*sizeof(char));
        ret->node_content = (char*)malloc(MAX_STR_LEN*sizeof(char));
        return ret;
    }

    class Diffs{
        private:
            const char* diff_file;
            const char* base_conf_dir;
            vector<Diff*> diff_rcds;
            int LoadRcdline(const char* line, Diff* rcd);
        public:
            Diffs(const char* diff_file, const char* base_conf_dir);
            int MergeCommon(const char* release_ver);
            /*  PickDiffs4Cate: Plus the diff records for the common category.
             *  return cate_diff = common + category */
            vector<Diff*> PickDiffs4Cate(const char* release_ver, const char* category, const char* feature);
            /*  PrintRcds: Print the merged diff points. */
            int PrintRcds();
    };
    Diffs::Diffs(\
            const char* diff_file, \
            const char* base_conf_dir\
            ){
        ifstream file_reader(diff_file);
        if(!file_reader){
            KK_LOG(KK_ERROR, "Could not open log file <%s>.\n", diff_file);
            exit(EXIT_FAILURE);
        }
        string line;
        uint32_t lineno=0;
        Diff* cate=NewDiff();
        memcpy(cate->category,"common", MAX_STR_LEN*sizeof(char));
        while(getline(file_reader, line)){
            lineno++;
            if (line.length() <= 0) continue;
            Diff* cur_rcd = NewDiff();
            cur_rcd->rcd_no = lineno;
            memcpy(cur_rcd->version, cate->version,sizeof(Version));
            memcpy(cur_rcd->category, cate->category, MAX_STR_LEN*sizeof(char));
            if ( this->LoadRcdline(line.c_str(), cur_rcd) > 1 ){
                this->diff_rcds.push_back(cur_rcd);
            }else{
                cate=cur_rcd;
            }
        }
    }

    int Diffs::MergeCommon(const char* release_ver){
        const Version* crn_ver = NewVersion(release_ver);
        //vector<Diff*>::iterator diff;
        for(auto diff=this->diff_rcds.begin(); diff!=this->diff_rcds.end(); diff++){
            if ( 0 == strcmp((*diff)->category, "common") ){
                if ( crn_ver->Weight() > (*diff)->version->Weight()){
                    memcpy((*diff)->version, crn_ver, sizeof(Version));
                }
            }
        }
        return 0;
    }

    vector<Diff*> Diffs::PickDiffs4Cate(const char* release_ver, \
            const char* category, \
            const char* feature = ""){
        const Version* crn_ver = NewVersion(release_ver);
        vector<Diff*> ret;
        //vector<Diff*>::iterator diff;
        for(auto diff=this->diff_rcds.begin(); diff!=this->diff_rcds.end(); diff++){
            if ( 0 == strcmp((*diff)->category, "common") ){
                if ( (*diff)->version->Weight() <= crn_ver->Weight() && \
                        ( 0 == strcmp((*diff)->version->FEATURE, "") || \
                        ( 0 == strcmp((*diff)->version->FEATURE, feature) ))){
                    ret.push_back(*diff);
                } else {
                    continue;
                }
            }else if ( 0 == strcmp((*diff)->category, category) ) {
                ret.push_back(*diff);
            }else {
                continue;
            }
        }
        return ret;
    }

    int Diffs::PrintRcds(){
        //vector<Diff*>::iterator diff;
        for (auto diff=this->diff_rcds.begin(); diff!=this->diff_rcds.end(); diff++){
            cout<<"No:"<<(*diff)->rcd_no<<"\t";
            cout<<"Version:"<<(*diff)->version->MAJOR<<"."<<\
                (*diff)->version->MINOR<<"."<<\
                (*diff)->version->PATCH<<"-"<<(*diff)->version->FEATURE<<"\t";
            cout<<"Category:"<<(*diff)->category<<"\t";
            cout<<"OP:"<<(int)(*diff)->operation<<"\t";
            cout<<"conf_file:"<<(*diff)->conf_file<<"\t";
            cout<<"diff_node:"<<(*diff)->diff_node<<"\t";
            cout<<"node_content:"<<(*diff)->node_content;
            cout<<endl;
        }
        return 0;
    }

    int Diffs::LoadRcdline(const char* line, Diff* rcd){
        regex ver_pattern ("\\<([^\\s]*)\\>");
        regex cate_pattern ("\\[([^\\s]*)\\]");
        regex node_pattern ("(ADD|UPDATE|REPLACE)\\s+([^\\s]*?)\\s+([^\\s]*?)\\s+(.*)");
        regex delete_node_pattern ("(DELETE)\\s+([^\\s]*?)\\s+([^\\s]*)");
        regex add_file_pattern ("(ACHIEVE)\\s+([^\\s]*?)\\s+(.*)");
        cmatch match;
        if ( regex_match(line, match, ver_pattern) ) {
            memcpy(rcd->category,"common", MAX_STR_LEN*sizeof(char));
            rcd->version=NewVersion(match[1].str().c_str());
            return 0;
        }else if ( regex_match(line, match, cate_pattern) ) {
            rcd->version = NewVersion("999.999.999");
            strcpy(rcd->category, match[1].str().c_str());
            return 1;
        }else if ( regex_match(line, match, node_pattern) ){
            if ( 0 == strcmp(match[1].str().c_str(), "ADD") ) rcd->operation = OpType::ADD;
            if ( 0 == strcmp(match[1].str().c_str(), "UPDATE") ) rcd->operation = OpType::UPDATE;
            if ( 0 == strcmp(match[1].str().c_str(), "REPLACE")) rcd->operation = OpType::REPLACE;
            strcpy(rcd->conf_file, match[2].str().c_str());
            strcpy(rcd->diff_node, match[3].str().c_str());
            strcpy(rcd->node_content, match[4].str().c_str());
            return 2;
        }else if ( regex_match(line, match, delete_node_pattern) ){
            if ( 0 == strcmp(match[1].str().c_str(), "DELETE")) rcd->operation = OpType::DELETE;
            strcpy(rcd->conf_file, match[2].str().c_str());
            strcpy(rcd->diff_node, match[3].str().c_str());
            return 2;
        }else if ( regex_match(line, match, add_file_pattern) ){
            if ( 0 == strcmp(match[1].str().c_str(), "ACHIEVE")) rcd->operation = OpType::ACHIEVE;
            strcpy(rcd->conf_file, match[2].str().c_str());
            strcpy(rcd->node_content, match[3].str().c_str());
            return 2;
        }
        else{
            return -1;
        }
    }

    map<uint32_t, Diff*> UniqByDiffPoint(vector<Diff*> diffs){
        map<uint32_t, Diff*> ret;
        //vector<Diff*>::iterator it;
        for(auto it=diffs.begin(); it!=diffs.end();it++){
            uint32_t node_hash = (*it)->NodeHash();
            if (ret.find(node_hash) == ret.end()){
                ret[node_hash] = *it;
            }else{
                bool it_cate_flag = (0 == strcmp((*it)->category, "common"));
                bool pre_it_cate_flag = (0 == strcmp(ret[node_hash]->category, "common"));
                if ((*it)->Weight() > ret[node_hash]->Weight()) {
                    if ( (*it)->operation < ret[node_hash]->operation){
                        KK_LOG(KK_WARNING, "The Operation priority in line:<%d> is lower than line:<%d>, %d<%d.\n", \
                            (*it)->rcd_no, ret[node_hash]->rcd_no, (*it)->operation, ret[node_hash]->operation);
                    }else{
                        KK_LOG(KK_NOTICE, "Diff line:<%d> is merged by diff line:<%d> with the same diff point: {%s:%s}.\n", \
                             ret[node_hash]->rcd_no, (*it)->rcd_no, \
                             ret[node_hash]->conf_file, ret[node_hash]->diff_node);
                        ret[node_hash] = *it;
                    }
                }else{
                    continue;
                }
            }
        }
        return ret;
    }

    map< uint32_t, vector<Diff*> > GroupByFile(map<uint32_t, Diff*> uniq_diffs){
        map< uint32_t, vector<Diff*> > ret;
        hash<string> hash_fn;
        for(auto it = uniq_diffs.begin(); \
                it!=uniq_diffs.end(); it++ ) {
            uint32_t fname_hash = (uint32_t)hash_fn(it->second->conf_file);
            if(ret.end() == ret.find(fname_hash)) ret[fname_hash]=vector<Diff*>();
            ret[fname_hash].push_back(it->second);
        }
        return ret;
    }

    class KxNode;
    typedef vector<KxNode*> KxPath;

    class KxNode{
        public:
            string _name;
            int _index;
            KxNode(const char* name);
            KxNode(int index);
            string index_str();
    };

    KxNode::KxNode(const char* name) : _name(name), _index(-1) {}
    KxNode::KxNode(int index) : _name(), _index(index) {}

    string KxNode::index_str(){
        if ( !this->_name.empty() ) {
            return this->_name;
        } else {
            char ret[10] = "";
            sprintf(ret, "[%d]", this->_index);
            return string(ret);
        }
    }

    string KxPathString(KxPath path){
        //string path_str = (*path.begin())->index_str();
        string path_str = "";
        for(auto pt=path.begin(); pt!=path.end(); pt++){
            if ( !(*pt)->_name.empty()){
                path_str += '.' + (*pt)->_name;
            } else {
                char ret[10] = "";
                sprintf(ret, "[%d]", (*pt)->_index);
                path_str += string(ret);
            }
        }
        return path_str;
    }

    KxPath KxParent(KxPath path, KxNode& last_index){
        last_index = *(path.back());
        path.pop_back();
        return path;
    }

    KxPath KxpathParse(const char* node_path, const string pattern="."){
        regex node_pattern("(\\w+)\\[(\\d+)\\]|(\\w+)");
        string::size_type pos;
        vector<KxNode*> ret;
        string path_str(node_path);
        path_str += pattern;
        int size = path_str.size();
        for(int i=0; i<size; i++){
            pos = path_str.find(pattern,i);
            if (pos < size){
                string node_str = path_str.substr(i, pos-i);
                smatch match;
                if ( regex_match(node_str, match, node_pattern) ) {
                    if ( ! match[1].str().empty()) {
                        KxNode* kxnode1 = (KxNode*)malloc(sizeof(KxNode));
                        KxNode* kxnode2 = (KxNode*)malloc(sizeof(KxNode));
                        *kxnode1 = KxNode(match[1].str().c_str());
                        *kxnode2 = KxNode(atoi(match[2].str().c_str()));
                        ret.push_back(kxnode1);
                        ret.push_back(kxnode2);
                    }else{
                        KxNode* kxnode = (KxNode*)malloc(sizeof(KxNode));
                        *kxnode = KxNode(match[3].str().c_str());
                        ret.push_back(kxnode);
                    }
                    i=pos+pattern.size()-1;
                }
            }
        }
        return ret;
    }
}
#endif /* DIFFS_H */
