# KIKIMORA

kikimora (кики́мора: a house spirit in Eastern mythology) is a tool to manage your application configuration changes by version, featrue & catagory.

```
Kikmora commands:
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
```


## SUPPORT FILE TYPE
|*type*| *status*|
|------|---------|
| JSON | Support |
| XML  |  Planed |
| YAML |  Planed |
| INI  |  Planed |

## DIFF SPECIFICATIONS
We support server operations on a exactly diff point:
### ADD
```ADD ${conf_file} ${diff_node} ${node_content}```
It means add ${node\_content} into ${diff\_node} for ${conf\_file}.
${diff\_node} is path of the node.

### UPDATE
```UPDATE ${conf_file} ${diff_node} ${node_content}```
It means update ${node\_content} into ${diff\_node} for ${conf\_file}.
${diff\_node} is path of the node.

### REPLACE
```REPLACE ${conf_file} ${pattern} ${content}```
It means replace the ${pattern} with ${content} for ${conf\_file}.

### DELETE
```DELETE ${conf_file} ${diff_node} ```
It means delete ${diff\_node} for ${conf\_file}.
${diff\_node} is path of the node.

## DIFFS MERGE
Before your diff file patched into the configurations. They would be sorted and merged, the same diff point would be combined and the latest change would be kept.
The Mergeration would follow this rules: **\<version\> + \<feature\> + {catagory}**

* Lastest version first:  the diffs in <1.0.1> would cover the diffs in <1.0.0>
* Feature would cover on version base: <1.0.0-featue> would cover <1.0.0>
* Catagory would cover on the version base + Feature
* The diffs in the same tag, the larger line_no would cover smaller ones.


