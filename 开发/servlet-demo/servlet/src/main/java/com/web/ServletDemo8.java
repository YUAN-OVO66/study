package com.web;

import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;

public class ServletDemo8 extends HttpServlet{
    @Override
    protected void doGet(ServletRequest req, ServletResponse res) {
        System.out.println("demo8 get...");
    }
}
