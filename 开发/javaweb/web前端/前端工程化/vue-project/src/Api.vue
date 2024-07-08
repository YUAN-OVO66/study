<script setup>

//声明响应式数据 ref 响应式对象有一个内部属性value 在组合式api中,一般需要把数据定义为响应式数据
import { ref , onMounted} from 'vue';
const count = ref(0);

//声明函数
function increment() {
    count.value++;
}

//声明钩子函数onMounted
onMounted(()=> {
    console.log('vue已挂载');
});
</script>

<template>
<button @click="increment"> count:{{count}}</button>
</template>