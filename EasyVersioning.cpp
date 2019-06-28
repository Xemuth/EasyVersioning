#include <Core/Core.h>
#include "EasyVersioning.h"
#include <fstream>
#include <cctype>
#include <chrono>
using namespace Upp;


EasyVersioning::EasyVersioning(Upp::String _pathNewSoftWare, Versioning* _target){
	pathNewSoftWare =_pathNewSoftWare;
	TargertedSoft =(Versioning*) _target;
}
EasyVersioning::EasyVersioning(){}

void EasyVersioning::CmdLineExecutor(){
	const Vector<String> &cmdline = CommandLine();
	Cout() << "getCount : " << cmdline.GetCount() <<"\n";
	for(int i = 0; i < cmdline.GetCount(); i++) {
		Cout() << "Commande : "  << i << " = " << cmdline[i] <<"\n";
		if( cmdline[i].Compare("--Outdated") == 0 ){
			if(isStringisANumber(cmdline[i+1])){
				int versionDudDemandeur = std::stoi(cmdline[i+1].ToStd());
				Cout() <<"Version du fichier : " << TargertedSoft->getVersioningVersion() <<"\n";
				if(versionDudDemandeur < TargertedSoft->getVersioningVersion()){
					Cout() << "Version outdater !" <<"\n";
					Exit(1); // On renvoie 1 si le client est outdate
				}else{
					Cout() << "Version a jour !" <<"\n";
					Exit(0); //On renvoie 0 si le client est à jour
				}
			}
		}
		else if( cmdline[i].Compare("--Version") == 0 ){
			std::ofstream myfile;
			myfile.open ("VERSION");
			myfile <<  std::to_string( TargertedSoft->getVersioningVersion() );
			myfile.close();
			Cout() << "Création du fichier VERSION !" <<"\n";
			exit(0);
		}
	}
}



bool EasyVersioning::isStringisANumber(Upp::String stringNumber){
	if (std::isdigit(stringNumber[0]) || (stringNumber.GetCount() > 1 && (stringNumber[0] == '+')))
    {
        for (int i = 1 ; i < stringNumber.GetCount(); ++i)
            if (!std::isdigit(stringNumber[i]))
                return false;

        return true;
    }
    return false;
}


void EasyVersioning::EnableTime(){ //Enable TimeCheck Before asking for update(good to use when .exe or VERSION file is far away from client and may take a long time to check)
	timerEnable = true;
}

void EasyVersioning::DisableTime(){ //Disable TimeCheck
	timerEnable = false;
}

void EasyVersioning::setTimeofEachUpdateinMinute(int timeInMinute){//Allow  you to set time in minute for timeCheck
	//30 Minutes = 180000000 milliseconde
	timerValue =  (timeInMinute *60)* 100000;
}

void EasyVersioning::CreateFileVersion(){ //Create a File named VERSION with the version value of .exe
	std::ofstream myfile;
	myfile.open ("VERSION");
	myfile <<  std::to_string( TargertedSoft->getVersioningVersion() );
	myfile.close();
	Cout() << "Création du fichier VERSION !" <<"\n";	
}

void EasyVersioning::EnableFileVersion(){ //Enable File Versionning instead of Cmd ask versionning
	fileVersionEnable=true;
}

void EasyVersioning::DisableFileVersion(){ //Diable File Versionning
	fileVersionEnable=false;
}


bool EasyVersioning::CheckForUpdate(){
	if(checkLegacy()){
		if(CheckDate()){
			int code =0;
			if(!fileVersionEnable){
				Upp::String path = pathNewSoftWare + "\\" + GetExeTitle() +".exe";
				Cout() << "Chemin sur lequel taper : " << path <<"\n";
				if(FileExists(path.ToStd().c_str())){
					try{
						STARTUPINFO si;
					    PROCESS_INFORMATION pi;
					    ZeroMemory( &si, sizeof(si) );
					    si.cb = sizeof(si);
					    ZeroMemory( &pi, sizeof(pi) );
					    
					    std::stringstream ss;
						ss <<  " --Outdated ";
						ss <<  std::to_string( TargertedSoft->getVersioningVersion());
						std::string name = ss.str();
					    char* cmd =(char*) name.c_str();
						    // Start the child process. */
						if( CreateProcess(path.ToStd().c_str(),   // No module name (use command line)
									       	cmd,        // Command line
									        NULL,           // Process handle not inheritable
									        NULL,           // Thread handle not inheritable
									        FALSE,          // Set handle inheritance to FALSE
									        0,              // No creation flags
									        NULL,           // Use parent's environment block
									        NULL,           // Use parent's starting directory 
									        &si,            // Pointer to STARTUPINFO structure
									        &pi )           // Pointer to PROCESS_INFORMATION structure
									    )
						{
							WaitForSingleObject(pi.hProcess,INFINITE);
							
							DWORD exit_code;
							GetExitCodeProcess(pi.hProcess, &exit_code);
							CloseHandle( pi.hProcess );
							CloseHandle( pi.hThread );
							code = exit_code;
						}
						Cout() << "Reponse du process : " << code <<"\n";
						if(code == 1){
							return true;
						}
					}catch(...){
						return false;
					}
					
				}
				return false;
			}else if(fileVersionEnable){
				Upp::String path = pathNewSoftWare + "\\VERSION";
				Cout() << "Chemin sur lequel taper : " << path <<"\n";
				if(FileExists(path.ToStd().c_str())){
					try{
						FileIn in(path);
						if(!in){
							Cout() << "fichier VERSION inexistant"<<"\n";
							return false;
						}
						String data ="";
						while(!in.IsEof())
							data << in.GetLine();
						
						in.Close();
						code = std::stoi(data.ToStd().c_str());
						Cout() << "Reponse du process : " << code <<"\n";
						if(code > TargertedSoft->getVersioningVersion()){
							return true;
						}
					}catch(...){
						return false;
					}
				}
			}
		}
		else{
			Cout() << "La dernière mise à jour était il y'a moins de "<<  (timerValue/100000)/60  << " min !"<<"\n";
			return false;	
		}
	}
	return false;
}


bool EasyVersioning::checkLegacy(){
	if(GetExeFolder().Compare(this->pathNewSoftWare) ==0){
		Cout() << "Vous utilisez l'exe serveur"<<"\n";
		return false;
	}
	if(!FileExists(GetExeFolder() +"\\versioning.exe")){
		Cout() << "Vous ne possédé pas Versionning.exe"<<"\n";
		return false;
	}
	return true;
}

bool EasyVersioning::CheckDate(){
	if(timerEnable){
		FileIn in("timer.tms");
		if(!in) {
				auto now = std::chrono::system_clock::now();
				std::chrono::time_point<std::chrono::system_clock> epoch;
				std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch);
				
				std::ofstream myfile;
				myfile.open ("timer.tms");
				myfile <<  std::to_string(ms.count());
				myfile.close();
				return true;
			}
			in.Seek(0);
			String data="";
			while(!in.IsEof())
				data << in.GetLine();
			
			auto now = std::chrono::system_clock::now();
			std::chrono::time_point<std::chrono::system_clock> epoch;
			std::chrono::microseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - epoch);
			Cout() << "Time between last update in ms : "  << (ms.count() - std::stod(data.ToStd())) <<"\n";
			if ((ms.count() - std::stod(data.ToStd())) > timerValue){ //Dégueulasse mais bon 'lol';
				std::ofstream myfile;
				myfile.open ("timer.tms");
				myfile <<  std::to_string(ms.count());
				myfile.close();
				return true;
			}
			return false;
	}else{
		return true;	
	}
}

void EasyVersioning::Update(){
	CmdLineExecutor(); 
	if(CheckForUpdate()){
		String exePath = GetExeFilePath();
		String exeToGet =pathNewSoftWare +"\\"+GetExeTitle() +".exe";
		Cout() <<"Exe to Get  : " << exeToGet <<"\n";
		if( FileExists(exeToGet)){
			STARTUPINFO lpStartupInfo;
		    PROCESS_INFORMATION lpProcessInfo;
		
		    ZeroMemory( &lpStartupInfo, sizeof( lpStartupInfo ) );
		    lpStartupInfo.cb = sizeof( lpStartupInfo );
		    ZeroMemory( &lpProcessInfo, sizeof( lpProcessInfo ) );
			exePath.Replace(" ","@");
			exeToGet.Replace(" ","@");
			std::stringstream ss;
			ss <<  exePath.ToStd();
			ss << ";";
			ss << exeToGet.ToStd();
			std::string name = ss.str();
			Cout() << "cmd : "  << String(ss.str()) <<"\n";
		    char* cmd =(char*) name.c_str();
		    std::string path = Upp::String(GetExeFolder() +"\\versioning.exe").ToStd();
		    BOOL result = CreateProcess( path.c_str() ,
										cmd,
										NULL,           // Process handle not inheritable
										NULL,           // Thread handle not inheritable
										FALSE,          // Set handle inheritance to FALSE
										0,              // No creation flags
										NULL,           // Use parent's environment block
										NULL,           // Use parent's starting directory
										&lpStartupInfo,
										&lpProcessInfo);
		    Exit(0);
		}
	}
}
	
int Versioning::getVersioningVersion(){
	Cout() << "GetVersion: " +String( std::to_string(Versionning_VERSION)) << "\n";
	return Versionning_VERSION;
}
void Versioning::setVersioningVersion(int _version){
	Versionning_VERSION	= _version;
	Cout() << "SetVersion: " + String(std::to_string(Versionning_VERSION)) << "\n";
}
/*
//-----------------------------------------------------------------------------
// converts x:\\folder -> \\\\server\\share\\folder
Upp::String EasyVersioning::ConvertLocalPathToUNC(Upp::String szFilePath )
{
  std::string strUNC="";
  // get size of the remote name buffer
  DWORD dwBufferSize = 0;
  char szBuff[2];
  if (::WNetGetUniversalName(szFilePath.ToStd().c_str(), UNIVERSAL_NAME_INFO_LEVEL, szBuff, &dwBufferSize) == ERROR_MORE_DATA)
  {
    // get remote name of the share
    char* buf = new char[dwBufferSize];
    UNIVERSAL_NAME_INFO* puni = (UNIVERSAL_NAME_INFO*) buf;

    if (::WNetGetUniversalName(szFilePath.ToStd().c_str(), UNIVERSAL_NAME_INFO_LEVEL, buf, &dwBufferSize) == NO_ERROR)
    {
      strUNC = puni->lpUniversalName;
      delete [] buf;
      return String(strUNC);
    }
    delete [] buf;
  }

  return szFilePath;
}

//-----------------------------------------------------------------------------
// converts \\\\server\\share\\folder -> x:\\folder
Upp::String EasyVersioning::ConvertUNCToLocalPath(Upp::String strUNC)
{
	std::string strLocalPath ="";
 // std::vector< std::string > vecTokens;
  Vector<String>   vecTokens = Split(strUNC,"\\");
 

  if (vecTokens.GetCount() < 4)
    return strUNC;

  // we need wchar for NetShareGetInfo()
  Upp::WString strShare = (WString)vecTokens[3].ToWString();
  
 // std::wstring strShare(vecTokens[3].GetCount(), L' ');
  //std::copy(vecTokens[3].ToStd().begin(), vecTokens[3].ToStd().end(), strShare.begin());

  PSHARE_INFO_502  BufPtr;
  NET_API_STATUS   res;
  if ((res = NetShareGetInfo(NULL, const_cast<LPWSTR>(strShare.ToStd().c_str()), 502, (LPBYTE*) &BufPtr)) == ERROR_SUCCESS)
  {
    // print the retrieved data.
    _RPTF3(_CRT_WARN, _T("%ls\t%ls\t%u\n"), BufPtr->shi502_netname, BufPtr->shi502_path, BufPtr->shi502_current_uses);

    std::wstring strPath(BufPtr->shi502_path);
    strLocalPath.assign(strPath.begin(), strPath.end());

    // build local path
    for (size_t i = 4; i < vecTokens.size(); ++i)
    {
       
      if (!String(strLocalPath).EndsWith("\\"))
        strLocalPath += "\\";
      strLocalPath += vecTokens[i];
    }

    // Validate the value of the shi502_security_descriptor member.
    if (IsValidSecurityDescriptor(BufPtr->shi502_security_descriptor))
      _RPTF0(_CRT_WARN, _T("It has a valid Security Descriptor.\n"));
    else
      _RPTF0(_CRT_WARN, _T("It does not have a valid Security Descriptor.\n"));

    // Free the allocated memory.
    NetApiBufferFree(BufPtr);
  }
  else
    return strUNC;

  return String(strLocalPath);
}
*/