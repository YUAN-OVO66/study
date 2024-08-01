package com.web;

import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.annotation.WebServlet;

/**
 * 一个Servlet，可以配置多个 urlPattern
 */

@WebServlet(urlPatterns= {"/demo7","/demo8"})
public class ServletDemo7 extends HttpServlet{
    @Override
    protected void doGet(ServletRequest req, ServletResponse res) {
        System.out.println("demo get...");
    }

    @Override
    protected void doPost(ServletRequest req, ServletResponse res) {
        super.doPost(req, res);
    }
}
