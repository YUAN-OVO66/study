请求数据:JSON字符串转为Java对象
响应数据:Java对象转为JSON字符串

现在我们需要获取到该 `JSON` 串中的 `name` 属性值，应该怎么处理呢？

如果它是一个 js 对象，我们就可以通过 `js对象.属性名` 的方式来获取数据。JS 提供了一个对象 `JSON` ，该对象有如下两个方法：

* `parse(str)` ：将 JSON串转换为 js 对象。使用方式是： ==`var jsObject = JSON.parse(jsonStr);`==
* `stringify(obj)` ：将 js 对象转换为 JSON 串。使用方式是：==`var jsonStr = JSON.stringify(jsObject)`==