
ORIGINAL_DIR=${PWD}

# establish the script directory absolute path

UTILITY_ENV_FILE=${BASH_SOURCE[0]}
UTILITY_DIR=`dirname ${UTILITY_ENV_FILE}`
cd ${UTILITY_DIR}
UTILITY_DIR=`pwd`
cd ${ORIGINAL_DIR}

# path

#export PATH=:${PATH}

# exportable aliases

export UTILITY_DIR
mu () { ${UTILITY_DIR}/build/my_utility.exe $* ; }
export -f mu
