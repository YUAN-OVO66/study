## Java Swing

> <b>Java Swing 是 Java 标准库中用于创建图形用户界面（GUI）的一个工具包。它是 Java Foundation Classes (JFC) 的一部分，用于构建跨平台的桌面应用程序。Swing 提供了丰富的 GUI 组件，比如按钮、文本框、表格、树、标签、对话框等，可以用于开发功能丰富的用户界面。</b>

### Swing 的主要特点

1. **轻量级组件**：
   - Swing 是轻量级的，它并不依赖于操作系统的本地 GUI 组件，而是用纯 Java 实现了所有的组件。这样一来，Swing 程序的外观和行为在不同平台上是一致的。
2. **可定制性强**：
   - Swing 提供了灵活的外观（Look and Feel）功能，允许开发者自定义组件的外观，可以使用默认的外观，也可以设置成与操作系统一致的外观，或者通过自定义来创建独特的界面风格。
3. **事件驱动模型**：
   - Swing 基于事件驱动模型。用户的操作（如点击按钮、输入文本等）会触发事件，程序通过监听器（Listener）来响应这些事件。
4. **MVC 架构**：
   - Swing 的设计基于 Model-View-Controller (MVC) 架构。这种设计模式将数据模型（Model）、视图（View）和控制逻辑（Controller）分离，使得程序的结构更加清晰，有利于维护和扩展。
5. **跨平台性**：
   - 由于 Swing 是纯 Java 实现的，它的应用程序可以在任何支持 Java 的平台上运行，具有良好的跨平台特性。

### Swing 的核心组件

1. **JFrame**：
   - JFrame 是顶层窗口，通常作为应用程序的主窗口。它包含其他组件，如菜单栏、工具栏、内容面板等。
2. **JPanel**：
   - JPanel 是一种通用的容器，可以用来组织和布局其他组件。它通常用于将组件分组或创建复杂的布局。
3. **JButton**：
   - JButton 是一个按钮组件，用户可以点击它来触发操作。它通常用于提交表单或执行特定命令。
4. **JLabel**：
   - JLabel 是一个标签，用于显示文本或图标，通常用于描述其他组件的功能或提供信息。
5. **JTextField**：
   - JTextField 是一个单行文本输入框，用户可以在其中输入数据。
6. **JTextArea**：
   - JTextArea 是一个多行文本区域，允许用户输入多行文本。
7. **JTable**：
   - JTable 是一个强大的表格组件，可以显示二维表格数据，并支持排序、编辑、选择等操作。
8. **JMenuBar**、**JMenu** 和 **JMenuItem**：
   - JMenuBar 是窗口的菜单栏，JMenu 是菜单栏中的菜单，JMenuItem 是菜单中的选项。它们共同用于创建下拉菜单。
9. **JScrollPane**：
   - JScrollPane 是一个带滚动条的容器，用于在空间有限的情况下显示大量内容，比如长文本或大的图片。

### 环境搭建

- 参考环境搭建文档

### JDBC基础

><b>JDBC（Java Database Connectivity）是Java程序访问关系型数据库的标准API（Application Programming Interface）。它允许Java应用程序通过标准接口与数据库进行交互，执行SQL查询、更新数据库记录等操作。</b>

#### 常规流程

- 加载驱动

```
Class.forName("com.mysql.cj.jdbc.Driver");
```

- 获取连接

```
Connection conn = DriverManager.getConnection("jdbc:mysql://localhost:3306/数据库名?characterEncoding=UTF-8","用户名","密码");
```

- 创建游标

```
Statement stat = conn.createStatement();
```

- 查询数据

```
ResultSet rs = stat.executeQuery(sql)
```

- 更新数据

```
stat.execute(sql)
```

#### 数据库连接池

- 定义游标

```
private QueryRunner queryRunner = new QueryRunner(JdbcUtil.getDataSource());
```

- 查询

```
Object[] obj = {};
queryRunner.query(sql,new BeanListHandler<查询表对应的实体类>(查询表对应的实体类.class),obj)
```

- 更新

```
Object[] obj = {};
queryRunner.update(sql,obj)
```

### 数据库基础

- 基础查询

```
SELECT * FROM USER;
SELECT * FROM USER WHERE username = 'zhangsan';
SELECT * FROM USER WHERE username = 'zhangsan' AND id = 1;
SELECT username,PASSWORD FROM USER WHERE username = 'zhangsan' AND id = 1;
```

- 模糊查询

>`_`：一个字符的占位符
>
>`%`：0个或多个字符的占位符
>
>关键字：like

```
SELECT * FROM USER WHERE PASSWORD LIKE '___'
```

- 增加操作

```java
INSERT INTO USER(username,PASSWORD) VALUES('wangwu','123');
```

- 删除操作

```java
DELETE FROM USER WHERE id = 5;
DELETE FROM USER WHERE id = 4 AND username = 'zhaosi';
```

- 修改操作

```java
UPDATE USER SET username = 'zhangsan1',PASSWORD = '123456' WHERE id = 1 AND username = 'zhangsan';
```

### 组件用法

- JFrame

  - 设置关闭此窗体退出系统

  ``` java
  setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
  ```

  - 设置大小

  ```java
  setBounds(x, y, w, h);
  ```

  - 禁止放缩

  ```java
  setResizable(false)
  ```

  - 显示窗体

  ```java
  setVisible(true);
  ```

  - 居中生成

  ```java
  setLocationRelativeTo(null);
  ```

  - 设置主面板

  ```java
  setContentPane(panel);
  ```

  - 设置菜单栏

  ```java
  setJMenuBar(jMenuBar);
  ```

  - 设置logo

  ```java
  Image img = Toolkit.getDefaultToolkit().getImage("图片地址");
  setIconImage(img);
  ```

- 加载美化包

  - 在主程序中添加加载代码

  ```java
  BeautyEyeLNFHelper.frameBorderStyle = BeautyEyeLNFHelper.FrameBorderStyle.translucencySmallShadow;
  BeautyEyeLNFHelper.launchBeautyEyeLNF();
  UIManager.put("RootPane.setupButtonVisible", false);
  ```

  - 加载jar包

  > 将beautyeye_lnf.jar加载到项目中

- JPanel

  - 设置背景图片

  ```java
  JPanel p1 = new BgPanel("图片地址");
  ```

  - 设置大小

  ```java
  p1.setBounds(x, y, w, h);
  ```

  - 设置绝对布局

  ```java
  p1.setLayout(null);
  ```

  - 添加组件

  ```java
  p1.add(组件变量名)
  ```

- JLabel

  - 设置大小

  ```java
  JLabel lb1 = new JLabel("");
  lb1.setBounds(x, y, w, h);
  ```

  - 设置成图片

  ```java
  Icon icon1 = new ImageIcon("images/username.png");
  lb1.setIcon(icon1);
  ```

  - 设置文本内容

  ```java
  lb1.setText("内容")
  ```

  - 获取文本内容

  ```java
  String text = lb1.getText()
  ```

  - 设置字体

  ```java
  lb1.setFont(new Font("幼圆", Font.PLAIN, 18));
  ```

- JTextField

  - 设置大小

  ```java
  JTextField tf = new JTextField();
  tf.setBounds(x, y, w, h);
  ```

  - 设置长度

  ```java
  tf.setColumns(10);
  ```

- JPasswordField

  - 设置大小

  ```java
  JPasswordField ptf = new JPasswordField();
  ptf.setBounds(x, y, w, h)
  ```

  - 设置密码显示样式

  ```java
  ptf.setEchoChar('*');
  ```

- JComboBox

  - 添加下拉框元素

  ```java
  JComboBox cb = new JComboBox();
  cb.addItem(i);
  ```

  - 获取选中的值

  ```java
  cb.getSelectedItem();
  ```

- JRadioButton

  - 绑定单选框组

  ```java
  JRadioButton rb1 = new JRadioButton("A");
  JRadioButton rb2 = new JRadioButton("B");
  JRadioButton rb3 = new JRadioButton("C");
  
  ButtonGroup bg = new ButtonGroup();
  bg.add(rb1);
  bg.add(rb2);
  bg.add(rb3);
  ```

  - 获取选定的值

  ```java
  ButtonGroupUtil bgu = new ButtonGroupUtil();
  String selectText = bgu.getSelectedButtonText(bg);
  ```

- JCheckBox

  - 获取选中的值

  ```java
  JCheckBox cb1 = new JCheckBox("C");
  cb1.setFont(new Font("Times New Roman", Font.PLAIN, 16));
  cb1.setBounds(221, 269, 42, 35);
  contentPane.add(cb1);
  
  JCheckBox cb2 = new JCheckBox("JAVA");
  cb2.setFont(new Font("Times New Roman", Font.PLAIN, 16));
  cb2.setBounds(268, 269, 67, 35);
  contentPane.add(cb2);
  
  JCheckBox cb3 = new JCheckBox("Python");
  cb3.setFont(new Font("Times New Roman", Font.PLAIN, 16));
  cb3.setBounds(337, 269, 71, 35);
  contentPane.add(cb3);
  
  JCheckBox cb4 = new JCheckBox("MySQL");
  cb4.setFont(new Font("Times New Roman", Font.PLAIN, 16));
  cb4.setBounds(410, 269, 82, 35);
  contentPane.add(cb4);
  
  List<JCheckBox> checkBoxs = new ArrayList<>();
  checkBoxs.add(cb4);
  checkBoxs.add(cb3);
  checkBoxs.add(cb2);
  checkBoxs.add(cb1);
  
  for (JCheckBox checkBox : checkBoxs) {
      if(checkBox.isSelected()) {
          System.out.println(checkBox.getText());
      }
  }
  ```

- JTable

  - 设置列名并写入数据

  ```java
  ListToArrayUtil lta = new ListToArrayUtil();
  Object[][] data = lta.getArrayFromList(List<List<Object>> table_data);
  Object[] columns = {"列1","列2","列3","列4"};
  MyTableModel mtm = new MyTableModel(data,columns);
  JTable t1 = new JTable(mtm);
  t1.setBounds(0, 0, 642, 362);
  ```

  - 禁止表头列重新排序

  ```java
  t1.getTableHeader().setReorderingAllowed(false);
  ```

  - 禁止表头调整列宽

  ```java
  t1.getTableHeader().setResizingAllowed(false);
  ```

  - 设置表头居中

  ```
  t1.getTableHeader().setDefaultRenderer(new DefaultTableCellRenderer() {
              {
                  setHorizontalAlignment(JLabel.CENTER); // 设置文本居中
              }
  });
  ```

  - 获取选中的行数

  ```java
  int row = t1.getSelectedRow()
  ```

  - 获取选中行的列数据

  ```java
  String name = (String) t1.getModel().getValueAt(row, 列索引)
  ```

  - 设置行高

  ```java
  t1.setRowHeight(45);
  ```

  - 显示列名

  > 注意：这里要显示列名需要将表格放到滚动面板才可以显示，因为JTable的表头是一个独立的组件，默认不显示，JScrollPane会自动渲染表格的列名。这里需要把JTbale和JScrollPane的位置和大小设置一致，保证表格在滚动面板中且大小一致。

  ```java
  JScrollPane scrollPane = new JScrollPane(t1);
  scrollPane.setBounds(0, 0, 642, 362);
  ```

  - 表格数据居中

  ```java
  t1 = new JTable(mtm){
      @Override
      public TableCellRenderer getDefaultRenderer(Class<?> columnClass) {
          DefaultTableCellRenderer renderer = (DefaultTableCellRenderer) super.getDefaultRenderer(columnClass);
          renderer.setHorizontalAlignment(SwingConstants.CENTER);
          return renderer;
      }
  };
  ```

  - 表格数据载入

  ```java
  t1.setModel(mtm)
  ```

  - 表格数据刷新

  > 注意：这里表格数据是通过模型载入的，所以要先通过模型设置完数据后，通知JTbale数据已变更

  ```java
  mtm.setData(data)
  mtm.fireTableDataChanged()
  ```

  

- JMenuBar

  > 菜单栏包含菜单和菜单项，菜单项在菜单中，菜单在菜单栏中。

  - 创建菜单栏

  ```java
  JMenuBar jMenuBar = new JMenuBar();
  ```

  - 创建菜单

  ```java
  JMenu jMenu1 = new JMenu("菜单1");
  JMenu jMenu2 = new JMenu("菜单2");
  ```

  - 创建菜单项

  ```java
  JMenuItem item1 = new JMenuItem("菜单1-1");
  JMenuItem item2 = new JMenuItem("菜单1-2");
  JMenuItem item3 = new JMenuItem("菜单2-1");
  ```

  - 将菜单项添加至菜单中

  ```java
  jMenu1.add(item1);
  jMenu1.add(item2);
  jMenu2.add(item3);
  ```

  - 将菜单添加至菜单栏中

  ```java
  jMenuBar.add(jMenu1);
  jMenuBar.add(jMenu2);
  ```

  - 将菜单栏添加至面板

  ```java
  setJMenuBar(jMenuBar);
  ```

- 提示框

  - 默认用法

  ```java
  JOptionPane.showMessageDialog(null, "提示内容");
  ```

  - 标识用法

  ```java
  JOptionPane.showMessageDialog(null, "提示内容","标题",JOptionPane.ERROR_MESSAGE);
  ```

- 对话框

  - 使用方法

  > 对话框会返回一个整型值，是：返回0，选择否：返回1，取消：返回2，直接关闭对话框：返回-1

  ```java
  int result = JOptionPane.showConfirmDialog(null, "文字内容");
  ```

### AIUI赋能

```java
MyYuyin.init("your_key")
```



