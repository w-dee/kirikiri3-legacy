@echo off
echo risse.y :
perl CR.pl risse.y
bison -dv risse.y
perl postbison.pl risse.tab.c
perl wrapRissens.pl risse.tab.h
echo copying ...
echo rissepp.y :
perl CR.pl rissepp.y
bison -dv --name-prefix=pp rissepp.y
perl postbison.pl rissepp.tab.c
perl wrapRissens.pl rissepp.tab.h
echo copying ...
copy risse.tab.c ..\risse.tab.cpp
copy risse.tab.h ..\risse.tab.h
copy rissepp.tab.c ..\rissepp.tab.cpp
copy rissepp.tab.h ..\rissepp.tab.h
echo rissedate.y :
perl CR.pl rissedate.y
bison -dv --name-prefix=dp rissedate.y
perl postbison.pl rissedate.tab.c
perl wrapRissens.pl rissedate.tab.h
echo copying ...
copy rissedate.tab.c ..\rissedate.tab.cpp
copy rissedate.tab.h ..\rissedate.tab.h
call gen_wordtable.bat
pause
