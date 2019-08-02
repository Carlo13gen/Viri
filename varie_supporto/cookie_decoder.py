import base64
import zlib
import json

#Prende in input un cookie relativo a una Flask session e lo decodifica
#ritornando un dizionario json
def decode(cookie):


    if cookie[0] == ".":
        payload = ".".join(cookie.split(".", 2)[:2])
        data = zlib.decompress(base64.urlsafe_b64decode(payload + "==="))
        new_data = data.decode('utf-8')
        json_data = json.loads(new_data)
    #se non è compresso
    else:
        payload = cookie.split(".")[0]
        data = base64.urlsafe_b64decode(payload + "===")
        new_data = data.decode('utf-8')
        json_data = json.dumps(new_data)
    return json_data