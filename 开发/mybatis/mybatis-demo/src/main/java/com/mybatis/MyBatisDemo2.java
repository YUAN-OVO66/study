package com.mybatis;

import com.mybatis.mapper.UserMapper;
import com.mybatis.pojo.User;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

/*
* 代理开发*/
public class MyBatisDemo2 {

    public static void main(String[] args) throws IOException {
        //1.读取配置文件
        String resource = "mybatis-config.xml";

        //2.创建SqlSessionFactory
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession();

        //4.执行sql
        //List<User> users = SqlSession.selectList("test.selectAll");
        //获取usermapper代理对象
        UserMapper userMapper = SqlSession.getMapper(UserMapper.class);
        //List<User> users = userMapper.selectAll();
        User users = userMapper.selectById(1);

        System.out.println(users);
        //5.关闭资源
        SqlSession.close();
    }
}

