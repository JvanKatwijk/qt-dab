import sys
import base64
import openlr
def PrintArray (a):
    test= base64.b64encode (a)
    try:
        test = openlr. binary_decode (test, is_base64=False)
        print (test)
    except NotImplementedError:
       print (f"Version Error")

    c = 0
    return c

