<script setup lang="ts">
import {reactive,ref,computed} from 'vue'
const firstName=ref('zhang')
const lastName=ref('san')
/*const fullName = computed(()=>{
  return firstName.value.slice(0,1).toUpperCase()+firstName.value.slice(1) +"-"+ lastName.value
})*/

const fullName = computed({
  get(){
    return firstName.value.slice(0,1).toUpperCase()+firstName.value.slice(1) +"-"+ lastName.value
  },
  set(val){
    const [str1,str2] = val.split('-')
    firstName.value = str1
    lastName.value = str2
  }
})
const changefullName = ()=>{
  fullName .value = "li-si"
}

</script>

<template>
  <div class="person">
    姓：<input type="text" v-model="firstName"><br>
    名：<input type="text" v-model="lastName"><br>
    全名：<sapn>{{fullName}}</sapn>
    <button @click = "changefullName">修改全名</button>
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