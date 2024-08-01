package com.response;

import org.apache.commons.io.IOUtils;

import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * 响应字节数据：设置字符数据的响应体
 */

@WebServlet("/resp4")
public class ResponseDemo4 extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
       //1.读取文件
        FileInputStream fis = new FileInputStream("D://a.png");
        //2.获取字节输出流
        ServletOutputStream os= response.getOutputStream();
        //3.完成流
//        byte[] buff = new byte[1024];
//        int len = 0;
//        while ((len = fis.read(buff))!=-1){
//            os.write(buff,0,len);
//        }
        IOUtils.copy(fis,os);
        fis.close();
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        this.doGet(request, response);
    }
}

