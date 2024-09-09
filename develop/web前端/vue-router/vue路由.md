Vue中路由:路径和组件的映射关系

vue-router

目标:认识插件 VueRouter，掌握 VueRouter的基本使用步骤
作用:修改地址栏路径时，切换显示匹配的组件
说明:Vue 官方的一个路由插件，是一个第三方包

5个基础步骤(固定)
1.下载:下载 VueRouter 模块到当前工程
2.引入
3.安装注册
4.创建路由对象
const router = new VueRouter()
5.注入，将路由对象注入到new Vue实例中，建立关联
new vue({
render:h=>h(App)
router
}).$mount('#app')
6.创建需要的组件(views目录)，配置路由规则
7.配置导航，配置路由出口(路径匹配的组件显示的位置)

<hr>
组件分类
页面组件
views存放-配合路由-页面
复用组件
components存放-常用于复用-展示数据
