package controller;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

//请求处理类
@RestController
public class  controller {
    @RequestMapping("/hello")
    public String hello(){
        System.out.println("hello world");
        return "hello world";

    }
}
