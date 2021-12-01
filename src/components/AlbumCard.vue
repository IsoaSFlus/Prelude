<script setup>
import { ref } from "vue";

const props = defineProps({
  id: String,
  cover_url: String,
  hires: {
    type: Boolean,
    default: false,
  },
  title: String,
});
</script>

<template>
  <el-row type="flex" justify="center">
    <el-card class="vc-card" :body-style="{ padding: '0px' }">
      <el-badge :hidden="!props.hires" value="Hi" class="hires-badge">
        <img :src="cover_url" class="image" />
      </el-badge>
      <div class="card-video-title">
        <span>{{title}}</span>
      </div>
    </el-card>
  </el-row>
</template>

<style scoped>
.bottom {
  margin-top: 13px;
  line-height: 12px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.hires-badge {
  /* margin: 1rem; */
}
.vc-card {
  margin: 1rem;
  overflow: visible;
}
.image {
  width: 16rem;
  height: 16rem;
  object-fit: cover;
  display: block;
}
.card-video-title {
  max-width: 15.5rem;
  min-height: 3rem;
  display: flex;
  align-items: center;
  justify-content: center;
}
.card-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.card-header-uploader {
  display: flex;
  align-items: center;
}
.card-header-uploader-avator {
  margin-right: 0.4rem;
}
</style>
