/** \file api_fms.cpp
 * \brief This file contains the VISHNU api functions for FMS package.
 * \author Ibrahima Cisse (ibrahima.cisse@sysfera.com)
 * \date MAY 2011
 */

#include <cstring>
#include <string>
#include <stdexcept>
#include <pthread.h>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <UMS_Data.hpp>
#include "QueryProxy.hpp"
#include "utilVishnu.hpp"
#include <api_fms.hpp>


//FMS client headers

#include "FileProxyFactory.hpp"
#include "FileTransferProxy.hpp"
#include "FMSServices.hpp"

/**
 * \brief create a new remote file
 * \param sessionKey the session key
 * \param path  the file path using host:path format
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::touch(const std::string& sessionKey,const std::string& path)
throw (UMSVishnuException, FMSVishnuException, UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);
  SessionProxy sessionProxy(sessionKey);
  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));
  int result= f->mkfile();

  return result;
}

/**
 * \brief create a directory
 * \param sessionKey the session key
 * \param path: the directory path using host:path format
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::mkdir(const std::string& sessionKey, const std::string& path,
              const FMS_Data::CreateDirOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));
  int result= f->mkdir(options);

  return result;
}

/** remove a file
 * \param sessionKey the session key
 * \param path    the file path using host:path format
 * \param options contains options used to perform the remove file function
 \return 0 if everything is OK, another value otherwise
*/
int
vishnu::rm(const std::string& sessionKey, const std::string& path,
           const FMS_Data::RmFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));

  int result= f->rm(options);

  return result;
}

/**
 * \brief  remove a directory
 * \param sessionKey the session key
 * \param path    the directory path using host:path format
 * \return 0 if everything is OK, another value otherwise
 */
int vishnu::rmdir(const std::string& sessionKey,const std::string& path)
throw (UMSVishnuException, FMSVishnuException, UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));
  int result= f->rmdir();

  return result;
}



/**
 * \brief change the group of a file
 * \param sessionKey the session key
 * \param group the name of the new group to use for this file
 * \param path  the file path using host:path format
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::chgrp(const std::string& sessionKey,
              const std::string& group,
              const std::string& path)
throw (UMSVishnuException, FMSVishnuException, UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);
  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));
  int result= f->chgrp(group);

  return result;
}

/**
 * \brief  change the permissions of a file
 * \param sessionKey the session key
 * \param mode the path new mode
 * \param path  the file path using host:path format
 * \param options contains the options used to set the new the permission mode  for this file
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::chmod(const std::string& sessionKey, const mode_t& mode,
              const std::string& path)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);
  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));
  int result= f->chmod(mode);

  return result;
}

/**
 * \brief  copy the file
 * \param sessionKey the session key
 * \param src  the "source" file path using host:path format
 * \param dest  the "destination" file path using host:path format
 * \param options contains the options
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::cp(const std::string& sessionKey, const std::string& src,
           const std::string& dest, const FMS_Data::CpFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {
  int result = 0;

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(dest);

  if ((options.getTrCommand() < 0) || options.getTrCommand() > 2) {
    throw UserException(ERRCODE_INVALID_PARAM, "Invalid transfer command type: its value must be 0 (scp) or 1 (rsync)");
  }
  FMSVishnuException e(ERRCODE_RUNTIME_ERROR, "Unknown copy error");

  try {
    FileTransferProxy fileTransferProxy(sessionKey, src, dest);
    result = fileTransferProxy.addCpThread(options);
    return result;
  } catch (FMSVishnuException& ex){
    e.appendMsgComp(" "+src+": "+ex.what());
  }

  throw e;
  return result;
}


/**
 * \brief copy the file in a asynchronous mode
 * \param sessionKey the session key
 * \param src   the "source" file path using host:path format
 * \param dest  the "destination" file path using host:path format
 * \param transferInfo contains different information about the submitted file
 * transfer (like the transfer identifier)
 * \param options contains options used to perform the file transfer
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::acp(const std::string& sessionKey,
            const std::string& src,
            const std::string& dest,
            FMS_Data::FileTransfer& transferInfo,
            const FMS_Data::CpFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(dest);

  if ((options.getTrCommand() < 0) || options.getTrCommand() > 2) {
    throw UserException(ERRCODE_INVALID_PARAM, "Invalid transfer commad type: its value must be 0 (scp) or 1 (rsync)");
  }
  FileTransferProxy fileTransferProxy(sessionKey, src, dest);
  int result = fileTransferProxy.addCpAsyncThread(options);
  transferInfo = fileTransferProxy.getFileTransfer();
  return result;
}

/**
 * \brief get the first lines of a file
 * \param sessionKey the session key
 * \param path   the file path using host:path format
 * \param contentOfFile  the first "nline" lines of the file
 * \param options   contains the options used to perform the service (like the maximum number of lines to get)
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::head(const std::string& sessionKey,
             const std::string& path,
             std::string& contentOfFile,
             const FMS_Data::HeadOfFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {
  //To check the remote path

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);
  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));

  contentOfFile = f->head(options);

  return 0;
}
/**
 * \brief get the content of a file
 * \param sessionKey the session key
 * \param path   the file path using host:path format
 * \param contentOfFile  the content of specified the file
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::cat(const std::string& sessionKey, const std::string& path, std::string& contentOfFile)
throw (UMSVishnuException, FMSVishnuException, UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,path));

  contentOfFile = f->getContent();

  return 0;
}

/**
 * \brief get the list of files and subdirectories of a directory
 * \param sessionKey the session key
 * \param path  the directory path using host:path format
 * \param dirContent  the directory content
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::ls(const std::string& sessionKey,
           const std::string& path,
           FMS_Data::DirEntryList& dirContent,
           const FMS_Data::LsDirOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  // if no path is provided, just display local account $HOME
  std::string path_(path);
  size_t pos = path.find(":");
  size_t last = path.length() - 1;
  if (pos == last) {
    path_.append("~");
  } else if (path[pos+1] != '/') {
    path_.insert(pos+1, "~/");
  }

  vishnu::validatePath(path_);

  //To check the remote path
  vishnu::checkRemotePath(path_);

  // initialize the list of dirEntries
  dirContent.getDirEntries().clear();


  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f (FileProxyFactory::getFileProxy(sessionProxy,
                                                                 path_));

  FMS_Data::DirEntryList* dirContent_ptr = f->ls(options);

  if(dirContent_ptr != NULL) {
    FMS_Data::FMS_DataFactory_ptr ecoreFactory = FMS_Data::FMS_DataFactory::_instance();
    for(unsigned int i = 0; i < dirContent_ptr->getDirEntries().size(); i++) {
      FMS_Data::DirEntry_ptr dirEntry = ecoreFactory->createDirEntry();

      //To copy the content and not the pointer
      *dirEntry = *dirContent_ptr->getDirEntries().get(i);
      dirContent.getDirEntries().push_back(dirEntry);
    }
    delete dirContent_ptr;
  }
  return 0;
}

/**
 * \brief move a file in synchronous mode
 * \param sessionKey the session key
 * \param src:   the "source" file path using host:path format
 * \param dest:  the "destination" file path using host:path format
 * \param transferInfo contains different information about the submitted file
 * transfer (like the transfer identifier)
 * \param options   contains the options used to perform the service (like the transfer command :scp or rsync)
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::mv(const std::string& sessionKey,
           const std::string& src,
           const std::string& dest,
           const FMS_Data::CpFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(dest);

  if ((options.getTrCommand() < 0) || options.getTrCommand() > 2) {
    throw UserException(ERRCODE_INVALID_PARAM, "Invalid transfer commad type: its value must be 0 (scp) or 1 (rsync)");
  }
  int result = 0;
  FMSVishnuException e(ERRCODE_RUNTIME_ERROR, "Unknwon move error");
  try{
    FileTransferProxy fileTransferProxy(sessionKey, src, dest);
    result = fileTransferProxy.addMvThread(options);
    return result;
  } catch (FMSVishnuException& ex){
    e.appendMsgComp(" "+src+": "+ex.what());
    throw e;
  }
  return result;
}
/**
 * \brief move a file in a asynchronous mode
 * \param sessionKey the session key
 * \param src:   the "source" file path using host:path format
 * \param dest:  the "destination" file path using host:path format
 * \param transferInfo contains different information about the submitted file
 * transfer (like the transfer identifier)
 * \param options   contains the options used to perform the service (like the transfer command :scp or rsync)
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::amv(const std::string& sessionKey,
            const std::string& src,
            const std::string& dest,
            FMS_Data::FileTransfer& transferInfo,
            const FMS_Data::CpFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(dest);

  if ((options.getTrCommand() < 0) || options.getTrCommand() > 2) {
    throw UserException(ERRCODE_INVALID_PARAM, "Invalid transfer commad type: its value must be 0 (scp) or 1 (rsync)");
  }

  FileTransferProxy fileTransferProxy(sessionKey, src, dest);
  int result = fileTransferProxy.addMvAsyncThread(options);
  transferInfo = fileTransferProxy.getFileTransfer();
  return result;
}

/**
 * \brief get the last lines of a file
 * \param sessionKey the session key
 * \param path    the file path using host:path format
 * \param contentOfFile  the last "nline" lines of the file
 * \param options  the options used to perform the service
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::tail(const std::string& sessionKey,
             const std::string& path,
             std::string& contentOfFile,
             const FMS_Data::TailOfFileOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  boost::scoped_ptr<FileProxy> f(FileProxyFactory::getFileProxy(sessionProxy,path));

  contentOfFile= f->tail(options);

  return 0;
}

/**
 * \brief  obtain informations about a file
 * \param sessionKey the session key
 * \param path the file path using host:path format
 * \param :  a buffer to store the informations
 * \return 0 if everything is OK, another value otherwise
 */
int
vishnu::stat(const std::string& sessionKey,
             const std::string& path,
             FMS_Data::FileStat& fileInfos)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {

  // Check that the file path doesn't contain characters subject to security issues
  vishnu::validatePath(path);

  //To check the remote path
  vishnu::checkRemotePath(path);

  SessionProxy sessionProxy(sessionKey);

  FileProxy* f = FileProxyFactory::getFileProxy(sessionProxy,path);

  f->getInfos();

  fileInfos=f->getFileStat();

  return 0;

}

/**
 * \brief cancel a file transfer
 * \param sessionKey the session key
 * \param options   contains the options used to perform the service (like the transfer id obtained after a call to acp or
 *          amv )
 \return 0 if everything is OK, another value otherwise
*/
int
vishnu::stopFileTransfer(const std::string& sessionKey,
                         const FMS_Data::StopTransferOptions& options)
throw (UMSVishnuException, FMSVishnuException, UserException, SystemException) {

  FMS_Data::StopTransferOptions optionsCompleted(options);
  FileTransferProxy fileTransferProxy(sessionKey);

  return fileTransferProxy.stopThread(optionsCompleted);
}


/**
 * \brief List  file transfers
 * \param sessionKey the session key
 * \param fileTransferList the file transfer list
 * \param options contains the options used to perform the service (like the transfer id obtained after a call to acp or
 amv)
 \return 0 if everything is OK, another value otherwise
*/
int
vishnu::listFileTransfers(const std::string& sessionKey,
                          FMS_Data::FileTransferList& fileTransferList,
                          const FMS_Data::LsTransferOptions& options)
throw (UMSVishnuException, FMSVishnuException,
       UserException, SystemException) {
  // initialize the list of file transfers
  fileTransferList.getFileTransfers().clear();

  if(options.getStatus() < 0 || options.getStatus() > 4) {
    throw UserException(ERRCODE_INVALID_PARAM, "The file transfer status option value is incorrect");
  }

  std::string serviceName = SERVICES_FMS[FILETRANSFERSLIST];

  SessionProxy sessionProxy(sessionKey);

  QueryProxy<FMS_Data::LsTransferOptions, FMS_Data::FileTransferList>
      query(options, sessionProxy, serviceName);

  FMS_Data::FileTransferList* listFileTransfers_ptr = query.list();

  if(listFileTransfers_ptr != NULL) {
    FMS_Data::FMS_DataFactory_ptr ecoreFactory = FMS_Data::FMS_DataFactory::_instance();
    for(unsigned int i = 0; i < listFileTransfers_ptr->getFileTransfers().size(); i++) {
      FMS_Data::FileTransfer_ptr fileTransfer = ecoreFactory->createFileTransfer();

      //To copy the content and not the pointer
      *fileTransfer = *listFileTransfers_ptr->getFileTransfers().get(i);
      fileTransferList.getFileTransfers().push_back(fileTransfer);
    }
    delete listFileTransfers_ptr;
  }

  return 0;
}
