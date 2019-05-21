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
			
			bool isStringisANumber(Upp::String stringNumber);
			
		//	Upp::String ConvertLocalPathToUNC(Upp::String szFilePath );
		//	Upp::String ConvertUNCToLocalPath(Upp::String strUNC);
			
		public:
		EasyVersioning();
		EasyVersioning(Upp::String _pathDirNewSoftWare, void* _target); //Path must be the path to the directory contening new Soft and VERSION file contening number of version
																		//Target represent
																		//object inheritted
																		//from Versioning 
																		
	    
		bool CheckForUpdate();
		void Update(bool force =false);
		void isOutDated(); //Check and Return if program on entry is outdated or not
	};
	
	class Versioning{
		private: 
			int Versionning_VERSION =0;
		public:
			int getVersioningVersion();
			void setVersioningVersion(int _version);
	};

#endif
