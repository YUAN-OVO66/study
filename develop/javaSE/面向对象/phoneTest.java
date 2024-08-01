public class phoneTest {
    public static void main(String[] args) {
        //创建对象
        phone p1 = new phone();
        //赋值
        p1.setBrand("phone1");
        p1.setPrice(3000.00);

        

        //获取对象中的值
        System.out.println(p1.getBrand());
        System.out.println(p1.getPrice());
        //调用方法
        p1.call();
        p1.playGame();


    }
}
