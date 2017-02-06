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
#include <string>
#include <dirent.h>
#include <fstream>
#include <kikimora/diffs.hpp>

namespace kikimora{
    using namespace std;
    class ConfPackage{
        private:
            string src_pkg_path;
            string dst_pkg_path;
            string file_list;
        public:
            ConfPackage(
                 const char *src_pkg_path, \
                 const char *dst_pkg_path, \
                 const char *file_list);
            int ScanSrcFiles(vector<string> *src_files);
    };
    ConfPackage::ConfPackage(
            const char *src_pkg_path, \
            const char *dst_pkg_path, \
            const char *file_list){
        this->src_pkg_path = src_pkg_path;
        this->dst_pkg_path = dst_pkg_path;
        this->file_list = file_list;
    }

    int ConfPackage::ScanSrcFiles(vector<string> *src_files){
        DIR *pDIR;
        struct dirent *entry;
        pDIR = opendir(this->src_pkg_path.c_str());
        if (pDIR == NULL) { return 1; }
        do{
            entry = readdir(pDIR);
            if (entry == NULL) { break; }
            if ( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 ) {
                //char *abspath = "";
                //strcat(abspath, this->src_pkg_path);
                //strcat(abspath, entry->d_name);
                //cout << abspath << endl;
                src_files->push_back(entry->d_name);
            }
        }while(true);
        return 0;
    }

    template<typename CONF_TYPE>
        int PatchDiffs(const CONF_TYPE* conf_file, vector<Diff*> diffs){
            vector<Diff*>::iterator it = diffs.begin();
            for(;it!=diffs.end();it++){
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
        int SaveTofile(const CONF_TYPE* conf_file, const char* save_to_dest){
            return conf_file->save(save_to_dest);
        }

} // namespace kikmora
#endif /* CONF_PACKAGE_HPP */
