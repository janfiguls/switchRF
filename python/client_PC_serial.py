import serial,json,time,os

port='COM4'
ser=serial.Serial(port,baudrate=9600,timeout=0.5)

def pd(d,level):
    if level == 0:
        print('\nPrinting dictonary:\n')
        level=level+1
    for f in d:
        i=0
        while(i<level):
            print('\t',end='')
            i=i+1
        print(f,end='')
        if type(d[f]) is dict:
            print('{\n',end='')
            pd(d[f],level+1)
        else:
            print(':',d[f])
def send2server(d):
    data=json.dumps(d).encode()
    ser.write(data)
    print('Message sent!')
    def waitForMessage():    
        print('Waiting for response...')
        recJSON=ser.read_until(b'\r',size=1024).decode()
        try:
            recdata=json.loads(recJSON)
            error=recdata.get('error') 
            if error is not None:
                print('Received error message:')
                print(error)
            else:
                ready=recdata.get('ready')
                if ready is not None:
                    print('switch status: ',ready)
                    pd(recdata,0)
                    '''if ready=='modifying':
                        waitForMessage()
                    if ready=='aborted':
                        print('Aborted because it took too long!')'''
                else:
                    pd(recdata,0)
        except:
            print('Invalid message received!')
            print(recJSON)
    waitForMessage()   

def listen2server(s):
    print('listening...')
    rec=s.read_until(b'\r',size=1024).decode()
    return(rec)
d= {
    "action":"view",
    "switch": { 
        "name":'s1',
        "Vout":0,
    }
}
user_input=input('\n**********************************************\nWhat do you want to do?\n\tSee current d --> type ''d''\n\tModify d --> type ''m''\n\tSend d --> type ''s''\n\tClear the terminal --> type ''c''\n').lower()
while(user_input!='q'):
    print('received: ',listen2server(ser))
    '''if( user_input=='d'):
        pd(d,0)
    elif( user_input=='m'):
        action=input('Enter "action" field: ')
        if action!='':
            d['action']=action
        name=input('Enter switch name: ')
        if name!='':
            d['switch']['name']=name
        vout=input('Enter desired Vout: ')
        if vout!='':
            d['switch']['Vout']=float(vout)

    elif(user_input=='s'):
        print('Sending to server...')
        send2server(d)
    elif(user_input=='c'):
        os.system('cls')
    else:
        print('Hmm, that''s not a valid input, try again!')
    '''
    #user_input=input('\n**********************************************\nWhat do you want to do?\n\tSee current d --> type ''d''\n\tModify d --> type ''m''\n\tSend d --> type ''s''\n\tClear the terminal --> type ''c''\n').lower()


print('Adéu siau!')