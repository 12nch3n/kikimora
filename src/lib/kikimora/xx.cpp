/*
 * =====================================================================================
 *
 *       Filename:  xx.cpp
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  10/30/2016 18:44:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <regex>
#include <fstream>
using namespace std;

struct X{
    const char* test;
};

typedef struct VERSION{ //reference to http://semver.org/
    uint16_t MAJOR;
    uint16_t MINOR;
    uint16_t PATCH;
    char* LABEL; // 8 charactor length;
    bool operator < (const VERSION& other) const {
        return (MAJOR*10000+MINOR)*10000+PATCH < (other.MAJOR*10000+other.MINOR)*10000+other.PATCH;
    }
    bool operator > (const VERSION& other) const {
        return (MAJOR*10000+MINOR)*10000+PATCH > (other.MAJOR*10000+other.MINOR)*10000+other.PATCH;
    }
    bool operator == (const VERSION& other) const {
        return (MAJOR*10000+MINOR)*10000+PATCH == (other.MAJOR*10000+other.MINOR)*10000+other.PATCH;
    }
} VERSION;
#ifndef LABEL_LEN
#define LABEL_LEN 9
#endif
VERSION* NewVersion(const char* version_str){
    VERSION* ret=(VERSION*)malloc(sizeof(VERSION));
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


int main(){
    vector<X*> s;
    int i = 0;
    const char* src="test";
    while(i<10){
        string tmp(src);
        struct X* t = (struct X*)malloc(sizeof(struct X));
        t->test = tmp.substr(1, strlen(src)-2).c_str();
        s.push_back(t);
        i++;
    }
    //vector<X*>::iterator iv;
    //for(iv=s.begin();iv!=s.end();iv++){
    //    cout<<(*iv)->test<<endl;
    //}
    //string test1("UPDATE ads.conf server.id 1234\t567 89");
    //regex rgx("(UPDATE|ADD)\\s+([^\\s]*?)\\s+([^\\s]*?)\\s+(.*)");
    //cmatch match;
    //if ( regex_match(test1.c_str(), match, rgx) ) {
    //    cout<< "|" << match[1] << "|\t|" << match[2] << "|\t|" << match[3] << "|\t|" << match[4] << "|" << endl;
    //}
    VERSION* test1=NewVersion("6.11");
    VERSION* test2=NewVersion("6.12");
    VERSION* test3=NewVersion("6.12.1-test");
    VERSION* test4=NewVersion("6.12.1");
    cout<<test1->MAJOR<<"\t"<<test1->MINOR<<"\t"<<test1->PATCH<<"\t"<<test1->LABEL<<endl;
    cout<<test2->MAJOR<<"\t"<<test2->MINOR<<"\t"<<test2->PATCH<<"\t"<<test2->LABEL<<endl;
    cout<<test3->MAJOR<<"\t"<<test3->MINOR<<"\t"<<test3->PATCH<<"\t"<<test3->LABEL<<endl;
    cout<<test4->MAJOR<<"\t"<<test4->MINOR<<"\t"<<test4->PATCH<<"\t"<<test4->LABEL<<endl;
    cout<<(*test1>*test2)<<endl;
    cout<<(*test1<*test2)<<endl;
    cout<<(*test2==*test4)<<endl;
    string path("conf.server[0].id");
    typedef struct xnode{
        uint16_t index;
        string name;
    } XNode;
    XNode a;
    a.name="conf";
    a.index=0;
    regex node_pattern("(\\w+)\\[(\\d+)\\]|(\\w+)");
    cmatch match;
    regex_match("server[1]", match, node_pattern);
    cout<<match[0]<<"\t";
    cout<<match[1]<<"\t";
    cout<<match[2]<<"\t";
    cout<<match[3]<<endl;
    ifstream file("test.diff");
    string temp((std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>() ));
    cout<<temp<<endl;
    return 0;
}
