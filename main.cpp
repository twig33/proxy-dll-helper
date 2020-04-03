#include <iostream>
#include <sstream>
#include <fstream>
#include <ios>
#include <windows.h> //for CreateProcess

const char* dumpbinOUTFileName = "temp.txt";

void DisplayUsage(){
	std::cout << "Usage: proxy-dll-helper.exe [DLL FILE Name/Path]\n"
			  << "Optional flags: -p [PROXY DLL Name/path]\n"
			  << "                   If omitted, the proxy dll will have\n"
			  << "                   the same name as the dll file\n"
			  << "                   with \"_proxy\" appended.\n"
			  << "                -h Display this Usage screen and exit\n\n"
			  << "The proxy dll files will be generated in the running directory\n";
}

int ParseArguments (int argc, char** argv, std::string* dllPath, bool* proxyDllNameSupplied, std::string* proxyDllName){ //returns 0 on success -1 to signal exit
	if (argc < 2){
		DisplayUsage();
		return -1;
	}
	*dllPath = std::string(argv[1]);
	for (int i = 2; i < argc; ++i){
		if (argv[i] == "-h"){
			DisplayUsage();
			return -1;
		}
		else if (argv[i] == "-p"){
			if (i+1 < argc){
				*proxyDllNameSupplied = true;
				*proxyDllName = argv[i+1];
				++i;
			}
			else {
				DisplayUsage();
				return -1;
			}
		}
	}
	return 0;
}

std::string FilenameFromPath(std::string path){
	std::size_t pos = 0, temp;
	if ((temp = path.find_last_of("/")) && temp != std::string::npos){
		pos = temp;
	}
	else if ((temp = path.find_last_of("\\")) && temp != std::string::npos){
		pos = temp;
	}
	return path.substr(pos, path.find_last_of(".") - pos);
}

void ParseDumpbinExportLine(std::string in, std::string* ordinal, std::string* name){
	/*  line format:
		ordinal hint RVA name
	OR  ordinal hint name (forwarded to ...)  */
	std::string scratch;
	std::stringstream stream(in);
	stream >> *ordinal;
	stream >> scratch; //skip hint
	if (in.find("(forwarded to ") != std::string::npos){ //If the export is forwarded
		stream >> *name; //then there is no RVA
	}
	else {
		stream >> scratch >> *name; //skip RVA
	}
}

int main(int argc, char** argv){
	std::string dllPath, dllFilename, proxyDllName;
	bool proxyDllNameSupplied;
	if (ParseArguments (argc, argv, &dllPath, &proxyDllNameSupplied, &proxyDllName) == -1){
		return 0;
	}
	dllFilename = FilenameFromPath(dllPath);
	if (!proxyDllNameSupplied){
		proxyDllName = dllFilename + "_proxy";
	}
	//Run dumpbin
	std::string command = "dumpbin.exe /EXPORTS " + dllPath + " /OUT:" + dumpbinOUTFileName;
	PROCESS_INFORMATION processInformation = {0};
	STARTUPINFO startupInfo                = {0};
	startupInfo.cb                         = sizeof(startupInfo);
	int nStrBuffer                         = command.length();
	char* writable_command = new char[command.size() + 1];
	std::copy(command.begin(), command.end(), writable_command);
	writable_command[command.size()] = '\0';
    BOOL result = CreateProcess(NULL, writable_command, 
								NULL, NULL, FALSE, 
								NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
								NULL, NULL, &startupInfo, &processInformation);
	WaitForSingleObject( processInformation.hProcess, INFINITE );
	//Dumpbin finished
	std::ofstream outputDefStream (proxyDllName + ".def"), outputHStream (proxyDllName + ".h");
	if (outputDefStream.fail() || outputHStream.fail()){
		std::cout << "Couldn't open one of the output files for writing.\n";
		return -1;
	}
	std::ifstream dumpbinResult (dumpbinOUTFileName);
	outputDefStream << "LIBRARY " << proxyDllName << '\n';
	outputDefStream << "EXPORTS\n";
	outputHStream << "#define UNKNOWN_IMPL(x) __declspec(dllexport) void __cdecl x () { }\n\n"; //credit to OldCigarettes (api wrapper pack source)
	std::string buffer;
	while (std::getline(dumpbinResult, buffer) && (buffer.find("ordinal hint RVA") == std::string::npos));
	std::getline(dumpbinResult, buffer); // skip a line
	std::string ordinal, name;
	std::stringstream stream;
	while (std::getline(dumpbinResult, buffer) && (buffer != "")){
		ParseDumpbinExportLine(buffer, &ordinal, &name);
		outputDefStream << "	" << name << "=" << dllFilename << '.' << name << " @" << ordinal << '\n';
		outputHStream << "UNKNOWN_IMPL(" << name << ");\n";
	}
	dumpbinResult.close();
	outputDefStream.close();
	outputHStream.close();
	return 0;
}