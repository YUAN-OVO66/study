
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Scanner;

public class choujiang {
    public static void main(String[] args) {
        List<String> participants = new ArrayList<>();
        
        // 添加参与抽奖的人员名单
        participants.add("张三");
        participants.add("李四");
        participants.add("王五");
        participants.add("赵六");
        participants.add("陈七");

        Scanner scanner = new Scanner(System.in);
        System.out.print("请输入抽取的奖品数量：");
        int numPrizes = scanner.nextInt();

        if (numPrizes >= participants.size()) {
            System.out.println("奖品数量不能大于等于参与人数！");
            return;
        }

        Random random = new Random();
        List<String> winners = new ArrayList<>();

        System.out.println("\n抽奖开始！");
        for (int i = 0; i < numPrizes; i++) {
            int index = random.nextInt(participants.size());
            String winner = participants.remove(index);
            winners.add(winner);
            System.out.println("第 " + (i + 1) + " 位中奖者是：" + winner);
        }

        System.out.println("\n恭喜以下人员获得奖品：");
        for (String winner : winners) {
            System.out.println(winner);
        }

        System.out.println("\n抽奖结束！");
    }
}
    

