from flask import Flask,redirect, render_template, url_for
import redis
import base64

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/config/')
def config():
    r = redis.StrictRedis(host='localhost',port=6379,db=0)
    BeaconData = r.hgetall('beaconlist')
    
    length = r.llen('devicelist')
    DeviceData = r.lrange('devicelist',0,length)
    
    length = r.llen('blackaplist')
    BlackListAPData = r.lrange('blackaplist',0,length)
    
    length = r.llen('blackdevicelist')
    BlackListDeviceData = r.lrange('blackdevicelist',0,length)

    for key in BeaconData:
        BeaconData[key] = base64.b64decode(BeaconData[key]).decode('utf-8')
        if BeaconData[key] == None or BeaconData[key][0] == '\x00':
            BeaconData[key] = '(hidden ssid)'

    data = [BeaconData,DeviceData,BlackListAPData,BlackListDeviceData]
    
    return render_template('config.html',data=data)

@app.route('/blackap/<apmac>')
def add_blackap(apmac):
    r = redis.StrictRedis(host='localhost',port=6379,db=0)
    r.lpush('blackaplist',apmac)
    return redirect(url_for('config'))

@app.route('/blackdevice/<devicemac>')
def add_blackdevice(devicemac):
    r = redis.StrictRedis(host='localhost',port=6379,db=0)
    r.lpush('blackdevicelist',devicemac)
    return redirect(url_for('config'))

if __name__ == '__main__':
    app.run(debug = True,host='0.0.0.0',port=30000)
