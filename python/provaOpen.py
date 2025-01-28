import time

while(1):
    time.sleep(1)
    with open('file.txt','r+',encoding="utf-8") as f:
        print(f.read())
    
    
