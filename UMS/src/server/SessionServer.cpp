/**
* \file SessionServer.cpp
* \brief This file implements the Class which manipulates VISHNU sessions on server side.
* \author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
* \date 31/01/2011
*/

#include "SessionServer.hpp"
#include "CommandServer.hpp"
#include "DbFactory.hpp"
#include "boost/format.hpp"


using namespace boost::posix_time;

/**
 * \brief Constructor
 */
SessionServer::SessionServer()
  : mtimeout(DEFAULT_CONNECTION_TIMEOUT)
{
  DbFactory factory;
  msession.setSessionKey("");
  mdatabaseVishnu = factory.getDatabaseInstance();
}

/**
 * \brief Constructor
 * \param sessionKey The session key of the session
 * \param timeout Lenght of the connection before timeout
 */
SessionServer::SessionServer(std::string sessionKey, int timeout)
  : mtimeout(timeout)
{
  DbFactory factory;
  msession.setSessionKey(sessionKey);
  mdatabaseVishnu = factory.getDatabaseInstance();
}
/**
 * \brief Constructor
 * \param session The session data structure
 * \param timeout Lenght of the connection before timeout
 */
SessionServer::SessionServer(const UMS_Data::Session& session, int timeout)
  : msession(session),
    mtimeout(timeout)
{
  DbFactory factory;
  mdatabaseVishnu = factory.getDatabaseInstance();
}
/**
 * \brief Function to connect the session
 * \param user The object which manipulates user information
 * \param host The object which manipulates client machine information
 * \param connectOpt The options data structure for connection
 * \return raises an exception on error
 */
int
SessionServer::connectSession(UserServer user, MachineClientServer host, UMS_Data::ConnectOptions* connectOpt) {
  std::string numSubstituteUserId;
  std::string numUserIdToconnect;

  msession.setAuthenId(user.getData().getUserId());

  //if the user exist
  if (user.isAuthenticate()) {
    if (! connectOpt->getSubstituteUserId().empty()) {
      if (user.isAdmin()) {
        numSubstituteUserId = user.getAttribut("where "
                                               "userid='"+mdatabaseVishnu->escapeData(connectOpt->getSubstituteUserId())+"'");
        //If the user to substitute exist
        if (! user.getNumUserId(connectOpt->getSubstituteUserId()).empty()) {
          numUserIdToconnect = numSubstituteUserId;
          msession.setUserId(connectOpt->getSubstituteUserId());
        } else {
          throw UMSVishnuException (ERRCODE_UNKNOWN_USERID);
        }
      } else {
        throw UMSVishnuException (ERRCODE_NO_ADMIN);
      }
    }

    //if there is not a numSubstituteUserId
    if (numUserIdToconnect.empty()) {
      numUserIdToconnect = user.getAttribut("where userid='"+mdatabaseVishnu->escapeData(user.getData().getUserId())+"'"
                                            " and pwd='"+mdatabaseVishnu->escapeData(user.getData().getPassword())+"'");
      msession.setUserId(user.getData().getUserId());
    } //END if There is not a numSubstituteUserId

    generateSessionKey(user.getData().getUserId());
    generateSessionId(user.getData().getUserId());

    //To solve the connection mode
    solveConnectionMode(connectOpt);

    host.recordMachineClient();
    recordSessionServer(host.getId(), numUserIdToconnect);

  } else {
    throw UMSVishnuException(ERRCODE_UNKNOWN_USER);
  }
  return 0;
}//END: connectSession(UserServer, MachineClientServer, ConnectOptions*)
/**
 * \brief Function to reconnect the session
 * \param user The object which manipulates user information
 * \param host The object which manipulates client machine information
 * \param sessionId The session identifier
 * \return raises an exception on error
 */
int
SessionServer::reconnect(UserServer user, MachineClientServer host,
                         std::string sessionId) {
  msession.setSessionId(sessionId);

  //If the user exists
  if (user.isAuthenticate()) {
    int state = getState(true);
    if (state != -1) {
      //if the session is active
      if (state == vishnu::SESSION_ACTIVE) {
        int existSessionKey = 0;

        //if user is an admin
        if (user.isAdmin()) {
          existSessionKey = getSessionkey("", "", true);
        } else {
          existSessionKey = getSessionkey(host.getId(), user.getAttribut("where userid='"+mdatabaseVishnu->escapeData(user.getData().getUserId())+"'"
                                                                         " and pwd='"+mdatabaseVishnu->escapeData(user.getData().getPassword())+"'"));
        }
        //if there is no session key with the previous parameters
        if (existSessionKey == -1) {
          throw UMSVishnuException (ERRCODE_SESSIONKEY_NOT_FOUND);
        }
      } else {
        throw UMSVishnuException (ERRCODE_SESSIONKEY_EXPIRED);
      }
    } else {
      throw UMSVishnuException (ERRCODE_UNKNOWN_SESSION_ID);
    }
  } else {
    throw UMSVishnuException (ERRCODE_UNKNOWN_USER);
  }
  return 0;
}


/**
* \brief Function to close the session
* \return raises an exception on error
*/
int
SessionServer::close() {
  std::string extractClosePolicyCond = "";
  UserServer user = UserServer(SessionServer(msession.getSessionKey()));
  CommandServer commanderServer = CommandServer(SessionServer(msession.getSessionKey()));

  // initialize and check the user
  user.init();
  if (user.exist()) {
    int state = getState();
    if (state != vishnu::SESSION_CLOSED) {
      if (! commanderServer.isRunning()) {
        mdatabaseVishnu->process((boost::format("UPDATE vsession"
                                                " SET state=0"
                                                " WHERE sessionkey='%1%';")%mdatabaseVishnu->escapeData(msession.getSessionKey())).str());
        mdatabaseVishnu->process((boost::format("UPDATE vsession"
                                                " SET closure=CURRENT_TIMESTAMP"
                                                " WHERE sessionkey='%1%';")%mdatabaseVishnu->escapeData(msession.getSessionKey())).str());
      } else {
        extractClosePolicyCond = boost::str(boost::format(" WHERE sessionkey='%1%';")
                                            % mdatabaseVishnu->escapeData(msession.getSessionKey()));
        int closePolicy = vishnu::convertToInt(getAttribut(extractClosePolicyCond, "closepolicy"));
        if (closePolicy == vishnu::CLOSE_ON_DISCONNECT) {
          disconnetToTimeout();
        } else {
          throw  UMSVishnuException (ERRCODE_COMMAND_RUNNING);
        }
      }
    } else {
      throw UMSVishnuException (ERRCODE_SESSIONKEY_EXPIRED);
    }
  }
  return 0;
}

/**
 * \brief Destructor
 */
SessionServer::~SessionServer() {
}
/**
 * \brief Function to get the session information
 * \return  The session data structure
 */
UMS_Data::Session
SessionServer::getData() const {
  return msession;
}

/**
 * \brief Function to get session information from the database vishnu
 * \param condition The condition of the select request
 * \param attrname the name of the attribut to get
 * \return the value of the attribut or empty string if no results
 */
std::string
SessionServer::getAttribut(std::string condition, std::string attrname) {

  std::string sqlCommand("SELECT "+attrname+" FROM vsession "+condition);
  boost::scoped_ptr<DatabaseResult> result(mdatabaseVishnu->getResult(sqlCommand));
  return result->getFirstElement();
}

/**
 * \brief Function to save the date of the last connection
 * \return raises an exception on error
 */
int
SessionServer::saveConnection() {

  std::string sqlCommand = "UPDATE vsession SET lastconnect=CURRENT_TIMESTAMP"
                           " WHERE sessionkey='"+mdatabaseVishnu->escapeData(msession.getSessionKey())+"'";

  mdatabaseVishnu->process(sqlCommand);
  return 0;
}

/**
 * \brief Function to get the list of sessions with close on timeout mode to close
 * \return the list of results
 */
DatabaseResult*
SessionServer::getSessionToclosebyTimeout() {
  DatabaseResult* result;
  std::string sqlCommand;

  switch(mdatabaseVishnu->getDbType()) {
    case DbConfiguration::MYSQL:
      sqlCommand = "SELECT sessionkey from vsession where "
                   " unix_timestamp(CURRENT_TIMESTAMP) - unix_timestamp(lastconnect) > timeout and state=1 "
                   " and closepolicy=1";
      break;
    case DbConfiguration::POSTGRESQL:
      sqlCommand = "SELECT sessionkey from vsession where "
                   " EXTRACT( epoch FROM  CURRENT_TIMESTAMP ) - EXTRACT( epoch FROM lastconnect ) > timeout and state=1 "
                   " and closepolicy=1";
      break;
    case DbConfiguration::ORACLE:
      throw SystemException(ERRCODE_DBERR, "SessionServer::getSessionToclosebyTimeout: Oracle query not defined");
      break;
    default:
      break;
  }

  result = mdatabaseVishnu->getResult(sqlCommand);
  return result;

}

/**
 * \brief Function to make a complete checking of the session key
 * \return raises an exception on error
 */
int
SessionServer::check() {

  int retCode = -1;

  std::string sqlQuery = (boost::format("SELECT state, status, passwordstate"
                                        " FROM users, vsession "
                                        " WHERE users.numuserid = vsession.users_numuserid"
                                        " AND vsession.sessionkey='%1%'"
                                        " AND vsession.state<>%2%"
                                        )
                          % mdatabaseVishnu->escapeData(msession.getSessionKey())
                          % vishnu::STATUS_DELETED).str();
  boost::scoped_ptr<DatabaseResult> result(mdatabaseVishnu->getResult(sqlQuery));

  //If the session key exists
  if (result->getNbTuples() != 0) {
    std::vector<std::string> tmp = result->get(0);
    if (vishnu::convertToInt(tmp[0]) == vishnu::SESSION_ACTIVE) {
      if (vishnu::convertToInt(tmp[1]) == vishnu::STATUS_ACTIVE) {
        if (vishnu::convertToInt(tmp[2]) == vishnu::STATUS_ACTIVE) {
          retCode = 0;
        } else {
          throw UMSVishnuException (ERRCODE_TEMPORARY_PASSWORD);
        }
      } else {
        throw UMSVishnuException (ERRCODE_USER_LOCKED);
      }
    } else {
      throw  UMSVishnuException (ERRCODE_SESSIONKEY_EXPIRED);
    }
  } else {
    throw UMSVishnuException (ERRCODE_SESSIONKEY_NOT_FOUND);
  }
  return retCode;
}

/**
* \brief Function to finalize the service
* \param cmdDescription The description of the command
* \param cmdType The type of the command (UMS, TMS, FMS)
* \param cmdStatus The status of the command
* \param newVishnuObjectID the new vishnu object
* \return raises an exception on error
*/

int
SessionServer::finish(std::string cmdDescription,
                      vishnu::CmdType cmdType,
                      vishnu::CmdStatus cmdStatus,
                      const std::string& newVishnuObjectID,
                      bool checkSession) {

  //To save the date of the last connection
  if (checkSession) {
    check();
  }
  saveConnection();
  //To save the command
  CommandServer commandServer = CommandServer(cmdDescription, *this);
  commandServer.record(cmdType, cmdStatus, newVishnuObjectID);
  return 0;
}


/**
 * \brief Function to generate the session key
 * \param salt This string is used to perturb the algorithm
 * \return an encrypted message registered on the session data structure
 */
int
SessionServer::generateSessionKey(std::string salt) {

  std::string sessionKey;
  //the current time
  ptime now = microsec_clock::local_time();

  //for the md5 encryption
  std::string tmpSalt = "$1$" + salt + "$";

  //for SHA1-512 encryption by using the userId as a salt
  std::string globalSalt = "$6$"+std::string(crypt(salt.c_str(), tmpSalt.c_str())).substr(tmpSalt.size())+"$";

  //SHA1-512 encryption of the salt encrypted using the md5 and the current time as the clef
  sessionKey = std::string(crypt(to_simple_string(now).c_str(), globalSalt.c_str()));

  //To put a randomize number at the end to complicate the string encrypted
  sessionKey.append(vishnu::convertToString(vishnu::generateNumbers()));
  msession.setSessionKey(sessionKey.substr(globalSalt.size()));

  return 0;
}
/**
 * \brief Function to generate the session identifier
 * \param userId the userId of the owner of the session
 * \return record the the session id on the session data structure
 */
int
SessionServer::generateSessionId(std::string userId) {

  std::string sessionId;

  //the current time
  ptime now = microsec_clock::local_time();

  //To get the current time as a string
  std::string nowToString = to_simple_string(now.date());
  nowToString.append("-"+to_simple_string(now.time_of_day()));

  //To construct the session key
  sessionId.append(userId+"-");
  sessionId.append(nowToString+":"+vishnu::convertToString( vishnu::generateNumbers() ));

  msession.setSessionId(sessionId);

  return 0;
}
/**
 * \brief Function to record the session on the database
 * \param idmachine the database number id of the client machine
 * \param iduser the database number id of the owner of the session
 * \return record the the session id on the session data structure
 */
int
SessionServer::recordSessionServer(std::string idmachine, std::string iduser) {

  std::string sqlInsert = "insert into vsession "
                          "(vsessionid, clmachine_numclmachineid, users_numuserid, lastconnect, "
                          "creation, sessionKey, state, closepolicy, timeout, authid) values ";

  std::string values = std::string("('" +mdatabaseVishnu->escapeData(msession.getSessionId())+"',"+idmachine+","+iduser+","
                                   "CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '"+mdatabaseVishnu->escapeData(msession.getSessionKey())+"',");

  values.append(vishnu::convertToString(msession.getStatus())+",");
  values.append(vishnu::convertToString(msession.getClosePolicy())+",");
  values.append(vishnu::convertToString(msession.getTimeout())+",'");
  values.append(mdatabaseVishnu->escapeData(msession.getAuthenId())+"')");

  sqlInsert.append(values);
  mdatabaseVishnu->process(sqlInsert);
  return 0;
}
/**
 * \brief Function to check the session on database
 * \param flagSessionId A flag to check the session id
 * \return true if the session key (or sessionId) exists else false
 */
bool
SessionServer::exist(bool flagSessionId) {

  if (flagSessionId) {
    return (getState(true) != -1);
  }
  else {
    return (getState() != -1);
  }

}


/**
 * \brief Function to check the session on database
 * \param flagSessionId A flag to check the session identifier
 * \return the session status
 */
int
SessionServer::getState(bool flagSessionId) {

  std::string sessionState;


  if (flagSessionId) {
    sessionState = getAttribut("where vsessionid='"+mdatabaseVishnu->escapeData(msession.getSessionId())+"'","state");
  }
  else {
    sessionState = getAttribut("where sessionkey='"+mdatabaseVishnu->escapeData(msession.getSessionKey())+"'","state");
  }

  if (sessionState.size() != 0) {
    return vishnu::convertToInt(sessionState);
  }
  else {
    return -1;
  }

}
/**
 * \brief Function to check the session on database
 * \param idmachine the database number id of the client machine
 * \param iduser the database number id of the owner of the session
 * \param flagAdmin A flag for an admin to get the session key from another user
 * \return 0 on success and -1 if there is no results
 */
int
SessionServer::getSessionkey(std::string idmachine, std::string iduser, bool flagAdmin) {

  std::string key = "";
  std::string closePolicyStr = "";

  //if the user is not an admin, the machine and the userid are checked
  if (!flagAdmin) {
    key = getAttribut("where vsessionid='"+mdatabaseVishnu->escapeData(msession.getSessionId())+"' "
                      "and clmachine_numclmachineid="+idmachine+" and users_numuserid="+iduser);
  }
  else {
    key = getAttribut("where vsessionid='"+mdatabaseVishnu->escapeData(msession.getSessionId())+"'");
  }
  if (key.size() != 0) {
    msession.setSessionKey(key);
    //To get the close policy associated to the session
    closePolicyStr =  getAttribut("where vsessionid='"+mdatabaseVishnu->escapeData(msession.getSessionId())+"'", "closepolicy");
    msession.setClosePolicy(vishnu::convertToInt(closePolicyStr));
    return 0;
  }
  else {
    return -1;
  }
}


/**
 * \brief Function to solve the session connection parameters
 )
 * \param connectOpt the connection parameters
 * \return the connection parameters are registered on the session data structure
 */
int
SessionServer::solveConnectionMode(UMS_Data::ConnectOptions* connectOpt) {
  switch (connectOpt->getClosePolicy()) {
    ////The closure mode is default
    case vishnu::DEFAULT_CONNECTION_CLOSE_POLICY :
      msession.setClosePolicy(vishnu::DEFAULT_CONNECTION_CLOSE_POLICY);
      if (msession.getClosePolicy() != vishnu::CLOSE_ON_DISCONNECT) {
        msession.setTimeout(DEFAULT_CONNECTION_TIMEOUT);
      }
      break;
    case vishnu::CLOSE_ON_TIMEOUT:
      msession.setClosePolicy(vishnu::CLOSE_ON_TIMEOUT);
      if (connectOpt->getSessionInactivityDelay() != 0) {
        msession.setTimeout(connectOpt->getSessionInactivityDelay());
      } else {
        msession.setTimeout(mtimeout);
      }
      break;
    case vishnu::CLOSE_ON_DISCONNECT:
      msession.setClosePolicy(vishnu::CLOSE_ON_DISCONNECT);
      break;

    default:
      throw SystemException(ERRCODE_SYSTEM, "Invalid close policy value in SessionServer::solveConnectionMode");
  }
  return 0;
}

/**
 * \brief Function to change the closure connection mode disconnet to timeout
 * \return the new connection parameters are registered on the session data structure
 */
int
SessionServer::disconnetToTimeout() {

  //To change the session close policy on CLOSE_ON_TIMEOUT on the database
  mdatabaseVishnu->process(boost::str(boost::format("UPDATE vsession SET closepolicy=%1%"
                                                    " WHERE sessionkey='%2%';")
                                      % vishnu::CLOSE_ON_TIMEOUT
                                      % mdatabaseVishnu->escapeData(msession.getSessionKey())));

  msession.setClosePolicy(vishnu::CLOSE_ON_TIMEOUT);
  msession.setTimeout(mtimeout);
  mdatabaseVishnu->process(boost::str(boost::format("UPDATE vsession SET closepolicy=%1%"
                                                    " WHERE sessionkey='%2%';")
                                      % msession.getTimeout()
                                      % mdatabaseVishnu->escapeData(msession.getSessionKey())));

  return 0;
}
