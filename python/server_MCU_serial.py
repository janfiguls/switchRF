import serial,json,time,pprint_dict

port='COM7'
ser=serial.Serial(port,baudrate=9600)

def send2client(d):
    data=json.dumps(d).encode()
    ser.write(data)
    ser.write(b'\r')

myd = {
    "s1": {
        "Vout":20,
        "adcread":20,
        "ready": True
    },
    "s2": {
        "Vout":5,
        "adcread":15,
        "ready": False,
    }
}


while(1):
    valid=False
    while(valid is False):
        print('Waiting for data to arrive...')
        dataJSON=ser.read_until(b'\r',size=1024).decode()
        try:
            recd=json.loads(dataJSON)
            valid=True
        except:
            print('Invalid message received, try again!')

    if(recd['action']=='init'):
        print('Init operation...')
        send2client(myd)
        
    if(recd['action']=='modify'):
        myd[recd['switch']['name']]['Vout']=recd['switch']['Vout']
        # cridem a la funció que sigui per actualitzar Vout i llegir adcread
        # deixem temps al circuit perquè s'actualitzi
        time.sleep(5) 
        # suposem que quan han passat 5 segons ja està tot bé, que seria
        myd[recd['switch']['name']]['adcread']=recd['switch']['Vout']
        myd[recd['switch']['name']]['ready']=True

        while
        send2client(myd)
