/**
 * \file api_ums.hpp
 * \brief This file contains the VISHNU api functions.
 * \author Daouda Traore (daouda.traore@sysfera.com) and Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 * \date February 2011
 */
#ifndef _API_UMS_H_
#define _API_UMS_H_

#include <iostream>
#include <string>

#include "SessionProxy.hpp"
#include "MachineProxy.hpp"
#include "LocalAccountProxy.hpp"
#include "ConfigurationProxy.hpp"
#include "OptionValueProxy.hpp"
#include "QueryProxy.hpp"
#include "UtilsProxy.hpp"
#include "UserException.hpp"
#include "utilVishnu.hpp"
/**
 * \brief Function to open a session 
 * \fn int connect(const std::string& userId,
 *                 const std::string& password, 
                   std::string& sessionKey(), 
 *                 const UMS_Data::ConnectOptions& connectOpt=UMS_Data::ConnectOptions())
 * \param userId The VISHNU user identifier
 * \param password The password of the user
 * \param sessionKey The encrypted identifier of the session generated by VISHNU 
 * \param connectOpt To encapsulate the options available for the connect method. It allows the user 
 *  to choose the way for way for closing the session automatically on TIMEOUT or on DISCONNECT and the
 *  possibility for an admin to open a session as she was a specific user
 *  \return raises an exception on error
 */
int
connect(const std::string& userId, 
        const std::string& password, 
        std::string& sessionKey, 
        const UMS_Data::ConnectOptions& connectOpt=UMS_Data::ConnectOptions())
                                                                             throw(UserException); 

/**
 * \brief Function to return the sessionKey of a session in which the user was disconnected previously without closing it
 * \fn int reconnect(const std::string& userId, 
 *                   const std::string& password, 
 *                   const std::string& sessionId,
 *                   UMS_Data::Session& session) 
 * \param userId represents the VISHNU user identifier
 * \param password represents the password of the user
 * \param sessionId is the identifier of the session defined in the database
 * \param session 
 * \return raises an exception on error
 */
int 
reconnect(const std::string& userId, 
          const std::string& password, 
          const std::string& sessionId, 
          UMS_Data::Session& session) 
                                 throw(UserException);

/**
 * \brief Function to close the session identfied by the session key
 * \fn int close(const std::string& sessionKey) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \return raises an exception on error 
 */
int 
close(const std::string& sessionKey) 
                                   throw(UserException);

/**
 * \brief Function to add a new user in VISHNU
 * \fn int addUser(const std::string& sessionKey, 
 *                 UMS_Data::User& newUser)
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU 
 * \param newUser is an object containing the new user information
 * \return raises an exception on error 
 */
int 
addUser(const std::string& sessionKey, 
        UMS_Data::User& newUser) 
                               throw(UserException);

/**
 * \brief Function to update the user information except the userId and the password
 * \fn int updateUser(const std::string& sessionKey,
 *                    const UMS_Data::User& user)
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU 
 * \param user is an object containing the new user information
 * \return raises an exception on error 
 */
int 
updateUser(const std::string& sessionKey, 
           const UMS_Data::User& user) 
                                     throw(UserException);

/**
 * \brief Function to remove a user from VISHNU
 * \fn int deleteUser(const std::string& sessionKey, 
 *                    const std::string& userId) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param userId represents the VISHNU user identifier of the user who will be deleted from VISHNU
 * \return raises an exception on error 
 */
int 
deleteUser(const std::string& sessionKey,
           const std::string& userId)
                                    throw(UserException);

/**
 * \brief Function to change the password
 * \fn int changePassword(const std::string& userId, 
 *                        const std::string& password, 
 *                        const std::string& passwordNew) 
 * \param userId represents the VISHNU user identifier
 * \param password represents the password of the user
 * \param passwordNew represents the new password of the user
 * \return raises an exception on error 
 */
int 
changePassword(const std::string& userId, 
               const std::string& password, 
               const std::string& passwordNew)
                                             throw(UserException);

/**
 * \brief Function to reset the password of a user
 * \fn int resetPassword(const std::string& sessionKey, 
 *                       const std::string& userId) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param userId represents represents the VISHNU user identifier of the user whose password will be reset
 * \return raises an exception on error 
 */
int 
resetPassword(const std::string& sessionKey, 
              const std::string& userId,
              std::string& tmpPassword)
                                       throw(UserException);

/**
 * \brief Function to add a new machine in VISHNU
 * \fn int addMachine(const std::string& sessionKey, 
 *                    const UMS_Data::Machine& newMachine) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param newMachine is an object which encapsulates the information of the machine which will be added in VISHNU except the
 * machine id which will be created automatically by VISHNU
 * \return raises an exception on error 
 */
int 
addMachine(const std::string& sessionKey, 
           UMS_Data::Machine& newMachine)
                                              throw(UserException);

/**
 * \brief Function to update machine description 
 * \fn int updateMachine(const std::string& sessionKey,
 *                       const UMS_Data::Machine& machine) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param machine is an existing machine information
 * \return raises an exception on error 
 */
int
updateMachine(const std::string& sessionKey, 
              const UMS_Data::Machine& machine)
                                              throw(UserException);

/**
 * \brief Function to remove a machine from VISHNU
 * \fn int deleteMachine(const std::string& sessionKey,
 *                       const std::string& machineId) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param machineId represents the identifier of the machine
 * \return raises an exception on error 
 */
int 
deleteMachine(const std::string& sessionKey, 
              const std::string& machineId)
                                           throw(UserException);

/**
 * \brief Function to add a new local user configuration
 * \fn int addLocalAccount(const std::string& sessionKey, 
 *                         const UMS_Data::LocalAccount& newLocalAccount,
 *                         std::string&  sshPublicKey) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param newLocalAccount is is the object which encapsulates the new local user configuration
 * \param sshPublicKey the SSH public key generated by VISHNU for accessing a local account
 * \return raises an exception on error 
 */
int 
addLocalAccount(const std::string& sessionKey,
                const UMS_Data::LocalAccount& newLocalAccount, 
                std::string&  sshPublicKey)
                                          throw(UserException);

/**
 * \brief Function to update a local user configuration
 * \fn int updateLocalAccount(const std::string& sessionKey,
 *                            const UMS_Data::LocalAccount& localAccount) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param localAccount is an object which encapsulates the local user configuration changes except the machineId and the userId
 * \return raises an exception on error 
 */
int
updateLocalAccount(const std::string& sessionKey, 
                   const UMS_Data::LocalAccount& localAccount)
                                                             throw(UserException);

/**
 * \brief Function to removes a local user configuration (for a given user on a given machine) from VISHNU
 * \fn int deleteLocalAccount(const std::string& sessionKey, 
 *                            const std::string& userId, 
 *                            const std::string& machineId) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param userId represents the VISHNU user identifier of the user whose local configuration will be deleted for the given machine
 * \param machineId represents the identifier of the machine whose local configuration will be deleted for the given user
 * \return raises an exception on error 
 */
int 
deleteLocalAccount(const std::string& sessionKey, 
                   const std::string& userId, 
                   const std::string& machineId) 
                                               throw(UserException);

/**
 * \brief Function to save the configuration of VISHNU
 * \fn int saveConfiguration(const std::string& sessionKey,
 *                           const std::string& filePath, 
 *                           UMS_Data::Configuration& config) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param config is an object which encapsulates the configuration description
 * \return raises an exception on error 
 */
int
saveConfiguration(const std::string& sessionKey,
                  UMS_Data::Configuration& config)
                                                 throw(UserException);

/**
 * \brief Function to restore the configuration of VISHNU
 * \fn int restoreConfiguration(const std::string& sessionKey, 
 *                              const std::string& filePath) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param filePath is the path of the file used to restore VISHNU configuration
 * \return raises an exception on error 
 */
int
restoreConfiguration(const std::string& sessionKey, 
                     const std::string& filePath)
                                                throw(UserException);

/**
 * \brief Function to configure an option of the user
 * \fn int configureOption(const std::string& sessionKey, 
 *                         const UMS_Data::OptionValue& optionValue) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param optionValue is an object which encapsulates the option information
 * \return raises an exception on error 
 */
int
configureOption(const std::string& sessionKey,
                const UMS_Data::OptionValue& optionValue)
                                                        throw(UserException);

/**
 * \brief Function to configure a default option value
 * \fn int configureDefaultOption(const std::string& sessionKey,
 *                                const UMS_Data::OptionValue& optionValue) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param optionValue is an object which encapsulates the option information
 * \return raises an exception on error 
 */
int
configureDefaultOption(const std::string& sessionKey, 
                       const UMS_Data::OptionValue& optionValue)
                                                               throw(UserException);
/**
 * \brief Function to list all sessions of the user
 * \fn int listSessions(const std::string& sessionKey,
 *                      UMS_Data::ListSessions& listSession,
 *                      const UMS_Data::ListSessionOptions& options) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listSession is an object which encapsulates the list of sessions
 * \param options allows the user to list sessions using several optional criteria such as: the state of sessions (actives or
 * inactives, by default, all sessions are listed), a period, a specific session or for admin to list all sessions of all
 * users or sessions of a specific user.
 * \return raises an exception on error 
 */
int 
listSessions(const std::string& sessionKey,
             UMS_Data::ListSessions& listSession, 
             const UMS_Data::ListSessionOptions& options=UMS_Data::ListSessionOptions())
                                                                                       throw(UserException);

/**
 * \brief Function to list the local user configurations
 * \fn int listLocalAccount(const std::string& sessionKey,
 *                          UMS_Data::ListLocalAccounts& listLocalAcc,
 *                          const UMS_Data::ListLocalAccOptions& options) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listLocalAcc is an object which encapsulates the list of the local user configuations
 * \param options allows an admin to list all local configurations of all users or a simple user to list his/her local user configurations on a 
 * specific machine
 * \return raises an exception on error 
 */
int 
listLocalAccount(const std::string& sessionKey, 
                 UMS_Data::ListLocalAccounts& listLocalAcc,
                 const UMS_Data::ListLocalAccOptions& options=UMS_Data::ListLocalAccOptions())
                                                                                              throw(UserException); 

/**
 * \brief Function to list the machines in which the local user configurations are defined for the given user
 * \fn int listMachine(const std::string& sessionKey,
 *                     UMS_Data::ListMachines& listMachine,
 *                     const UMS_Data::ListMachineOptions& options) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listMachine is an object which encapsulates the list of the machines 
 * \param options allows a user to list all VISHNU machines or information about a specific machine and an admin to list machines used by 
 * a specific user
 * \return raises an exception on error 
 */
int 
listMachine(const std::string& sessionKey, 
            UMS_Data::ListMachines& listMachine,
            const UMS_Data::ListMachineOptions& options=UMS_Data::ListMachineOptions())
                                                                                      throw(UserException) ;

/**
 * \brief Function to list the commands
 * \fn int listHistoryCmd(const std::string& sessionKey,
 *                        UMS_Data::ListCommands& listCommands,
 *                        const UMS_Data::ListCmdOptions& options) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listCommands is an object which encapsulates the list of commands
 * \param options allows the user to list commands by using several optional criteria: a period, specific session and for admin
 * to list all commands of all VISHNU users or commands from a specific user
 * \return raises an exception on error 
 */
int 
listHistoryCmd(const std::string& sessionKey, 
               UMS_Data::ListCommands& listCommands,
               const UMS_Data::ListCmdOptions& options=UMS_Data::ListCmdOptions())
                                                                                 throw(UserException);

/**
 * \brief Function to list the options of the user
 * \fn int listOptions(const std::string& sessionKey,
 *                     UMS_Data::ListOptionsValues& listOptValues,
 *                     const UMS_Data::ListOptOptions& options) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listOptValues is an object which encapsulates the list of options
 * \param options allows to list a specific option or all default options values or for an admin to list options of a specific user
 * \return raises an exception on error 
 */
int 
listOptions(const std::string& sessionKey,
            UMS_Data::ListOptionsValues& listOptValues,
            const UMS_Data::ListOptOptions& options=UMS_Data::ListOptOptions())
                                                                             throw(UserException) ;

/**
 * \brief Function to list VISHNU users
 * \fn int listUsers(const std::string& sessionKey,
 *                   UMS_Data::ListUsers& listUsers,
 *                   const std::string& userIdOption) 
 * \param sessionKey is the encrypted identifier of the session generated by VISHNU
 * \param listUsers is an object which encapsulates the list of users
 * \param options allows an admin to get information about a specific user identified by his/her userId
 * \return raises an exception on error 
 */
int
listUsers(const std::string& sessionKey, 
          UMS_Data::ListUsers& listUsers, 
          const std::string& userIdOption=std::string())
                                                       throw(UserException);

/**
 * \brief Function to initialize the SYSFERA-DS configuration
 * \fn int vishnuInitialize(char* cfg, int argc, char** argv)
 * \param cfg The SYSFERA-DS configuration file
 * \param argc The number of arguments of the program 
 * \param argv The list of arguments
 * \return an error code 
 */
int 
vishnuInitialize(char* cfg, int argc, char** argv);

/**
 * \brief Function to finalize 
 * \fn int vishnuFinalize() 
 * \return an error code  
 */
void
vishnuFinalize();

/**
 * \brief Function to initialize the data base 
 * \fn int restore(const std::string& filePath)
 * \param filePath the file  
 * \return an error code  
 */
int 
restore(const std::string& filePath);


#endif
