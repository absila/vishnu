/**
 * \file reconnect_m.cpp
 * This file defines the VISHNU reconnect command
 * \author Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 */

#include <stdlib.h>                     // for getenv
#include <unistd.h>                     // for getppid
#include <boost/smart_ptr/shared_ptr.hpp>  // for shared_ptr
#include <exception>                    // for exception
#include <string>                       // for string, allocator, etc

#include "ListUsers.hpp"                // for ListUsers
#include "Options.hpp"                  // for Options, ::CONFIG, ::HIDDEN
#include "Session.hpp"                  // for Session
#include "UMS_DataFactory.hpp"          // for UMS_DataFactory
#include "User.hpp"                     // for User
#include "UserException.hpp"
#include "VishnuException.hpp"          // for VishnuException
#include "api_ums.hpp"                  // for reconnect, vishnuInitialize
#include "cliUtil.hpp"                  // for errorUsage, helpUsage, etc
#include "connectUtils.hpp"             // for makeConnectOptions
#include "daemon_cleaner.hpp"           // for cleaner
#include "ecorecpp/mapping/EList.hpp"   // for EList
#include "sessionUtils.hpp"             // for storeLastSession

using namespace std;
using namespace vishnu;


int
main (int ac, char* av[]) {
  string sessionId;
  string configFile;
  string userId;
  string password;

  // FIXME: this code does not use Options but instead parses av... :-(

  /*********** In parameters **********************/
  UMS_Data::ListUsers listUsers;
  UMS_Data::UMS_DataFactory_ptr ecoreFactory = UMS_Data::UMS_DataFactory::_instance();
  boost::shared_ptr<Options> opt=makeConnectOptions(av[0],userId,0, configFile,CONFIG);

  opt->add("sessionId,s",
           "represents the identifier of the session",
           HIDDEN,
           sessionId,1);

  opt->setPosition("sessionId", 1);


  opt->add("password,w","To give the password ",CONFIG,password );

  /********* Out parameters     *****************/
  UMS_Data::Session session;

  try {
    configFile = string(getenv("VISHNU_CONFIG_FILE"));
    if (ac<2) {
      helpUsage(*opt);
      return ERRCODE_CLI_ERROR_MISSING_PARAMETER;
    }

    sessionId = string(av[1]);

    /************** Call UMS reconnect service *******************************/
    cleaner(const_cast<char*>(configFile.c_str()), ac, av);// lauch the daemon cleaner if it is not already running

    /********* Get all the couples user/pwd ******************/
    int counter(2);
    while (counter < ac) {
      string tmp(av[counter]);
      if (counter+1>=ac) {
        helpUsage(*opt);
        return ERRCODE_CLI_ERROR_MISSING_PARAMETER;
      }
      UMS_Data::User_ptr user = ecoreFactory->createUser();
      user->setUserId(tmp);
      user->setPassword(string(av[counter+1]));
      listUsers.getUsers().push_back(user);
      counter += 2;
    }

    // initializing VISHNU
    if (vishnuInitialize(const_cast<char*>(configFile.c_str()), ac, av)) {
      errorUsage(av[0],communicationErrorMsg,EXECERROR);
      return  ERRCODE_CLI_ERROR_COMMUNICATION ;
    }

    reconnect(listUsers, sessionId, session);

    // store the session information
    storeLastSession(session,getppid());
  } catch(VishnuException& e){// catch all Vishnu runtime error
    std::string  msg = e.getMsg()+" ["+e.getMsgComp()+"]";
    errorUsage(av[0], msg,EXECERROR);
    return e.getMsgI() ;
  } catch(std::exception& e){//// catch all other command line runtime error
    errorUsage(av[0], e.what()) ;
    return ERRCODE_CLI_ERROR_RUNTIME;
  }

  return 0;
  // }}RELAX<CODEREDUCER>
}// end of main
