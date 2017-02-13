/*
 * =====================================================================================
 *
 *       Filename:  package.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/19/2016 17:15:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CONF_PACKAGE_HPP
#define CONF_PACKAGE_HPP
#include <vector>
#include <regex>
#include <string>
#include <dirent.h>
#include <fstream>
#include <kikimora/diffs.hpp>
#include <kikimora/json_conf.hpp>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace kikimora{
    vector<string> SUPPORTED_TYPES = {"JSON", "XML", "YAML", "INI"};
    using namespace std;

    class ConfPackage{
        private:
            string src_pkg_path;
            string dst_pkg_path;
            string list_file;
            map<uint32_t, string> file_list;
            int BuildFileslist(map<uint32_t, string> &file_list);
        public:
            ConfPackage(
                 const char *src_pkg_path, \
                 const char *dst_pkg_path, \
                 const char *list_file);
            int ProcessFiles(map< uint32_t, vector<Diff*> > file_diffs);
    };
    ConfPackage::ConfPackage(
            const char *src_pkg_path, \
            const char *dst_pkg_path, \
            const char *list_file) :
        src_pkg_path(src_pkg_path), \
        dst_pkg_path(dst_pkg_path), \
        list_file(list_file){
            BuildFileslist(this->file_list);
        }

    int ConfPackage::BuildFileslist(map<uint32_t, string> &src_files){
        ifstream file_reader(this->list_file);
        if(!file_reader){
            KK_LOG(KK_ERROR, "Could not open file list <%s>.\n", this->list_file.c_str());
            exit(EXIT_FAILURE);
        }
        string line;
        regex file_pattern("(.+)\\s+(\\w+)");
        while(getline(file_reader, line)){
            if (line.length() <= 0) continue;
            smatch match;
            if ( regex_match(line, match, file_pattern) ) {
                if ( find(SUPPORTED_TYPES.begin(), SUPPORTED_TYPES.end(), match[2].str()) != SUPPORTED_TYPES.end()){
                    char buffer[2*1024];
                    sprintf(buffer, "%s/%s", this->src_pkg_path.c_str(), match[1].str().c_str());
                    char real_buf[2*1024];
                    if ( NULL == realpath(buffer, real_buf) ){
                        KK_LOG(KK_ERROR, \
                                "Could not locate conf_file: %s.\n", \
                                buffer);
                    }
                    hash<string> hash_fn;
                    src_files[hash_fn(string(real_buf))] = match[2].str().c_str();
                } else {
                    KK_LOG(KK_WARNING, \
                            "Could not recongize the file type: %s\n", \
                            match[2].str().c_str());
                }
            } else{
                continue;
            }
        }
        return 0;
    }

    template<typename CONF_TYPE>
        int PatchDiffs(CONF_TYPE* conf_file, vector<Diff*> diffs){
            for(auto it = diffs.begin();it!=diffs.end();it++){
                KK_LOG(KK_NOTICE, \
                       "Patch Diff Line: %d.\n", (*it)->rcd_no);
                switch ((*it)->operation){
                    case OpType::ACHIEVE: {
                        cout<<"Conf file archive would supported in the further version."<<endl;
                        break; }
                    case OpType::ADD: {
                        conf_file->add((*it)->diff_node, (*it)->node_content);
                        break; }
                    case OpType::UPDATE: {
                        conf_file->update((*it)->diff_node, (*it)->node_content);
                        break;}
                    case OpType::REPLACE: {
                        conf_file->replace((*it)->diff_node, (*it)->node_content);
                        break;}
                    case OpType::DELETE: {
                        conf_file->drop((*it)->diff_node);
                        break;}
                    default:  break;
                }
            }
            return 0;
        }

    template<typename CONF_TYPE>
        int SaveToFile(CONF_TYPE* conf_file, const char* save_to_dest){
            char* bname = dirname((char *)save_to_dest);
            mkdir(bname, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
            return conf_file->save(save_to_dest);
        }

    int ConfPackage::ProcessFiles(map< uint32_t, vector<Diff*> > file_diffs){
        for (auto conf_it = file_diffs.begin(); conf_it != file_diffs.end(); conf_it++){
            char buffer[2*1024];
            char real_buf[2*1024];
            sprintf(buffer, "%s/%s", this->src_pkg_path.c_str(),  (*(conf_it->second.begin()))->conf_file);
            if ( NULL == realpath(buffer, real_buf) ){
                KK_LOG(KK_ERROR, \
                        "Could not locate conf_file: %s.\n",
                        buffer);
                exit(-1);
            }
            string src_conf(real_buf);
            hash<string> hash_fn;
            string conf_type = this->file_list.end() != this->file_list.find(hash_fn(string(real_buf))) ? \
                               this->file_list[hash_fn(string(real_buf))] : \
                               "XML";
            sprintf(buffer, "%s/%s", this->dst_pkg_path.c_str(),  (*(conf_it->second.begin()))->conf_file);
            string dest_conf = string(buffer);
            if ( "JSON" == conf_type ) {
                ifstream in(src_conf, ios::in);
                istreambuf_iterator<char> beg(in), end;
                string content(beg, end);
                in.close();
                KmrJsonConf conf(content.c_str());
                PatchDiffs<KmrJsonConf>(&conf, conf_it->second);
                SaveToFile<KmrJsonConf>(&conf, dest_conf.c_str());
            }
        }
        return 0;
    }

} // namespace kikmora
#endif /* CONF_PACKAGE_HPP */
