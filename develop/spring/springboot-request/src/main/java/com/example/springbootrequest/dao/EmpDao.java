package com.example.springbootrequest.dao;

import com.example.springbootrequest.pojo.Emp;

import java.io.UnsupportedEncodingException;
import java.util.List;

public interface EmpDao {
    public List<Emp> listEmp() throws UnsupportedEncodingException;
}
