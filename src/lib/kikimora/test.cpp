/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/30/2016 12:35:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#define KK_NOTICE 0
#define KK_WARNING 1
#define KK_ERROR 2
#define KK_LOG(LEVEL, fmt, ...)\
        do {\
                if (KK_NOTICE == LEVEL)\
                    fprintf(stdout, "NOTICE:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
                else if (KK_WARNING == LEVEL)\
                    fprintf(stdout, "WARNING:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
                else if (KK_ERROR == LEVEL)\
                    fprintf(stdout, "ERROR:[%s]:[%d]:" fmt, __FILE__, __LINE__ , ##__VA_ARGS__);\
            } while(0)

#include <stdlib.h>
#include <string>
#include <diffs.hpp>
using namespace kikimora;
int main()
{
    string diff_file="test.diff";
    string base_conf_dir="/";
    Diffs diffs(diff_file.c_str(), base_conf_dir.c_str());
    diffs.MergeCommon("6.11");
    vector<Diff*> x=diffs.PickDiffs4Cate("6.11", "staging");
    map<uint32_t, Diff*> y = UniqByDiffPoint(x);
    map< uint32_t, vector<Diff*> > z = GroupByFile(y);
    cout<<z.size()<<endl;
    for(map< uint32_t, vector<Diff*> >::iterator it=z.begin(); it !=z.end(); it++){
        cout<<"FILE:"<<it->first<<endl;
        for(vector<Diff*>::iterator i=it->second.begin(); i!=it->second.end();i++){
            cout<<(*i)->diff_node<<"\t"<<(*i)->node_content<<endl;
        }
    }
    return 0;
}
