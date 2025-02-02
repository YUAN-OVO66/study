<template>
  <view class="list">
    <view class="list">
      <!-- 左侧滑动内容 -->
      <scroll-view scroll-y="true" class="list-left" :style="'height:'+clentHeight+'px'">
        <view
            v-for="(item, index) in categories"
            :key="index"
            @tap="selectCategory(index)"
        >
          <view :class="['left-name', { 'left-name-actice': selectedCategoryIndex === index }]">
            {{ item.name }}
          </view>
        </view>
      </scroll-view>
      <!-- 右侧滑动内容 -->
      <scroll-view scroll-y="true" class="list-right" :style="'height:'+clentHeight+'px'">
        <block v-for="(item, index) in selectedCategoryData" :key="index">
          <view class="main">
            {{ item.name }}
          </view>
          <view class="product-item" v-for="(product, j) in item.list" :key="j">
            <image :src="product.imgUrl" mode="aspectFit" class="product-image"></image>
            <view class="product-info">
              <text class="product-title">{{ product.name }}</text>
              <text class="product-price">{{ product.price }}</text>
            </view>
            <button class="add-button" @click="addToCart(product)">+</button>
          </view>
        </block>
      </scroll-view>
    </view>
  </view>
</template>

<script>
export default {
  data() {
    return {
      categories: [], // 存放所有分类的数据
      selectedCategoryIndex: 0, // 当前选中的分类索引
      selectedCategoryData: [], // 当前选中的分类商品数据
      clentHeight: 0, // 页面高度
    };
  },
  mounted() {
    // 模拟从后台获取数据
    this.fetchData();
  },
  methods: {
    // 获取数据
    fetchData() {
      const response = {
        code: 0,
        data: [
          {
            id: 1,
            name: '全部',
            data: [
              {
                name: '文创类',
                list: [
                  { id: 1, name: '周边1', imgUrl: '../../static/logo.png', price: '100$' },
                  { id: 2, name: '周边2', imgUrl: '../../static/logo.png', price: '100$' },
                  // 更多商品...
                ]
              },
              {
                name: '彩石类',
                list: [
                  { id: 3, name: '周边3', imgUrl: '../../static/logo.png', price: '100$' },
                  { id: 4, name: '周边4', imgUrl: '../../static/logo.png', price: '100$' },
                  // 更多商品...
                ]
              },
              // 更多分类...
            ]
          },
        ]
      };

      if (response.code === 0) {
        // 设置分类数据
        this.categories = response.data[0].data;
        // 默认显示第一个分类的数据
        this.selectedCategoryData = this.categories[0].list;
      }
    },
    // 选择分类
    selectCategory(index) {
      this.selectedCategoryIndex = index;
      this.selectedCategoryData = this.categories[index].list;
    },
    // 添加到购物车
    addToCart(product) {
      console.log('添加到购物车', product);
      // 这里可以加入具体的添加到购物车逻辑
    }
  }
};
</script>

<style scoped>
.list {
  display: flex;
}

.main {
  font-weight: bold;
  padding: 20rpx 0;
}

.list-left {
  width: 200rpx;
}

.left-name {
  padding: 30rpx 6rpx;
  text-align: center;
}

.left-name-actice {
  border-left: 8rpx solid #49BDFB;
  background-color: #FFFFFF;
}

.list-right {
  flex: 1;
  align-items: center;
  padding: 20px;
  border-bottom: 1px solid #ddd;
}

.product-item {
  display: flex;
  align-items: center;
  padding: 20rpx;
  border-bottom: 1px solid #ddd;
}

.product-image {
  width: 120rpx;
  height: 120rpx;
  margin-right: 20px;
}

.product-info {
  flex: 1;
}

.product-title {
  font-size: 25rpx;
  margin-bottom: 10px;
  color: #333;
}

.product-price {
  font-size: 24rpx;
  color: red;
}

.add-button {
  background-color: #8b5a39;
  border: none;
  color: white;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  cursor: pointer;
  border-radius: 25%;
}
</style>
