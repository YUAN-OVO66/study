package com.mybatis;

import com.mybatis.mapper.BrandMapper;
import com.mybatis.pojo.Brand;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;


import java.io.IOException;
import java.io.InputStream;
import java.util.List;

public class MyBatisTest {

    public void testSelectALL() throws IOException {
        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession();

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
        List<Brand> brands= brandMapper.selectAll();

        System.out.println(brands);

        SqlSession.close();
    }

    public void testSelectById() throws IOException {
        int id = 1;
        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession();

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
      Brand brand = brandMapper.selectById(id);
        System.out.println(brand);

        SqlSession.close();
    }


}
