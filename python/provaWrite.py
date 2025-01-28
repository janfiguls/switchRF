import time
start_time=time.time()
while (1):
    time.sleep(1)
    with open('file.txt',"w",encoding="utf-8") as f:
        f.write(str(time.time()-start_time))