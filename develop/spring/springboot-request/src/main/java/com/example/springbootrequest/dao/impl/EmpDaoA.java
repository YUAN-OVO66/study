package com.example.springbootrequest.dao.impl;

import com.example.springbootrequest.pojo.Emp;
import com.example.springbootrequest.dao.EmpDao;
import com.example.springbootrequest.utils.XmlParserUtils;
import com.sun.tools.javac.Main;
import org.springframework.stereotype.Repository;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.util.List;
//@Component //将这个类交给ioc管理成为bean
@Repository
public class EmpDaoA implements EmpDao {
    //解析emp.xml
    @Override
    public List<Emp> listEmp() throws UnsupportedEncodingException {
    // 获取资源文件的 URL
        java.net.URL resourceUrl = Main.class.getClassLoader().getResource("emp.xml");
        // 获取文件路径
        String filePath = resourceUrl.getFile();
        // URL 解码
        String file = URLDecoder.decode(filePath, "UTF-8");
        //加载解析xml
        List<Emp> empList = XmlParserUtils.parse(file, Emp.class);
        return empList;
    };
}
