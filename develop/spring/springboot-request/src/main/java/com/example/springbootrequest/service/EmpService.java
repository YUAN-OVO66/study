package com.example.springbootrequest.service;

import com.example.springbootrequest.pojo.Emp;

import java.io.UnsupportedEncodingException;
import java.util.List;

public interface EmpService {
    //获取员工信息
    public List<Emp> listEmp() throws UnsupportedEncodingException;
}
