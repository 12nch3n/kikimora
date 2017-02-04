#include "docopt/docopt.h"
#include <iostream>

static const char USAGE[] =
R"(Kikmora commands:
    [diff merge]     Merge the all configure diff records before the specified release,
                     feature & stage diffs would not be merged.

    [conf generate]  Generate the configuration files migrated with the specified configure diff.
                     The conf diffs would be patched to conf file with order: <release>, <feature>, <stage>.
                     The conf diffs in the same tag would be patched as line number ascends.

    Usage:
      kikimora diff merge <release> [ --diff=<df>|--new-diff=<ndf> ]
      kikimora conf generate <release> <feature> <stage> [--diff=<df>]

    Options:
      -h --help     Show this screen.
      --version     Show version.
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

    for(auto const& arg : args) {
        std::cout << arg.first << ": " << arg.second << std::endl;
    }

    return 0;
}
