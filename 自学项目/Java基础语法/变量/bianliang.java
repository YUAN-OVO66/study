public class bianliang{
   public static void main(String[] args) {
    // 
    //1定义一个整型变量记住一个整数
    //数据类型 变量名=数据
    int age=23;
    //注意=是赋值的意思，从右往左看
    System.out.println(age);
   //2记住一个小数
   double score=99.5;
   System.out.println(score);
System.out.println("----------------------");
   //使用变量好处，便于扩展和维护
   //变量的特点，里面的数据可以替换
   int age2 =18; 
   System.out.println(age2);
   age2 = 19;
   System.out.println(age2);
   age2=age2+1;
   System.out.println(age2);
   //从右往左进行
System.out.println("----------------------");
//5，钱包 9.5收10，发5
double money= 9.5;
System.out.println(money);
money=money+10;
System.out.println(money);
money=money-5;
System.out.println(money);
//变量要先声明
//需要注意类型
//大括号内有效
{int a = 19;
System.out.println(a);}

   } 

}
