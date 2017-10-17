const PALZZIUUD = ['dfb0'];
noble.on('stateChange', function (state) {

  if (state === 'poweredOn') {
    noble.startScanning(PALZZIUUD, false);
  } else {
    noble.stopScanning();
  }
})

noble.on('discover', function (peripheral) {
  // we found a peripheral, stop scanning
  noble.stopScanning(function(){
    peripheral.connect(function (err) {

        if(err)console.log(err);
        console.log('팔찌 모듈과 접속완료!'); 
        peripheral.discoverServices(PALZZIUUD, function (err, services) {
          services.forEach(function (service) { 
            service.discoverCharacteristics([], function (err, characteristics) { 
        
                characteristics.forEach(function (characteristic) {  
                /* 팔찌로부터 받는 DATA FORMAT
                 *{
                 *  "id":정수값,
                 *  "data":실수값, | data는 비정상이면 -1
                 *  "error":정수값 | 정상일때 0, 아닐 시 error번호 정수값.
                 *}  
                 */
                characteristic.on('read', function(jsonData){
                    console.log(jsonData);
                    if(!jsonData.error){

                    }
                    
                })

                //팔찌로 보내주는 진동여부 결정 0이 진동 x, 1이 진동o 
                let isVibration = 0;
                if(isVibration){
                    const vibrationData = new Buffer(1);
                    characteristic.write(vibrationData, false, function(err){
                        console.log(err)
                    }) 
                } 
              }) 
            })
          })
        })
      })
  }); 
}) 