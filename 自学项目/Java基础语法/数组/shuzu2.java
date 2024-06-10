public class shuzu2 {
    public static void main(String[] args) {
        //用索引对数组中的元素进行访问
        //1.获取数组里的元素
        //格式：数组名【索引】
         int arr[]={11,22,33};
         int number = arr[0];
        System.out.println(number);
        System.out.println(arr[1]);//直接获取数组1索引
        //2.。把数据存储到数组之中
        //格式：数组名【索引】=具体数据/变量；
        arr[0]=100;//覆盖原数组arr【0】
        System.out.println(arr[0]);
    }
}
