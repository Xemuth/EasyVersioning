#ifndef _EasyVersioning_EasyVersioning_h_
#define _EasyVersioning_EasyVersioning_h_
#include <Core/Core.h>
#include <windows.h>
using namespace Upp;

class Versioning;
class EasyVersioning;

	class EasyVersioning{
	private:
		Versioning* TargertedSoft= nullptr; //Ptr to the actual software that need update
		Upp::String pathNewSoftWare="";
		
		bool timerEnable = true; //Timer mode allow to check for version at launch only every 'timerValue' (in milliseconde)
		bool fileVersionEnable = false; // boolean that handle if you want to use FileVersionning or not
		double timerValue = 180000000; //Time between each update launch (in milliseconde)

		bool checkLegacy();
		bool isStringisANumber(Upp::String stringNumber);
		bool CheckDate();//Check if time before next update is higher than defined
		bool CheckForUpdate();//Check for a possible update
		void CmdLineExecutor(); //Check is programm was launch with cmd line to check for update
			
	public:
			/*Parametrage*/
		void EnableTime(); //Enable TimeCheck Before asking for update(good to use when .exe or VERSION file is far away from client and may take a long time to check)
		void DisableTime(); //Disable TimeCheck
		void setTimeofEachUpdateinMinute(int timeInMinute=30);//Allow  you to set time in minute for timeCheck
		
		void CreateFileVersion(); //Create a File named VERSION with the version value of .exe
		void EnableFileVersion(); //Enable File Versionning instead of Cmd ask versionning
		void DisableFileVersion(); //Diable File Versionning

			/*Creation et mise à jour*/
		void Update(); //Launch An update of the programm
		
		EasyVersioning();
		EasyVersioning(Upp::String _pathDirNewSoftWare,Versioning* _target); //Path must be the path to the directory contening new Soft and VERSION file contening number of version
																		//Target represent
																		//object inheritted
																		//from Versioning
																		
	};
	
	class Versioning{ //This class must be  public inherritance of your main class that handle your programme
		private:
			int Versionning_VERSION = 0; //In that carry actual version of programme (DOnt forget to redifined it at begining of your code by using setVersionningVersion)
		public:
			int getVersioningVersion(); //Used to get actual version of programme
			void setVersioningVersion(int _version); //Used to set version 
	};

#endif
