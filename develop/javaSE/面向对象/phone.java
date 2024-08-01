public class phone {
    //属性
    private String brand;
    private Double price;
    //行为

    //针对每一个私有化的成员变量提供一个get和set方法
    //set方法，给成员变量赋值
    //get方法，获取成员变量


    public void setBrand(String brand){
        this.brand = brand;

    }
    public String getBrand(){
        return brand;
    }

    public void setPrice(double price){

        if (price >2000 && price<20000){
            this.price = price;
        }else{
            System.out.println("非法变量");
        }
    }
    public Double getPrice(){
        return price;

    }
    public void call(){
        System.out.println("打电话");
    }
    public void playGame(){
        System.out.println("玩游戏");
    }
}
