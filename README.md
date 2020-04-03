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
The application generates a .def file and a header file.
The .def file forwards every original dll's export function to the original dll.  
The header file is needed when compiling using msvc (todo: note what msvc complained about in my case) 
Note that the generated header file is needed **only** for the msvc compiler, on mingw-w64 it breaks the compilation of the proxy dll.
The files will be generated in the running directory.  
## TODO
  Make it crossplatform  
  Remove the dependency on dumpbin
