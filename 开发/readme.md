## web核心

B/S 架构:Browser/Server，浏览器/服务器 架构模式，客户端只需要浏览器，应用程序的逻辑和数据都存储在服务器端。浏览器只需要请求服务器，获取Web资源，服务器把Web资源发送给浏览器即可。

静态资源：html，css，js  负责页面展示

动态资源：servlet，jsp 负责逻辑处理

数据库：负责存储数据

http协议：定义通讯规则、

web服务器：负责解析http协议，解析请求数据，并发送响应数据

<hr>

## HTTP协议

超文本传输协议

HTTP 协议特点:
1.基于TCP协议:面向连接，安全
2.基于请求-响应模型,一次请求对应一次响应
3.HTTP协议是无状态的协议:对于事务处理没有记忆能力。每次请求-响应都是独立的。
缺点:多次请求间不能共享数据。(解决方案：java会话技术)
优点:速度快

### =======================================

HTTP-请求数据格式
请求数据分为3部分

1.请求行:请求数据的第一行。其中GET表示请求方式，/表示请求资源路径，HTTP/1.1表示协议版本
2.请求头:第二行开始，格式为key:value形式

常见请求头

Host: 表示请求的主机名
User-Agent: 浏览器版本，例如Chrome浏览器的标识类似Mozila/5.0
Chrome/79：iE浏览器的标识类似Mozilla/5.0(Windows NT.) like Gecko;
Accept: 表示浏览器能接收的资源类型，如text/*，image/*或者*/*表示所有;
Accept-Language: 表示浏览器偏好的语言，服务器可以据此返回不同语言的网页;
Accept-Encoding: 表示浏览器可以支持的压缩类型，例如gzip,deflate等

3.请求体: POST请求的最后一部分，存放请求参数

GET请求和 POST请求区别:

1.GET请求请求参数在请求行中，没有请求体。
POST请求请求参数在请求体中
2.GET请求请求参数大小有限制，POST没有

### =====================================

HTTP-响应数据格式
响应数据分为3部分
1.响应行:响应数据的第一行。其中HTTP/1.1表示协议版本，200表示响应状态码，OK表示状态码描述
2.响应头:第二行开始，格式为key:value形式

常见HTTP 响应头:
Content-Type:表示该响应内容的类型，例如text/htmI/image/jpeg;
Content-Length:表示该响应内容的长度(字节数);
Content-Encoding:表示该响应压缩算法，例如gzip;
Cache-Control:指示客户端应如何缓存，例如max-age=300表示可以最多缓存300秒

3.响应体: 最后一部分。存放响应数据



 一、状态码

| 状态码分类 | 说明 --状态码大全：https://cloud.tencent.com/developer/chapter/13553 |
| ---------- | ------------------------------------------------------------ |
| 1xx        | **响应中**——临时状态码，表示请求已经接受，告诉客户端应该继续请求或者如果它已经完成则忽略它 |
| 2xx        | **成功**——表示请求已经被成功接收，处理已完成                 |
| 3xx        | **重定向**——重定向到其它地方：它让客户端再发起一个请求以完成整个处理。 |
| 4xx        | **客户端错误**——处理发生错误，责任在客户端，如：客户端的请求一个不存在的资源，客户端未被授权，禁止访问等 |
| 5xx        | **服务器端错误**——处理发生错误，责任在服务端，如：服务端抛出异常，路由出错，HTTP版本不支持等 |





 二、常见的响应状态码

| 状态码 | 英文描述                               | 解释                                                         |
| ------ | -------------------------------------- | ------------------------------------------------------------ |
| 200    | **`OK`**                               | 客户端请求成功，即**处理成功**，这是我们最想看到的状态码     |
| 302    | **`Found`**                            | 指示所请求的资源已移动到由`Location`响应头给定的 URL，浏览器会自动重新访问到这个页面 |
| 304    | **`Not Modified`**                     | 告诉客户端，你请求的资源至上次取得后，服务端并未更改，你直接用你本地缓存吧。隐式重定向 |
| 400    | **`Bad Request`**                      | 客户端请求有**语法错误**，不能被服务器所理解                 |
| 403    | **`Forbidden`**                        | 服务器收到请求，但是**拒绝提供服务**，比如：没有权限访问相关资源 |
| 404    | **`Not Found`**                        | **请求资源不存在**，一般是URL输入有误，或者网站资源被删除了  |
| 428    | **`Precondition Required`**            | **服务器要求有条件的请求**，告诉客户端要想访问该资源，必须携带特定的请求头 |
| 429    | **`Too Many Requests`**                | **太多请求**，可以限制客户端请求某个资源的数量，配合 Retry-After(多长时间后可以请求)响应头一起使用 |
| 431    | **` Request Header Fields Too Large`** | **请求头太大**，服务器不愿意处理请求，因为它的头部字段太大。请求可以在减少请求头域的大小后重新提交。 |
| 405    | **`Method Not Allowed`**               | 请求方式有误，比如应该用GET请求方式的资源，用了POST          |
| 500    | **`Internal Server Error`**            | **服务器发生不可预期的错误**。服务器出异常了，赶紧看日志去吧 |
| 503    | **`Service Unavailable`**              | **服务器尚未准备好处理请求**，服务器刚刚启动，还未初始化好   |
| 511    | **`Network Authentication Required`**  | **客户端需要进行身份验证才能获得网络访问权限**               |



<hr>

## Tomcat

概念: Tomcat是Apache 软件基金会一个核心项目，是一个开源免费的轻量级Web服务器，支持Servlet/JSP少量JavaEE规范。
JavaEE: Java Enterprise Edition，Java企业版。指Java企业级开发的技术规范总和。包含13项技术规范:JDBC，JNDI、EJB、RMI、JSP、Servlet、XML、JMS、Java IDL、JTS、JTA、JavaMail、JAF

Tomcat 也被称为 Web容器、Servlet容器。Servlet 需要依赖于 Tomcat才能运行

配置：

1.修改端口号：conf/server.xml

connector：0-65535 

注:HTTP协议默认端口号为80，如果将Tomcat端口号改为80，则将来访问Tomcat时，将不用输入端口号

部署：
将项目放置到 webapps 目录下，即部署完成
一般 JavaWeb项目会被打成war包，然后将war包放到 webapps目录下，Tomcat会自动解压缩 war文件

## servlet

就是一个接口，java提供的一门web动态资源开发技术

Servlet 是JavaEE 规范之一，是一个接口，需要定义Servlet类实现Servlet接口，并由web服务器运行Servlet

#### 快速入门

1.创建 web项目，导入 Servlet依赖坐标

```
    <dependency>
      <groupId>javax.servlet</groupId>
      <artifactId>javax.servlet-api</artifactId>
      <version>3.1.0</version>
      <scope>provided</scope>
    </dependency>
```

2.创建:定义一个类，实现 Servlet接口，并重写接口中所有方法，并在 service方法中输入

```
public class ServletDemo1 implements Servlet {
public void service()}
```

3.配置:在类上使用@WebServlet 注解，配置该 Servlet的访问路径

```
@WebServlet("/demo1")
public class ServletDemo1 implements Servlet {
```

4.访问:启动 Tomcat，浏览器输入URL访问该Servlet

#### 执行流程

1.Servlet由web服务器创建，Servlet方法由web服务器调用:

2.因为我们自定义的Servlet，必须实现Servlet接口并复写其方法，而Servlet接口中有service方法

#### 生命周期

Servlet运行在Servlet容器(web服务器)中，其生命周期由容器来管理，分为4个阶段

1.加载和实例化 :默认情况下，当Servlet第一次被访问时，由容器创建Servlet对象

`@WebServlet(urlPatterns="/demo1",loadOnStartup = 1)`

1.负整数:第一次被访问时创建Servet对象(默认)
2.0或正整数:服务器启动时创建Servlet对象数字越小优先级越高

2.初始化:在Servlet实例化之后，容器将调用Servlet的init()方法初始化这个对象，完成一些如加载配置文件、创建连接等初始化的工作。该方法只调用一次
3.请求处理:每次请求Servlet时，Servlet容器都会调用Servlet的service()方法对请求进行处理
4.服务终止:当需要释放内存或者容器关闭时，容器就会调用Servlet实例的destroy()方法完成资源的释放。在destroy()方法调用之后，容器会释放这个Servlet实例，该实例随后会被Java的垃圾收集器所回收

#### servlet 方法

初始化方法，在Servlet被创建时执行，只执行一次

`void init(ServletConfig config) `

提供服务方法，每次Servlet被访问，都会调用该方法

`void service(ServletRequest req, ServletResponse res)`

销毁方法，当Servlet被销毁时，调用该方法。在内存释放或服务器关闭时销毁Servlet

`void destroy()`

获取ServletConfig对象

`ServletConfig getServletConfig()`

获取Servlet信息

`String getServletlnfo()`

### 体系结构

| Servlet        | Servlet体系根接口             |
| -------------- | ----------------------------- |
| GenericServlet | Servlet抽象实现类             |
| HttpServlet    | 对HTTP协议封装的Servlet实现类 |

1.HttpServlet 使用步骤
继承HttpServlet
重写doGet和doPost方法
2.HttpServlet原理
获取请求方式，并根据不同的请求方式，调用不同的doXxx方法



####  Servlet urlPattern配置
Servlet 要想被访问，必须配置其访问路径:(urlPattern)
1.一个Servlet，可以配置多个 urlPattern
`@WebServlet (urlPatterns ={"/demo1","/demo2"})`

2.urlPattern 配置规则

① 精确匹配

配置路径
@WebServlet("/user/select")
访问路径:
localhost:8080/web-demo/user/select

② 目录匹配


配置路径:
@WebServlet("/user/*")
访问路径:
localhost:8080/web-demo/user/aaa
localhost:8080/web-demo/user/bbb

③ 扩展名匹配

配置路径:
@WebServlet("*.do")
访问路径:
localhost:8080/web-demo/aaa.do
localhost:8080/web-demo/bbb.do

④ 任意匹配

配置路径:
@Webservlet("/")
@WebServlet("/*")
访问路径:
localhost:8080/web-demo/hehe
localhost:8080/web-demo/haha

/和 /* 区别:
1.当我们的项目中的Servlet配置了“/”，会覆盖掉tomcat中的DefaultServlet，当其他的 url-pattern都匹配不上时都会走这个Servlet
2.当我们的项目中配置了“/*”，意味着匹配任意访问路径

优先级:
精确路径 >目录路径>扩展名路径>/*>/

#### XML编写servlet

