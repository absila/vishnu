/**
 * \file AuthSystemProxy.cpp
 * \brief This file contains the VISHNU AuthSystemProxy class.
 * \author Daouda Traore (daouda.traore@sysfera.com)
 * \date February 2011
 */

#include "AuthSystemProxy.hpp"

#include <string>                       // for string, allocator, etc

#include "DIET_client.h"                // for diet_string_set, etc
#include "ecorecpp/serializer/serializer.hpp"  // for serializer
#include "utilClient.hpp"               // for raiseCommunicationMsgException, etc
#include "UMSServices.hpp"

/**
 * \param authSystem The object which encapsulates the user aythentication system
 * \param session The object which encapsulates the session information (ex: identifier of the session)
 * \brief Constructor
 */
AuthSystemProxy::AuthSystemProxy(const UMS_Data::AuthSystem& authSystem, const SessionProxy& session):
  mauthSystem(authSystem), msessionProxy(session)
{
}


/**
 * \brief Function to add an authentication system
 * \return raises an exception on error
 */
int
AuthSystemProxy::add() {

  std::string authSystemToString;
  std::string authSystemInString;

  diet_profile_t* profile = diet_profile_alloc(SERVICES_UMS[AUTHSYSTEMCREATE], 2);


  ::ecorecpp::serializer::serializer _ser;
  //To serialize the mauthSystem object in to authSystemToString
  authSystemToString =  _ser.serialize_str(const_cast<UMS_Data::AuthSystem_ptr>(&mauthSystem));

  //IN Parameters
  diet_string_set(profile, 0, msessionProxy.getSessionKey());
  diet_string_set(profile, 1, authSystemToString);

  if (diet_call(profile)) {
    raiseCommunicationMsgException("RPC call failed");
  }
  raiseExceptionOnErrorResult(profile);

  diet_string_get(profile, 1, authSystemInString);

  UMS_Data::AuthSystem_ptr authSystem_ptr = NULL;
  parseEmfObject(authSystemInString, authSystem_ptr, "Error by receiving AuthSystem object serialized");
  mauthSystem = *authSystem_ptr;
  delete authSystem_ptr;

  diet_profile_free(profile);

  return 0;
}

/**
 * \brief Function to update an authentication system
 * \return raises an exception on error
 */
int
AuthSystemProxy::update() {

  diet_profile_t* profile = diet_profile_alloc(SERVICES_UMS[AUTHSYSTEMUPDATE], 2);

  //To serialize the mauthSystem object in to authSystemToString
  ::ecorecpp::serializer::serializer _ser;
  std::string authSystemToString =  _ser.serialize_str(const_cast<UMS_Data::AuthSystem_ptr>(&mauthSystem));

  //IN Parameters
  diet_string_set(profile, 0, msessionProxy.getSessionKey());
  diet_string_set(profile, 1, authSystemToString);


  if (diet_call(profile)) {
    raiseCommunicationMsgException("RPC call failed");
  }
  raiseExceptionOnErrorResult(profile);

  diet_profile_free(profile);
  return 0;
}

/**
 * \brief Function to removes an authentication system
 * \return raises an exception on error
 */
int
AuthSystemProxy::deleteAuthSystem()
{
  diet_profile_t* profile = diet_profile_alloc(SERVICES_UMS[AUTHSYSTEMDELETE], 2);
  //IN Parameters
  diet_string_set(profile, 0, msessionProxy.getSessionKey());
  diet_string_set(profile, 1, mauthSystem.getAuthSystemId());

  if (diet_call(profile)) {
    raiseCommunicationMsgException("RPC call failed");
  }
  raiseExceptionOnErrorResult(profile);

  diet_profile_free(profile);

  return 0;
}

/**
  * \brief Function get AuthSystem information
  * \return the AuthSystem data structure
  */
UMS_Data::AuthSystem
AuthSystemProxy::getData() const {
  return mauthSystem;
}
/**
 * \brief Function get SessionProxy object which contains the VISHNU session identifier
 * \return a SessionProy object which contains the VISHNU session information
 * \return raises an exception on error
 */
SessionProxy AuthSystemProxy::getSessionProxy() const
{
  return msessionProxy;
}

/**
 * \brief Destructor, raises an exception on error
 */
AuthSystemProxy::~AuthSystemProxy()
{
}
