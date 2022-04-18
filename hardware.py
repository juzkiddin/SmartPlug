import urllib.request
def get_data(url):
    data = urllib.request.urlopen(url+"data").read()
    data = data.decode("utf-8")
    adata = data.split()
    volt = float(adata[0])
    curr = float(adata[1])
    pow = float(adata[2])
    ene = float(adata[3])
    freq = float(adata[4])
    pf = float(adata[5])
    temp = float(adata[6])
    return volt,curr,pow,ene,freq,pf,temp

def trigger_data(url):
    urllib.request.urlopen(url)
    return 0


