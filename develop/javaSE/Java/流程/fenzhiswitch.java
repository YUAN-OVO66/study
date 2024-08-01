package 流程;

import java.util.Scanner;

public class fenzhiswitch {
    public static void main(String[] args) {
        String week = "周三";
            //电子备忘录
            switch(week){
                case"周一":
                    System.out.println(1);
                    break;
                case"周二":
                    System.out.println(2);
                    break;
                case"周三":
                    System.out.println(3);
                    break;
                case"周四":
                    System.out.println(4);
                    break;
                case"周五":
                    System.out.println(5);
                    break;
                case"周六":
                    System.out.println(6);
                    break;
                case"周天":
                    System.out.println(7);
                    break;
            default:
                    System.err.println("有误");
            }
        }

    }
    

