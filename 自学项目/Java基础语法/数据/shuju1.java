public class shuju1 {
    public static void main(String[] args) {
        //自动类型转换机制
        byte a=12;
        int b=a;//发生自动转换
        System.out.println(a);//12
        System.out.println(b);//12


        int c=100;//4字节
        double d=c;//8字节
        System.out.println(d);
        

        char ch='a';
        int i=ch; 
        



    }
    
}
