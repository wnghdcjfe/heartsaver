<template>
  <chart :options="chartOption" />
</template>

<script>
  import ECharts from 'vue-echarts/components/ECharts.vue'
  import 'echarts'

  //socket으로 userlist를 받는다. 
  //filter로 순위로 filter를 해서 렌더링을 한다. 
  //socket으로 받은 data를 기반으로 한다.     

  export default {
    name: 'HelloWorld',
    props: ['chartData'],
    data() {
      return { 
        
      }
    },
    mounted() {},
    methods: {},
    computed: {
      chartOption() {
        const option = {
          tooltip: {
            trigger: 'axis',
            axisPointer: {
              type: 'cross'
            }
          },
          xAxis: {
            type: 'category',
            boundaryGap: false,
            data: this.chartData.times, 
          },
          yAxis: {
            type: 'value',
            axisLabel: {
              formatter: '{value}'
            },
            axisPointer: {
              snap: true
            }
          },
          visualMap: [{
            show: false,
            type: 'continuous',
            seriesIndex: 0,
            min: 0,
            max: 400
          }],
          color: ['#79b4b0'],
          background: ['#fff'],
          series: [{
            name: '심박수',
            type: 'line',
            smooth: true,
            data: this.chartData.heartData,
            markArea: {
              data: [
                [{
                  // name: '평균값',
                  yAxis:  this.chartData.minData
                }, {
                  yAxis: this.chartData.maxData
                }]
              ]
            },
            markPoint: {
              data: [{
                  type: 'max',
                  name: '최대'
                },
                {
                  type: 'min',
                  name: '최소'
                }
              ]
            },
            markLine: {
              data: [{
                type: 'average',
                name: '평균'
              }]
            }
          }]
        };
        return option;
      }
    },
    components: {
      chart: ECharts
    }
  }

</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style> 

  .echarts {
    height: 200px !important;
    width: 400px !important;
    
  }
 
</style>
