
#ifndef _CONSTANTS_HPP_
#define _CONSTANTS_HPP_

#include <string>
#include <map>

namespace vishnu {
  /**
 * \brief Define a constant type
 */
  enum c_type_t {
    NONE_PARAMETER,
    BOOL_PARAMETER,
    INT_PARAMETER,
    ULONG_PARAMETER,
    STRING_PARAMETER,
    URI_PARAMETER
  };

  /**
 * \brief Define a parameter type
 */

  enum param_type_t {
    DBTYPE,
    DBHOST,
    DBPORT,
    DBNAME,
    DBUSERNAME,
    DBPASSWORD,
    SENDMAILSCRIPT,
    BATCHTYPE,
    BATCHVERSION,
    MACHINEID,
    TIMEOUT,
    REMOTEBINDIR,
    INTERVALMONITOR,
    DBPOOLSIZE,
    AUTHENTYPE,
    DEFAULTBATCHCONFIGFILE,
    URISUPERVISOR,
    DISP_URIADDR,
    DISP_URISUBS,
    NBTHREADS,
    HOST_URIADDR,
    SED_URIADDR,
    CLOUDENDPOINT,
    DEBUG_LEVEL,
    DB_USE_SSL,
    DB_SSL_CA_FILE,
    USE_SSL,
    SERVER_PUBLIC_KEY,
    SERVER_PRIVATE_KEY,
    SERVER_SSL_CERTICATE,
    SSL_CA,
    SUBSCRIBE,
    STANDALONE,
    HAS_UMS,
    HAS_TMS,
    HAS_FMS,
    IPC_URI_BASE,
    OPTION_SESSION_TIMEOUT,
    OPTION_TRANSFER_TIMEOUT,
    OPTION_DEFAULT_TRANSFER_CMD,
    OPTION_DEFAULT_CONNECTION_CLOSE_POLICY
  };

  /**
 * \brief For cloud
 */
  enum cloud_env_vars_t {
    CLOUD_ENDPOINT,
    CLOUD_USER,
    CLOUD_USER_PASSWORD,
    CLOUD_TENANT,
    CLOUD_VM_IMAGE,
    CLOUD_VM_USER,
    CLOUD_VM_USER_KEY,
    CLOUD_CONTEXT_INIT,
    CLOUD_VIRTUAL_NET,
    CLOUD_VIRTUAL_NET_MASK,
    CLOUD_VIRTUAL_NET_DNS,
    CLOUD_VIRTUAL_NET_GATEWAY,
    CLOUD_DEFAULT_FLAVOR,
    CLOUD_NFS_SERVER,
    CLOUD_NFS_MOUNT_POINT,
    CLOUD_SCRIPT_SUBMISSION_OUTPUT
  };

  /**
 * \brief States of jobs
 */
  enum job_status_t {
    STATE_UNDEFINED=0,
    STATE_SUBMITTED=1,
    STATE_QUEUED=2,
    STATE_WAITING=3,
    STATE_RUNNING=4,
    STATE_COMPLETED=5,
    STATE_CANCELLED=6,
    STATE_DOWNLOADED=7,
    STATE_FAILED=8
  } ;

  /**
   * \brief States of components
   */
  enum component_status_t {
    STATUS_UNDEFINED=-1,
    STATUS_LOCKED=0,
    STATUS_ACTIVE=1,
    STATUS_DELETED=2
  } ;

  enum session_state_t {
    SESSION_CLOSED=0,
    SESSION_ACTIVE=1
  };

  enum privilege_type_t {
    PRIVILEGE_UNDEFINED = -1,
    PRIVILEGE_USER = 0,
    PRIVILEGE_ADMIN = 1
  };

  enum transfert_type_t {
    SCP_TRANSFER = 0,
    RSYNC_TRANSFER = 1,
    UNDEFINED_TRANSFER_MANAGER = 1
  };


  enum TransfertStatusT {
    TRANSFER_INPROGRESS = 0,
    TRANSFER_COMPLETED = 1,
    TRANSFER_CANCELLED = 2,
    TRANSFER_FAILED = 3,
    TRANSFER_WAITING_CLIENT_RESPONSE = 4,
  };

  enum FileCopyDirectionT {
    CopyLocalRemote = 0,
    CopyRemoteLocal = 1
  };

  /**
 * \brief A convenient data type to store a parameter
 */
  struct param_t {
    /**
   * \brief key to retrieve the parameter
   */
    param_type_t key;
    /**
   * \brief the value of the parameter
   */
    const std::string value;
    /**
   * \brief the type of the parameter
   */
    c_type_t type;
  };

  /**
   * \brief An array to store all parameters
  */
  extern param_t params[];  //%RELAX<MISRA_3_1_3> Because this table is defined in constants.cpp
  extern std::map<cloud_env_vars_t, std::string> CLOUD_ENV_VARS;

  enum option_t {
    VISHNU_CLOSE_POLICY,
    VISHNU_TIMEOUT,
    VISHNU_TRANSFER_CMD,
    VISHNU_TRANSFER_TIMEOUT
  };

  enum close_policy_t {
    DEFAULT_CONNECTION_CLOSE_POLICY = 0,
    CLOSE_ON_TIMEOUT = 1,
    CLOSE_ON_DISCONNECT = 2
  };
}

#endif /* _CONSTANTS_HPP_ */
