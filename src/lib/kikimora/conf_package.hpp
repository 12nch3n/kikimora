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
#include <kikimora/diffs.hpp>

namespace kikimora{
    using namespace std;
    class ConfPackage{
        private:
            string src_pkg_path;
            string dst_pkg_path;
            string diff_spec_path;
        public:
            ConfPackage(
                 const char *src_pkg_path, \
                 const char *dst_pkg_path, \
                 const char *diff_spec_path);
            int ScanSrcFiles(vector<string> *src_files);
    };
    ConfPackage::ConfPackage(
            const char *src_pkg_path, \
            const char *dst_pkg_path, \
            const char *diff_spec_path){
        this->src_pkg_path = src_pkg_path;
        this->dst_pkg_path = dst_pkg_path;
        this->diff_spec_path = diff_spec_path;
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

    template<class CONF_TYPE> class DiffPatcher{
        public:
            int UpdateConf(const CONF_TYPE* conf_file, vector<Diff*> diffs);
            int SaveTofile(const CONF_TYPE* conf_file, const char* save_to_dest);
    };

    template<class CONF_TYPE>
        int UpdateConf(const CONF_TYPE* conf_file, vector<Diff*> diffs){
            return 0;
        }
    template<class CONF_TYPE>
        int SaveTofile(const CONF_TYPE* conf_file, const char* save_to_dest){
            return 0;
        }

} // namespace kikmora
#endif /* CONF_PACKAGE_HPP */
