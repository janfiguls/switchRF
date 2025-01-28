import serial,json

def send2server(d,ser):
    data=json.dumps(d).encode()
    ser.write(data)
    ser.write(b'\r')
    print('Message sent!')
    valid=False
    while(valid is False):
        print('Waiting for response...')
        recJSON=ser.read_until(b'\r',size=1024).decode()
        try:
            recdata=json.loads(recJSON)
            valid=True
        except:
            print('Invalid message received, try again!')
            ser.write(data)
            ser.write(b'\r')
            print('Message sent!')

    for s in recdata:
        print('Connected with switch ',s)
        print('\tVout= ',recdata[s]['Vout'],'V')
        print('\tAdcread= ',recdata[s]['adcread'],'V')
        print('\tReady= ',recdata[s]['ready'])
        print('')
        
