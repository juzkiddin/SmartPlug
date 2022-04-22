import firebase_admin
from firebase_admin import credentials,firestore
import hardware as hw
cred = credentials.Certificate("Path_to_File/smartplugauth.json")
firebase_admin.initialize_app(cred,{'storageBucket': 'farmhelptest2.appspot.com'})
db=firestore.client()
root_url = "http://192.168.10.9/"
def update(volt,curr,pow,ene,freq,pf,temp):
    db.collection('smart_plug').document('data').update({'voltage':str(volt)})
    db.collection('smart_plug').document('data').update({'current':str(curr)})
    db.collection('smart_plug').document('data').update({'power':str(pow)})
    db.collection('smart_plug').document('data').update({'energy':str(ene)})
    db.collection('smart_plug').document('data').update({'frequency':str(freq)})
    db.collection('smart_plug').document('data').update({'pf':str(pf)})
    db.collection('smart_plug').document('data').update({'temperature':str(temp)})
    print("updated")
def safety():
    result=db.collection('smart_plug').document('variables').get()
    relay_r = db.collection('smart_plug').document('relay').get()
    result_data = db.collection('smart_plug').document('data').get()
    if (result.exists and relay_r.exists and result_data.exists):
        result=result.to_dict()
        relay_r=relay_r.to_dict()
        result_data = result_data.to_dict()
        s_voltage = result.get('s_voltage')
        s_current = result.get('s_current')
        su_voltage = result.get('Su_voltage')
        t_temp = result.get('T_temperature')
        flag = result.get('flag')
        relay1 = relay_r.get('relay1')
        relay2 = relay_r.get('relay2')
        relay3 = relay_r.get('relay3')
        relay4 = relay_r.get('relay4')
        r_flag = relay_r.get('r_flag')
        if(flag=="true"):
            hw.trigger_data(root_url+"v_"+str(s_voltage))
            hw.trigger_data(root_url+"c_"+str(s_current))
            hw.trigger_data(root_url+"vu_"+str(su_voltage))
            hw.trigger_data(root_url+"t_"+str(t_temp))
            db.collection('smart_plug').document('variables').update({'flag':'false'})
            print("safety_updated")
        if((float(result_data.get('voltage'))>=result.get('Su_voltage'))and(float(result_data.get('voltage'))<=result.get('s_voltage'))and(float(result_data.get('current'))<=result.get('s_current'))and(float(result_data.get('temperature'))<=result.get('T_temperature'))):
            if(relay1=="true"):
                hw.trigger_data(root_url+"r1on")
                print("on 1")
            if(relay2=="true"):
                hw.trigger_data(root_url+"r2on")
                print("on 2")
            if(relay3=="true"):
                hw.trigger_data(root_url+"r3on")
                print("on 3")
            if(relay4=="true"):
                hw.trigger_data(root_url+"r4on")
                print("on 4")
        if(r_flag=="true"):
            if(relay1=="false"):
                hw.trigger_data(root_url+"r1of")
                print("off 1")
            if(relay2=="false"):
                hw.trigger_data(root_url+"r2of")
                print("off 2")
            if(relay3=="false"):
                hw.trigger_data(root_url+"r3of")
                print("off 3")
            if(relay4=="false"):
                hw.trigger_data(root_url+"r4of")
                print("off 4")
            db.collection('smart_plug').document('relay').update({'r_flag':"false"})
            

while(True):
    volt,curr,pow,ene,freq,pf,temp = hw.get_data(root_url)
    update(volt,curr,pow,ene,freq,pf,temp)
    safety()
update(0.00,0.00,0.00,0.00,0.00,0.00,0.00)
