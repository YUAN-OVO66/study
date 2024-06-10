public class yunsuanfu4 {
    public static void main(String[] args) {
        //三元运算符
        double score=89.5;
        String rs= score>=60?"成绩及格":"成绩不及格";//真前假后
        System.out.println(rs);


        //找出两整数中的较大值并输出
        int a=99;
        int b=67;
        int max =a>b ? a:b;
        System.out.println(max);

        int i=10;
        int j=45;
        int k=34;
        int temp=i>j?i:j;
        int max2= temp>k?temp:k;
        System.out.println(max2);
    }
    
}
