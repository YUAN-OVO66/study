package com.mybatis;

import com.mybatis.mapper.BrandMapper;
import com.mybatis.mapper.UserMapper;
import com.mybatis.pojo.User;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashSet;

public class MyBatisUserTest {
    public static void main(String[] args) throws IOException {

        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession(true);

        //获取mapper
        UserMapper userMapper  = SqlSession.getMapper(UserMapper.class);

        String username = "zhangsan";
        String password = "123";
        //User user = userMapper.select(username, password);
        User user = userMapper.select(new HashSet());
        System.out.println(user);
        //执行sql
        SqlSession.commit();
        SqlSession.close();
    }


}
