Data Access:数据访问
Data Integration:数据集成
Web:Web开发
AOP:面向切面编程
Aspects:AOP思想实现
Core Container:核心容器
Test:单元测试与集成测试

-----------------------------------------------------------------

##### IoC(Inversion of control)控制反转

对象的创建控制权由程序转移到外部了，这种思想称为控制反转

Spring技术对IoC思想进行了实现
Spring提供了一个容器，称为IoC容器，用来充当IoC思想中的“外部”
IoC容器负责对象的创建、初始化等一系列工作，被创建或被管理的对象在Ioc容器中统称为**Bean**

##### DI(Dependency Injection)依赖注入

在容器中建立bean与bean之间的依赖关系的整个过程，称为依赖注入

目标:充分解耦
使用IoC容器管理bean(IoC)
在IoC容器内将有依赖关系的bean进行关系绑定(DI)

最终效果
使用对象时不仅可以直接从IoC容器中获取，并且获取到的bean已经绑定了所有的依赖关系

IoC入门案例思路分析
1.管理什么?(Service与Dao)
2.如何将被管理的对象告知IoC容器?(配置)
3.被管理的对象交给IoC容器，如何获取到IoC容器?(接口)
4.IoC容器得到后，如何从容器中获取bean?(接口方法)
5.使用Spring导入哪些坐标?(pom.xm1)

DI入门案例思路分析
1.基于IoC管理bean
2.Service中使用new形式创建的Dao对象是否保留?(否)
3.Service中需要的Dao对象如何进入到Service中?(提供方法)
4.Service与Dao间的关系如何描述?(配置)

# 三层结构

三个部分：

- 数据访问：负责业务数据的维护操作，包括增、删、改、查等操作。
- 逻辑处理：负责业务逻辑处理的代码。
- 请求处理、响应数据：负责，接收页面的请求，给页面响应数据。

三层

- Controller：控制层。接收前端发送的请求，对请求进行处理，并响应数据。
- Service：业务逻辑层。处理具体的业务逻辑。
- Dao：数据访问层(Data Access Object)，也称为持久层。负责数据访问操作，包括数据的增、删、改、查。

## 基于三层架构的程序执行流程：

- 前端发起的请求，由Controller层接收（Controller响应数据给前端）
- Controller层调用Service层来进行逻辑处理（Service层处理完后，把处理结果返回给Controller层）
- Serivce层调用Dao层（逻辑处理过程中需要用到的一些数据要从Dao层获取）
- Dao层操作文件中的数据（Dao拿到的数据会返回给Service层）

# 分层解耦

内聚:软件中各个功能模块内部的功能联系
耦合:衡量软件中各个层/模块之间的依赖、关联的程度。

原则：高内聚低耦合

Spring中的两个核心概念：

- **控制反转：** Inversion Of Control，简称IOC。对象的创建控制权由程序自身转移到外部（容器），这种思想称为控制反转。

  > 对象的创建权由程序员主动创建转移到容器(由容器创建、管理对象)。这个容器称为：IOC容器或Spring容器

- **依赖注入：** Dependency Injection，简称DI。容器为应用程序提供运行时，所依赖的资源，称之为依赖注入。

  > 程序运行时需要某个资源，此时容器就为其提供这个资源。
  >
  > 例：EmpController程序运行时需要EmpService对象，Spring容器就为其提供并注入EmpService对象

IOC容器中创建、管理的对象，称之为：bean对象

## IOC详解

### bean的声明

Spring框架为了更好的标识web应用程序开发当中，bean对象到底归属于哪一层，又提供了@Component的衍生注解：

- @Controller    （标注在控制层类上）
- @Service          （标注在业务层类上）
- @Repository    （标注在数据访问层类上）

要把某个对象交给IOC容器管理，需要在对应的类上加上如下注解之一：

| 注解        | 说明                 | 位置                                            |
| :---------- | -------------------- | ----------------------------------------------- |
| @Controller | @Component的衍生注解 | 标注在控制器类上                                |
| @Service    | @Component的衍生注解 | 标注在业务类上                                  |
| @Repository | @Component的衍生注解 | 标注在数据访问类上（由于与mybatis整合，用的少） |
| @Component  | 声明bean的基础注解   | 不属于以上三类时，用此注解                      |

注意事项: 

- 声明bean的时候，可以通过value属性指定bean的名字，如果没有指定，默认为类名首字母小写。
- 使用以上四个注解都可以声明bean，但是在springboot集成web开发中，声明控制器bean只能用@Controller。
## Bean管理

  ### 1.bean的扫描

  标签：<context:component-scan **base-package**="com.itheima"/>

  注解：@ComponentScan(basePackages = "com.itheima")

  springboot默认扫描启动类所在的包及其子包

  Bean组件扫描
  前面声明bean的四大注解，要想生效，还需要被组件扫描注解@ComponentScan扫描。
  @ComponentScan注解虽然没有显式配置，但是实际上已经包含在了启动类声明注解 @SpringBootApplication中，默认扫描的范围是启动类所在包及其子包。

### 2.bean的注册

如果要注册的bean对象来自于第三方（不是自定义的），是无法用 @Component 及衍生注解声明bean的

| 注解        | 说明                 | 位置                                            |
| :---------- | -------------------- | ----------------------------------------------- |
| @Controller | @Component的衍生注解 | 标注在控制器类上                                |
| @Service    | @Component的衍生注解 | 标注在业务类上                                  |
| @Repository | @Component的衍生注解 | 标注在数据访问类上（由于与mybatis整合，用的少） |
| @Component  | 声明bean的基础注解   | 不属于以上三类时，用此注解                      |

@Bean  //将方法返回值交给IoC容器管理,成为IoC容器的bean对象（启动类，不建议）

如果要注册第三方bean，建议在配置类中集中注册

对象的默认名字是：方法名

如果方法的内部需要使用到ioc容器中已经存在的bean对象,那么只需要在方法上声明即可,spring会自动的注入

@Import

导入 配置类

导入 ImportSelector接口实现类

### bean注册条件

SpringBoot提供了设置注册生效条件的注解 @Conditional 	

| 注解                      | 说明                                     |
| ------------------------- | ---------------------------------------- |
| @ConditionalOnProperty    | 配置文件中存在对应的属性，才声明该bean   |
| @ConditionalOnMissingBean | 当不存在当前类型的bean时，才声明该bean   |
| @ConditionalOnClass       | 当前环境存在指定的这个类时，才声明该bean |

### 3.bean的配置

```
<!--bean标签标示配置bean
id属性标示给bean起名字
class属性表示给bean定义类型-->
```

```
<!--property标签表示配置当前bean的属性
name属性表示配置哪一个具体的属性
ref属性表示参照哪一个bean-->
```

| 类别     | 描述                                                         |
| -------- | ------------------------------------------------------------ |
| 名称     | bean                                                         |
| 类型     | 标签                                                         |
| 所属     | beans标签                                                    |
| 功能     | 定义Spring核心容器管理的对象                                 |
| 格式     | <beans><bean/><br/><bean></bean><br/></beans>                |
| 属性列表 | id:bean的id,使用容器可以通过id值获取对应的bean,在一个容器中id值唯一<br/>class:bean的类型，即配置的bean的全路径类名 |
| 范例     | <bean id="bookDao" class="com.itheima.dao.impl.BookDaoImpl"/><br/><bean id="bookSeryice" class="com.itheima.service.impl.BookServiceImpl"></bean> |

```
<!--name:为bean指定别名，别名可以有多个，使用逗号，分号，空格进行分隔-->
<!--scope：为bean设置作用范围，可选值为单例singloton，非单例prototype-->
```

适合交给容器进行管理的bean：
表现层对象
业务层对象
数据层对象
工具对象
不适合交给容器进行管理的bean：
封装实体的域对象

## bean实例化

bean本质上就是对象，创建bean使用构造方法完成



## DI详解

依赖注入，是指IOC容器要为应用程序去提供运行时所依赖的资源，而资源指的就是对象。

在入门程序案例中，我们使用了@Autowired这个注解，完成了依赖注入的操作，而这个Autowired翻译过来叫：自动装配。

@Autowired注解，默认是按照**类型**进行自动装配的（去IOC容器中找某个类型的对象，然后完成注入操作）

> 入门程序举例：在EmpController运行的时候，就要到IOC容器当中去查找EmpService这个类型的对象，而我们的IOC容器中刚好有一个EmpService这个类型的对象，所以就找到了这个类型的对象完成注入操作。

Spring提供了以下几种解决方案：

- @Primary 设置优先级

- @Qualifier 限定注入什么类

- @Resource

使用@Primary注解：当存在多个相同类型的Bean注入时，加上@Primary注解，来确定默认的实现。

使用@Qualifier注解：指定当前要注入的bean对象。 在@Qualifier的value属性中，指定注入的bean的名称。

- @Qualifier注解不能单独使用，必须配合@Autowired使用

使用@Resource注解：是按照bean的名称进行注入。通过name属性指定要注入的bean的名称。

> - @Autowired 是spring框架提供的注解，而@Resource是JDK提供的注解
> - @Autowired 默认是按照类型注入，而@Resource是按照名称注入

依赖注入的注解
@Autowired:默认按照类型自动装配。
如果同类型的bean存在多个:
@Primary
@Autowired + @Qualifier("bean的名称")
@Resource(name="bean的名称")

## Spring自动配置原理

遵循约定大约配置的原则，在boot程序启动后，起步依赖中的一些bean对象会自动注入到ioc容器

程序引入spring-boot-starter-web 起步依赖，启动后，会自动往ioc容器中注入DispatcherServlet

---------------------

在主启动类上添加了SpringBootApplication注解这个注解组合了EnableAutoConfiguration注解

EnableAutoConfiguration注解又组合了Import注解,导入了AutoConfigurationImportSelector类

实现selectImports方法,这个方法经过层层调用,最终会读取META-INF 目录下的 后缀名 为imorts的文件,当然了,boot2.7以前的版本,读取的是spring.factories文件,

读取到全类名了之后,会解析注册条件,也就是@Conditional及其衍生注解,把满足注册条件的Bean对象自动注入到IOC容器中

## 自定义Starter

l在实际开发中，经常会定义一些公共组件，提供给各个项目团队使用。而在SpringBoot的项目中，一般会将这些公共组件封装为SpringBoot 的 starter。
