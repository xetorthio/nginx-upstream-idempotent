#!/bin/bash

NGINX=/usr/local/nginx/sbin/nginx

cd test
$NGINX -c nginx.conf -p .
RESPONSE1=$(curl --connect-timeout 3 --max-time 4 --write-out %{http_code} --silent --output /dev/null -X GET -H "Content-Length: 0" "http://localhost:8080/")
RESPONSE2=$(curl --connect-timeout 3 --max-time 4 --write-out %{http_code} --silent --output /dev/null -X GET -H "Content-Length: 0" "http://localhost:8080/")
if [ $(echo $RESPONSE1$RESPONSE2) == "200200" ]; then
  echo "."
else
  echo "GET should go next upstream server (response was $RESPONSE1 and $RESPONSE2)"
fi
$NGINX -p . -s stop -c nginx.conf

$NGINX -c nginx.conf -p .
RESPONSE1=$(curl --connect-timeout 3 --max-time 4 --write-out %{http_code} --silent --output /ev/null -X POST -d "hola" "http://localhost:8080/")
RESPONSE2=$(curl --connect-timeout 3 --max-time 4 --write-out %{http_code} --silent --output /ev/null -X POST -d "hola" "http://localhost:8080/")
if [ $(echo $RESPONSE1$RESPONSE2) == "200500" -o  $(echo $RESPONSE1$RESPONSE2) == "500200" ]; then
  echo "."
else
  echo "POST should NOT go next upstream server (response was $RESPONSE1 and $RESPONSE2)"
fi
$NGINX -p . -s stop -c nginx.conf
cd ..
