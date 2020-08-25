#!/bin/bash
# Processing options & parameters with getopts
#
DEBUG=n
PREFIX=/usr/local/handy

#====================== FUNCTIONS START =====================================
function build() {
  JOBS=`grep -c ^processor /proc/cpuinfo 2>/dev/null`
  if [ "${JOBS}" == ""  ]; then
    JOBS=1
  fi

  CMD="make  -j${JOBS} DEBUG=${DEBUG} PREFIX=${PREFIX}"
  echo ${CMD}
  eval ${CMD}
}

#====================== FUNCTIONS END =====================================


#============ USAGE =================
#while getopts :dcbirp: opt

USAGE="$0 [options]\n"
USAGE+=" -d set debug modee\n "
USAGE+=" -p set prefix\n "
USAGE+=" -c clean project\n "
USAGE+=" -b build project\n "
USAGE+=" -i install project\n "
USAGE+=" -r rebuild project\n "

#============ USAGE END =================


# getopt option:
#   x -  no arg data
#   y: - with arg data
while getopts :dcbirph: opt
do
  case "$opt" in
    d) 
      echo "--with-debug" 
      DEBUG=y
      ;;
    p) 
      PREFIX=${OPTARG}
      echo "--prefix=${PREFIX}"
      ;;
    c)
      echo "cleanning..."
      make clean;
      exit 0
      ;;
    b) 
      echo "building..."
      build;
      exit 0
      ;;
    r)
      echo "rebuilding..."
      make clean;
      build;
      exit 0
      ;;
    i)
      echo "installing..."
      make install;
      exit 0;
      ;;
    *) 
      echo "Unknown option: $opt" 
      echo -e ${USAGE}
      exit 0;
    ;;
  esac
done

build;

