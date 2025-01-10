<script setup lang="ts">
import {watch,ref,computed} from 'vue'

const sum = ref(0)
const changeSum = () =>{
  sum.value +=1;
}
const stopWatch = watch(sum,(newValue,oldValue)=>{
  console.log("变化",newValue,oldValue)
  if(newValue>=10){
    stopWatch()
  }
})
</script>

<template>
  <div class="person">
    <h1>一：监视ref定义的基本的数据类型</h1>
    <h2>当前求和为：{{sum}}</h2>
    <button @click = "changeSum">+1</button>
  </div>

</template>


<style scoped>
.person {
  background-color: skyblue;
  box-shadow: 0 0 10px;
  border-radius: 20px;
  padding: 20px;
}
</style>