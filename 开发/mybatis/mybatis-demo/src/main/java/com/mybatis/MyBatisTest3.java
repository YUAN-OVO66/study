package com.mybatis;

import com.mybatis.mapper.BrandMapper;
import com.mybatis.pojo.Brand;
import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MyBatisTest3 {
    public static void main(String[] args) throws IOException {
        int status = 1;
        String companyName = "华为";
        String brandName = "华为";

        companyName = "%"+companyName+"%";
        brandName = "%"+brandName+"%";

        //封装对象
        Brand brand = new Brand();
        brand.setStatus(status);
        brand.setCompanyName(companyName);
        brand.setBrandName(brandName);
        //MAP集合
//        Map map = new HashMap();
//        map.put("status",status);
//        map.put("companyName",companyName);
//        map.put("brandName",brandName);

        //1.获取sqlsessionfactory
        String resource = "mybatis-config.xml";
        InputStream inputStream = Resources.getResourceAsStream(resource);
        SqlSessionFactory sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
        //3.获取SqlSession对象，执行sql
        SqlSession SqlSession = sqlSessionFactory.openSession();

        //获取mapper
        BrandMapper brandMapper = SqlSession.getMapper(BrandMapper.class);

        //执行sql
        //List<Brand> brands = brandMapper.selectByCondition(status, companyname, brandname);
        //List<Brand> brands = brandMapper.selectByCondition(brand);
        //List<Brand> brands = brandMapper.selectByCondition(map);
        List<Brand> brands = brandMapper.selectByConditionSingle(brand);
        System.out.println(brands);

        SqlSession.close();
    }


}
