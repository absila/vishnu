#include <string>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <iostream>

#include "DIET_server.h"
#include "RemoteFile.hh"
#include "FileFactory.hh"
#include "File.hh"
#include "services.hh"
#include "UserServer.hpp"
using namespace std;
using namespace FMS_Data;

/// DIET profile construction.

diet_profile_desc_t* getInfosProfile() {
  diet_profile_desc_t* result;
  
  result = diet_profile_desc_alloc("FileGetInfos", 3, 3, 5);
  diet_generic_desc_set(diet_param_desc(result, 0), DIET_STRING, DIET_CHAR);
  diet_generic_desc_set(diet_param_desc(result, 1), DIET_STRING, DIET_CHAR);
  diet_generic_desc_set(diet_param_desc(result, 2), DIET_STRING, DIET_CHAR);
  diet_generic_desc_set(diet_param_desc(result, 3), DIET_PARAMSTRING, DIET_CHAR);
  diet_generic_desc_set(diet_param_desc(result, 4), DIET_STRING, DIET_CHAR);
  diet_generic_desc_set(diet_param_desc(result, 5), DIET_STRING, DIET_CHAR);
  
  
  
  /*
  diet_generic_desc_set(diet_param_desc(result, 6), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 7), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 8), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 9), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 10), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 11), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 12), DIET_SCALAR, DIET_LONGINT);
  diet_generic_desc_set(diet_param_desc(result, 13), DIET_SCALAR, DIET_INT);
  diet_generic_desc_set(diet_param_desc(result, 14), DIET_STRING, DIET_CHAR);
  */
  return result;
}

/* get information DIET callback function. Proceed to the group change using the
 client parameters. Returns an error message if something gone wrong. */
/* The function returns all the information about a file:
 *  - The local owner
 *  - The local group
 *  - The local uid & gid
 *  - The creation, modification and acces time.
 *  - The file type.
 */
int get_infos(diet_profile_t* profile) {
  char* path, * user, * host, *sessionKey, *fileStatSerialized=NULL;
  string localPath, localUser, userKey;
  char* errMsg = NULL;
  File* file;
  FileStat_ptr fileStat_ptr;


  diet_string_get(diet_parameter(profile, 0), &sessionKey, NULL);
  diet_string_get(diet_parameter(profile, 1), &path, NULL);
  diet_string_get(diet_parameter(profile, 2), &user, NULL);
  diet_paramstring_get(diet_parameter(profile, 3), &host, NULL);

  std::cout << "Service get info, sessionKey : " << sessionKey << " bien recu\n";
  std:: cout << "path: " << path << "\n";
  std::cout << "user: "  << user << "\n";
  std::cout << "host: "  << host<< "\n";

 // if (user!=NULL && path!=NULL) {
//    try {
 //     if (path[0]!='/'){
     //   localPath = users->getHome(user) + "/" + path;
   //   }
   //   else{
     //   localPath = path;
 //     localUser = users->getLocalID(user);
   //   userKey = users->getKey(user);
 //     }
 //   } catch (std::exception& err) {
      localUser = user;
      localPath = path;
   // }
 // }

  try {

    SessionServer sessionServer (sessionKey);

    // check the sessionKey

    sessionServer.check();
    
    std::string acLogin = UserServer(sessionServer).getUserAccountLogin(host);

    std::cout << "acLogin: " << acLogin << "\n";
 
    file = FileFactory::getFileServer(sessionServer,localPath, acLogin, userKey);

    if ( file->exists()) {

      *fileStat_ptr=file->getFileStat();
      std::cout << "apres le getFileStat reussi\n";
      const char* name = "solve_getInfos";
      ::ecorecpp::serializer::serializer _ser(name);
      fileStatSerialized = strdup(_ser.serialize(const_cast<FMS_Data::FileStat_ptr>(fileStat_ptr)).c_str());

  } else {
 throw std::runtime_error("this file does not exist");
  }

  } catch (std::exception& err) {
    errMsg = strdup(err.what());
    std::cout << " errMsg" << errMsg << "\n";
  fileStatSerialized=strdup(""); 
 }

  if (errMsg==NULL) errMsg = strdup("");
  

  diet_string_set(diet_parameter(profile, 4),fileStatSerialized, DIET_VOLATILE);
  diet_string_set(diet_parameter(profile, 5), errMsg, DIET_VOLATILE);
  return 0;
}
