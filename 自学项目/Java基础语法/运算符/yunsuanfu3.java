public class yunsuanfu3 {
    public static void main(String[] args) {
        double size = 6.8;
        int storage = 16;
         boolean rs = size>=6.95 & storage>=8;
         //与运算
        System.out.println(rs);
        boolean rs2 = size>=6.95 | storage>=8;
        //或运算
        System.out.println(rs2);
        System.out.println(!true);
        System.out.println(!false);
        System.out.println(!(2>1));
        //异或，前后相同时返回fales 前后不相同返回true
        System.out.println(true^true);//false
        System.out.println(false^false);//false
        System.out.println(true^false);//true
        System.out.println(false^true);//true

        //双与 左边false 右边不执行
        int i=10;
        int j=20;
        //System.out.println(i>100 && ++j>99);
        System.out.println(i>100 & ++j>99);
        System.out.println(j);

        //双或，左边true 右边不执行
        int m=10;
        int n=30;
        //System.out.println(m>3||++n>40);
        System.out.println(m>3|++n>40);
        System.out.println(n);

        
        
    }
}
