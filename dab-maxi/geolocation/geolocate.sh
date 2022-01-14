
#!/bin/sh

OUTPUT_FILE=/tmp/server_location.txt

# Grab this server's public IP address
PUBLIC_IP=`curl -s https://ipinfo.io/ip`

# Call the geolocation API and capture the output
curl https://ipinfo.io/${PUBLIC_IP}  &> /tmp/x1
#grep "loc" /tmp/x1 > ${OUTPUT_FILE}
grep loc /tmp/x1 > /tmp/x2

