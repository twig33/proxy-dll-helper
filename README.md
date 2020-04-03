# proxy-dll-helper
## Dependencies
  This application depends on win32 api and the **dumpbin utility**  
  which is provided with the msvc compiler
## Usage
```
proxy-dll-helper.exe [DLL FILE Name/Path]  
   Optional flags: -p [PROXY DLL Name/path]  
                        If omitted, the proxy dll will have
                        the input dll's name
                        with _proxy appended.  
                   -h Display this Usage screen and exit  
```
The proxy dll files will be generated in the running directory.  
Note that the generated header file is required only for the msvc compiler, on mingw-w64 it breaks the compilation of the proxy dll.
## TODO
  Make it crossplatform  
  Remove the dependency on dumpbin
