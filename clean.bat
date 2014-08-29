@echo off::删除不上传文件
rd .\_Build /s /q
rd .\_Bin\Debug /s /q
rd .\_Bin\Release /s /q
del .\_Bin\*.dmf

