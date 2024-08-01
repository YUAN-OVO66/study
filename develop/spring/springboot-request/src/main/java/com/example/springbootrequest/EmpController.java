package com.example.springbootrequest;

import com.example.springbootrequest.Pojo.Emp;
import com.example.springbootrequest.Pojo.Result;
import com.example.springbootrequest.utils.XmlParserUtils;
import com.sun.tools.javac.Main;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.List;

@RestController
public class EmpController {
    @RequestMapping("/listEmp")
    public Result list() throws UnsupportedEncodingException {
        // 获取资源文件的 URL
        URL resourceUrl = Main.class.getClassLoader().getResource("emp.xml");
        // 获取文件路径
        String filePath = resourceUrl.getFile();
        // URL 解码
        String file = URLDecoder.decode(filePath, "UTF-8");
        //加载解析xml
        List<Emp> empList = XmlParserUtils.parse(file, Emp.class);
        //数据转换
        empList.stream().forEach(emp -> {
            String gender = emp.getGender();
            if ("1".equals(gender)) {
                emp.setGender("男");
            } else if ("0".equals(gender)) {
                emp.setGender("女");
            }

            String job = emp.getJob();
            if ("1".equals(job)) {
                emp.setJob("讲师");
            } else if ("2".equals(job)) {
                emp.setJob("班主任");
            } else if ("3".equals(job)) {
                emp.setJob("就业指导");
            }
        });

        //响应
        return Result.success(empList);
    }
}
