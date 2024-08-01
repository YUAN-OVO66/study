<script lang="ts" setup>
import {
  Check,
  Delete,
  Edit,
  Message,
  Search,
  Star,
} from '@element-plus/icons-vue'
</script>

<template>
    <div class="mb-4">
      
      <el-button type="primary" disabled="true">编辑</el-button>
      <el-button type="success" loading="true">查看</el-button>
      <el-button type="info">Info</el-button>
      <el-button type="warning">Warning</el-button>
      <el-button type="danger">Danger</el-button>
    </div>
  
    <div class="mb-4">
      <el-button type="warning" plain>Warning</el-button>
      <el-button type="danger" plain>Danger</el-button>
    </div>
  

  </template>
  

  