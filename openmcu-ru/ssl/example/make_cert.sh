#!/bin/sh

echo "
  [ req ]
  default_bits            = 1024
  prompt                  = no
  distinguished_name      = req_dn

  [ req_dn ]
  commonName              = OpenMCU-ru CA
  organizationName        = OpenMCU-ru

  [ ext ]
  basicConstraints=CA:TRUE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid,issuer:always
" > root.cfg

echo "
  [ req ]
  default_bits            = 1024
  prompt                  = no
  distinguished_name      = req_dn

  [ req_dn ]
  commonName              = OpenMCU-ru CA
  organizationName        = OpenMCU-ru

  [ ext ]
  basicConstraints=CA:FALSE
  subjectKeyIdentifier=hash
  authorityKeyIdentifier=keyid,issuer:always
  subjectAltName=DNS:openmcu-ru
" > agent.cfg

openssl req -newkey rsa -nodes -keyout "root.key" -sha1 -out "root.req" -config "root.cfg"
openssl x509 -req -in "root.req" -sha1 -extensions ext -signkey "root.key" -out "root.cert" -extfile "root.cfg"

openssl req -newkey rsa -nodes -keyout "agent.key" -sha1 -out "agent.req" -config "agent.cfg"
openssl x509 -req -in "agent.req" -sha1 -extensions ext -CA "root.cert" -CAkey "root.key" -out a"gent.cert" -CAcreateserial -extfile "agent.cfg"

cat agent.key agent.cert > ../agent.pem
cat root.key root.cert > ../root.pem

rm -f root.*
rm -f agent.*
