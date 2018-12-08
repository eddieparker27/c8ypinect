import socket
import requests
import datetime
import random
import time

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8080        # The port used by the server

url = 'https://edsdemo.cumulocity.com/measurement/measurements'
headers = {'content-type': 'application/vnd.com.nsn.cumulocity.measurement+json; charset=UTF-8; ver=0.9',
'Accept': 'application/vnd.com.nsn.cumulocity.measurement+json; charset=UTF-8; ver=0.9'}

f=open("authentication.txt","r")
lines=f.readlines()
username=lines[0].strip()
password=lines[1].strip()
f.close()

print "name = " + username
print "password = " + password

while True:
	
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.connect((HOST, PORT))
	s.sendall(b'#')
	data = s.recv(1024)
	vals = [x.strip()[3:] for x in data.split(',')]

	print('Received', repr(data))
	
	timestr = str(datetime.datetime.now())
	timestr = timestr[:10] + "T" + timestr[11:22] + "+00:00"
	
	
	payload = '''{
    "c8y_AccelerationMeasurement": {
        "dx": {
            "value": '''
	#dx = -10.0 + 20.0 * random.random()
	payload += vals[ 0 ]
	payload += ''',
            "unit":"m/s2"
        },
        "dy": {
            "value": '''
	#dy = -10.0 + 20.0 * random.random()
	payload += vals[ 1 ]
	payload += ''',
            "unit":"m/s2"
        },
        "dz": {
            "value": '''
	#dz = -10.0 + 20.0 * random.random()
	payload += vals[ 2 ]
	payload += ''',
            "unit":"m/s2"
        }
    },
    "time": "'''
	payload += timestr
	payload += '''",
    "source": {
        "id": "299"
    },
    "type":"Kinect360Accelerations"
}'''
	print payload
	r = requests.post(url, data=payload, headers=headers, auth=(username, password))
	print r.status_code
	print r.json()
	time.sleep(5)
