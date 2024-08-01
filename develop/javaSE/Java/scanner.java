import java.util.Scanner;//导包

public class scanner {
    public static void main(String[] args) {

    Scanner sc = new Scanner(System.in);//扫描键盘
        //接受数据
    System.out.println("请输入年龄");
    int age=sc.nextInt();//等待用户输入一个整数，直到回车
    System.out.println("您的年龄是："+age);
    String name=sc.next();    
    System.out.println(name+"欢迎您进入系统：");
}
    
}
