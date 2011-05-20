/**
 * \file MYSQLDatabase.hpp
 * \brief This file presents a SQL database.
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 * \date 15/12/10
 */

#ifndef _MYSDLDATABASE_H_
#define _MYSQLDATABASE_H_

#include "Database.hpp"
#include "mysql.h"

/**
 * \class MYSQLDatabase
 * \brief MYSQL implementation of the Database
 */
class MYSQLDatabase : public Database{
public :
  /**
   * \brief Function to process the request in the database
   * \param request The request to process (must contain a SINGLE SQL statement without a semicolumn)
   * \return 0 on success, an error code otherwise
   */
  int
  process(std::string request);
  /**
  * \brief To make a connection to the database
  * \fn int connect()
  * \return raises an exception on error
  */
  int
  connect();
  /**
   * \brief Constructor, raises an exception on error
   * \fn MYSQLDatabase(DbConfiguration dbConfig)
   * \param dbConfig  the configuration of the database client
   */
  MYSQLDatabase(DbConfiguration dbConfig);
  /**
   * \fn ~MYSQLDatabase()
   * \brief Destructor, raises an exception on error
   */
  ~MYSQLDatabase();
  /**
   * \brief To set the db to use
   * \param db The database to use
   * \return 0 on success, an error code otherwise
   */
//   int
//   setDatabase(std::string db);
  /**
  * \brief To get the result of a select request
  * \fn DatabaseResult* getResult(std::string request)
  * \param request The request to process
  * \return An object which encapsulates the database results
  */
  DatabaseResult*
  getResult(std::string request);
  /**
   * \brief To get the type of database
   * \return An enum identifying the type of database
   */
  DbConfiguration::db_type_t
  getDbType() { return DbConfiguration::MYSQL; };

private :
  /**
   * \brief To get a valid connexion
   * \fn MYSQL* getConnexion(int& pos)
   * \param pos The position of the connexion gotten in the pool
   * \return A valid and free connexion
   */
  MYSQL* getConnexion(int& pos);

  /**
   * \brief To release a connexion
   * \fn void releaseConnexion(int pos)
   * \param pos The position of the connexion to release
   */
  void releaseConnexion(int pos);
  /**
   * \brief Size of the pool
   */
  static const int MPOOLSIZE = 20;

  /**
   * \brief An element of the pool
   */
  typedef struct pool_t{
    /**
     * \brief If the connexion is used
     */
    bool mused;
    /**
     * \brief The connexion mysql structure
     */
    MYSQL mmysql;
    /**
     * \brief The associated mutex
     */
    pthread_mutex_t mmutex;
  }pool_t;
  /////////////////////////////////
  // Attributes
  /////////////////////////////////
  /**
   * \brief The configuration of the database client
   */
  DbConfiguration mconfig;
  /**
   * \brief The pool of connection
   */
  pool_t *mpool;

  /////////////////////////////////
  // Functions
  /////////////////////////////////
  /**
   * \brief To disconnect from the database
   * \fn disconnect()
   * \return 0 on success, an error code otherwise
   */
  int
  disconnect();
};


#endif // MYSQLDATABASE
