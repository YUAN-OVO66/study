package com.request;


import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;

public class URLdemo {
    public static void main(String[] args) throws UnsupportedEncodingException {
        String username = "张三";
        //url编码
        String encode = URLEncoder.encode(username, "UTF-8");
        System.out.println(encode);
        //url解码
        String decode = URLDecoder.decode(encode, "ISO-8859-1");
        System.out.println(decode);
        //转换为字节数据 编码
        byte[] bytes = decode.getBytes("ISO-8859-1");
//        for (byte b : bytes) {
//            System.out.print(b + " ");
//        }
        //字节转化字符串 解码
        String s = new String(bytes, "UTF-8");

        System.out.println(s);
    }
}
