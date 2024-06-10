package com.example.springbootrequest;

import com.example.springbootrequest.Pojo.Address;
import com.example.springbootrequest.Pojo.Result;
import com.example.springbootrequest.Pojo.User;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@RestController
public class requsetController {
    //原始方法
//    @RequestMapping("/simpleParam")
//    public String simpleParam (HttpServletRequest requset) {
//        //获取请求
//        String name = requset.getParameter("name");
//        String ageStr = requset.getParameter("age");
//        int age = Integer.parseInt(ageStr);
//        System.out.println(name+":"+age);
//        return "ok";
//    }
   //springboot方式
    @RequestMapping("/simpleParam")
    public String simpleParam (String name, Integer age) {
        System.out.println(name+":"+age);
        return "ok";
    }
//简单实体
    @RequestMapping("/simplePojo")
    public String simplePojo(User user) {
        System.out.println(user);
        return "ok";
    }
    //复杂实体
    @GetMapping("/complexPojo")
    public String complexPojo(User user){
        System.out.println(user);
        return "ok";

    }
    //数组参数
    @GetMapping("/arrayParam")
    public String arrayParam(String[] hobby){
        System.out.println(Arrays.toString(hobby));
        return "ok";

    }
    //集合数组参数
    @GetMapping("/listParam")
    public String listParam(@RequestParam List<String> hobby){
        System.out.println(hobby);
        return "ok";
    }
    //时间参数
    @GetMapping("/dataParam")
    public String dataParam(@DateTimeFormat (pattern="yyyy-MM-dd HH:mm:ss") LocalDateTime updateTime){
        System.out.println(updateTime);
        return "ok";
    }
    //json实体参数
    @RequestMapping("/jsonParam")
    public String jsonParam(@RequestBody User user){
        System.out.println(user);
        return "ok";
    }
    //路径参数
    @RequestMapping("pathParam/path/{id}/{name}")
    public String pathParam(@PathVariable Integer id , @PathVariable String name){
        System.out.println(id+":"+name);
        return "ok";
    }
    @RequestMapping("/hello")
    public Result hello (){
        System.out.println("hello world");
//        return new Result(1,"success","hello world");
        return Result.success("hello world");
    }

    @RequestMapping("/getAddr")
    public Result getAddr() {
        Address addr = new Address();
        addr.setProvince("广东");
        addr.setCity("深圳");
        return Result.success(addr);

    }
    @RequestMapping("/listAddr")
    public Result listAddr(){
        List<Address> list = new ArrayList<>();
        Address addr = new Address();
        addr.setProvince("广东");
        addr.setCity("深圳");
        Address addr2 = new Address();
        addr2.setProvince("陕西");
        addr2.setCity("西安");
        list.add(addr);
        list.add(addr2);
        return Result.success(list);
    }
}
