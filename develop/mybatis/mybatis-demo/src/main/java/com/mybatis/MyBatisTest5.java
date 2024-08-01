package com.mybatis;

import com.mybatis.mapper.BrandMapper;
import com.mybatis.pojo.Brand;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;

public class MyBatisTest5 {
    public static void main(String[] args) throws IOException {
        int status = 0;
        String companyName = "126";
        String brandName = "123";
        String description = "123厉害yjqdbz";
        int ordered = 200;
        int id = 8;

        //封装对象
        Brand brand = new Brand();
        brand.setStatus(status);
//        brand.setCompanyName(companyName);
//        brand.setBrandName(brandName);
        brand.setDescription(description);
//        brand.setOrdered(ordered);
        brand.setId(id);

        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession(true);

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
        int count = brandMapper.updata(brand);
        System.out.println(count);
        SqlSession.commit();
        SqlSession.close();
    }


}
