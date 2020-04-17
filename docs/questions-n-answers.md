We collect typical questions by Artery novices on this page, along with suggested answers and solutions.
Don't be shy and send us your questions (possibly along with matching answers) so we can include them here!

## INET dependency does not compile because of osgEarth

INET comes with cool-looking visualization tools relying on osgEarth.
However, even if you have osgEarth installed on your system, its version needs to be supported by INET.
Incompatible versions may end in compiler errors as in [issue ticket #101](https://github.com/riebl/artery/issues/101).

Although this is not strictly an issue caused by Artery, you may experience this problem with Artery for the first time if you have never used the INET framework before.
Since Artery does not use osgEarth explicitly and it is only an optional dependency of the INET framework, you may simply turn it off.

You can turn off the osgEarth in OMNeT++ by setting `WTIH_OSGEARTH=no` in *configure.user* found in OMNeT++'s root directory.
Remember to recompile OMNeT++ after editing its *configure.user* file.
Otherwise, this change does not come into effect.

