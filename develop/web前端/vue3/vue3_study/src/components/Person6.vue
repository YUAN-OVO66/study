<template>
  <div class="container">
    <!-- 左侧分类列表 -->
    <div class="left-list">
      <div
          v-for="(category, index) in categories"
          :key="category.id"
          class="category-item"
          :class="{'active': selectedCategoryIndex === index}"
          @click="selectCategory(index)"
      >
        {{ category.name }}
      </div>
    </div>

    <!-- 右侧商品展示 -->
    <div class="right-list">
      <div v-if="selectedCategoryData.length > 0">
        <div
            v-for="(item, index) in selectedCategoryData"
            :key="item.id"
            class="product-item"
        >
          <img :src="item.imgUrl" alt="商品图片" class="product-image" />
          <div class="product-info">
            <h3>{{ item.name }}</h3>
            <p>{{ item.price }}</p>
          </div>
        </div>
      </div>
      <div v-else>
        <p>请选择一个分类</p>
      </div>
    </div>
  </div>
</template>

<script>
export default {
  data() {
    return {
      categories: [], // 存放所有分类的数据
      selectedCategoryIndex: 0, // 当前选中的分类索引
      selectedCategoryData: [] // 当前选中的分类商品数据
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
    }
  }
};
</script>

<style scoped>
.container {
  display: flex;
}

.left-list {
  width: 200px;
  margin-right: 20px;
}

.category-item {
  padding: 10px;
  cursor: pointer;
}

.category-item.active {
  background-color: #f0f0f0;
  font-weight: bold;
}

.right-list {
  flex-grow: 1;
}

.product-item {
  display: flex;
  margin-bottom: 20px;
}

.product-image {
  width: 100px;
  height: 100px;
  margin-right: 20px;
}

.product-info {
  display: flex;
  flex-direction: column;
}

.product-info h3 {
  margin: 0;
}

.product-info p {
  color: #888;
}
</style>
