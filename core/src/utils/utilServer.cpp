/**
 * \file utilServer.cpp
 * \brief This file implements the utils functions of the vishnu system
 * \author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
 * \date 15/02/2011
 */

#include "utilServer.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/format.hpp>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "DatabaseResult.hpp"
#include "utilVishnu.hpp"
#include "DbFactory.hpp"
#include "SystemException.hpp"
#include "DbFactory.hpp"
#include "Server.hpp"
#include "vishnu_version.hpp"
#include "TMSVishnuException.hpp"

bool
vishnu::isNew(const std::string& urlsup, const std::string& mid, const std::string& type) {

  DbFactory factory;
  Database *mdatabase;
  mdatabase = factory.getDatabaseInstance();
  std::string req = (boost::format("SELECT machineid"
                                   " FROM process"
                                   " WHERE machineid='%1%'"
                                   " AND vishnuname='%2%'"
                                   " AND dietname='%3%'"
                                   " AND pstatus<>%4%")%mdatabase->escapeData(mid) %mdatabase->escapeData(type) %mdatabase->escapeData(urlsup) %vishnu::STATUS_DELETED).str();
  try {
    boost::scoped_ptr<DatabaseResult> result(mdatabase->getResult(req));
    if (result->getNbTuples() != 0) {
      return false;
    }
  } catch (SystemException& e) {
    e.appendMsgComp(" Failed to determine if the process "+type + " already exist");
    throw;
  }
  return true;
}


/**
 * \brief Function to get a random number
 * \return
 */
bool
vishnu::isMonth (const char * s) {
  return (s[0]=='M' && s[1]=='O' && s[2]=='N' && s[3]=='T' && s[4]=='H');
}
bool
vishnu::isYear (const char * s) {
  return (s[0]=='Y' && s[1]=='E' && s[2]=='A' && s[3]=='R');
}
bool
vishnu::isSite (const char * s) {
  return (s[0]=='S' && s[1]=='I' && s[2]=='T' && s[3]=='E');
}
bool
vishnu::isType (const char * s) {
  return (s[0]=='T' && s[1]=='Y' && s[2]=='P' && s[3]=='E');
}
bool
vishnu::isUName (const char * s) {
  return (s[0]=='U' && s[1]=='N' && s[2]=='A' && s[3]=='M' && s[4]=='E');
}
bool
vishnu::isName (const char * s) {
  return (s[0]=='N' && s[1]=='A' && s[2]=='M' && s[3]=='E');
}
bool
vishnu::isMaName (const char * s) {
  return (s[0]=='M' && s[1]=='A' && s[2]=='N' && s[3]=='A' && s[4]=='M' && s[5]=='E');
}
bool
vishnu::isDay (const char * s) {
  return (s[0]=='D' && s[1]=='A' && s[2]=='Y');
}
bool
vishnu::isCpt (const char * s) {
  return (s[0]=='C' && s[1]=='P' && s[2]=='T');
}

/**
 * \brief Parse the format and fill the array with the variable and their value
 * \param size Size of the array (OUT)
 * \param array Array to fill with variables and their value (OUT)
 * \param format The string to parse
 * \param cpt Value of the counter variable
 * \param type Type of the id to generate
 * \param name Name of the user or machine
 * \param site Site of the machine
 * \return 0 on success and -1 when an error occurs
 */
int
vishnu::getKeywords(int* size, Format_t* array, const std::string& format, int cpt, IdType type,
                    const std::string& name, const std::string& site) {
  unsigned int charIndex;
  *size = 0;

  char buf[500];
  memset (buf, 0, 500);

  //the current time
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
  boost::gregorian::date::ymd_type ymd = now.date().year_month_day();
  std::string year = boost::str(boost::format("%1%") % ymd.year);
  std::string month = boost::str(boost::format("%1%") % ymd.month);
  std::string day = boost::str(boost::format("%1%") % ymd.day);

  // Loop parsing for the variables, setting their position and their value
  /* >RELAX<MISRA_6_3_1> avoid using too many brackets */
  /* >RELAX<MISRA_6_4_1> avoid using too many brackets */
  for (charIndex=0; charIndex< format.size(); ++charIndex) {
    if (format[charIndex]=='$') {
      std::string rightSubStr = format.substr(charIndex+1, std::string::npos);
      if (isDay(rightSubStr.c_str())) {
        array[*size].start = charIndex;
        array[*size].end = charIndex+3;
        array[*size].value = day;
        (*size) ++;
      } else if (isMonth(rightSubStr.c_str())) {
        array[*size].value = month;
        array[*size].start = charIndex;
        array[*size].end = charIndex+5;
        (*size) ++;
      } else if (isYear(rightSubStr.c_str())) {
        array[*size].start = charIndex;
        array[*size].end = charIndex+4;
        array[*size].value = year;
        (*size) ++;
      } else if (isCpt (rightSubStr.c_str())) {
        char tmp[10];
        sprintf (tmp, "%d", cpt);
        array[*size].value = std::string (tmp);
        array[*size].start = charIndex;
        array[*size].end = charIndex+3;
        (*size) ++;
      } else if (isSite (rightSubStr.c_str())) {
        array[*size].value = site;
        array[*size].start = charIndex;
        array[*size].end = charIndex+4;
        (*size) ++;
      } else if (isMaName (rightSubStr.c_str())) {
        array[*size].value = name;
        array[*size].start = charIndex;
        array[*size].end = charIndex+6;
        (*size) ++;
      } else if (isUName (rightSubStr.c_str())) {
        array[*size].value = name;
        array[*size].start = charIndex;
        array[*size].end = charIndex+5;
        (*size) ++;
      } else if (isName (rightSubStr.c_str())) {
        array[*size].value = name;
        array[*size].start = charIndex;
        array[*size].end = charIndex+4;
        (*size) ++;
      } else if (isType (rightSubStr.c_str())) {
        switch (type) {
          case 0 :
            array[*size].value = "M";
            break;
          case 1 :
            array[*size].value = "U";
            break;
          case 2 :
            array[*size].value = "J";
            break;
          case 3 :
            array[*size].value = "F";
            break;
          case 4 :
            array[*size].value = "A";
            break;
          case 5 :
            array[*size].value = "W";
            break;
          default :
            break;
        }
        array[*size].start = charIndex;
        array[*size].end = charIndex+4;
        (*size) ++;
      } else {
        return -1;
      }
    }
  }
  return 0;
}


/**
 * \brief Function to generate an ID
 * \param format Format to use to generate the id
 * \param cpt Value of the counter to use
 * \param type Type of the id generated
 * \param name Name of the user or machine (optionnal)
 * \param site Site of the machine (optionnal)
 */
std::string
vishnu::getGeneratedName(const char* format,
                          int cpt,
                          IdType type,
                          const std::string& name ,
                          const std::string& site) {
  std::string res;
  res.clear ();
  res = std::string ("");
  int  size;
  Format_t *keywords;

  keywords = new Format_t[strlen(format)+1];
  int ret = getKeywords (&size, keywords, format, cpt, type, name, site); // Getting var and their value

  // if there is no error with the getKeywords function
  if (ret != -1) {
    // Building the id using the format and the values of the var
    if (size > 0) {
      res.append(format, keywords[0].start);
    } else {
      res = std::string (format);
    }

    for (int i = 0; i < size; i++) {
      res.append (keywords[i].value);
      // If other variables
      if (*(format+keywords[i].end + 1) != '\0' && i!=size-1) {
        res.append (format+keywords[i].end+1, keywords[i+1].start-keywords[i].end-1);
        // If text after the variable
      } else if (*(format+keywords[i].end + 1) != '\0' ) {
        res.append (format+keywords[i].end+1, strlen (format)-keywords[i].end-1);
      }
    }
  }
  delete [] keywords;
  return res;
}

/**
 * \brief Function to get a specific vishnu counter
 * \param type : the type of id generated
 * \return The int counter value
 */
int
vishnu::getVishnuCounter(IdType type) {
  DbFactory factory;
  Database *databaseVishnu;
  int ret;

  std::string table;
  std::string fields;
  std::string val;
  std::string primary;


  bool insert=true;
  switch(type) {
    case MACHINE:
      table="machine";
      fields=" (status) ";
      val = " (0) ";
      primary="nummachineid";
      break;
    case USER:
      table="users";
      fields=" (pwd,userid) ";
      val = " ('','') ";
      primary="numuserid";
      break;
    case JOB:
      table="job";
      fields=" (job_owner_id, machine_id, workId, vsession_numsessionid) ";
      val= " ((select max(numuserid) from users), (select max(nummachineid) from machine),"
           "NULL, (select max(numsessionid) from vsession)) "; //FIXME insert invalid value then update it
      primary="numjobid";
      break;
    case FILETRANSFERT:
      table="filetransfer";
      fields=" (vsession_numsessionid) ";
      val= " ((select max(numsessionid) from vsession)) "; //FIXME insert invalid value then update it
      primary="numfiletransferid";
      break;
    case AUTH:
      table="authsystem";
      fields=" (status) ";
      val = boost::str(boost::format(" (%1%) ") % vishnu::STATUS_UNDEFINED);
      primary="numauthsystemid";
      break;
    case WORK:
      //FIXME : no auto-increment field in work
      fields = " (application_id"
               ",date_created,done_ratio, identifier,"
               "nbcpus, owner_id, "
               "project_id, "
               "status, subject, consolidated) ";
      val = " ((select min(id) from application_version),"
            " CURRENT_TIMESTAMP, 1, 't',"
            " 1, (select min(numuserid) from users), "
            "(select min(id) from project), "
            "1,'toto', false) ";
      table = "work";
      primary="id";
      break;
    default:
      fields = " (updatefreq, formatiduser, formatidjob, formatidfiletransfer, formatidmachine, formatidauth) ";
      val = " (1, 't', 't', 't', 't', 't') ";
      table = "vishnu";
      insert=false;
      primary="vishnu_vishnuid";
      break;
  }

  databaseVishnu = factory.getDatabaseInstance();
  int tid = databaseVishnu->startTransaction();
  try {
    ret = databaseVishnu->generateId(table, fields, val, tid, primary);
  } catch (const std::exception& e) {
    databaseVishnu->cancelTransaction(tid);
    throw;
  }

  if (insert) {
    databaseVishnu->endTransaction(tid);
  } else {
    databaseVishnu->cancelTransaction(tid);
  }
  return ret;
}

/**
 * \brief To set the objectId in the specified row in the database
 * \param key : the key to identify the reserved row
 * \param objectId : the objectId to set
 * \param type : the type of the object
 */
void
vishnu::reserveObjectId(int key, std::string &objectId, IdType type) {
  std::string table;
  std::string keyname;
  std::string idname;
  bool uniq = false;

  switch(type) {
    case MACHINE:
      table="machine";
      keyname="nummachineid";
      idname="machineid";
      break;
    case USER:
      table="users";
      keyname="numuserid";
      idname="userid";
      break;
    case JOB:
      table="job";
      keyname="numjobid";
      idname="jobid";
      break;
    case FILETRANSFERT:
      table="filetransfer";
      keyname="numfiletransferid";
      idname="transferid";
      break;
    case AUTH:
      table="authsystem";
      keyname="numauthsystemid";
      idname="authsystemid";
      break;
    case WORK:
      table="work";
      keyname="id";
      idname="identifier";
      break;
    default:
      throw SystemException(ERRCODE_SYSTEM,"Cannot reserve Object id, type in unrecognized");
      break;
  }
  while (!uniq){
    uniq = checkObjectId(table, idname, objectId);
    if (!uniq) {
      objectId += convertToString(key);
    }
  }

  DbFactory factory;
  std::string sqlReserve="UPDATE "+table+" ";
  sqlReserve+="set "+idname+"='"+factory.getDatabaseInstance()->escapeData(objectId)+"' ";
  sqlReserve+="where "+keyname+"="+convertToString(key)+";";

  try {
    factory.getDatabaseInstance()->process(sqlReserve);
  } catch (std::exception const & e) {
    throw SystemException(ERRCODE_SYSTEM,
                          boost::str(boost::format("Cannot reserve Object id: ")% e.what()));
  }

}

bool
vishnu::checkObjectId(const std::string& table,
                      const std::string& idname,
                      const std::string& objectId){
  DbFactory factory;
  Database *mdatabase;
  mdatabase = factory.getDatabaseInstance();
  std::string request = "SELECT "+ idname + " FROM " + table + " WHERE " + idname + "='" + mdatabase->escapeData(objectId) +"';";
  try {
    boost::scoped_ptr<DatabaseResult> result(mdatabase->getResult(request));
    if (result->getNbTuples() != 0) {
      return false;
    }
  } catch (SystemException& e) {
    throw;
  }
  return true;
}


/**
 * \brief Function to increment a counter of the table vishnu
 * \param cptName the name of the counter to increment
 * \param cpt     the current value of the counter
 * \return raises an exception
 */
void
vishnu::incrementCpt(const std::string& cptName, int cpt, int transacId) {
  DbFactory factory;
  Database *databaseVishnu;
  databaseVishnu = factory.getDatabaseInstance();

  cpt = cpt+1;

  std::string sqlCommand("UPDATE vishnu set "+cptName+"="+databaseVishnu->escapeData(cptName)+"+1");

  databaseVishnu->process(sqlCommand, transacId);

}

/**
* \brief Function to get an Id generated by VISHNU
* \param type the type of the Id generated
* \param stringforgeneration the string used for generation
* \return the corresponding conversion
*/
std::string
vishnu::getObjectId(IdType type, std::string stringforgeneration) {


  std::string errorString = "";

  pthread_mutex_t mutex;
  pthread_mutex_init(&(mutex), NULL);
  pthread_mutex_lock(&(mutex));

  int counter = getVishnuCounter(type);
  std::string format = getIdFormatTemplate(type);
  std::string idGenerated = getGeneratedName(format.c_str(), counter, type, stringforgeneration);

  if (! idGenerated.empty()) {
  } else {
    errorString = boost::str(boost::format("Failed to generate ID for object type %1%") % type);
  }
  reserveObjectId(counter,idGenerated,type); // set the idGenerated in the related row
  pthread_mutex_unlock(&(mutex));

  if (! errorString.empty()) {
    throw SystemException(ERRCODE_SYSTEM, errorString);
  }
  return idGenerated;
}

/**
 * @brief Validate session key and return details on the user and the session
 * @param authKey The authentication key
 * @param machineId The machine Id
 * @param databasePtr A pointer to a database instance
 * @param info The resulting information
 */
void
vishnu::validateAuthKey(const std::string& authKey,
                        const std::string& machineId,
                        Database* database,
                        UserSessionInfo& info)
{
  std::string sqlQuery = (boost::format("SELECT vsession.numsessionid, machine.name, machine.nummachineid,"
                                        "  users.numuserid, users.userid, users.privilege, "
                                        "  account.aclogin, account.home"
                                        " FROM vsession, users, account, machine"
                                        " WHERE vsession.sessionkey='%1%'"
                                        "  AND vsession.state=%2%"
                                        "  AND users.numuserid=vsession.users_numuserid"
                                        "  AND users.numuserid=account.users_numuserid"
                                        "  AND account.status=%3%"
                                        "  AND account.machine_nummachineid=machine.nummachineid"
                                        "  AND machine.machineid='%4%';"
                                        )
                          % database->escapeData(authKey)
                          % vishnu::SESSION_ACTIVE
                          % vishnu::STATUS_ACTIVE
                          % database->escapeData(machineId)
                          ).str();

  boost::scoped_ptr<DatabaseResult> sqlResult(database->getResult(sqlQuery));
  if (sqlResult->getNbTuples() < 1) {
    throw TMSVishnuException(ERRCODE_PERMISSION_DENIED,
                             "Can't get user information from the session token provided");
  }

  std::vector<std::string> rowResult = sqlResult->get(0);
  std::vector<std::string>::iterator rowResultIter = rowResult.begin();

  info.num_session = vishnu::convertToInt(*rowResultIter++);
  info.machine_name = *rowResultIter++;
  info.num_machine = *rowResultIter++;
  info.num_user = vishnu::convertToInt(*rowResultIter++);
  info.userid = *rowResultIter++;
  info.user_privilege = vishnu::convertToInt(*rowResultIter++);
  info.user_aclogin = *rowResultIter++;
  info.user_achome = *rowResultIter++;
}


/**
 * @brief Validate session key and return details on the user and the session
 * @param authKey The authentication key
 * @param databasePtr A pointer to a database instance
 * @param info The resulting information
 */
void
vishnu::validateAuthKey(const std::string& authKey,
                        Database* database,
                        UserSessionInfo& info)
{
  std::string sqlQuery = (boost::format("SELECT vsession.numsessionid, "
                                        "  users.numuserid, users.userid, users.privilege, "
                                        "  account.aclogin, account.home"
                                        " FROM vsession, users, account, machine"
                                        " WHERE vsession.sessionkey='%1%'"
                                        "  AND vsession.state=%2%"
                                        "  AND users.numuserid=vsession.users_numuserid"
                                        "  AND users.numuserid=account.users_numuserid"
                                        "  AND account.status=%3%"
                                        )
                          % database->escapeData(authKey)
                          % vishnu::SESSION_ACTIVE
                          % vishnu::STATUS_ACTIVE
                          ).str();
  boost::scoped_ptr<DatabaseResult> sqlResult(database->getResult(sqlQuery));
  if (sqlResult->getNbTuples() < 1) {
    throw TMSVishnuException(ERRCODE_INVALID_PARAM,
                             "Can't get user local account. Check that:\n"
                             "  * your session is still active\n"
                             "  * you have a local account on this server");
  }

  std::vector<std::string> rowResult = sqlResult->get(0);
  std::vector<std::string>::iterator rowResultIter = rowResult.begin();

  info.num_session = vishnu::convertToInt(*rowResultIter++);
  info.num_user = vishnu::convertToInt(*rowResultIter++);
  info.userid = *rowResultIter++;
  info.user_privilege = vishnu::convertToInt(*rowResultIter++);
  info.user_aclogin = *rowResultIter++;
  info.user_achome = *rowResultIter++;
}


/**
 * @brief Get the template to build object identifier
 * @param objectType The object type
 * @return The template string
 */
std::string
vishnu::getIdFormatTemplate(IdType objectType)
{
  std::string format = "$CPT";
  switch(objectType) {
    case MACHINE:
      format = "$NAME";
      break;
    case USER:
      format = "$NAME";
      break;
    case JOB:
      format = "J_$CPT";
      break;
    case FILETRANSFERT:
      format = "TR_$CPT";
      break;
    case AUTH:
      format = "AUTH_$CPT";
      break;
    case WORK:
      format = "$CPT";
      break;
    default:
      break;
  }

  return format;
}
