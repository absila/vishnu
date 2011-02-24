/**
* \file utilServer.hpp
* \brief This file presents the utils functions of server objects
* \author Eugène PAMBA CAPO-CHICHI (eugene.capochichi@sysfera.com)
* \date 15/02/2011
*/

#ifndef _UTILSERVER_H_
#define _UTILSERVER_H_

#include <iostream>
#include <sstream>
#include <ctime>
#include <cstdio>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"
#include "VishnuException.hpp"
#include "SystemException.hpp"
#include "POSTGREDatabase.hpp"
#include "DbFactory.hpp"
#include "DatabaseResult.hpp"
#include "VishnuException.hpp"

using namespace vishnu;
using namespace boost::posix_time;
using namespace boost::gregorian;

/**
* \class Vishnuid
* \brief The Vishnuid class encapsulates the global vishnu id
*/
class Vishnuid {
  public:
    static std::string mvishnuid;
};

/**
* \enum IdType
* \brief The type of id generated
*/
typedef enum IdType{
  MACHINE = 0,
  USER = 1,
  JOB = 2,
  FILETRANSFERT = 3
} IdType;

/**
* \class Format_t
* \brief The class used to get the variables of the string to parse
*/
class Format_t{
public :
  int start;
  int end;
  std::string value;
};

/**
* \brief Function to encrypt data and password
* \fn    char* crypt(const char* clef, const char* salt);
* \param clef is the data to be encrypted
* \param salt This string is used to perturb the algorithm
* \return the string encrypted
*/
char* crypt(const char* clef, const char* salt);

/**
* \namespace utilServer
* \brief The utils functions of server objects
*/
namespace utilServer {

  static const std::string ROOTUSERNAME = "root";
  static const std::string DATABASENAME = "vishnu";
  static const std::string UMSMAPPERNAME = "UMS";

  /**
  * \brief Function to get a random number
  * \fn    int generate_numbers()
  * \return the number generated
  */
  int
  generate_numbers();

  int isMonth (const char * s);
  int isYear (const char * s);
  int isSite (const char * s);
  int isType (const char * s);
  int isUName (const char * s);
  int isMaName (const char * s);
  int isDay (const char * s);
  int isCpt (const char * s);

  void
  getKeywords (int* size, Format_t* array,
               const char* format,
               int cpt, IdType type,
	             std::string name,
               std::string site);
  std::string
  getGeneratedName (const char* format,
                    int cpt,
                    IdType type,
                    std::string name = "",
                    std::string site ="");

  std::string
  getAttrVishnu(std::string attrname, std::string vishnuid);

  void
  incrementCpt(std::string cptName, int cpt);
}
#endif//UTILSERVER