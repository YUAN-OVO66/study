<script setup lang="ts">
import {ref,watch} from 'vue'
// 数据
let person = ref({
  name:'张三',
  age:18
})
// 方法
function changeName(){
  person.value.name += '~'
}
function changeAge(){
  person.value.age += 1
}
function changePerson(){
  person.value = {name:'李四',age:90}
}
watch(person,(newValue,oldValue)=>{
  console.log('person变化了',newValue,oldValue)
},{deep:true})
/*
    监视，情况一：监视【ref】定义的【对象类型】数据，监视的是对象的地址值，若想监视对象内部属性的变化，需要手动开启深度监视
    watch的第一个参数是：被监视的数据
    watch的第二个参数是：监视的回调
    watch的第三个参数是：配置对象（deep、immediate等等.....）
  */

</script>

<template>
  <div class="person">
    <h1>情况二：监视【ref】定义的【对象类型】数据</h1>
    <h2>姓名：{{ person.name }}</h2>
    <h2>年龄：{{ person.age }}</h2>
    <button @click="changeName">修改名字</button>
    <button @click="changeAge">修改年龄</button>
    <button @click="changePerson">修改整个人</button>
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
