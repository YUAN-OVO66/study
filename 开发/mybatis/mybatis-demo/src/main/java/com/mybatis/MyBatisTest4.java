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

public class MyBatisTest4 {
    public static void main(String[] args) throws IOException {
        int status = 1;
        String companyName = "123";
        String brandName = "123";
        String description = "123厉害";
        int ordered = 100;

        //封装对象
        Brand brand = new Brand();
        brand.setStatus(status);
        brand.setCompanyName(companyName);
        brand.setBrandName(brandName);
        brand.setDescription(description);
        brand.setOrdered(ordered);

        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession(true);

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
        brandMapper.add(brand);
        Integer id = brand.getId();
        System.out.println(id);
        SqlSession.commit();
        SqlSession.close();
    }


}
