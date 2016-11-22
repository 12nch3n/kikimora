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
    return 0;
}
