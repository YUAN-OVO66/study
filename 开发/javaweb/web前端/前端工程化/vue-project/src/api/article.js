//引入axios
import axios, { Axios } from 'axios'
//定义一个变量，记录一个公共的前缀baseURL
const baseURL ='http://localhost:8080';
const instance =  axios.create({baseURL})

//封装函数
//获取所有文章函数
export async function articleGetAllService() {
    //发送异步数据获取文章请求
    //同步等待服务器相应的结果，并返回，async await
    return await instance.get('/article/getAll')
        .then((result) => {
           return result.data;
            //把服务器相应的数据保存起来
        }).catch((err) => {
            console.log(err);
        });

}

//根据文章分类和发布状态搜索的函数
export async function articleSearchService(conditions) {
    //声明search函数

    return await instance.get('/article/search', { params: conditions})
        .then((result) => {
            return result.data;
        })
        .catch((err) => {
            console.log(err);
        });
}