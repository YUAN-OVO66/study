public class shuju3 {
    public static void main(String[] args) {
        int a=20;
        byte b=(byte)a;//大范围不能赋值给小范围
        //强转
        System.out.println(a);
        System.out.println(b);

        int i=1500;
        byte j=(byte) i;
        System.out.println(j);

        double d=99.5;
        int m=(int)d;
        System.out.println(m);


    }
    
}
