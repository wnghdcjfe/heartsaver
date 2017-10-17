const express = require('express');
const path = require('path');
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const cors = require('cors');
const favicon = require('serve-favicon')
//noble는 우분투에서 설치해야 함. 
//const noble = require('noble');

app.use(cors());
app.use(express.static(__dirname + '/dist'));
// app.use(favicon(path.join(__dirname, 'dist', 'favicon.ico')))
app.use(favicon(path.join(__dirname,'favicon.ico')))


const diseaseList = require('./diseaseList.js')();
const testingUser = require('./testingUser.js')(); 

//나이와 병에 따라 maxData를 설정하여 testingUser이란 json에 등록시킨다. 
for (let i = 0; i < testingUser.length; i++) {

  for (let j = 0; j < diseaseList.length; j++) {

    if (testingUser[i].disease === diseaseList[j].name) {
      diseaseIndex = diseaseList[j].index;
      break;
    }
  }
  testingUser[i].maxData = 220 - testingUser[i].age - diseaseIndex;
  testingUser[i].timesList = ['00:00', '01:15', '02:30', '03:45', '05:00', '06:15', '07:30', '08:45', '10:00', '11:15', '12:30', '13:45', '15:00', '16:15', '17:30', '18:45', '20:00', '21:15', '22:30', '23:45'];
  testingUser[i].heartsList =[120, 120, 140, 160, 180, 200, 180, 120, 160, 180, 200, 220, 200, 220, 220, 180, 230, 160, 140, 120];
} 

//유명환씨의 데이타를 설정하는 모듈. 
const isClassAlert = (sensorValue, maxData)=>{
  
  if(sensorValue < 60 || sensorValue > maxData){
    return "alert";
  }else{
    return "success"; 
  }
}

let sensorValue4 = 100; 
let maxData4 = testingUser[4].maxData;

const makeOnePerson = ()=>{
  sensorValue = 40; 
  
  const d = new Date();
  const hour = ("0" + d.getHours()).slice(-2);
  const mininute = ("0" + d.getMinutes()).slice(-2);
  const time = hour + ":" + mininute;

  let heartAndClass = {
    "value": sensorValue4,
    "class": "success",
    "time": time
  };
  heartAndClass.class = isClassAlert(sensorValue4,maxData4); 
  return heartAndClass;
}

const getSensor = (maxData) => { 
  const sensorValue = Math.floor(Math.random() * 160) + 60;
  const d = new Date();
  const hour = ("0" + d.getHours()).slice(-2);
  const mininute = ("0" + d.getMinutes()).slice(-2);
  const time = hour + ":" + mininute;

  let heartAndClass = {
    "value": sensorValue,
    "class": "success",
    "time": time
  };
  heartAndClass.class = isClassAlert(sensorValue,maxData); 

  return heartAndClass;
}

const makeData = () => {

  let userlist = [];

  for (let i = 0; i < testingUser.length; i++) { 
    let heartAndClass;
    if(i === 4){
      heartAndClass = makeOnePerson();
    }else{
      heartAndClass = getSensor(testingUser[i].maxData);
    } 

    testingUser[i].timesList.shift();
    testingUser[i].timesList.push(heartAndClass.time);
    testingUser[i].heartsList.shift();
    testingUser[i].heartsList.push(heartAndClass.value);

    const testObj = {
      "id": i + 1,
      "name": testingUser[i].name,
      "heart": heartAndClass.value,
      "age": testingUser[i].age,
      "class": heartAndClass.class,
      "disease": testingUser[i].disease,
      "chartData": {
        "times": testingUser[i].timesList,
        "heartData": testingUser[i].heartsList,
        "minData": 60,
        "maxData": testingUser[i].maxData,
      }
    };
    userlist.push(testObj);
  }
  return userlist;
}

io.on('connection', function (socket) {
  //userlist를 만드는 모듈  
  setInterval(() => {
    const userlist = makeData();
    socket.emit('sendUserlist', userlist);
  }, 1000 * 5);

})
http.listen(52273, () => {
  console.log("52273번 포트에서 시작됩니다.")
})
