public class Student {

    private int age;
    private String name;

    //如果没有写构造方法
    //虚拟机自动生成构造方法

    //有参构造
    public Student(int age,String name){
        this.age=age;
        this.name=name;

    }

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }


}

