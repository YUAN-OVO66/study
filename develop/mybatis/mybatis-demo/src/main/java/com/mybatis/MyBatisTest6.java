package com.mybatis;

import com.mybatis.mapper.BrandMapper;
import com.mybatis.pojo.Brand;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;

public class MyBatisTest6 {
    public static void main(String[] args) throws IOException {

        int ids[] = {9,10,11};
        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession(true);

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
        brandMapper.deleteByIds(ids);
        SqlSession.commit();
        SqlSession.close();
    }


}
