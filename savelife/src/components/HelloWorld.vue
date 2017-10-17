<template>
  <div class="hello">
    <nav>
      <h1>
        <span class="icon heart">
        </span>
        <span>
          하트세이버
        </span>
        <span class="icon setting">
        </span>
      </h1>
    </nav>
    <div class="userListWrap">
      <div class="userWrap" v-for="(user, index ) in userlist" :key="user.num" :class="user.class">
        <div class="userInfo"> 
          <span>{{user.id}}번 {{user.name}} ( {{user.age}} ) {{user.disease}}</span>
        </div> 
        
        <div class="userHeart">
          <span class="num">{{user.heart}}</span>
          <span class="unit">bpm</span>
        </div> 

        <div>
          <chartComponent :chartData="user.chartData"></chartComponent>
        </div>
      </div>
    </div> 
  </div>
</template>

<script>
  import chartComponent from './chartComponent';

  import io from '../../lib/socket.io';

  //socket으로 userlist를 받는다. 
  //filter로 순위로 filter를 해서 렌더링을 한다. 
  //socket으로 받은 data를 기반으로 한다.   
  export default {
    name: 'HelloWorld',
    data() {
      return { 
        userlist: {},
        socketServer: null
      }
    },
    mounted() {
      this.connect();
    },
    methods: {
      connect() {
        this.socketServer = io.connect('http://127.0.0.1:52273');

        this.socketServer.on('sendUserlist', (userlist) => {
          this.userlist = userlist;
        })
      }
    },
    components: {
      chartComponent
    }
  }

</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
  .userListWrap {
    display: flex;
    flex-wrap: wrap;
    justify-content:space-around;
  }
  .userListWrap > div{
    margin-bottom:10px;
  }
  .userInfo{
    background-color: #c9e1df;
    padding: 10px;
        font-size: 24px;
  }
  .userHeart {
      top: 26px; 
    position: relative;
  } 
  .userHeart > .num{
    font-size: 50px;
    font-weight: bold;
  }

  .userWrap {
    display: inline-block;
     border: 1px solid #79b4b0;
     background:#fff;
  } 
  .alert {
    background: #c92325 !important;
    animation: .8s infinite beatHeart;
  } 

  @keyframes beatHeart {
    0% {
      transform: scale(1);
    }
    25% {
      transform: scale(1.05);
    }
    40% {
      transform: scale(1);
    }
    60% {
      transform: scale(1.05);
    }
    100% {
      transform: scale(1);
    }
  }  

  .icon {
    width: 50px;
    height: 50px;
  }

  nav {
    position: relative;
    width: 1000px;
    margin: 0 auto;
  }

  nav span {
    vertical-align: middle;
    display: inline-block;
    position: relative;
  }

  .heart {
    background: url('../assets/heart.png');
  }

  .setting {
    position: absolute;
    top: 0;
    right: 0;
    background: url('../assets/setting.png');
  }

  h1 {
    font-size: 40px;
    margin-bottom: 5px;
    margin-top: 5px;
  }

  td .num {
    font-size: 50px;
    font-weight: bold;
  }

  td .unit {
    font-size: 14px;
  }

</style>
