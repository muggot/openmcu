#!/bin/sh

CA_PATH="./CA"
SSL_PATH="../ssl"
COMMON_NAME="OpenMCU-ru"
ORG_NAME="OpenMCU-ru"

echo "
  [ req ]
  default_bits            = 2048
  prompt                  = no
  distinguished_name      = req_dn
  x509_extensions         = v3_ca

  [ req_dn ]
  commonName              = ${COMMON_NAME} CA
  organizationName        = ${ORG_NAME}

  [ v3_ca ]
  basicConstraints=CA:TRUE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid:always,issuer
" > root.cfg

echo "
  [ req ]
  default_bits            = 2048
  prompt                  = no
  distinguished_name      = req_dn
  x509_extensions         = v3_ca

  [ req_dn ]
  commonName              = ${COMMON_NAME}
  organizationName        = ${ORG_NAME}

  [ ext ]
  basicConstraints=CA:TRUE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid,issuer:always
  nsComment=\"OpenMCU-ru agent certificate\"
  nsCertType=server
  extendedKeyUsage=serverAuth

  [ v3_ca ]
  basicConstraints=CA:TRUE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid:always,issuer
" > agent.cfg

mkdir -p ./CA

openssl ecparam -name secp384r1 -out "${CA_PATH}/CA_CURVE.pem"
openssl req -out "${CA_PATH}/cacert.pem" -new -x509 -keyout "${CA_PATH}/cakey.pem" -newkey ec:"${CA_PATH}/CA_CURVE.pem" -config "root.cfg" -nodes -sha1
cat ${CA_PATH}/cacert.pem > ${SSL_PATH}/cafile.pem
rm -f root.cfg

openssl req -new -out "agent.req" -newkey ec:"${CA_PATH}/CA_CURVE.pem" -keyout "agent.key" -config "agent.cfg" -nodes -sha1
openssl x509 -req -CAkey "${CA_PATH}/cakey.pem" -CA "${CA_PATH}/cacert.pem" -CAcreateserial -in "agent.req" -out "agent.crt" -extfile "agent.cfg" -extensions ext -sha1
cat agent.key agent.crt > ${SSL_PATH}/agent.pem
rm -f agent.*
