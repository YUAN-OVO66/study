//定制请求的实例
import axios from 'axios';
//定义一个变量，记录一个公共的前缀baseURL
const baseURL ='http://localhost:8080';
const instance =  axios.create({baseURL})

//添加响应拦截器
instance.interceptors.response.use(
    result=>{
        return result.data;
    } ,
    err=>{
        alert('服务异常')
        return Promise.reject(err);//异步状态转化为失败的状态
    }
)

export default instance;