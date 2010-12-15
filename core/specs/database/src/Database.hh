/**
 * \file Database.hh
 * \brief This file presents an abstract database.
 * \author Kevin Coulomb (kevin.coulomb@sysfera.com)
 */

#ifndef __ABSTRACTDATABASE__
#define __ABSTRACTDATABASE__

class Database{
public :
  /**
   * \brief Factory to create a database
   * \fn    virtual int createDatabase() = 0
   * \param hostname The name of the host to connect
   * \param username The username to connect to the database
   * \param pwd      The password to connect to the database
   * \return Return the database created, nill otherwise
   */
  virtual Database*
  createDatabase(std::string hostname,
		 std::string username,
		 std::string pwd) = 0;
  /**
   * \brief Function to delete a database
   * \fn    virtual int deleteDatabase() = 0
   * \return 0 on success, an error code otherwise
   */
  virtual int 
  deleteDatabase(Database& db) = 0;
  /**
   * \brief Function to process the request in the database
   * \fn    virtual int createDatabase() = 0
   * \param request The request to process
   * \return 0 on success, an error code otherwise
   */
  virtual int 
  process(std::string request) = 0;
  /**
   * \brief To reconnect to the database
   * \fn reconnect()
   * \return 0 on success, an error code otherwise
   */
  virtual int 
  reconnect() = 0;
protected :
  /**
   * \fn Database()
   * \brief Constructor, raises an exception on error
   */
  Database();
  /**
   * \fn ~Database()
   * \brief Destructor, raises an exception on error
   */
  ~Database();
private :
  /**
   * \brief To disconnect from the database
   * \fn disconnect()
   * \return 0 on success, an error code otherwise
   */
  virtual int 
  disconnect() = 0;
};


#endif // ABSTRACTDATABASE
