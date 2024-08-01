package com.mybatis.mapper;

import com.mybatis.pojo.User;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Select;

import java.util.Collection;
import java.util.List;

public interface UserMapper {
    List<User> selectAll();
    @Select("select * from tb_user where id=#{id}")
    User selectById (int id);
/*
* MyBatis 参数封装:
*单个参数:
1.P0J0类型:直接使用，属性名和参数占位符名称一致
2.Map集合:直接使用，键名和参数占位符名称一致
3.Collection:封装为Map集合,可以使用Param注解,替换Map集合中默认的键名
map.put("argo",collection集合);
map.put("collection",collection集合);
4.List:封装为Map集合,可以使用Param注解,替换Map集合中默认的键名
map.put("arg",list集合);
map.put("collection",list集合);
map.put("list",list集合);
5.Array:封装为Map集合,可以使用Param注解,替换Map集合中默认的键名
map.put("arg",数组);
map.put("array",数组);
6.其他类型:直接使用
* 多个参数:封装为Map集合,可以使用Param注解,替换Map集合中默认的键名
* */
    User select(@Param("username") String username , @Param("password") String password);
    User select(Collection collection);
}
