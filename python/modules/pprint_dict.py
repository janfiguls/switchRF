def pd(d,level):
    if level is None:
        level=0
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