<script setup lang="ts">

import { ref } from 'vue';

  const name = ref('张三');
  const age = ref(18);
  const tel = ref('13800000000');
  const address = ref("北京市")

  const showTel = () => {
  alert(tel.value);
  };

  const changeName = () => {
  name.value = "zhangsan"
  };

  const changeAge = () => {
  age.value +=1 ;
  }
</script>

<template>
<div class="person">
    <h1>姓名：{{name}}</h1>
    <h2>年龄：{{age}}</h2>
    <h2>地址：{{address}}</h2>
    <button @click="changeName">修改姓名</button>
    <button @click="changeAge">修改年龄</button>
    <button @click="showTel">显示联系方式</button>
</div>
</template>

<style scoped>
  .person{
    background-color: skyblue;
    box-shadow: 0 0 10px;
    border-radius: 20px;
    padding: 20px;
  }
  button{
    margin: 10px;
    padding: 10px;
    border-radius: 10px;
    background-color: #fff;
    color: #000;
    border: none;
    cursor: pointer;
  }
  button:hover{
    background-color: #000;
    color: #fff;
  }
</style>