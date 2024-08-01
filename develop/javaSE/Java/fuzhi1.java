public class fuzhi1 {
    public static void main(String[] args) {
        //+=
        double a= 9.5;
        double b= 520;
        a+=b;//a=a+b;
        System.out.println(a);
        double i=600;
        double j= 520;
        i-=j;//i=i-j;
        System.out.println(i);


        int m=10;
        int n = 5;
        m*=n;//m=m*n
        //m/=n  m%=n  
        System.out.println(m);



System.out.println("-------------------------");
byte x=10;
byte y=30;
//x=x+y;编译报错
//x=(byte)(x+y);
x+=y;
System.out.println(x);
    }
    
}
