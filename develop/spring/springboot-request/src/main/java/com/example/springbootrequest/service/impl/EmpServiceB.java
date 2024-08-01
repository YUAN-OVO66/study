package com.example.springbootrequest.service.impl;

import com.example.springbootrequest.pojo.Emp;
import com.example.springbootrequest.dao.EmpDao;
import com.example.springbootrequest.service.EmpService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.io.UnsupportedEncodingException;
import java.util.List;

//@Component //将这个类交给ioc管理成为bean
@Service
public class EmpServiceB implements EmpService {
    @Autowired//运行时，IOC容器会提供该类型的bean对象，并赋值给该变量
    //调用dao获取数据
    private EmpDao empDao;

    @Override
    public List<Emp> listEmp() throws UnsupportedEncodingException {
        //调用dao
        List<Emp> empList = empDao.listEmp();
        empList.stream().forEach(emp -> {
            String gender = emp.getGender();
            if ("1".equals(gender)) {
                emp.setGender("男士");
            } else if ("2".equals(gender)) {
                emp.setGender("女士");
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
        return empList;
    }


}
