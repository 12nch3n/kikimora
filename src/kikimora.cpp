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
#include "docopt/docopt.h"
#include <iostream>

static const char USAGE[] =
R"(Kikmora commands:
    [diff merge]     Merge the all configure diff records before the specified release,
                     feature & stage diffs would not be merged.

    [conf aggregate] Generate the configuration files aggregated with the specified configure diff.
                     The conf diffs would be patched to conf file with order: <release>, <feature>, <stage>.
                     The conf diffs in the same tag would be patched as line number ascends.

    Usage:
      kikimora diff merge <release> [--diff=<df>]
      kikimora conf aggregate <release> <feature> <stage> [--diff=<df>]

    Options:
      -h --help     Show this screen.
      --version     Show version.
      --diff=<df>   The diff file path [default: conf.diff].
      <release>     The release version like 0.0.0, reference to http://semver.org/.
      <feature>     The feature diffs need covered on <release>.
      <stage>       The stage diffs need covered on <release> + <feature>.
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
    }
    return 0;
}
