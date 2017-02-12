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

#include "kikimora/diffs.hpp"
#include "kikimora/conf_package.hpp"
#include "docopt/docopt.h"
#include <vector>
#include <iostream>

static const char USAGE[] =
R"(Kikmora commands:
    [diff merge]        Merge the all configure diff records before the specified release,
                        feature & stage diffs would not be merged.

    [conf aggregate]    Generate the configuration files aggregated with the specified configure diff.
                        The conf diffs would be patched to conf file with order: <release>, <feature>, <stage>.
                        The conf diffs in the same tag would be patched as line number ascends.

    Usage:
      kikimora diff merge <release> [--diff=<df>]
      kikimora conf aggregate <release> <feature> <stage> <conf_root> [--diff=<df>] [--list=<cl>]

    Options:
      -h --help         Show this screen.
      --version         Show version.
      --diff=<df>       The diff file path [default: conf.diff].
      --list=<cl>       The conf file type specification list. [default: file_type.list]
      <release>         The release version like 0.0.0, reference to http://semver.org/.
      <feature>         The feature diffs need covered on <release>. If no feature needed, please use "None"
      <stage>           The stage diffs need covered on <release> + <feature>.
      <conf_root>       The conf files root path.
)";

int main(int argc, const char** argv)
{
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE, \
                                                  { argv + 1, argv + argc }, \
                                                  true,               // show help if requested
                                                  "Kikimora 0.1");  // version string
    if (args["diff"].asBool() && args["merge"].asBool()) {
        std::string release = args["<release>"].asString();
        std::string diff_file = args["--diff"].asString();
        kikimora::DiffsMerge(release.c_str(), diff_file.c_str());
    }else if (args["conf"].asBool() && args["aggregate"].asBool()){
        std::string release = args["<release>"].asString();
        std::string feature = "None" != args["<feature>"].asString() ? \
                               args["<feature>"].asString() : \
                               "";
        std::string stage = args["<stage>"].asString();
        std::string diff_file = args["--diff"].asString();
        std::string file_list = args["--list"].asString();
        map< uint32_t, vector<kikimora::Diff*> >  diffs_by_file = \
               kikimora::ArchieveDiffs(diff_file.c_str(), release.c_str(), stage.c_str(), feature.c_str());
        char conf_root_buff[1024];
        if ( NULL == realpath(args["<conf_root>"].asString().c_str(), conf_root_buff)){
            KK_LOG(KK_ERROR, "Could not locate the conf root path: %s", \
                    args["<conf_root>"].asString().c_str());
            exit(-1);
        }
        std::string conf_root(conf_root_buff);
        std::string dest_root = "" != feature ? \
                                 conf_root + "_" + release + "_" + stage + "_" + feature : \
                                 conf_root + "_" + release + "_" + stage;
        kikimora::ConfPackage conf_pack(conf_root.c_str(), dest_root.c_str(), file_list.c_str());
        conf_pack.ProcessFiles(diffs_by_file);
    }else{
        cout << USAGE << endl;
    }
    return 0;
}
