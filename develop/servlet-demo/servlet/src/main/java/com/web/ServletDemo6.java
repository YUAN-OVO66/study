package com.web;
import javax.servlet.*;
import javax.servlet.http.HttpServletRequest;
import java.io.IOException;

public class ServletDemo6 implements Servlet{
    @Override
    public void init(ServletConfig servletConfig) throws ServletException {

    }

    @Override
    public ServletConfig getServletConfig() {
        return null;
    }

    @Override
    public void service(ServletRequest servletRequest, ServletResponse servletResponse) throws ServletException, IOException {
    //根据请求方式的不同，进行不同的处理方式
        HttpServletRequest request = (HttpServletRequest) servletRequest;
        //1.获取请求方式
        String mathod = request.getMethod();
        //2.判断
        if ("GET".equals(mathod)){
            //get方式处理逻辑
        }
        else if ("POST".equals(mathod)){
            //post方式处理逻辑
        }

    }

    @Override
    public String getServletInfo() {
        return null;
    }

    @Override
    public void destroy() {

    }
}
