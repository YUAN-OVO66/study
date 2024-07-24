package com.request;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
@WebServlet("/req3")
public class RequestDemo3 extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
    //获取username
        //解决乱码：post
        request.setCharacterEncoding("UTF-8");//设置编码
        String username = request.getParameter("username");
        System.out.println(username);//乱码前
        //解决乱码：get
        //    Tomcat对获取到的内容进行了`ISO-8859-1`的URL解码
        byte[] bytes = username.getBytes("ISO-8859-1");
        username = new String(bytes,"UTF-8");
        System.out.println(username);//乱码后

    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        this.doGet(request, response);
    }
}
