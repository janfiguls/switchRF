from threading import Thread
import time,serial,queue,json

thread_running = True
send=queue.Queue()

def my_forever_while():
    global thread_running

    start_time = time.time()
    ser=serial.Serial(port='COM4',baudrate=9600,timeout=0.5)

    # run this while there is no input
    while thread_running:

        if time.time() - start_time >= 1:
            start_time = time.time()
            rec=ser.readline().decode()
            if rec!='':
                print('received: ',rec)
            if send.empty()==False:
                sending=send.get(False)
                print('\n\rSending: ',sending)
                ser.write(json.dumps(sending).encode())
                ser.write(b'\r')



def take_input():

    d= {
        "action":"getNames",
        "switch": { 
            "name":'s1',
            "Vout":8,
        }
    }

    user_input = input('Què vols fer? ')
    # doing something with the input
    while user_input!='q':
        time.sleep(0.1)
        if(user_input=='getNames'):
            d['action']=user_input
            send.put(d)
        elif(user_input=='getStatus'):
            d['action']=user_input
            send.put(d)
        elif(user_input=='setVout'):
            d['action']=user_input
            d['switch']['name']=input('Quin switch vols modificar? ')
            d['switch']['Vout']=int(input('Quina tensió vols (en V)? '))
            send.put(d)
        user_input = input('Què vols fer? ')
        
if __name__ == '__main__':
    t1 = Thread(target=my_forever_while)
    t2 = Thread(target=take_input)

    t1.start()
    t2.start()

    t2.join()  # interpreter will wait until your process get completed or terminated
    thread_running = False
    print('Adéu-siau!')