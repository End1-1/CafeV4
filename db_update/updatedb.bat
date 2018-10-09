cd c:\qt\projects\cafev4\db_update
set isql=c:\dev\firebird\bin\isql.exe
for /F "eol=; tokens=1-3 delims=," %%i  in (db.txt) do (
%isql% %%i -u %%j -p %%k -in sql.txt
)
pause