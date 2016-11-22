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

#ifndef DEFF_H
#define DEFF_H
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>

#ifndef MAX_STR_LEN
#define MAX_STR_LEN 1024
#endif
#ifndef LABEL_LEN
#define LABEL_LEN 9
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
        char* LABEL; // 8 charactor length;
        bool operator < (const Version& other) const {
            return (MAJOR<<8+MINOR)<<8+PATCH < (other.MAJOR<<8+other.MINOR)<<8+other.PATCH;
        }
        bool operator > (const Version& other) const {
            return (MAJOR<<8+MINOR)<<8+PATCH > (other.MAJOR<<8+other.MINOR)<<8+other.PATCH;
        }
        bool operator == (const Version& other) const {
            return (MAJOR<<8+MINOR)<<8+PATCH == (other.MAJOR<<8+other.MINOR)<<8+other.PATCH;
        }
    } Version;

    Version* NewVersion(const char* version_str){
        Version* ret=(Version*)malloc(sizeof(Version));
        ret->LABEL = (char*)malloc(LABEL_LEN*sizeof(char));
        regex version_pattern("(\\d+)\\.(\\d+)\\.(\\d+)\\-(.{1,8})|(\\d+)\\.(\\d+)\\.(\\d+)|(\\d+)\\.(\\d+)");
        cmatch match;
        if ( regex_match(version_str, match, version_pattern) ) {
            unsigned index=1;
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
                strcpy(ret->LABEL, match[index+3].str().c_str());
            }else{
                memcpy(ret->LABEL, "default", LABEL_LEN*sizeof(char));
            }
        }
        return ret;
    }

    typedef struct Diff{
        int rcd_no;
        OpType operation;
        char* conf_file;
        Version* version;
        char* category;
        char* diff_node;
        /* diff_node: If operation is REPLACE, diff_node would be the keyword to replace. */
        char* node_content;
        bool operator < (const Diff& other) const {
            hash<string> h;
            uint64_t weight = 0;
            weight += (uint16_t)h(string(conf_file)+":"+string(diff_node))<<48;
            weight += version->MAJOR<<32;
            weight += version->MINOR<<16;
            weight += version->PATCH;
            uint64_t other_weight = 0;
            other_weight += (uint16_t)h(string(other.conf_file)+":"+string(other.diff_node))<<48;
            other_weight += other.version->MAJOR<<32;
            other_weight += other.version->MINOR<<16;
            other_weight += other.version->PATCH;
            if (weight == other_weight) return rcd_no < other.rcd_no;
            else return weight < other_weight;
        }
        bool operator > (const Diff& other) const {
            hash<string> h;
            uint64_t weight = 0;
            weight += (uint16_t)h(string(conf_file)+":"+string(diff_node))<<48;
            weight += version->MAJOR<<32;
            weight += version->MINOR<<16;
            weight += version->PATCH;
            uint64_t other_weight = 0;
            other_weight += (uint16_t)h(string(other.conf_file)+":"+string(other.diff_node))<<48;
            other_weight += other.version->MAJOR<<32;
            other_weight += other.version->MINOR<<16;
            other_weight += other.version->PATCH;
            if (weight == other_weight) return rcd_no > other.rcd_no;
            else return weight > other_weight;
        }
    } Diff;

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
            /*  MergeMain: merge the all common diff by diff_point */
            int Merge();
            /*  Merge4Cate: Plus the diff records for the specified category.
             *  return cate_diff = common + category */
            vector<Diff*> PlusDiff4Cate(const char* category);
            /*  PrintRcds: Print the merged diff points. */
            int PrintRcds(vector<Diff*> diff_package);
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
        int lineno=0;
        Diff* cate=NewDiff();
        memcpy(cate->category,"base", MAX_STR_LEN*sizeof(char));
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
        this->PrintRcds(this->diff_rcds);
    }

    int Diffs::Merge(){
        return 0;
    }
    vector<Diff*> Diffs::PlusDiff4Cate(const char* category){
        vector<Diff*> diff_package;
        return diff_package;
    }
    int Diffs::PrintRcds(vector<Diff*> diff_package){
        vector<Diff*>::iterator diff;
        for (diff=diff_package.begin(); diff!=diff_package.end(); diff++){
            cout<<"No:"<<(*diff)->rcd_no<<"\t";
            cout<<"Version:"<<(*diff)->version->MAJOR<<"."<<(*diff)->version->MINOR<<"\t";
            cout<<"Category:"<<(*diff)->category<<"\t";
            cout<<"OP:"<<(int)(*diff)->operation<<"\t";
            cout<<"conf_file:"<<(*diff)->conf_file<<"\t";
            cout<<"diff_node:"<<(*diff)->diff_node<<"\t";
            cout<<"node_content:"<<(*diff)->node_content;
            cout<<endl;
            cout<<(*diff<*diff_package.begin())<<(*diff>*diff_package.begin())<<endl;
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
            memcpy(rcd->category,"base", MAX_STR_LEN*sizeof(char));
            rcd->version=NewVersion(match[1].str().c_str());
            return 0;
        }else if ( regex_match(line, match, cate_pattern) ) {
            rcd->version = NewVersion("65535.65535");
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
}
#endif /* DEFF_H */
