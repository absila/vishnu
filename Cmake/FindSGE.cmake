#
# Try to find the SGE installation
#

find_path(SGE_INCLUDE_DIR
 drmaa.h
  paths
        ${SGE_DIR}/include
        $ENV{SGE_ROOT}/include
        /usr/include
        /usr/local/include
        /opt/local/include
)

find_library(SGE_LIB
  drmaa 
  paths
        ${SGE_DIR}/lib
	$ENV{SGE_ROOT}/lib/linux-x86
        $ENV{SGE_ROOT}/lib/linux-x64
        /usr/lib
        /usr/local/lib
	/opt/local/lib
)

find_path(SGE_ROOT
  inst_sge
  paths
      $ENV{SGE_ROOT}
)

find_path(SGE_BIN_DIR
  sge_execd
  paths
      $ENV{SGE_ROOT}/bin/linux-x86
      $ENV{SGE_ROOT}/bin/linux-x64
      /usr/local/bin
      /usr/bin
      /opt/local/bin
)

if (SGE_INCLUDE_DIR AND  SGE_LIB AND SGE_ROOT AND SGE_BIN_DIR)
  set(SGE_FOUND "Yes")
  mark_as_advanced(SGE_BIN_DIR)
  mark_as_advanced(SGE_ROOT)
  mark_as_advanced(SGE_INCLUDE_DIR)
  mark_as_advanced(SGE_LIB)
endif(SGE_INCLUDE_DIR AND  SGE_LIB AND SGE_ROOT AND SGE_BIN_DIR)

