

import socket
import sys
import openlr
import base64

HOST ="127.0.0.1"   # The remote host
PORT = 8888             # The same port as used by the server

while(True):
    print(f"Run")
    try:
        with socket. socket (socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            data = s.recv(2048*32)

        test= base64.b64encode (data)
        print(openlr.binary_decode(test, is_base64=True))

    except NotImplementedError:
        print(f"Version Error")
    
    except ValueError:
        print(f"ValueError: Location type cannot be identified")

