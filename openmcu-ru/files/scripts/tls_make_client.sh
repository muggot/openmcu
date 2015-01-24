#!/bin/sh

CA_PATH="./CA"
SSL_PATH="../ssl"
COMMON_NAME="client"
ORG_NAME="OpenMCU-ru"

if [ ! -z $1 ] ; then
  COMMON_NAME=$1
fi

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
  nsComment=\"OpenMCU-ru client certificate\"
  nsCertType=client
  extendedKeyUsage=clientAuth

  [ v3_ca ]
  basicConstraints=CA:TRUE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid:always,issuer
" > client.cfg

openssl req -new -out "client.req" -newkey ec:"${CA_PATH}/CA_CURVE.pem" -keyout "client.key" -config "client.cfg" -nodes -sha1
openssl x509 -req -CAkey "${CA_PATH}/cakey.pem" -CA "${CA_PATH}/cacert.pem" -CAcreateserial -in "client.req" -out "client.crt" -extfile "client.cfg" -extensions ext -sha1

cat client.key client.crt > ${SSL_PATH}/${COMMON_NAME}.pem
rm -f client.*
