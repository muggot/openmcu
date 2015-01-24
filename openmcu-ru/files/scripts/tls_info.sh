#!/bin/sh

SSL_PATH="../ssl"
FILE_NAME=${SSL_PATH}/agent.pem

if [ ! -z $1 ] ; then
  FILE_NAME=$1
fi

openssl x509 -noout -inform pem -text -in ${FILE_NAME}
