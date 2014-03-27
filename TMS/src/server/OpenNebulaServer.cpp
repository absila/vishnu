/*
 * OpenNebulaServer.cpp
 *
 *  Created on: 27 nov. 2012
 *      Author: r
 */

#include <string.h>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "constants.hpp"
#include "utilServer.hpp"
#include "tmsUtils.hpp"
#include "SSHJobExec.hpp"
#include "OpenNebulaServer.hpp"
#include "TMSVishnuException.hpp"
#include "FMSVishnuException.hpp"
#include "OneRPCManager.hpp"


OpenNebulaServer::OpenNebulaServer()
  : mcloudUser(""),
    mcloudUserPassword(""),
    mvmImageId(""),
    mvmFlavor(""),
    mvmUser(""),
    mvmUserKey(""),
    mnfsServer(""),
    mnfsMountPoint("")
{
  mcloudEndpoint = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_ENDPOINT], false);
  mcloudUser= vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_USER], false);
  mcloudUserPassword = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_USER_PASSWORD], false);
  mcontextInitScript = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_CONTEXT_INIT], false);
  mvirtualNetwork = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VIRTUAL_NET], false);
  mvirtualNetworkMask = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VIRTUAL_NET_MASK], true);
  mvirtualNetworkGateway = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VIRTUAL_NET_GATEWAY], true);
  mvirtualNetworkDns = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VIRTUAL_NET_DNS], true);
}

OpenNebulaServer::~OpenNebulaServer() {
}

/**
 * \brief Function to submit a job
 * \param scriptPath the path to the script containing the job characteristique
 * \param options the options to submit job
 * \param job The job data structure
 * \param envp The list of environment variables used by submission function
 * \return raises an exception on error
 */
int
OpenNebulaServer::submit(const std::string& scriptPath,
                         const TMS_Data::SubmitOptions& options,
                         TMS_Data::ListJobs& jobSteps,
                         char** envp)
{
  replaceEnvVariables(scriptPath);
  OneRPCManager rpcManager(mcloudEndpoint);
  rpcManager.setMethod("one.vm.allocate");
  rpcManager.addParam(getSessionString());
  rpcManager.addParam(getKvmTemplate(options));
  rpcManager.addParam(false);   // to create VM on pending state
  rpcManager.execute();

  if (! rpcManager.lastCallSucceeded()) {
    throw TMSVishnuException(ERRCODE_BATCH_SCHEDULER_ERROR, rpcManager.getStringResult());
  }

  LOG(boost::str(boost::format("[INFO] Virtual machine created: %1%") % rpcManager.getIntResult()), 1);

  TMS_Data::Job_ptr jobPtr = new TMS_Data::Job();
  jobPtr->setVmId(vishnu::convertToString(rpcManager.getIntResult()));
  jobPtr->setStatus(vishnu::STATE_SUBMITTED);
  jobPtr->setJobName("PID_"+jobPtr->getBatchJobId());
  jobPtr->setOutputPath(jobPtr->getOutputDir()+"/stdout");
  jobPtr->setErrorPath(jobPtr->getOutputDir()+"/stderr");
  jobPtr->setNbNodes(1);

  jobSteps.getJobs().push_back(jobPtr);

  //FIXME: job.setBatchJobId(vishnu::convertToString(jobPid));
  return 0;
}

/**
 * \brief Function to cancel job:  just shutdown and destroy the related VM
 * \param vmId the VM ID
 * \param jobDescr the description of the job in the form of jobId@vmId
 * \return raises an exception on error
 */
int
OpenNebulaServer::cancel(const std::string& vmId)
{
  OneRPCManager rpcManager(mcloudEndpoint);
  rpcManager.setMethod("one.vm.action");
  rpcManager.addParam(getSessionString());
  rpcManager.addParam(std::string("delete"));
  rpcManager.addParam(vishnu::convertToInt(vmId));
  rpcManager.execute();

  if (! rpcManager.lastCallSucceeded()) {
    throw TMSVishnuException(ERRCODE_BATCH_SCHEDULER_ERROR, rpcManager.getStringResult());
  }

  LOG(boost::str(boost::format("[INFO] VM deleted: %1%") % vmId), 1);
  return 0;
}

/**
 * \brief Function to get the status of the job
 * \param jobDescr the job description in the form of pid@user@vmaddress@vmId
 * \return -1 if the job is unknown or server not unavailable
 */
int
OpenNebulaServer::getJobState(const std::string& jobDescr) {

  // Get job infos
  ListStrings jobInfos = getJobInfos(jobDescr, 4);
  std::string pid = jobInfos[0];
  std::string vmUser = jobInfos[1];
  std::string vmIp = jobInfos[2];
  std::string vmId = jobInfos[3];

  SSHJobExec sshEngine(vmUser, vmIp);
  std::string statusFile = boost::str(boost::format("/tmp/%1%") % jobDescr);
  std::string cmd = boost::str(boost::format("ps -o pid= -p %1% | wc -l > %2%") % pid % statusFile);
  sshEngine.execCmd(cmd, false);

  // Check if the job is completed
  // If yes stop the virtual machine and release the resources
  int status = vishnu::STATE_RUNNING;
  if (vishnu::getStatusValue(statusFile) == 0) {
    releaseResources(vmId);
    status = vishnu::STATE_COMPLETED;
  }

  vishnu::deleteFile(statusFile.c_str());

  return status;
}

/**
 * \brief Function to get the start time of the job
 * \param jobDescr the description of the job in the form of jobId@vmId
 * \return 0 if the job is unknown
 */
time_t
OpenNebulaServer::getJobStartTime(const std::string& jobDescr) {

  long long startTime = 0;
  ListStrings jobInfos = getJobInfos(jobDescr, 2); // Get the job information
  OneRPCManager rpcManager(mcloudEndpoint);
  rpcManager.setMethod("one.vm.info");
  rpcManager.addParam(getSessionString());
  rpcManager.addParam(vishnu::convertToInt(jobInfos[1]));
  rpcManager.execute();
  return startTime;
}

/**
 * \brief Function to request the status of queues
 * \param optQueueName (optional) the name of the queue to request
 * \return The requested status in to ListQueues data structure
 */
TMS_Data::ListQueues*
OpenNebulaServer::listQueues(const std::string& optQueueName) {

  //TODO The semantic is no yet defined
  return new TMS_Data::ListQueues();
}


/**
 * \brief Function to get a list of submitted jobs
 * \param listOfJobs the ListJobs structure to fill
 * \param ignoredIds the list of job ids to ignore
 */
void OpenNebulaServer::fillListOfJobs(TMS_Data::ListJobs*& listOfJobs,
                                      const std::vector<std::string>& ignoredIds) { }


int
create_plugin_instance(void **instance)
{
  try {
    *instance = new OpenNebulaServer;
  } catch (const std::bad_alloc& e) {
    return 1;
  }
  return PLUGIN_OK;
}


/**
 * \brief Function for cleaning up virtual machine
 * \param vmId The id of the virtual machine
 */
void OpenNebulaServer::releaseResources(const std::string& vmId)
{
  OneRPCManager rpcManager(mcloudEndpoint);
  rpcManager.setMethod("one.vm.action");
  rpcManager.addParam(getSessionString());
  rpcManager.addParam(std::string("stop"));
  rpcManager.addParam(vishnu::convertToInt(vmId));
  rpcManager.execute();
  if (! rpcManager.lastCallSucceeded()) {
    throw TMSVishnuException(ERRCODE_BATCH_SCHEDULER_ERROR, rpcManager.getStringResult());
  }
  //FIXME: check that the vm is shutdown and clear it
  LOG(boost::str(boost::format("[INFO] VM deleted: %1%") % vmId), 1);
}

/**
 * \brief Function to decompose job information
 * \param: jobDescr The description of the job in the form of param1@param2@...
 * \param: numParams The number of expected parameters
 */
ListStrings OpenNebulaServer::getJobInfos(const std::string jobDescr, const int & numParams)
{
  ListStrings jobInfos;
  boost::split(jobInfos, jobDescr, boost::is_any_of("@"));
  if(jobInfos.size() != numParams) {
    throw TMSVishnuException(ERRCODE_INVALID_PARAM, "Bad job description "+std::string(jobDescr)+ "\n"
                             "Expects "+vishnu::convertToString(numParams)+" parameters following the pattern param1@param2...");
  }
  return jobInfos;
}


/**
 * \brief To retrieve specific submission parameters
 * \param specificParamss The string containing the list of parameters
 */
void OpenNebulaServer::retrieveUserSpecificParams(const std::string& specificParams) {
  ListStrings listParams;
  boost::split(listParams, specificParams, boost::is_any_of(" "));
  for (ListStrings::iterator it = listParams.begin(); it != listParams.end(); ++it) {
    size_t pos = it->find("=");
    if (pos != std::string::npos) {
      std::string param = it->substr(0, pos);
      std::string value = it->substr(pos+1, std::string::npos);
      if (param == "endpoint") {
        mcloudEndpoint = value;
      } else if (param == "user") {
        mcloudUser = value;
      } else if (param == "user-password") {
        mcloudUserPassword = value;
      } else if (param == "vm-image") {
        mvmImageId = value;
      } else if (param == "vm-user") {
        mvmUser = value;
      } else if (param == "vm-key") {
        mvmUserKey = value;
      } else if (param == "vm-flavor") {
        mvmFlavor = value;
      } else if (param == "nfs-server") {
        mnfsServer = value;
      } else if (param == "nfs-mountpoint") {
        mnfsMountPoint = value;
      } else {
        throw TMSVishnuException(ERRCODE_INVALID_PARAM, param);
      }
    }
  }
}

/**
 * \brief Function to replace some environment varia*bles in a string
 * \param scriptContent The string content to modify
 */
void OpenNebulaServer::replaceEnvVariables(const std::string& scriptPath) {
  std::string scriptContent = vishnu::get_file_content(scriptPath);

  //To replace VISHNU_BATCHJOB_ID
  vishnu::replaceAllOccurences(scriptContent, "$VISHNU_BATCHJOB_ID", "$$");
  vishnu::replaceAllOccurences(scriptContent, "${VISHNU_BATCHJOB_ID}", "$$");
  //To replace VISHNU_BATCHJOB_NAME
  vishnu::replaceAllOccurences(scriptContent, "$VISHNU_BATCHJOB_NAME", "$(ps -o comm= -C -p $$)");
  vishnu::replaceAllOccurences(scriptContent, "${VISHNU_BATCHJOB_NAME}", "$(ps -o comm= -C -p $$)");
  //To replace VISHNU_BATCHJOB_NUM_NODES. Depends on the number of nodes in VISHNU_BATCHJOB_NODEFILE
  //Note: The variable VISHNU_BATCHJOB_NODEFILE is set later in JobServer
  vishnu::replaceAllOccurences(scriptContent, "$VISHNU_BATCHJOB_NUM_NODES", "$(wc -l ${VISHNU_BATCHJOB_NODEFILE} | cut -d' ' -f1)");
  vishnu::replaceAllOccurences(scriptContent, "${VISHNU_BATCHJOB_NUM_NODES}", "$(wc -l ${VISHNU_BATCHJOB_NODEFILE} | cut -d' ' -f1)");

  std::ofstream ofs(scriptPath.c_str());
  ofs << scriptContent;
  ofs.close();
}

/**
 * @brief Return a string as expected by OpenNebula API (username:password)
 * @return string
 */
std::string
OpenNebulaServer::getSessionString(void)
{
  if (mcloudUser.empty() && mcloudUserPassword.empty()) {
    throw TMSVishnuException(ERRCODE_INVALID_PARAM,
                             "Either the username or the password to authenticate against OpenNebula is empty");
  }
  return boost::str(boost::format("%1%:%2%") % mcloudUser % mcloudUserPassword);
}

/**
 * @brief return a VM template for KVM virtual machine
 * @param options submit options
 * @return
 */
std::string
OpenNebulaServer::getKvmTemplate(const TMS_Data::SubmitOptions& options)
{
  retrieveUserSpecificParams(options.getSpecificParams());

  // Get configuration parameters
  if (mvmImageId.empty()) {
    mvmImageId = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VM_IMAGE], false);
  }
  if (mvmFlavor.empty()) {
    mvmFlavor = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_DEFAULT_FLAVOR], false);
  }
  if (mvmUser.empty()) {
    mvmUser = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VM_USER], false);
  }
  if (mvmUserKey.empty()) {
    mvmUserKey = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_VM_USER_KEY], false);
  }
  if (mnfsServer.empty()) {
    mnfsServer = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_NFS_SERVER], true);
  }
  if(mnfsMountPoint.empty()) {
    mnfsMountPoint = vishnu::getVar(vishnu::CLOUD_ENV_VARS[vishnu::CLOUD_NFS_MOUNT_POINT], true);
  }

  return boost::str(
        boost::format(
          "NAME=\"vishnu-vm\"                                                     \n"
          "CPU=%1%                                                               \n"
          "VCPU=%1%                                                              \n"
          "MEMORY=%2%                                                            \n"
          "DISK = [ IMAGE = \"%3%\", DRIVER=\"qcow2\"]                           \n"
          "OS=[                                                                  \n"
          "  ARCH=\"i686\",                                                      \n"
          "  ROOT=\"sda1\",                                                      \n"
          "  BOOT=\"hd,fd,cdrom,network\" ]                                      \n"
          "NIC = [NETWORK=\"%4%\"]                                               \n"
          "GRAPHICS = [TYPE=\"vnc\", LISTEN=\"0.0.0.0\",KEYMAP=\"fr\"]           \n"
          "RAW=[                                                                 \n"
          "  TYPE=\"kvm\",                                                       \n"
          "  DATA=\"                                                             \n"
          "    <serial type='pty'><target port='0'/></serial>                    \n"
          "    <console type='pty'><target type='serial' port='0'/></console>\"] \n"
          "CONTEXT=[                                                             \n"
          "  HOSTNAME=\"vm-$VMID\",                                              \n"
          "  NETWORK=\"YES\",                                                    \n"
          "  ETH0_IP=\"$NIC[IP, NETWORK=\\\"%4%\\\"]\",                          \n"
          "  ETH0_NETMASK=\"%5%\",                                               \n"
          "  ETH0_GATEWAY=\"%6%\",                                               \n"
          "  ETH0_DNS=\"%7%\",                                                   \n"
          "  FILES=\"%8%\",                                                      \n"
          "  TARGET=\"hdb\"                                                      \n"
          "]")
        % returnInputOrDefaultIfNegativeNull(options.getNbCpu(), 1)
        % returnInputOrDefaultIfNegativeNull(options.getMemory(), 512)
        % mvmImageId
        % mvirtualNetwork
        % mvirtualNetworkMask
        % mvirtualNetworkGateway
        % mvirtualNetworkDns
        % mcontextInitScript);
}


/**
 * @brief Return the defaultValue if a given string is empty
 * @param value The input string
 * @param defaultValue The default value
 * @return string
 */
std::string
OpenNebulaServer::returnInputOrDefaultIfEmpty(const std::string& value, const std::string& defaultValue)
{
  if (value.empty()) {
    return defaultValue;
  }
  return value;
}

/**
 * @brief Return the defaultValue if a given string is empty
 * @param value The input string
 * @param defaultValue The default value
 * @return int
 */
int
OpenNebulaServer::returnInputOrDefaultIfNegativeNull(int value, int defaultValue)
{
  if (value <= 0) {
    return defaultValue;
  }
  return value;
}

