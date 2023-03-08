import RPi.GPIO as GPIO
import time
from flask import Flask              # 플라스크 모듈 호출

in1 = 12
in2 = 16
in3 = 20
in4 = 21

def setsteps(w1, w2, w3, w4) :
    
    
    GPIO.output( in1, w1 )
    GPIO.output( in2, w2 )
    GPIO.output( in3, w3)
    GPIO.output( in4, w4 )



#clockwise
def forward (delay,steps) :
    i=0
    steps = int(steps)
    for i in range(steps) :
          
        setsteps(1,1,0,0)
        time.sleep(delay)

        setsteps(0,1,1,0)
        time.sleep(delay)

        setsteps(0,0,1,1)
        time.sleep(delay)
                
        setsteps(1,0,0,1)
        time.sleep(delay)
                

#counterclockwise
def backward (delay,steps) :

    k=0
    steps = int(steps)
    for k in range(steps) :
           
        setsteps(1,0,0,1)
        time.sleep(delay)

        setsteps(0,0,1,1)
        time.sleep(delay)          

        setsteps(0,1,1,0)
        time.sleep(delay)          

        setsteps(1,1,0,0)
        time.sleep(delay)
           


# setting up
def startup():
    GPIO.setmode( GPIO.BCM )
    GPIO.setwarnings(False)
    GPIO.setup( in1, GPIO.OUT )
    GPIO.setup( in2, GPIO.OUT )
    GPIO.setup( in3, GPIO.OUT )
    GPIO.setup( in4, GPIO.OUT )

def cleanup():
    GPIO.output( in1, 0 )
    GPIO.output( in2, 0 )
    GPIO.output( in3, 0 )
    GPIO.output( in4, 0 )
    GPIO.cleanup()

#========================================main task======================================
startup()
app = Flask(__name__)               # 플라스크 앱 생성

@app.route('/')                     # 기본('/') 웹주소로 요청이 오면                     
def hello():                        #hello 함수 실행
    message1="change url for control stepmotor! /cw : clockwise /ccw : counterclockwise /clean : gpio cleanup() /start : gpio startup()"
    
    return message1

@app.route('/cw')
def cw():
    forward(0.002, 14.444)   #(속도, 스텝 수)
    

@app.route('/ccw')
def ccw():
    backward(0.002, 14.444)   #(속도, 스텝 수)

@app.route('/clean')
def clean():
    cleanup()

@app.route('/start')
def start():
    startup()
    
if __name__ == '__main__':          # 현재 파일 실행시 개발용 웹서버 구동
    app.run(debug=True, port=8082, host='192.168.0.32')

