echo off

set "program_file_directory=x64\Debug"

echo[


cd "%cd%\%program_file_directory%"

set "json_file=%~dp0JSON_test_files\JSON_test_0.json"

echo Contents of a JSON file being tested (JSON_test_0.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"

echo[
echo[
echo Calling command v.c
JB_test.exe "%json_file%" "v.c"

echo[
echo[
echo Calling command v
JB_test.exe "%json_file%" "v"

echo[
echo[
echo Calling command #$%%\*"
JB_test.exe "%json_file%" "#$%%\*\""

echo[
echo[
echo Calling command #$%%\*"[#$%%\*"[1]]
JB_test.exe "%json_file%" "#$%%\*\"[#$%%\*\"[1]]"

echo[
echo[
echo Calling command g[2].p
JB_test.exe "%json_file%" "g[2].p"

echo[
echo[
echo Calling command max(#$%%\*")
JB_test.exe "%json_file%" "max(#$%%\*\")"

echo[
echo[
echo Calling command min(#$%%\*"[3], -20000)
JB_test.exe "%json_file%" "min(#$%%\*\"[3],-20000)"

echo[
echo[
echo Calling command min(#$%%\*"[3],-20000\.23242) //must escape '.' in fp literals that are part of command strings
JB_test.exe "%json_file%" "min(#$%%\*\"[3],-20000\.23242)"
:: if one wishes to pass a floating point literal to the program, one must escape it with character \ like 
:: in the previous example

echo[
echo[
echo Calling command g[5/2]
JB_test.exe "%json_file%" "g[5/2]"

echo[
echo[
echo Calling command min(d[0]*d[1]+d[2], d[2]+ d[0]*2 )+ sum(d)*size(v.c)
JB_test.exe "%json_file%" "min(d[0]*d[1]+d[2], d[2]+ d[0]*2 )+ sum(d)*size(v.c)"

echo[
echo[
echo Calling command size(v)
JB_test.exe "%json_file%" "size(v)"

echo[
echo[
echo Calling command size(v.c)
JB_test.exe "%json_file%" "size(v.c)"

echo[
echo[
echo Calling command min(d[0]*d[1]+d[2], d[2]+ d[0]*2 )
JB_test.exe "%json_file%" "min(d[0]*d[1]+d[2], d[2]+ d[0]*2 )"

echo[
echo[
echo Calling command sum(d)
JB_test.exe "%json_file%" "sum(d)"

echo[
echo[
echo Calling command sum(1,2,3,4,-11.23)
JB_test.exe "%json_file%" "sum(1,2,3,4,-11\.23)"

echo[
echo[
echo Calling command sum(23 + 5/5,100)
JB_test.exe "%json_file%" "sum(23 + 5/5,100)"

echo[
echo[
echo[
echo TESTING erroneous commands:

echo[
echo Calling command abcd
JB_test.exe "%json_file%" "abcd"

echo[
echo[
echo Calling command #$%%\*"[3000]
JB_test.exe "%json_file%" "#$%%\*\"[3000]"

echo[
echo[
echo Calling command max(v)
JB_test.exe "%json_file%" "max(v)"

echo[
echo[
echo Calling command max(g)
JB_test.exe "%json_file%" "max(g)"

echo[
echo[
echo Calling command max(#$%%\*", g)
JB_test.exe "%json_file%" "max(#$%%\*\", g)"

echo[
echo[
echo Calling command max(g
JB_test.exe "%json_file%" "max(g"

echo[
echo[
echo Calling command g[2
JB_test.exe "%json_file%" "g[2"

echo[
echo[
echo Calling command g[a]
JB_test.exe "%json_file%" "g[a]"

echo[
echo[
echo Calling command a.b
JB_test.exe "%json_file%" "a.b"

echo[
echo[
echo Calling command size(v,g)
JB_test.exe "%json_file%" "size(v,g)"


echo[
echo[
echo Calling command size(c,g)
JB_test.exe "%json_file%" "size(c,g)"

echo[
echo[
echo Calling command 1/0
JB_test.exe "%json_file%" "1/0"

echo[
echo[
echo Calling command sum(g)
JB_test.exe "%json_file%" "sum(g)"

echo[
echo[
echo[
echo TESTING parsing ill-formed JSON files:

set "json_file=%~dp0JSON_test_files\JSON_test_1.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_1.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"





set "json_file=%~dp0JSON_test_files\JSON_test_2.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_2.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"





set "json_file=%~dp0JSON_test_files\JSON_test_3.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_3.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"




set "json_file=%~dp0JSON_test_files\JSON_test_4.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_4.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"





set "json_file=%~dp0JSON_test_files\JSON_test_5.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_5.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"




set "json_file=%~dp0JSON_test_files\JSON_test_6.json"

echo[
echo[
echo --------------------------------------------------------
echo Contents of a JSON file being tested (JSON_test_6.json):
echo[
type "%json_file%"

echo[
echo[
echo Calling command a
JB_test.exe "%json_file%" "a"



cd "%~dp0"